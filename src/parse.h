#ifndef PARSE_H
#define PARSE_H

#include "mfile.h"

#define PARSE_GIT_HEADER \
  PARSE_LINE(ParseFail_GitHeader, 2, "a/%s b/%s\n", "Expected format: `diff --git a/<pathA> b/<pathB>\\n`", gh.pathA, gh.pathB); \
  PARSE_LINE(ParseFail_GitHeader_Index, 3, "index %[0-9a-f]..%[0-9a-f] %d\n", "Expected format: `index <indexA>..<indexB> <mode>\\n`", gh.indexA, gh.indexB, &gh.mode)

#define PARSE_DIFF_HEADER \
  PARSE_LINE(ParseFail_DiffHeader_PathMinus, 1, "--- a/%s\n", "Expected format: `--- a/<path>\\n`", dh.pathMinus); \
  PARSE_LINE(ParseFail_DiffHeader_PathPlus, 1, "+++ b/%s\n", "Expected format: `+++ b/<path>\\n`", dh.pathPlus)

enum ErrorId parseHunkHeader(struct MFile CP patch, struct HunkHeader CP hh);

#define PARSE_BUF_SIZE BUFSIZ + 2 * MAX(PATH_MAX, INDEX_MAX)
extern char parseBuf[PARSE_BUF_SIZE];

#define PARSE_LINE(errorId, numberOfParsedParameters, formatString, errorMessage, ...) \
  LOAD_PARSE_LINE(errorId); \
  i = sscanf(parseBuf, formatString, __VA_ARGS__); \
  ERROR_CONDITION(errorId, numberOfParsedParameters > i, PLM(patch, errorMessage))

#define LOAD_PARSE_LINE(errorId) \
  parseBuf[PARSE_BUF_SIZE - 2] = '\0'; \
  s = mGets(parseBuf, PARSE_BUF_SIZE, patch); \
  ERROR_CONDITION(errorId, NULL == s, PLM(patch, "Failed to get line when parsing.")); \
  ERROR_CONDITION(ParseFail_BufferOverflow, '\0' != s[PARSE_BUF_SIZE - 2], errorArg.pathLine = ((struct PathLine){ patch->path, patch->line - 1 }))

#define PLM(f, msg) \
  errorArg.pathLineMsg = ((struct PathLineMsg){ f->path, f->line - 1, msg, s })

#endif
