#ifndef DIFF_TREE_H
#define DIFF_TREE_H

#include <stdint.h>
#include "string_builder.h"
#include "element.h"
#include "new.h"

typedef enum { FileDiffHeaderT, GitHeaderT } PatchHeaderType;

typedef char Path;

#define SLL_TYPE Path
#include "json.h"
#undef SLL_TYPE

#define FILEDIFFHEADER_FIELDS(fieldType, fieldHeader, field, fieldPtr, fieldList, fieldUnion, infix) \
  fieldType(PatchHeaderType, FileDiffHeaderT, infix) \
  fieldPtr(Path, pathA, infix) \
  fieldPtr(Path, pathB, EMPTY)

TYPEDEF(FileDiffHeader, struct, FILEDIFFHEADER_FIELDS)

NEW_FORWARD(FileDiffHeader, FILEDIFFHEADER_FIELDS);

#define SLL_TYPE FileDiffHeader
#include "json.h"
#undef SLL_TYPE

typedef char Hash;

#define SLL_TYPE Hash
#include "json.h"
#undef SLL_TYPE

typedef uint16_t FileMode;

#define GITHEADER_FIELDS(fieldType, fieldHeader, field, fieldPtr, fieldList, fieldUnion, infix) \
  fieldType(PatchHeaderType, GitHeaderT, infix) \
  fieldPtr(Path, pathA, infix) \
  fieldPtr(Path, pathB, infix) \
  fieldPtr(Hash, indexA, infix) \
  fieldPtr(Hash, indexB, infix) \
  field(FileMode, fileMode, infix) \
  fieldPtr(FileDiffHeader, fileDiff, EMPTY)

TYPEDEF(GitHeader, struct, GITHEADER_FIELDS)

NEW_FORWARD(GitHeader, GITHEADER_FIELDS);

#define SLL_TYPE GitHeader
#include "json.h"
#undef SLL_TYPE

#define PATCHHEADER_FIELDS(fieldType, fieldHeader, field, fieldPtr, fieldList, fieldUnion, infix) \
  fieldHeader(PatchHeaderType, infix) \
  fieldUnion(GitHeader, GitHeaderT, git, infix) \
  fieldUnion(FileDiffHeader, FileDiffHeaderT, fileDiff, EMPTY)

TYPEDEF(PatchHeader, union, PATCHHEADER_FIELDS)

#define SLL_TYPE PatchHeader
#include "json.h"
#undef SLL_TYPE

typedef int Line;
typedef int Column;

#define HUNKHEADER_FIELDS(fieldType, fieldHeader, field, fieldPtr, fieldList, fieldUnion, infix) \
  field(Line, lineA, infix) \
  field(Line, lineB, infix) \
  field(Column, columnA, infix) \
  field(Column, columnB, EMPTY)

TYPEDEF(HunkHeader, struct, HUNKHEADER_FIELDS)

NEW_FORWARD(HunkHeader, HUNKHEADER_FIELDS);

#define SLL_TYPE HunkHeader
#include "json.h"
#undef SLL_TYPE

typedef enum { ELEMENT_ENUM } ElementType;

typedef union {
  String *string;
  int count;
} ElementValue;

typedef struct {
  ElementType type;
  ElementValue value;
} Element;

//Element  *newElementString(ElementType, char *);

Element  *newElementCount(ElementType, int);

Element  *newElement(ElementType);

#define SLL_TYPE Element
#include "json.h"
#undef SLL_TYPE

typedef enum { Match, Addition, Removal } DiffType;

#define DIFF_FIELDS(fieldType, fieldHeader, field, fieldPtr, fieldList, fieldUnion, infix) \
  field(DiffType, type, infix) \
  fieldList(Element, value, EMPTY)

TYPEDEF(Diff, struct, DIFF_FIELDS);

Diff *match(Element *);

Diff *addition(NodeElement *);

Diff *removal(NodeElement *);

#define SLL_TYPE Diff
#include "json.h"
#undef SLL_TYPE

#define HUNK_FIELDS(fieldType, fieldHeader, field, fieldPtr, fieldList, fieldUnion, infix) \
  fieldPtr(HunkHeader, header, infix) \
  fieldList(Diff, diffs, EMPTY)

TYPEDEF(Hunk, struct, HUNK_FIELDS)

NEW_FORWARD(Hunk, HUNK_FIELDS);

#define SLL_TYPE Hunk
#include "json.h"
#undef SLL_TYPE

#define PATCH_FIELDS(fieldType, fieldHeader, field, fieldPtr, fieldList, fieldUnion, infix) \
  fieldPtr(PatchHeader, header, infix) \
  fieldList(Hunk, hunks, EMPTY)

TYPEDEF(Patch, struct, PATCH_FIELDS)

NEW_FORWARD(Patch, PATCH_FIELDS);

#define SLL_TYPE Patch
#include "json.h"
#undef SLL_TYPE

#endif
