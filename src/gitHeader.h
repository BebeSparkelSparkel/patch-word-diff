#ifndef GITHEADER_H
#define GITHEADER_H

#ifndef INDEX_MAX
#define INDEX_MAX 256
#endif
typedef struct {
  char pathA[PATH_MAX],
       pathB[PATH_MAX];
  char indexA[INDEX_MAX],
       indexB[INDEX_MAX];
  uint32_t mode;
} GitHeader;

#define FORMAT_GIT_HEADER(gh, ...) \
  "diff --git a/%s b/%s\nindex %s..%s %d\n", (gh).pathA, (gh).pathB, (gh).indexA, (gh).indexB, (gh).mode

typedef struct {
  char pathMinus[PATH_MAX],
       pathPlus[PATH_MAX];
} DiffHeader;

#define FORMAT_DIFF_HEADER(dh, ...) \
  "--- a/%s\n+++ b/%s\n", (dh).pathMinus, (dh).pathPlus

#endif
