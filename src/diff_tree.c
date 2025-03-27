#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "diff_tree.h"
#include <assert.h>

TO_JSON_HEAP_STRING(Path);

NEW_INSTANCE(FileDiffHeader, FILEDIFFHEADER_FIELDS);

TO_JSON_HEAP_STRING(Hash);

TO_JSON_UNSIGNED(FileMode);

NEW_INSTANCE(GitHeader, GITHEADER_FIELDS);

TO_JSON_INT(Line);
TO_JSON_INT(Column);

NEW_INSTANCE(HunkHeader, HUNKHEADER_FIELDS);

//Element  *newElementString(ElementType type, char *string) {
//  Element *x = malloc(sizeof(Element));
//  x->type = type;
//  x->value.string = string;
//  return x;
//}

Element  *newElementCount(ElementType type, int count) {
  Element *x = malloc(sizeof(Element));
  x->type = type;
  x->value.count = count;
  return x;
}

Element  *newElement(ElementType type) {
  Element *x = malloc(sizeof(Element));
  x->type = type;
  return x;
}

NEW_NODE(Element);

SLL_FOLDL(Element, StringListBuilder);

TO_JSON_ENUM(DiffType, DIFFTYPE_ENUM);

Diff *match(Element *value) {
  assert(value != NULL);
  Diff *x = malloc(sizeof(Diff));
  x->type = Match;
  x->value = sslNewNode_Element(value);
  return x;
}

Diff *addition(SLLNode_Element *value) {
  assert(value != NULL);
  Diff *x = malloc(sizeof(Diff));
  x->type = Addition;
  x->value = value;
  return x;
}

Diff *removal(SLLNode_Element *value) {
  assert(value != NULL);
  Diff *x = malloc(sizeof(Diff));
  x->type = Removal;
  x->value = value;
  return x;
}

NEW_INSTANCE(Hunk, HUNK_FIELDS);
SLL_FOLDL(Hunk, StringListBuilder);

NEW_INSTANCE(Patch, PATCH_FIELDS);
SLL_FOLDL(Patch, StringListBuilder);

TO_JSON_OBJ(FileDiffHeader, FILEDIFFHEADER_FIELDS);

TO_JSON_OBJ(GitHeader, GITHEADER_FIELDS);

TO_JSON_UNION(PatchHeader, PATCHHEADER_FIELDS);

TO_JSON_OBJ(HunkHeader, HUNKHEADER_FIELDS);

TO_JSON_LIST(Element);

void toJSON_Element(StringBuilder *b, Element *e) {
  char *dst;
  int *length;
  appendDataString(b,
    JSON_OBJ_OPEN
    "\"type\""
    JSON_OBJ_COLON
    "\"Element\""
    JSON_OBJ_COMMA
    "\"element-type\""
    JSON_OBJ_COLON
  );
  switch (e->type) {
#define ELEMENT_F(pattern, enumId, handler) \
    case enumId: \
      appendDataString(b, JSON_STRINGIFY(enumId)); \
      handler \
      break;
#define ELEMENT_COUNT \
      appendHeapString(b, asprintf(&dst, "%d", e->value.count), dst);
#define ELEMENT_STRING \
      appendHeapString(b, e->value.string->header.length, e->value.string->string);
    ELEMENT_MAP( , ELEMENT_COUNT, ELEMENT_STRING, , ELEMENT_F)
#undef ELEMENT_F
#undef ELEMENT_COUNT
#undef ELEMENT_STRING
  }
  appendDataString(b, JSON_OBJ_CLOSE);
}

SLL_FOLDL(Diff, StringListBuilder);

TO_JSON_LIST(Diff);
TO_JSON_OBJ(Diff, DIFF_FIELDS);

TO_JSON_LIST(Hunk);
TO_JSON_OBJ(Hunk, HUNK_FIELDS);

TO_JSON_LIST(Patch);
TO_JSON_OBJ(Patch, PATCH_FIELDS);
