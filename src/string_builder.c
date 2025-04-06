#include "string_builder.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

INITWITH(StringHeader, STRINGHEADER_FIELDS);


//String *newCopyString(size_t length, char *string);
//String *newPossessedString(size_t length, char *string);
//String *newWildString(size_t length, char *string);

//String *newStringCopy(size_t length, char *string) {
//  assert(length > 0);
//  assert(string != NULL);
//  assert(length == strlen(string));
//  string = strndup(string, length);
//  return newPossessedString(length, string);
//}
//
//String *newPossessedString(size_t length, char *string) {
//  assert(length > 0);
//  assert(string != NULL);
//  assert(length == strlen(string));
//  String *s = malloc(sizeof(String));
//  s->header.type = StringT;
//  s->header.length = length;
//  s->shouldFree = true;
//  s->string = string;
//  return s;
//}
//
//String *newWildString(size_t length, char *string) {
//  assert(length > 0);
//  assert(string != NULL);
//  assert(length == strlen(string));
//  String *s = newPossessedString(length, string);
//  s->shouldFree = false;
//  return s;
//}

TO_JSON_FORWARD(String) {
  appendWildString(b, "\"");
  appendPossessedString(b, x->header.length, x->string);
  free(x);
  appendWildString(b, "\"");
}

SLL_C(Strings);

INIT_INSTANCE(StringBuilder, STRINGBUILDER_FIELDS);

void appendPossessedString(StringBuilder *b, size_t length, char *string) {
  assert(b != NULL);
  assert(length > 0);
  assert(string != NULL);
  String *s = newPossessedString(length, string);
  appendPossessedStrings(b, String2Strings(s));
}

void appendWildString(StringBuilder *b, char *string) {
  assert(b != NULL);
  assert(string != NULL);
  String *s = newWildString(strlen(string), string);
  appendPossessedStrings(b, String2Strings(s));
}

void appendPossessedString(StringBuilder *b, Strings *s) {
  assert(b != NULL);
  assert(s != NULL);
  sllBuildAppend_Strings(&b->strings, s);
  b->header.length += Strings2StringHeader(s)->length;
  ++b->count;
}

INIT_INSTANCE(StringListBuilder, STRINGLISTBUILDER_FIELDS);

//void appendStringListBuilder(StringListBuilder *l, StringWriter *w) {
//  assert(l != NULL);
//  assert(w != NULL);
//  l->length += w->header.length;
//  ++l->count;
//  sllBuildAppend_Strings(&l->strings, StringWriter2Strings(w));
//}

StringWriter *finalizeBuilder(StringBuilder *b) {
void finalizeBuilder(StringBuilder *b, StringWriter *w);
  assert(b != NULL);
  StringWriter *w = malloc(sizeof(StringWriter));
  w->header.type = StringWriterT;
  w->header.length = b->header.length;
  w->offset = 0;
  w->strings = sllMaterialize_Strings(&b->strings);
  return w;
}

void appendPossessedStringWriter(StringBuilder *b, StringWriter *w) {
    assert(b != NULL);
    assert(w != NULL);
    assert(w->header.type == StringWriterT);
    appendPossessedStrings(b, StringWriter2Strings(w));
}

StringWriter *intersperseWildString(StringListBuilder *l, char *s) {
  assert(l != NULL);
  assert(s != NULL);
  return intersperseString(l, String2Strings(newWildString(strlen(s), s)));
}

//StringWriter *newStringWriter(size_t length, SLLNode_Strings *strings) {
//  StringWriter *w = malloc(sizeof(StringWriter));
//  w->header.type = StringWriterT;
//  w->header.length = length;
//  w->offset = 0;
//  w->strings = strings;
//  return w;
//}

