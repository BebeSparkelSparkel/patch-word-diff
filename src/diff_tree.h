#ifndef DIFF_TREE_H
#define DIFF_TREE_H

#define FIELD(type, name) type name;
#define FIELD_PTR(type, name) type *name;
#define FIELD_SLL(type, name) Node##type *name;
#define FIELD_UNION(type, enum, name) type name;

#include <stdint.h>
#include "string_builder.h"
#include "element.h"

typedef enum { GitHeaderT, FileDiffHeaderT } PatchHeaderType;

typedef char* Path;

#define FILEDIFFHEADER_FIELDS \
  FIELD(Path, pathA) \
  FIELD(Path, pathB)
typedef struct {
  PatchHeaderType type;
  FILEDIFFHEADER_FIELDS
} FileDiffHeader;

FileDiffHeader *newFileDiff(Path pathA, Path pathB);

#define SLL_TYPE FileDiffHeader
#include "json.h"
#undef SLL_TYPE

typedef char *Hash;
typedef uint16_t FileMode;

#define GITHEADER_FIELDS \
  FIELD(Path, pathA) \
  FIELD(Path, pathB) \
  FIELD(Hash, indexA) \
  FIELD(Hash, indexB) \
  FIELD(FileMode, fileMode) \
  FIELD_PTR(FileDiffHeader, fileDiff)
typedef struct {
  PatchHeaderType type;
  GITHEADER_FIELDS
} GitHeader;

GitHeader *newGit( Path pathA, Path pathB, Hash indexA, Hash indexB, FileMode fileMode);

#define SLL_TYPE GitHeader
#include "json.h"
#undef SLL_TYPE

#define PATCHHEADER_FIELDS \
  FIELD_UNION(GitHeader, GitHeaderT, git) \
  FIELD_UNION(FileDiffHeader, FileDiffHeaderT, fileDiff)
typedef union {
  PatchHeaderType type;
  PATCHHEADER_FIELDS
} PatchHeader;

#define SLL_TYPE PatchHeader
#include "json.h"
#undef SLL_TYPE

typedef int Line;
typedef int Column;

#define HUNKHEADER_FIELDS \
  FIELD(Line, lineA) \
  FIELD(Line, lineB) \
  FIELD(Column, columnA) \
  FIELD(Column, columnB)
typedef struct {
  HUNKHEADER_FIELDS
} HunkHeader;

HunkHeader *newHunkHeader(Line lineA, Column columnA, Line lineB, Column columnB);

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

#define DIFF_FIELDS \
  FIELD_SLL(Element, value)
typedef struct {
  DiffType type;
  DIFF_FIELDS
} Diff;

Diff *match(Element *);

Diff *addition(NodeElement *);

Diff *removal(NodeElement *);

#define SLL_TYPE Diff
#include "json.h"
#undef SLL_TYPE

#define HUNK_FIELDS \
  FIELD_PTR(HunkHeader, header) \
  FIELD_SLL(Diff, diffs)
typedef struct Hunk {
  HUNK_FIELDS
} Hunk;

Hunk *newHunk(HunkHeader *, NodeDiff *);

#define SLL_TYPE Hunk
#include "json.h"
#undef SLL_TYPE

#define PATCH_FIELDS \
  FIELD_PTR(PatchHeader, header) \
  FIELD_SLL(Hunk, hunks)
typedef struct {
//  PatchHeader *header;
//  NodeHunk *hunks;
  PATCH_FIELDS
} Patch;

Patch *newPatch(PatchHeader *, NodeHunk *);

typedef struct NodePatch NodePatch;

#define SLL_TYPE Patch
#include "json.h"
#undef SLL_TYPE

#endif
