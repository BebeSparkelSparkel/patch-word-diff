#include <stdlib.h>
#include <stdio.h>
#include "diff_tree.h"
#include <assert.h>

//FileDiffHeader *newFileDiffHeader(Path pathA, Path pathB) {
//  FileDiffHeader *x = malloc(sizeof(FileDiffHeader));
//  x->pathA = pathA;
//  x->pathB = pathB;
//  return x;
//}
NEW_INSTANCE(FileDiffHeader, FILEDIFFHEADER_FIELDS)

//GitHeader *newGitHeader(
//  Path pathA, Path pathB,
//  Hash indexA, Hash indexB,
//  FileMode fileMode) {
//  GitHeader *x = malloc(sizeof(GitHeader));
//  x->type = GitHeaderT;
//  x->pathA = pathA;
//  x->pathB = pathB;
//  x->indexA = indexA;
//  x->indexB = indexB;
//  x->fileMode = fileMode;
//  return x;
//}
NEW_INSTANCE(GitHeader, GITHEADER_FIELDS)

//HunkHeader *newHunkHeader(Line lineA, Column columnA, Line lineB, Column columnB) {
//  HunkHeader *x = malloc(sizeof(HunkHeader));
//  x->lineA = lineA;
//  x->lineB = lineB;
//  x->columnA = columnA;
//  x->columnB = columnB;
//  return x;
//}
NEW_INSTANCE(HunkHeader, HUNKHEADER_FIELDS)

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

Diff *match(Element *value) {
  assert(value != NULL);
  Diff *x = malloc(sizeof(Diff));
  x->type = Match;
  x->value = newNodeElement(value);
  return x;
}

Diff *addition(NodeElement *value) {
  assert(value != NULL);
  Diff *x = malloc(sizeof(Diff));
  x->type = Addition;
  x->value = value;
  return x;
}

Diff *removal(NodeElement *value) {
  assert(value != NULL);
  Diff *x = malloc(sizeof(Diff));
  x->type = Removal;
  x->value = value;
  return x;
}

//Hunk *newHunk(HunkHeader *header, NodeDiff *diffs) {
//  Hunk *x = malloc(sizeof(Hunk));
//  x->header = header;
//  x->diffs = diffs;
//  return x;
//}
NEW_INSTANCE(Hunk, HUNK_FIELDS)

//Patch *newPatch(PatchHeader *header, NodeHunk *hunks) {
//  Patch *x = malloc(sizeof(Patch));
//  x->header = header;
//  x->hunks = hunks;
//  return x;
//}
NEW_INSTANCE(Patch, PATCH_FIELDS)

#define SLL_TYPE FileDiffHeader
#define JSON_OBJ FILEDIFFHEADER_FIELDS
#include "json.c"
#undef SLL_TYPE
#undef JSON_OBJ

#define SLL_TYPE GitHeader
#define JSON_OBJ GITHEADER_FIELDS
#include "json.c"
#undef SLL_TYPE
#undef JSON_OBJ

#define SLL_TYPE PatchHeader
#define JSON_UNION PATCHHEADER_FIELDS
#include "json.c"
#undef SLL_TYPE
#undef JSON_UNION

#define SLL_TYPE HunkHeader
#define JSON_OBJ HUNKHEADER_FIELDS
#include "json.c"
#undef SLL_TYPE
#undef JSON_OBJ

#define SLL_TYPE Element
#include "json.c"
#undef SLL_TYPE

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

#define SLL_TYPE Diff
#define JSON_OBJ DIFF_FIELDS
#include "json.c"
#undef SLL_TYPE
#undef JSON_OBJ

#define SLL_TYPE Hunk
#define JSON_OBJ HUNK_FIELDS
#include "json.c"
#undef SLL_TYPE
#undef JSON_OBJ

#define SLL_TYPE Patch
#define JSON_OBJ PATCH_FIELDS
#include "json.c"
#undef SLL_TYPE
#undef JSON_OBJ
