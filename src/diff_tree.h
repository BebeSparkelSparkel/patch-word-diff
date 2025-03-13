#ifndef DIFF_TREE_H
#define DIFF_TREE_H

#pragma push_macro("QUEUE_TYPE")

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

struct QueueElement;

typedef enum { Match, Addition, Removal } DiffType;
typedef struct {
  DiffType type;
  struct QueueElement *value;
} Diff;

Diff *match(Element *value);

Diff *addition(struct QueueElement *value);

Diff *removal(struct QueueElement *value);

struct QueueDiff;

typedef struct Hunk {
  HunkHeader *header;
  struct QueueDiff *diffs;
} Hunk;

Hunk *newHunk(HunkHeader *header, struct QueueDiff *diffs);

struct QueueHunk;

typedef struct {
  Header header;
  struct QueueHunk *hunks;
} Patch;

struct QueuePatch;

Patch *newPatch(Header header, struct QueueHunk *hunks);

#pragma pop_macro("QUEUE_TYPE")

#endif
