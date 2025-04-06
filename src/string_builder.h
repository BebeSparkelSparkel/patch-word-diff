#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "singly_linked_list.h"
#include "new.h"
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
  fieldPtr(char, string, end)
TYPEDEF(String, struct, STRING_FIELDS);
TO_JSON_FORWARD(String);

typedef union Strings Strings;
SLL_H(Strings);

#define STRINGBUILDER_FIELDS(fieldType, fieldHeader, field, fieldPtr, fieldList, fieldUnion, infix, end) \
  fieldHeader(StringHeader, infix, StringBuilderT, 0) \
  field(size_t, count, infix) \
  field(SLLB_Strings, strings, end)
TYPEDEF(StringBuilder, struct StringBuilder, STRINGBUILDER_FIELDS);
#define DEFAULT_StringBuilder_count(x) x = 0
#define DEFAULT_StringBuilder_strings(x) sllInitBuilder_Strings(&x)
INIT_FORWARD(StringBuilder);
void appendPossessedString(StringBuilder *b, size_t length, char *string);
void appendWildString(StringBuilder *b, char *string);
void appendPossessedStrings(StringBuilder *b, Strings *string);

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
  field(SLLB_Strings, strings, end)
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
void appendPossessedStringWriter(StringBuilder *b, StringWriter *string);
StringWriter *intersperseWildString(StringListBuilder *l, char *s);
StringWriter *intersperseString(StringListBuilder *l, Strings *s);
void appendPossessedStringListBuilder(StringListBuilder *, StringWriter *);
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
void free_Strings(Strings *s);

#endif
