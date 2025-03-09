#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 16384

// Function to apply word-diff patch to original file
int apply_word_diff_patch(const char *original_file_path, const char *patch_file_path);

// Function to process a line with word-diff markers, preserving whitespace
char *process_diff_line(const char *line);

// Function to check if a line is a Git diff header
bool is_diff_header(const char *line);

// Function to check if a line is a Git hunk header
bool is_hunk_header(const char *line);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <original_file_path> <patch_file_path>\n", argv[0]);
        return 1;
    }
    
    return apply_word_diff_patch(argv[1], argv[2]);
}

int apply_word_diff_patch(const char *original_file_path, const char *patch_file_path) {
    // Open the original file for reading
    FILE *original_file = fopen(original_file_path, "r");
    if (!original_file) {
        perror("Error opening original file");
        return 1;
    }
    
    // Open the patch file
    FILE *patch_file = fopen(patch_file_path, "r");
    if (!patch_file) {
        perror("Error opening patch file");
        fclose(original_file);
        return 1;
    }
    
    // Create a temporary file for output
    char temp_file_path[strlen(original_file_path) + 5];
    snprintf(temp_file_path, sizeof(temp_file_path), "%s.tmp", original_file_path);
    
    FILE *temp_file = fopen(temp_file_path, "w");
    if (!temp_file) {
        perror("Error creating temporary file");
        fclose(original_file);
        fclose(patch_file);
        return 1;
    }
    
    // Read the entire original file into memory for direct processing
    fseek(original_file, 0, SEEK_END);
    long original_size = ftell(original_file);
    fseek(original_file, 0, SEEK_SET);
    
    char *original_content = malloc(original_size + 1);
    if (!original_content) {
        perror("Memory allocation error");
        fclose(original_file);
        fclose(patch_file);
        fclose(temp_file);
        return 1;
    }
    
    size_t bytes_read = fread(original_content, 1, original_size, original_file);
    original_content[bytes_read] = '\0';
    fclose(original_file);
    
    // Parse the patch file
    char patch_line[MAX_LINE_LENGTH];
    bool in_content = false;
    bool modifications_made = false;
    
    // Skip Git headers first
    while (fgets(patch_line, sizeof(patch_line), patch_file)) {
        if (is_diff_header(patch_line)) {
            continue;
        } else if (is_hunk_header(patch_line)) {
            in_content = true;
            break; // Found first hunk header, start processing content
        }
    }
    
    // If no hunk header was found, rewind and try to process as a simpler format
    if (!in_content) {
        rewind(patch_file);
        in_content = true; // Assume all content is potentially patchable
    }
    
    // Copy the original content to start with
    char *modified_content = strdup(original_content);
    if (!modified_content) {
        perror("Memory allocation error");
        free(original_content);
        fclose(patch_file);
        fclose(temp_file);
        return 1;
    }
    
    // Process the patch directly in memory
    while (in_content && fgets(patch_line, sizeof(patch_line), patch_file)) {
        // Skip Git diff and hunk headers
        if (is_diff_header(patch_line) || is_hunk_header(patch_line)) {
            continue;
        }
        
        // Skip lines that don't have word-diff markers
        if (strstr(patch_line, "[-") == NULL && strstr(patch_line, "{+") == NULL) {
            continue;
        }
        
        // Process the line
        char *processed_line = process_diff_line(patch_line);
        if (processed_line) {
            // For word-diff, we need to find the matching line and apply the changes
            // This is a simplified approach for demonstration - real implementation would be more robust
            
            // Create a version of patch_line with markers stripped for comparison
            char *compare_line = process_diff_line(patch_line);
            if (compare_line && strlen(compare_line) > 0) {
                // Find the corresponding content in the original
                char *match_pos = strstr(modified_content, compare_line);
                if (match_pos) {
                    // Replace the content
                    size_t match_len = strlen(compare_line);
                    char *remainder = strdup(match_pos + match_len);
                    if (remainder) {
                        *match_pos = '\0';
                        char *new_content = malloc(strlen(modified_content) + strlen(processed_line) + strlen(remainder) + 1);
                        if (new_content) {
                            sprintf(new_content, "%s%s%s", modified_content, processed_line, remainder);
                            free(modified_content);
                            modified_content = new_content;
                            modifications_made = true;
                        }
                        free(remainder);
                    }
                } else {
                    // If we can't find a match, try a more direct approach with markers
                    char *marker_start = strstr(modified_content, "[-");
                    char *addition_start = strstr(modified_content, "{+");
                    
                    if (marker_start || addition_start) {
                        // Process directly with a new copy
                        char *direct_processed = process_diff_line(modified_content);
                        if (direct_processed) {
                            free(modified_content);
                            modified_content = direct_processed;
                            modifications_made = true;
                        }
                    }
                }
            }
            free(compare_line);
            free(processed_line);
        }
    }
    
    fclose(patch_file);
    
    // Write the modified content to the temporary file
    fputs(modified_content, temp_file);
    fclose(temp_file);
    
    // Clean up
    free(original_content);
    free(modified_content);
    
    if (!modifications_made) {
        fprintf(stderr, "Warning: No modifications were made to the file\n");
        remove(temp_file_path);
        return 1;
    }
    
    // Replace the original file with the temporary file
    if (remove(original_file_path) != 0) {
        perror("Error removing original file");
        remove(temp_file_path);
        return 1;
    }
    
    if (rename(temp_file_path, original_file_path) != 0) {
        perror("Error renaming temporary file");
        return 1;
    }
    
    printf("Successfully applied word-diff patch to %s\n", original_file_path);
    
    return 0;
}

bool is_diff_header(const char *line) {
    return (strncmp(line, "diff ", 5) == 0 ||
            strncmp(line, "index ", 6) == 0 ||
            strncmp(line, "--- ", 4) == 0 ||
            strncmp(line, "+++ ", 4) == 0);
}

bool is_hunk_header(const char *line) {
    return (strncmp(line, "@@ ", 3) == 0);
}

char *process_diff_line(const char *line) {
    if (!line) return NULL;
    
    // Allocate memory for the result
    char *result = malloc(strlen(line) * 2 + 1); // Extra space for safety
    if (!result) return NULL;
    
    const char *p = line;
    char *r = result;
    
    while (*p) {
        if (strncmp(p, "[-", 2) == 0) {
            // Skip removed text
            p += 2; // Skip "[-"
            int depth = 1; // To handle nested markers
            
            while (*p && depth > 0) {
                if (strncmp(p, "[-", 2) == 0) {
                    depth++;
                    p += 2;
                } else if (strncmp(p, "-]", 2) == 0) {
                    depth--;
                    p += 2;
                } else {
                    p++;
                }
            }
        } else if (strncmp(p, "{+", 2) == 0) {
            // Copy added text
            p += 2; // Skip "{+"
            
            while (*p && !(p[0] == '+' && p[1] == '}')) {
                *r++ = *p++;
            }
            
            if (*p) p += 2; // Skip "+}"
        } else {
            // Copy regular text (preserving whitespace)
            *r++ = *p++;
        }
    }
    
    *r = '\0'; // Null-terminate the result
    return result;
}
