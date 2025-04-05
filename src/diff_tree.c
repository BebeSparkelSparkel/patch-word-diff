#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "diff_tree.h"
#include "json_common.h"
#include <assert.h>

TO_JSON_HEAP_STRING(Path);

INITWITH(FileDiffHeader, FILEDIFFHEADER_FIELDS);
NEW_INSTANCE(FileDiffHeader, FILEDIFFHEADER_FIELDS);

TO_JSON_HEAP_STRING(Hash);

TO_JSON_UNSIGNED(FileMode);

INITWITH(GitHeader, GITHEADER_FIELDS);
NEW_INSTANCE(GitHeader, GITHEADER_FIELDS);

TO_UNION(PatchHeader, PATCHHEADER_FIELDS);

TO_JSON_INT(Line);
TO_JSON_INT(Column);

INITWITH(HunkHeader, HUNKHEADER_FIELDS);
NEW_INSTANCE(HunkHeader, HUNKHEADER_FIELDS);

Element  *newElementString(ElementType type, String *string) {
  Element *x = malloc(sizeof(Element));
  x->type = type;
  x->value.string = string;
  return x;
}

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

SLL_C(Element);

SLL_FOLDL(Element, StringListBuilder);

TO_JSON_ENUM(DiffType, DIFFTYPE_ENUM);
SLL_C(Diff);
SLL_FOLDL(Diff, StringListBuilder);
TO_JSON_LIST(Diff);
TO_JSON_OBJ(Diff, DIFF_FIELDS);

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

INITWITH(Hunk, HUNK_FIELDS);
NEW_INSTANCE(Hunk, HUNK_FIELDS);
SLL_C(Hunk);
SLL_FOLDL(Hunk, StringListBuilder);
TO_JSON_LIST(Hunk);
TO_JSON_OBJ(Hunk, HUNK_FIELDS);

INITWITH(Patch, PATCH_FIELDS);
NEW_INSTANCE(Patch, PATCH_FIELDS);
SLL_C(Patch);
SLL_FOLDL(Patch, StringListBuilder);
TO_JSON_LIST(Patch);
TO_JSON_OBJ(Patch, PATCH_FIELDS);

TO_JSON_OBJ(FileDiffHeader, FILEDIFFHEADER_FIELDS);

TO_JSON_OBJ(GitHeader, GITHEADER_FIELDS);

TO_JSON_UNION(PatchHeader, PATCHHEADER_FIELDS);

TO_JSON_OBJ(HunkHeader, HUNKHEADER_FIELDS);

TO_JSON_LIST(Element);

void toJSON_Element(StringBuilder *b, Element *e) {
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
#define JE_NULL(enumerator, ...) \
    case enumerator: \
      appendDataString(b, JSON_STRINGIFY(enumerator)); \
      break;
#define JE_COUNT(enumerator, ...) \
    case enumerator: \
      appendDataString(b, \
        JSON_STRINGIFY(enumerator) \
        JSON_OBJ_COMMA \
        "\"count\"" \
        JSON_OBJ_COLON \
      ); \
      toJSON_int(b, &e->value.count); \
      break;
#define JE_STRING(enumerator, ...) \
    case enumerator: \
      appendDataString(b, \
        JSON_STRINGIFY(enumerator) \
        JSON_OBJ_COMMA \
        "\"string\"" \
        JSON_OBJ_COLON \
      ); \
      toJSON_String(b, e->value.string); \
      break;
    ELEMENT_MAP(JE_NULL, JE_COUNT, JE_STRING, EMPTY_F, EMPTY_F);
  }
  appendDataString(b, JSON_OBJ_CLOSE);
}

