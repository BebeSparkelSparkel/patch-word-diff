#ifndef DIFF_TREE_H
#define DIFF_TREE_H

#include <stdint.h>

typedef enum { Git, FileDiff } HeaderType;

typedef char* Path;

typedef struct {
  HeaderType type;
  Path pathA, pathB;
} FileDiffHeader;

FileDiffHeader *newFileDiff(Path pathA, Path pathB);

typedef char *Hash;
typedef uint16_t FileMode;

typedef struct {
  HeaderType type;
  Path pathA, pathB;
  Hash indexA, indexB;
  FileMode fileMode;
  FileDiffHeader *fileDiff;
} GitHeader;

GitHeader *newGit( Path pathA, Path pathB, Hash indexA, Hash indexB, FileMode fileMode);

typedef union {
  GitHeader *git;
  FileDiffHeader *fileDiff;
} Header;

typedef int Line;
typedef int Column;

typedef struct {
  Line lineA, lineB;
  Column columnA, columnB;
} HunkHeader;

HunkHeader *newHunkHeader(Line lineA, Column columnA, Line lineB, Column columnB);

typedef enum { Newlines, Spaces, OpenSquare, CloseSquare, OpenCurly, CloseCurly, Word } ElementType;

typedef union {
  char *string;
  int count;
} ElementValue;

ElementValue newElementString(char *string);

ElementValue newElementCount(int count);

ElementValue newElementNull();

typedef struct {
  ElementType type;
  ElementValue value;
} Element;

Element *newElement(ElementType type, ElementValue value);

typedef struct QueueElement QueueElement;
// #define QUEUE_TYPE Element
// #include "queue.h"
// #undef QUEUE_TYPE

typedef enum { Match, Addition, Removal } DiffType;
typedef struct {
  DiffType type;
  QueueElement *value;
} Diff;

Diff *match(Element *value);

Diff *addition(QueueElement *value);

Diff *removal(QueueElement *value);

typedef struct QueueDiff QueueDiff;
// #define QUEUE_TYPE Diff
// #include "queue.h"
// #undef QUEUE_TYPE

typedef struct Hunk {
  HunkHeader *header;
  QueueDiff *diffs;
} Hunk;

Hunk *newHunk(HunkHeader *header, QueueDiff *diffs);

typedef struct QueueHunk QueueHunk;
// #define QUEUE_TYPE Hunk
// #include "queue.h"
// #undef QUEUE_TYPE

typedef struct {
  Header header;
  QueueHunk *hunks;
} Patch;

typedef struct QueuePatch QueuePatch;
// #define QUEUE_TYPE Patch
// #include "queue.h"
// #undef QUEUE_TYPE

Patch *newPatch(Header header, QueueHunk *hunks);

#endif
