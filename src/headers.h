#ifndef GITHEADER_H
#define GITHEADER_H

#include <limits.h>

/* Git Patch Header */

#ifndef INDEX_MAX
#define INDEX_MAX 256
#endif
typedef struct {
  char pathA[PATH_MAX],
       pathB[PATH_MAX],
       indexA[INDEX_MAX],
       indexB[INDEX_MAX];
  int mode; /* should check to ensure enough bits to hold mode */
} GitHeader;

#define FORMAT_GIT_HEADER(gh, ...) \
  "diff --git a/%s b/%s\nindex %s..%s %d\n", (gh).pathA, (gh).pathB, (gh).indexA, (gh).indexB, (gh).mode

#define PARSE_GIT_HEADER \
  PARSE_LINE(ParseFail_GitHeader, 2, "a/%s b/%s\n", "Expected format: `diff --git a/<pathA> b/<pathB>\\n`", gh.pathA, gh.pathB); \
  PARSE_LINE(ParseFail_GitHeader_Index, 3, "index %[0-9a-f]..%[0-9a-f] %d\n", "Expected format: `index <indexA>..<indexB> <mode>\\n`", gh.indexA, gh.indexB, &gh.mode)

/* Diff Patch Header */

typedef struct {
  char pathMinus[PATH_MAX],
       pathPlus[PATH_MAX];
} DiffHeader;

#define FORMAT_DIFF_HEADER(dh, ...) \
  "--- a/%s\n+++ b/%s\n", (dh).pathMinus, (dh).pathPlus

#define PARSE_DIFF_HEADER \
  PARSE_LINE(ParseFail_DiffHeader_PathMinus, 1, "--- a/%s\n", "Expected format: `--- a/<path>\\n`", dh.pathMinus); \
  PARSE_LINE(ParseFail_DiffHeader_PathPlus, 1, "+++ b/%s\n", "Expected format: `--- b/<path>\\n`", dh.pathPlus)

/* Hunk Patch Header */

typedef struct {
  int start,
      end;
} StartEnd;

typedef struct {
  StartEnd minus,
           plus;
} HunkHeader;

#define FORMAT_HUNK_HEADER(h) \
  "@@ -%d,%d +%d,%d @@\n", (h).minus.start, (h).minus.end, (h).plus.start, (h).plus.end

#define PARSE_HUNK_HEADER \
  PARSE_LINE(ParseFail_HunkHeader, 4, "-%d,%d +%d,%d @@\n", "Hunk Format: `@@ -<old line start>,<old line end> +<new line start>,<new line end> @@\\n", &hh.minus.start, &hh.minus.end, &hh.plus.start, &hh.plus.end)

#endif

