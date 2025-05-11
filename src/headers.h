#ifndef HEADERS_H
#define HEADERS_H

#include <limits.h>

/* Git Patch Header */

#ifndef INDEX_MAX
#define INDEX_MAX 256
#endif
struct GitHeader {
  char pathA[PATH_MAX],
       pathB[PATH_MAX],
       indexA[INDEX_MAX],
       indexB[INDEX_MAX];
  int mode; /* should check to ensure enough bits to hold mode */
};

#define FORMAT_GIT_HEADER(gh, linePrefix, ...) \
  "diff --git a/%s b/%s\n%sindex %s..%s %d\n", (gh).pathA, (gh).pathB, linePrefix, (gh).indexA, (gh).indexB, (gh).mode

/* Diff Patch Header */

struct DiffHeader {
  char pathMinus[PATH_MAX],
       pathPlus[PATH_MAX];
};

#define FORMAT_DIFF_HEADER(dh, linePrefix, ...) \
  "--- a/%s\n%s+++ b/%s\n", (dh).pathMinus, linePrefix, (dh).pathPlus

/* Hunk Patch Header */

struct StartEnd {
  int start,
      end;
};

struct HunkHeader {
  struct StartEnd minus,
                  plus;
};

#define FORMAT_HUNK_HEADER(h) \
  "@@ -%d,%d +%d,%d @@\n", (h).minus.start, (h).minus.end, (h).plus.start, (h).plus.end

#endif

