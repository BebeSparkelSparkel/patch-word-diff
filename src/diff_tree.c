#include "diff_tree.h"
#include <stdlib.h>

FileDiffHeader *newFileDiff(Path pathA, Path pathB) {
  FileDiffHeader *x = malloc(sizeof(FileDiffHeader));
  x->pathA = pathA;
  x->pathB = pathB;
  return x;
}

GitHeader *newGit(
  Path pathA, Path pathB,
  Hash indexA, Hash indexB,
  FileMode fileMode) {
  GitHeader *x = malloc(sizeof(GitHeader));
  x->type = Git;
  x->pathA = pathA;
  x->pathB = pathB;
  x->indexA = indexA;
  x->indexB = indexB;
  x->fileMode = fileMode;
  return x;
}

HunkHeader *newHunkHeader(Line lineA, Column columnA, Line lineB, Column columnB) {
  HunkHeader *x = malloc(sizeof(HunkHeader));
  x->lineA = lineA;
  x->lineB = lineB;
  x->columnA = columnA;
  x->columnB = columnB;
  return x;
}

ElementValue newElementString(char *string) {
  ElementValue x;
  x.string = string;
  return x;
}

ElementValue newElementCount(int count) {
  ElementValue x;
  x.count = count;
  return x;
}

ElementValue newElementNull() {
  ElementValue x;
  x.string = NULL;
  return x;
}

Element *newElement(ElementType type, ElementValue value) {
  Element *x = malloc(sizeof(Element));
  x->type = type;
  x->value = value;
  return x;
}

Diff *match(Element *value) {
  Diff *x = malloc(sizeof(Diff));
  x->type = Match;
  x->value = newOneQueueElement(value);
  return x;
}

Diff *addition(QueueElement *value) {
  Diff *x = malloc(sizeof(Diff));
  x->type = Addition;
  x->value = value;
  return x;
}

Diff *removal(QueueElement *value) {
  Diff *x = malloc(sizeof(Diff));
  x->type = Removal;
  x->value = value;
  return x;
}

Hunk *newHunk(HunkHeader *header, QueueDiff *diffs) {
  Hunk *x = malloc(sizeof(Hunk));
  x->header = header;
  x->diffs = diffs;
  return x;
}

Patch *newPatch(Header header, QueueHunk *hunks) {
  Patch *x = malloc(sizeof(Patch));
  x->header = header;
  x->hunks = hunks;
  return x;
}

