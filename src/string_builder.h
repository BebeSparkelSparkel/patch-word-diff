#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "new.h"
#include "singly_linked_list.h"
#include "json.h"

#define STRINGTYPE_ENUM(enumerator, infix, end) \
  enumerator(StringT, infix) \
  enumerator(StringBuilderT, infix) \
  enumerator(InterspersedStringT, infix) \
  enumerator(StringWriterT, end)
TYPEDEF_ENUM(StringType, STRINGTYPE_ENUM);

#define STRINGHEADER_FIELDS(fieldType, fieldHeader, field, fieldPtr, fieldList, fieldUnion, infix, end) \
  field(StringType, type, infix) \
  field(size_t, length, end) /*  length: Total contained string length */
TYPEDEF(StringHeader, struct, STRINGHEADER_FIELDS);
INITWITH_FORWARD(StringHeader, STRINGHEADER_FIELDS);

typedef struct StringBuilder StringBuilder;

#define STRING_FIELDS(fieldType, fieldHeader, field, fieldPtr, fieldList, fieldUnion, infix, end) \
  fieldHeader(StringHeader, infix, StringT, 0) \
  field(bool, shouldFree, infix) /* if the char * should be freed when freeing String */ \
  fieldPtr(char, string, end)
TYPEDEF(String, struct, STRING_FIELDS);
TO_JSON_FORWARD(String);
String *newStringCopy(size_t length, char *string);
String *newHeapString(size_t length, char *string);
String *newDataString(size_t length, char *string);

typedef union Strings Strings;
SLL_H(Strings);

#define STRINGBUILDER_FIELDS(fieldType, fieldHeader, field, fieldPtr, fieldList, fieldUnion, infix, end) \
  fieldHeader(StringHeader, infix, StringBuilderT, 0) \
  field(size_t, count, infix) \
  field(SLLBuilder_Strings, strings, end)
TYPEDEF(StringBuilder, struct StringBuilder, STRINGBUILDER_FIELDS);
#define DEFAULT_StringBuilder_count(x) x = 0
#define DEFAULT_StringBuilder_strings(x) sllInitBuilder_Strings(&x)
INIT_FORWARD(StringBuilder);
void appendHeapString(StringBuilder *b, size_t length, char *string);
void appendDataString(StringBuilder *b, char *string);
void appendString(StringBuilder *b, Strings *string);

/* writeIntersperse: if non-zero then write the toIntersperse string */
#define INTERSPERSEDSTRING_FIELDS(fieldType, fieldHeader, field, fieldPtr, fieldList, fieldUnion, infix, end) \
  fieldHeader(StringHeader, infix, InterspersedStringT, 0) \
  field(bool, writeIntersperse, infix) \
  fieldPtr(Strings, toIntersperse, infix) \
  fieldList(Strings, strings, end)
TYPEDEF(InterspersedString, struct, INTERSPERSEDSTRING_FIELDS);

#define STRINGLISTBUILDER_FIELDS(fieldType, fieldHeader, field, fieldPtr, fieldList, fieldUnion, infix, end) \
  field(size_t, length, infix) \
  field(size_t, count, infix) \
  field(SLLBuilder_Strings, strings, end)
TYPEDEF(StringListBuilder, struct, STRINGLISTBUILDER_FIELDS);
#define DEFAULT_StringListBuilder_length(x) x = 0
#define DEFAULT_StringListBuilder_count(x) x = 0
#define DEFAULT_StringListBuilder_strings(x) sllInitBuilder_Strings(&x)
INIT_FORWARD(StringListBuilder);

#define STRINGWRITER_FIELDS(fieldType, fieldHeader, field, fieldPtr, fieldList, fieldUnion, infix, end) \
  fieldHeader(StringHeader, infix, StringWriterT, 0) \
  field(size_t, offset, infix) \
  fieldList(Strings, strings, end)
TYPEDEF(StringWriter, struct, STRINGWRITER_FIELDS);
#define DEFAULT_StringWriter_offset(x) x = 0
INIT_FORWARD(StringWriter);
void appendStringWriter(StringBuilder *b, StringWriter *string);
StringWriter *intersperseDataString(StringListBuilder *l, char *s);
StringWriter *intersperseString(StringListBuilder *l, Strings *s);
void appendStringListBuilder(StringListBuilder *, StringWriter *);
StringWriter *finalizeBuilder(StringBuilder *b);

#define STRINGS_FIELDS(fieldType, fieldHeader, field, fieldPtr, fieldList, fieldUnion, infix, end) \
  fieldHeader(StringHeader, infix) \
  fieldUnion(String, StringT, string, infix) \
  fieldUnion(InterspersedString, InterspersedStringT, interspersed, infix) \
  fieldUnion(StringWriter, StringWriterT, writer, end)
TYPEDEF(Strings, union Strings, STRINGS_FIELDS);
TO_UNION_FORWARD(Strings, STRINGS_FIELDS);
FROM_UNION_FORWARD(Strings, STRINGS_FIELDS);
size_t materializeString(char *buffer, size_t bufRemaining, StringWriter *w);
int fprintStringWriter(FILE *stream, StringWriter *w);

#endif