StringWriter *intersperseString(StringListBuilder *l, Strings *s) {
  assert(l != NULL);
  assert(s != NULL);
  InterspersedString *i = malloc(sizeof(InterspersedString));
  i->header.type = InterspersedStringT;
  i->header.length = l->length + (l->count > 1 ? (l->count - 1) * Strings2StringHeader(s)->length : 0);
  i->writeIntersperse = false; // Start with a string, not the interspersed one
  i->toIntersperse = s;
  i->strings = sllMaterialize_Strings(&l->strings);
  return newStringWriter(i->header.length, sslNewNode_Strings(InterspersedString2Strings(i)));
}

TO_UNION(Strings, STRINGS_FIELDS);
FROM_UNION(Strings, STRINGS_FIELDS);

Strings *popFromInterspersedString(InterspersedString *i) {
  assert(!i->writeIntersperse || i->toIntersperse != NULL);
  Strings *s = i->writeIntersperse ? i->toIntersperse : sllPop_Strings(&i->strings);
  if (s != NULL) {
    i->header.length -= Strings2StringHeader(s)->length;
    i->writeIntersperse = !i->writeIntersperse;
  }
  return s;
}

void freeString(String *s) {
  assert(s != NULL);
  assert(s->header.type == StringT);
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
    free_Strings(s);
  }
}

void freeInterspersedString(InterspersedString *s) {
  assert(s != NULL);
  assert(s->header.type == InterspersedStringT);
  free_Strings(s->toIntersperse);
  freeNodeStrings(s->strings);
  free(s);
}

void free_Strings(Strings *s) {
  switch (Strings2StringHeader(s)->type) {
    case StringT:
      freeString(Strings2String(s));
      break;
    case InterspersedStringT:
      freeInterspersedString(Strings2InterspersedString(s));
      break;
    default:
      assert(0);
      free(s);
      break;
  }
}

void freeStringWriter(StringWriter *w) {
  assert(w != NULL);
  assert(w->header.type == StringWriterT);
  freeNodeStrings(w->strings);
  free(w);
}

size_t materializeString(char *buffer, size_t bufRemaining, StringWriter *w) {
  assert(buffer != NULL);
  assert(bufRemaining > 1);
  assert(w != NULL);
  while (bufRemaining > 1 && w->header.length > 0) {
    Strings *h = w->strings->x;
    StringHeader *header = Strings2StringHeader(h);
    assert(header->length > w->offset);
    assert(w->offset >= 0);
    switch (header->type) {
      case StringT: {
        String *s = Strings2String(h);
        assert(s != NULL);
        assert(s->string != NULL);
        int l = header->length - w->offset;
        char *src = s->string + w->offset;
        if (l < bufRemaining) {
          w->offset = 0;
          sllPop_Strings(&w->strings);
        } else {
          l = bufRemaining - 1;
          w->offset += l;
        }
        strncpy(buffer, src, l);
        bufRemaining -= l;
        buffer += l;
        w->header.length -= l;
        break; }
      case InterspersedStringT: {
        InterspersedString *i = Strings2InterspersedString(h);
        Strings *s = popFromInterspersedString(i);
        assert(s != NULL);
        sllPush_Strings(&w->strings, s);
        break; }
      case StringBuilderT: {
        fprintf(stderr, "Unimplemented sub StringBuilderT materialization\n");
        exit(1);
        break; }
      case StringWriterT: {
        StringWriter *w2 = Strings2StringWriter(h);
        size_t l = materializeString(buffer, bufRemaining, w2);
        if (l == 0)
          sllPop_Strings(&w->strings);
        break; }
      default:
        assert(0);
        break;
    }
  }
  *buffer = '\0';
  if (w->header.length <= 0) {
    freeStringWriter(w);
    return 0;
  } else return w->header.length;
}

int fprintStringWriter(FILE *stream, StringWriter *w) {
  assert(stream != NULL);
  assert(w != NULL);
  assert(w->header.type == StringWriterT);
  assert(w->header.length > 0);
  int length = w->header.length;
  int l;
  char buf[BUFSIZ];
  while (materializeString(buf, sizeof(buf), w)) {
    if ((l = fprintf(stream, "%s", buf)) < 0)
      return l;
    assert(l >= 0 && l < BUFSIZ);
  }
  return length;
}

