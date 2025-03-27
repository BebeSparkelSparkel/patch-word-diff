#include "string_builder.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

String *newHeapString(size_t length, char *string) {
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

String *newDataString(size_t length, char *string) {
  assert(length > 0);
  assert(string != NULL);
  assert(length == strlen(string));
  String *s = newHeapString(length, string);
  s->shouldFree = false;
  return s;
}

SLL_C(Strings);

void initStringBuilder(StringBuilder *b) {
  b->header.type = StringBuilderT;
  b->header.length = 0;
  b->count = 0;
  sllInitBuilder_Strings(&b->strings);
}

void appendHeapString(StringBuilder *b, size_t length, char *string) {
  assert(b != NULL);
  assert(length > 0);
  assert(string != NULL);
  String *s = newHeapString(length, string);
  appendString(b, asStrings_String(s));
}

void appendDataString(StringBuilder *b, char *string) {
  assert(b != NULL);
  assert(string != NULL);
  String *s = newDataString(strlen(string), string);
  appendString(b, asStrings_String(s));
}

void appendString(StringBuilder *b, Strings *s) {
  assert(b != NULL);
  assert(s != NULL);
  sllBuildAppend_Strings(&b->strings, s);
  b->header.length += ((StringHeader *)s)->length;
  ++b->count;
}

void initStringListBuilder(StringListBuilder *l) {
  assert(l != NULL);
  l->length = 0;
  l->count = 0;
  sllInitBuilder_Strings(&l->strings);
}

void appendStringListBuilder(StringListBuilder *l, StringWriter *w) {
  assert(l != NULL);
  assert(w != NULL);
  l->length += w->header.length;
  ++l->count;
  sllBuildAppend_Strings(&l->strings, asStrings_StringWriter(w));
}

StringWriter *finalizeBuilder(StringBuilder *b) {
  assert(b != NULL);
  StringWriter *w = malloc(sizeof(StringWriter));
  w->header.type = StringWriterT;
  w->header.length = b->header.length;
  w->offset = 0;
  w->strings = sllMaterialize_Strings(&b->strings);
  return w;
}

void appendStringWriter(StringBuilder *b, StringWriter *w) {
    assert(b != NULL);
    assert(w != NULL);
    appendString(b, asStrings_StringWriter(w));
}

StringWriter *intersperseDataString(StringListBuilder *l, char *s) {
  assert(l != NULL);
  assert(s != NULL);
  return intersperseString(l, asStrings_String(newDataString(strlen(s), s)));
}

StringWriter *intersperseString(StringListBuilder *l, Strings *s) {
  assert(l != NULL);
  assert(s != NULL);
  
  InterspersedString *i = malloc(sizeof(InterspersedString));
  i->header.type = InterspersedStringT;
  i->header.length = l->length + (l->count > 0 ? (l->count - 1) * ((StringHeader *)s)->length : 0);
  i->writeIntersperse = false; // Start with a string, not the interspersed one
  i->toIntersperse = s;
  i->strings = sllMaterialize_Strings(&l->strings);
  
  StringWriter *w = malloc(sizeof(StringWriter));
  w->header.type = StringWriterT;
  w->header.length = i->header.length;
  w->offset = 0;
  
  // Create a new node for the interspersed string
  SLLNode_Strings *n = sslNewNode_Strings(asStrings_InterspersedString(i));
  w->strings = n;
  
  return w;
}

Strings *asStrings_String(String *s) {
  assert(s != NULL);
  assert(s->header.type == StringT);
  return (Strings *)s;
}

Strings *asStrings_InterspersedString(InterspersedString *i) {
  assert(i != NULL);
  assert(i->header.type == InterspersedStringT);
  return (Strings *)i;
}

Strings *asStrings_StringWriter(StringWriter *w) {
  assert(w != NULL);
  assert(w->header.type == StringWriterT);
  return (Strings *)w;
}

Strings *popFromInterspersedString(InterspersedString *i) {
  Strings *s = i->writeIntersperse ? i->toIntersperse : sllPop_Strings(&i->strings);
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
  sllPush_Strings(&w->strings, s);
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

void freeNodeStrings(SLLNode_Strings *n) {
  Strings *s;
  while (n != NULL) {
    s = sllPop_Strings(&n);
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

size_t materializeString(char *buffer, size_t bufRemaining, StringWriter *w) {
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

