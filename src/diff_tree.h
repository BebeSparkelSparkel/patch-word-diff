#ifndef DIFF_TREE_H
#define DIFF_TREE_H

#include <stdint.h>
#include "new.h"
#include "singly_linked_list.h"
#include "string_builder.h"
#include "json.h"
#include "element.h"

typedef enum { FileDiffHeaderT, GitHeaderT } PatchHeaderType;

typedef char Path;

TO_JSON_FORWARD(Path);

#define FILEDIFFHEADER_FIELDS(fieldType, field, fieldPtr, fieldList, fieldUnion, infix, end) \
  fieldType(PatchHeaderType, FileDiffHeaderT, infix) \
  fieldPtr(Path, pathA, infix) \
  fieldPtr(Path, pathB, end)

TYPEDEF(FileDiffHeader, struct, FILEDIFFHEADER_FIELDS);

NEW_FORWARD(FileDiffHeader, FILEDIFFHEADER_FIELDS);

TO_JSON_FORWARD(FileDiffHeader);

typedef char Hash;

TO_JSON_FORWARD(Hash);

typedef uint16_t FileMode;

TO_JSON_FORWARD(FileMode);

#define GITHEADER_FIELDS(fieldType, field, fieldPtr, fieldList, fieldUnion, infix, end) \
  fieldType(PatchHeaderType, GitHeaderT, infix) \
  fieldPtr(Path, pathA, infix) \
  fieldPtr(Path, pathB, infix) \
  fieldPtr(Hash, indexA, infix) \
  fieldPtr(Hash, indexB, infix) \
  field(FileMode, fileMode, , infix) \
  fieldPtr(FileDiffHeader, fileDiff, end)

TYPEDEF(GitHeader, struct, GITHEADER_FIELDS);

NEW_FORWARD(GitHeader, GITHEADER_FIELDS);

TO_JSON_FORWARD(GitHeader);

#define PATCHHEADER_FIELDS(fieldType, field, fieldPtr, fieldList, fieldUnion, infix, end) \
  fieldType(PatchHeaderType, infix) \
  fieldUnion(GitHeader, GitHeaderT, git, infix) \
  fieldUnion(FileDiffHeader, FileDiffHeaderT, fileDiff, end)

TYPEDEF(PatchHeader, union, PATCHHEADER_FIELDS);

TO_JSON_FORWARD(PatchHeader);

typedef int Line;

TO_JSON_FORWARD(Line);

typedef int Column;

TO_JSON_FORWARD(Column);

#define HUNKHEADER_FIELDS(fieldType, field, fieldPtr, fieldList, fieldUnion, infix, end) \
  field(Line, lineA, , infix) \
  field(Line, lineB, , infix) \
  field(Column, columnA, , infix) \
  field(Column, columnB, , end)

TYPEDEF(HunkHeader, struct, HUNKHEADER_FIELDS);

NEW_FORWARD(HunkHeader, HUNKHEADER_FIELDS);

TO_JSON_FORWARD(HunkHeader);

#define ELEMENTTYPE_ENUM(enumerator, infix, end) ELEMENT_MAP(enumerator, enumerator, enumerator, infix, end)
TYPEDEF_ENUM(ElementType, ELEMENTTYPE_ENUM);

typedef union {
  String *string;
  int count;
} ElementValue;

typedef struct {
  ElementType type;
  ElementValue value;
} Element;

Element *newElementString(ElementType, char *);

Element *newElementCount(ElementType, int);

Element *newElement(ElementType);

SLL_H(Element);

SLL_FOLDL_FORWARD(Element, StringListBuilder);

TO_JSON_FORWARD(Element);

#define DIFFTYPE_ENUM(enumerator, infix, end) \
  enumerator(Match, infix) \
  enumerator(Addition, infix) \
  enumerator(Removal, end)
TYPEDEF_ENUM(DiffType, DIFFTYPE_ENUM);

TO_JSON_FORWARD(DiffType);

#define DIFF_FIELDS(fieldType, field, fieldPtr, fieldList, fieldUnion, infix, end) \
  field(DiffType, type, , infix) \
  fieldList(Element, value, end)

TYPEDEF(Diff, struct, DIFF_FIELDS);

Diff *match(Element *);

Diff *addition(SLLNode_Element *);

Diff *removal(SLLNode_Element *);

SLL_H(Diff);

TO_JSON_LIST_FORWARD(Diff);
TO_JSON_FORWARD(Diff);

#define HUNK_FIELDS(fieldType, field, fieldPtr, fieldList, fieldUnion, infix, end) \
  fieldPtr(HunkHeader, header, infix) \
  fieldList(Diff, diffs, end)

TYPEDEF(Hunk, struct, HUNK_FIELDS);

NEW_FORWARD(Hunk, HUNK_FIELDS);

SLL_H(Hunk);
SLL_FOLDL_FORWARD(Hunk, StringListBuilder);

TO_JSON_LIST_FORWARD(Hunk);
TO_JSON_FORWARD(Hunk);

#define PATCH_FIELDS(fieldType, field, fieldPtr, fieldList, fieldUnion, infix, end) \
  fieldPtr(PatchHeader, header, infix) \
  fieldList(Hunk, hunks, end)

TYPEDEF(Patch, struct, PATCH_FIELDS);

NEW_FORWARD(Patch, PATCH_FIELDS);

SLL_H(Patch);

TO_JSON_LIST_FORWARD(Patch);
TO_JSON_FORWARD(Patch);

#endif
