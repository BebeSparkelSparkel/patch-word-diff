#include "string_builder.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

String *newHeapString(int length, char *string) {
  assert(length > 0);
  assert(string != NULL);
  assert(length == strlen(string));
  String *s = malloc(sizeof(String));
  s->header.type = StringT;
  s->header.length = length;
  s->shouldFree = true;
  s->string = string;
  return s;
}

String *newDataString(int length, char *string) {
  assert(length > 0);
  assert(string != NULL);
  assert(length == strlen(string));
  String *s = newHeapString(length, string);
  s->shouldFree = false;
  return s;
}

void initStringBuilder(StringBuilder *b) {
//StringBuilder *newStringBuilder() {
//  StringBuilder *b = malloc(sizeof(StringBuilder));
  b->header.type = StringBuilderT;
  b->header.length = 0;
  b->count = 0;
  b->strings = newSLLBuilderStrings();
  return b;
}

void appendString(StringBuilder *b, Strings *s) {
  assert(b != NULL);
  assert(s != NULL);
  sllBuildAppendStrings(b->strings, s);
  b->header.length += ((StringHeader *)s)->length;
  ++b->count;
}

void appendHeapString(StringBuilder *b, int length, char *string) {
  assert(b != NULL);
  assert(length > 0);
  assert(s != NULL);
  String *s = newHeapString(length, string);
  appendString(b, (Strings *)s);
}

void appendDataString(StringBuilder *b, char *string) {
  assert(b != NULL);
  assert(string != NULL);
  String *s = newDataString(strlen(string), string);
  appendString(b, (Strings *)s);
}

StringWriter *finalizeBuilder(StringBuilder *b) {
  StringWriter *w = malloc(sizeof(StringWriter));
  w->header.type = StringWriterT;
  w->header.length = b->header.length;
  w->offset = 0;
  w->strings = sllMaterializeStrings(b->strings);
  return w;
}

InterspersedString *intersperseDataString(StringBuilder *b, char *s) {
  assert(b != NULL);
  assert(s != NULL);
  return intersperseString(b, newDataString(s));
}

InterspersedString *intersperseString(StringBuilder *b, Strings *s) {
  assert(b != NULL);
  assert(s != NULL);
  StringHeader *bh = (StringHeader *)b;
  StringHeader *sh = (StringHeader *)s;
  InterspersedString *i = malloc(sizeof(InterspersedString));
  i->header.type = InterspersedStringT;
  i->header.length = (b->count - 1) * sh->length + bh->length;
  i->writeIntersperse = 0;
  i->toIntersperse = s;
  i->strings = sllMaterializeStrings(b->strings);
  free(b);
  return i;
}

Strings *popFromInterspersedString(InterspersedString *i) {
  Strings *s = i->writeIntersperse ? i->toIntersperse : sllPopStrings(&i->strings);
  if (s != NULL) {
    i->header.length -= ((StringHeader *)s)->length;
    i->writeIntersperse = !i->writeIntersperse;
  }
  return s;
}

void moveNextInterspersedStringToWriterHead(StringWriter *w) {
  InterspersedString *i = (InterspersedString *)w->strings->x;
  assert(i->header.type == InterspersedStringT);
  Strings *s = popFromInterspersedString(i);
  sllPushStrings(&w->strings, s);
}

void freeStrings(Strings *s);

void freeString(String *s) {
  assert(s != NULL);
  assert(s->header.type = StringT);
  if (s->shouldFree == true) {
    assert(s->string != NULL);
    free(s->string);
  }
  free(s);
}

void freeNodeStrings(NodeStrings *n) {
  Strings *s;
  while (n != NULL) {
    s = sllPopStrings(&n);
    freeStrings(s);
  }
}

void freeInterspersedString(InterspersedString *s) {
  assert(s != NULL);
  assert(s->header.type = InterspersedStringT);
  freeStrings(s->toIntersperse);
  freeNodeStrings(s->strings);
  free(s);
}

void freeStrings(Strings *s) {
  switch (((StringHeader *)s)->type) {
    case StringT:
      freeString((String *)s);
      break;
    case InterspersedStringT:
      freeInterspersedString((InterspersedString *)s);
      break;
    default:
      assert(0);
      free(s);
      break;
  }
}

void freeStringWriter(StringWriter *w) {
  assert(w != NULL);
  assert(w->header.type = StringWriterT);
  freeNodeStrings(w->strings);
  free(w);
}

int materializeString(char *buffer, size_t bufRemaining, StringWriter *w) {
  assert(buffer != NULL);
  assert(bufRemaining > 1);
  assert(w != NULL);
  while(bufRemaining > 1 && w->header.length > 0) {
    Strings *h = w->strings->x;
    StringHeader *header = (StringHeader *)h;
    assert(header->length > w->offset);
    switch (header->type) {
      case StringT: {
        String *s = (String *)h;
        int l = header->length - w->offset;
        char *src = s->string + w->offset;
        if (l < bufRemaining) {
          w->offset = 0;
        } else {
          l = bufRemaining - 1;
          w->offset += l;
        }
        strncpy(buffer, src, l);
        bufRemaining -= l;
        buffer += l;
        w->header.length -= l;
        break; }
      case InterspersedStringT:
        moveNextInterspersedStringToWriterHead(w);
        break;
      case StringBuilderT: {
        fprintf(stderr, "Unimplemented sub StringBuilderT materialization\n");
        exit(1);
        break; }
      case StringWriterT: {
        fprintf(stderr, "Unimplemented sub StringWriterT materialization\n");
        assert(0);
        break; }
    }
  }
  *buffer = '\0';
  if (w->header.length <= 0) {
    freeStringWriter(w);
    return 0;
  } else return w->header.length;
}

