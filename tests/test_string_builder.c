#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string_builder.h"

// Test for creating a simple string
START_TEST(test_new_string) {
    const char* test_str = "Hello, World!";
    String* str = newStringCopy(strlen(test_str), (char*)test_str);
    
    ck_assert_ptr_nonnull(str);
    ck_assert_int_eq(str->header.type, StringT);
    ck_assert_int_eq(str->header.length, strlen(test_str));
    ck_assert_str_eq(str->string, test_str);
    ck_assert(str->shouldFree);
    
    free(str);
}
END_TEST

// Test for string builder initialization
START_TEST(test_string_builder_init) {
    StringBuilder builder;
    initStringBuilder(&builder);
    
    ck_assert_int_eq(builder.header.type, StringBuilderT);
    ck_assert_int_eq(builder.header.length, 0);
    ck_assert_int_eq(builder.count, 0);
    ck_assert_ptr_null(builder.strings.head);
    ck_assert_ptr_null(builder.strings.tail);
}
END_TEST

// Test for appending a string to a builder
START_TEST(test_append_data_string) {
    StringBuilder builder;
    initStringBuilder(&builder);
    
    const char* test_str = "Test String";
    appendDataString(&builder, (char*)test_str);
    
    ck_assert_int_eq(builder.header.length, strlen(test_str));
    ck_assert_int_eq(builder.count, 1);
    ck_assert_ptr_nonnull(builder.strings.head);
    
    // Check that the string was properly stored
    Strings* str = builder.strings.head->x;
    ck_assert_int_eq(str->header.type, StringT);
    ck_assert_int_eq(str->header.length, strlen(test_str));
    
    String* string_obj = Strings2String(str);
    ck_assert_str_eq(string_obj->string, test_str);
}
END_TEST

// Test for appending multiple strings
START_TEST(test_append_multiple_strings) {
    StringBuilder builder;
    initStringBuilder(&builder);
    
    const char* str1 = "Hello, ";
    const char* str2 = "World!";
    
    appendDataString(&builder, (char*)str1);
    appendDataString(&builder, (char*)str2);
    
    ck_assert_int_eq(builder.header.length, strlen(str1) + strlen(str2));
    ck_assert_int_eq(builder.count, 2);
    ck_assert_ptr_nonnull(builder.strings.head);
    ck_assert_ptr_nonnull(builder.strings.tail);
}
END_TEST

// Test for finalizing a builder into a StringWriter
START_TEST(test_finalize_builder) {
    StringBuilder builder;
    initStringBuilder(&builder);
    
    const char* str1 = "Hello, ";
    const char* str2 = "World!";
    
    appendDataString(&builder, (char*)str1);
    appendDataString(&builder, (char*)str2);
    
    StringWriter* writer = finalizeBuilder(&builder);
    
    ck_assert_ptr_nonnull(writer);
    ck_assert_int_eq(writer->header.type, StringWriterT);
    ck_assert_int_eq(writer->header.length, strlen(str1) + strlen(str2));
    ck_assert_int_eq(writer->offset, 0);
    ck_assert_ptr_nonnull(writer->strings);
    
    free(writer);
}
END_TEST

// Test for materializing a string
START_TEST(test_materialize_string) {
    StringBuilder builder;
    initStringBuilder(&builder);
    
    const char* str1 = "Hello, ";
    const char* str2 = "World!";
    const char* expected = "Hello, World!";
    
    appendDataString(&builder, (char*)str1);
    appendDataString(&builder, (char*)str2);
    
    StringWriter* writer = finalizeBuilder(&builder);
    
    // Allocate a buffer large enough for the resulting string plus null terminator
    size_t bufSize = writer->header.length + 1;
    char* buffer = (char*)malloc(bufSize);
    memset(buffer, 0, bufSize);
    
    // Materialize the string
    size_t written = materializeString(buffer, bufSize, writer);
    
    // Check results
    ck_assert_int_eq(written, strlen(expected));
    ck_assert_str_eq(buffer, expected);
    
    free(buffer);
    free(writer);
}
END_TEST

