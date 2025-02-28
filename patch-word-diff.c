#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 8192
#define MAX_LINES 100000

// Function to apply word-diff patch to original file
int apply_word_diff_patch(const char *original_file_path, const char *patch_file_path);

// Function to process a line with word-diff markers
char *process_diff_line(const char *line);

// Function to check if a line is a Git diff header
bool is_diff_header(const char *line);

// Function to parse hunk header and extract line numbers
bool parse_hunk_header(const char *header, int *orig_start, int *orig_count, int *new_start, int *new_count);

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
    
    // Read the entire original file into memory
    char **original_lines = malloc(MAX_LINES * sizeof(char *));
    if (!original_lines) {
        perror("Memory allocation error");
        fclose(original_file);
        return 1;
    }
    
    int original_line_count = 0;
    char line_buffer[MAX_LINE_LENGTH];
    
    while (fgets(line_buffer, sizeof(line_buffer), original_file) && original_line_count < MAX_LINES) {
        original_lines[original_line_count] = strdup(line_buffer);
        if (!original_lines[original_line_count]) {
            perror("Memory allocation error");
            fclose(original_file);
            // Free previously allocated lines
            for (int i = 0; i < original_line_count; i++) {
                free(original_lines[i]);
            }
            free(original_lines);
            return 1;
        }
        original_line_count++;
    }
    
    fclose(original_file);
    
    // Create a copy of the original lines that we'll modify
    char **result_lines = malloc(MAX_LINES * sizeof(char *));
    if (!result_lines) {
        perror("Memory allocation error");
        for (int i = 0; i < original_line_count; i++) {
            free(original_lines[i]);
        }
        free(original_lines);
        return 1;
    }
    
    // Initially, copy all original lines to result
    for (int i = 0; i < original_line_count; i++) {
        result_lines[i] = strdup(original_lines[i]);
        if (!result_lines[i]) {
            perror("Memory allocation error");
            // Free allocated memory
            for (int j = 0; j < i; j++) {
                free(result_lines[j]);
            }
            for (int j = 0; j < original_line_count; j++) {
                free(original_lines[j]);
            }
            free(result_lines);
            free(original_lines);
            return 1;
        }
    }
    
    int result_line_count = original_line_count;
    
    // Open the patch file
    FILE *patch_file = fopen(patch_file_path, "r");
    if (!patch_file) {
        perror("Error opening patch file");
        // Free allocated memory
        for (int i = 0; i < original_line_count; i++) {
            free(original_lines[i]);
            free(result_lines[i]);
        }
        free(original_lines);
        free(result_lines);
        return 1;
    }
    
    // Parse and apply the patch
    char patch_buffer[MAX_LINE_LENGTH];
    bool in_hunk = false;
    int current_orig_line = 0;
    int current_new_line = 0;
    int orig_start = 0, orig_count = 0, new_start = 0, new_count = 0;
    bool modifications_made = false;
    
    while (fgets(patch_buffer, sizeof(patch_buffer), patch_file)) {
        // Skip Git diff headers
        if (is_diff_header(patch_buffer)) {
            continue;
        }
        
        // Check for hunk header
        if (strncmp(patch_buffer, "@@ ", 3) == 0) {
            if (parse_hunk_header(patch_buffer, &orig_start, &orig_count, &new_start, &new_count)) {
                // Adjust to 0-based indexing
                current_orig_line = orig_start - 1;
                current_new_line = new_start - 1;
                in_hunk = true;
                modifications_made = true;
            }
            continue;
        }
        
        // If we're in a hunk, process the lines
        if (in_hunk) {
            // Handle context, addition, and removal lines
            if (patch_buffer[0] == ' ') {
                // Context line - just advance the line counters
                current_orig_line++;
                current_new_line++;
            } else if (patch_buffer[0] == '+') {
                // Added line - process and insert into result
                if (current_new_line < result_line_count) {
                    // Replace the line
                    char *processed = process_diff_line(patch_buffer + 1);
                    if (processed) {
                        free(result_lines[current_new_line]);
                        result_lines[current_new_line] = processed;
                    }
                } else {
                    // Append the line
                    char *processed = process_diff_line(patch_buffer + 1);
                    if (processed) {
                        // Resize result_lines if needed
                        if (result_line_count >= MAX_LINES) {
                            free(processed);
                            fprintf(stderr, "Error: Too many lines in the result\n");
                            break;
                        }
                        result_lines[result_line_count++] = processed;
                    }
                }
                current_new_line++;
            } else if (patch_buffer[0] == '-') {
                // Removed line - skip it in the result (handled by shifts later)
                current_orig_line++;
            } else if (strchr(patch_buffer, '[') || strchr(patch_buffer, '{')) {
                // Line with word-diff markers - process and replace
                if (current_orig_line < original_line_count && current_new_line < MAX_LINES) {
                    char *processed = process_diff_line(patch_buffer);
                    if (processed) {
                        // Replace the line at the current position
                        free(result_lines[current_new_line]);
                        result_lines[current_new_line] = processed;
                        modifications_made = true;
                    }
                }
                current_orig_line++;
                current_new_line++;
            }
        } else if (strchr(patch_buffer, '[') || strchr(patch_buffer, '{')) {
            // Handle word-diff markers outside of hunks (simplified format)
            // Find the matching line in the original file and replace it
            char *line_without_markers = process_diff_line(patch_buffer);
            if (line_without_markers) {
                // Try to find the line that this patch applies to
                // (This is a simplified approach - real patch tools use more complex matching)
                for (int i = 0; i < original_line_count; i++) {
                    if (strstr(original_lines[i], patch_buffer) != NULL) {
                        free(result_lines[i]);
                        result_lines[i] = line_without_markers;
                        modifications_made = true;
                        break;
                    }
                }
                // If no matching line was found, free the processed line
                if (!modifications_made) {
                    free(line_without_markers);
                }
            }
        }
    }
    
    fclose(patch_file);
    
    if (!modifications_made) {
        fprintf(stderr, "Warning: No modifications were made to the file\n");
        // Free allocated memory
        for (int i = 0; i < original_line_count; i++) {
            free(original_lines[i]);
            free(result_lines[i]);
        }
        free(original_lines);
        free(result_lines);
        return 1;
    }
    
    // Write the result to a temporary file
    char temp_file_path[strlen(original_file_path) + 5];
    snprintf(temp_file_path, sizeof(temp_file_path), "%s.tmp", original_file_path);
    
    FILE *temp_file = fopen(temp_file_path, "w");
    if (!temp_file) {
        perror("Error creating temporary file");
        // Free allocated memory
        for (int i = 0; i < original_line_count; i++) {
            free(original_lines[i]);
        }
        for (int i = 0; i < result_line_count; i++) {
            free(result_lines[i]);
        }
        free(original_lines);
        free(result_lines);
        return 1;
    }
    
    // Write all result lines to the temporary file
    for (int i = 0; i < result_line_count; i++) {
        fputs(result_lines[i], temp_file);
        // Ensure the line ends with a newline
        if (result_lines[i][0] != '\0' && 
            result_lines[i][strlen(result_lines[i]) - 1] != '\n') {
            fputc('\n', temp_file);
        }
    }
    
    fclose(temp_file);
    
    // Replace the original file with the temporary file
    if (remove(original_file_path) != 0) {
        perror("Error removing original file");
        remove(temp_file_path);
        // Free allocated memory
        for (int i = 0; i < original_line_count; i++) {
            free(original_lines[i]);
        }
        for (int i = 0; i < result_line_count; i++) {
            free(result_lines[i]);
        }
        free(original_lines);
        free(result_lines);
        return 1;
    }
    
    if (rename(temp_file_path, original_file_path) != 0) {
        perror("Error renaming temporary file");
        // Free allocated memory
        for (int i = 0; i < original_line_count; i++) {
            free(original_lines[i]);
        }
        for (int i = 0; i < result_line_count; i++) {
            free(result_lines[i]);
        }
        free(original_lines);
        free(result_lines);
        return 1;
    }
    
    printf("Successfully applied word-diff patch to %s\n", original_file_path);
    
    // Free allocated memory
    for (int i = 0; i < original_line_count; i++) {
        free(original_lines[i]);
    }
    for (int i = 0; i < result_line_count; i++) {
        free(result_lines[i]);
    }
    free(original_lines);
    free(result_lines);
    
    return 0;
}

bool is_diff_header(const char *line) {
    return (strncmp(line, "diff ", 5) == 0 ||
            strncmp(line, "index ", 6) == 0 ||
            strncmp(line, "--- ", 4) == 0 ||
            strncmp(line, "+++ ", 4) == 0);
}

bool parse_hunk_header(const char *header, int *orig_start, int *orig_count, int *new_start, int *new_count) {
    // Format: @@ -a,b +c,d @@
    if (sscanf(header, "@@ -%d,%d +%d,%d @@", orig_start, orig_count, new_start, new_count) == 4) {
        return true;
    }
    
    // Handle special cases where count is omitted: @@ -a +c @@
    if (sscanf(header, "@@ -%d +%d @@", orig_start, new_start) == 2) {
        *orig_count = 1;
        *new_count = 1;
        return true;
    }
    
    return false;
}

char *process_diff_line(const char *line) {
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
            // Copy regular text
            *r++ = *p++;
        }
    }
    
    *r = '\0'; // Null-terminate the result
    return result;
}
