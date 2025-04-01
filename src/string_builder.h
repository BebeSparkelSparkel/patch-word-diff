#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <stdlib.h>
#include <stdbool.h>
#include "singly_linked_list.h"
#include "json.h"

typedef enum { StringT, StringBuilderT, InterspersedStringT, StringWriterT } StringType;

typedef struct {
  StringType type;
  size_t length; //  length: Total contained string length
} StringHeader;

typedef struct {
  StringHeader header;
  bool shouldFree; // if the char * should be freed when freeing String
  char *string;
} String;

String *newHeapString(size_t length, char *string);

String *newDataString(size_t length, char *string);

typedef union Strings Strings;

SLL_H(Strings);

typedef struct {
  StringHeader header;
  size_t count;
  SLLBuilder_Strings strings;
} StringBuilder;

void initStringBuilder(StringBuilder *b);

void appendHeapString(StringBuilder *b, size_t length, char *string);

void appendDataString(StringBuilder *b, char *string);

void appendString(StringBuilder *b, Strings *string);

TO_JSON_FORWARD(String);

/* writeIntersperse: if non-zero then write the toIntersperse string */
typedef struct {
  StringHeader header;
  bool writeIntersperse;
  Strings *toIntersperse;
  SLLNode_Strings *strings;
} InterspersedString;

typedef struct {
  size_t length;
  size_t count;
  SLLBuilder_Strings strings;
} StringListBuilder;

void initStringListBuilder(StringListBuilder *);

typedef struct StringWriter {
  StringHeader header;
  size_t offset;
  SLLNode_Strings *strings;
} StringWriter;

void appendStringWriter(StringBuilder *b, StringWriter *string);

StringWriter *intersperseDataString(StringListBuilder *l, char *s);

StringWriter *intersperseString(StringListBuilder *l, Strings *s);

void appendStringListBuilder(StringListBuilder *, StringWriter *);

StringWriter *finalizeBuilder(StringBuilder *b);

typedef union Strings {
  StringHeader header;
  String string;
  InterspersedString interspersed;
  StringWriter writer;
} Strings;

Strings *asStrings_String(String *s);

Strings *asStrings_InterspersedString(InterspersedString *i);

Strings *asStrings_StringWriter(StringWriter *w);

size_t materializeString(char *buffer, size_t bufRemaining, StringWriter *w);

#endif