// Test for string list builder and intersperse
START_TEST(test_string_list_builder_intersperse) {
    StringListBuilder list;
    initStringListBuilder(&list);
    
    StringBuilder builder1;
    initStringBuilder(&builder1);
    appendDataString(&builder1, (char*)"First");
    StringWriter* writer1 = finalizeBuilder(&builder1);
    
    StringBuilder builder2;
    initStringBuilder(&builder2);
    appendDataString(&builder2, (char*)"Second");
    StringWriter* writer2 = finalizeBuilder(&builder2);
    
    StringBuilder builder3;
    initStringBuilder(&builder3);
    appendDataString(&builder3, (char*)"Third");
    StringWriter* writer3 = finalizeBuilder(&builder3);
    
    // Add writers to the list
    appendStringListBuilder(&list, writer1);
    appendStringListBuilder(&list, writer2);
    appendStringListBuilder(&list, writer3);
    
    // Intersperse with commas
    StringWriter* interspersed = intersperseDataString(&list, (char*)", ");
    
    ck_assert_ptr_nonnull(interspersed);
    ck_assert_int_eq(interspersed->header.type, StringWriterT);
    // Expected length: "First" + ", " + "Second" + ", " + "Third" = 5 + 2 + 6 + 2 + 5 = 20
    ck_assert_int_eq(interspersed->header.length, 20);
    
    // Materialize the interspersed string
    size_t bufSize = interspersed->header.length + 1;
    char* buffer = (char*)malloc(bufSize);
    memset(buffer, 0, bufSize);
    
    size_t written = materializeString(buffer, bufSize, interspersed);
    
    // Check results
    ck_assert_int_eq(written, 20);
    ck_assert_str_eq(buffer, "First, Second, Third");
    
    free(buffer);
    free(interspersed);
    free(writer1);
    free(writer2);
    free(writer3);
}
END_TEST

// Test for fprintStringWriter
START_TEST(test_fprint_string_writer) {
    StringBuilder builder;
    initStringBuilder(&builder);
    
    const char* test_str = "Print to file test";
    appendDataString(&builder, (char*)test_str);
    
    StringWriter* writer = finalizeBuilder(&builder);
    
    // Create a temporary file
    FILE* tmp_file = tmpfile();
    ck_assert_ptr_nonnull(tmp_file);
    
    // Print the string to the file
    int result = fprintStringWriter(tmp_file, writer);
    
    // Check if printing was successful
    ck_assert_int_eq(result, strlen(test_str));
    
    // Reset file position to beginning
    rewind(tmp_file);
    
    // Read back the content
    char buffer[100] = {0};
    size_t read = fread(buffer, 1, sizeof(buffer) - 1, tmp_file);
    
    // Check the content
    ck_assert_int_eq(read, strlen(test_str));
    ck_assert_str_eq(buffer, test_str);
    
    fclose(tmp_file);
    free(writer);
}
END_TEST

// Test for edge case: empty strings
START_TEST(test_empty_strings) {
    StringBuilder builder;
    initStringBuilder(&builder);
    
    // Append an empty string
    appendDataString(&builder, (char*)"");
    
    StringWriter* writer = finalizeBuilder(&builder);
    
    ck_assert_ptr_nonnull(writer);
    ck_assert_int_eq(writer->header.length, 0);
    
    // Materialize the empty string
    char buffer[10] = {0};
    size_t written = materializeString(buffer, 10, writer);
    
    ck_assert_int_eq(written, 0);
    ck_assert_str_eq(buffer, "");
    
    free(writer);
}
END_TEST

// Create the test suite
Suite* string_builder_suite(void) {
    Suite* s;
    TCase* tc_core;
    
    s = suite_create("StringBuilder");
    
    // Core test case
    tc_core = tcase_create("Core");
    
    tcase_add_test(tc_core, test_new_string);
    tcase_add_test(tc_core, test_string_builder_init);
    tcase_add_test(tc_core, test_append_data_string);
    tcase_add_test(tc_core, test_append_multiple_strings);
    tcase_add_test(tc_core, test_finalize_builder);
    tcase_add_test(tc_core, test_materialize_string);
    tcase_add_test(tc_core, test_string_list_builder_intersperse);
    tcase_add_test(tc_core, test_fprint_string_writer);
    tcase_add_test(tc_core, test_empty_strings);
    
    suite_add_tcase(s, tc_core);
    
    return s;
}

// Main function
int main(void) {
    int number_failed;
    Suite* s;
    SRunner* sr;
    
    s = string_builder_suite();
    sr = srunner_create(s);
    
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

