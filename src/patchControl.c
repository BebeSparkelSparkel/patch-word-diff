#include "assert.h"
#include <ctype.h>

#include "mfile.h"
#include "patchControl.h"

const enum PatchControl minPatchControl = MIN_PATCH_CONTROL;

const char *patchControl2enumStr(enum PatchControl x) {
  switch (x) {
    PATCH_CONTROL_TABLE(END_EXPRESSION_INTER, END_EXPRESSION_INTER, COMPOSE, CASE_RETURN_STRINGIFIED, HEAD);
  }
}

const char *patchControl2commandStr(enum PatchControl x) {
  switch (x) {
    PATCH_CONTROL_TABLE(END_EXPRESSION_INTER, END_EXPRESSION_INTER, COMPOSE, IDENTITY, CASE_RETURN);
  }
}


#define UNGETC \
  { \
    c = mUngetc(c, f); \
    if (EOF == c) \
      return PC_FileError; \
  }

/* Uses a slow matching technique */
enum PatchControl parsePatchControl(struct MFile CP f) {
  int c = mGetCOrEOF(f);
  {
    enum PatchControl r;
    switch (c) {
      case EOF: return PC_EOF;
      case '\n': r = PC_LineFeed; break;
      case '\t': r = PC_Tab;      break;
      case  ' ': r = PC_Space;    break;
      default: goto notWhitespace;
    }
    UNGETC;
    return r;
  }
  notWhitespace:
  {
    const char * commandStrings[] = {PATCH_CONTROL_TABLE(TAIL, COMMA_INTER, THD)};
    int i,
        j = 0,
        viableCount = PATCH_CONTROL_TABLE(TAIL, PLUS_INTER, HEAD, 1);
    enum PatchControl r;
    while (1) {
      for (i = 0; i < PATCH_CONTROL_TABLE(TAIL, PLUS_INTER, HEAD, 1); ++i) {
        if (NULL != commandStrings[i]) {
          if (commandStrings[i][j] == c) {
            if ('\0' == commandStrings[i][j + 1])
              return i + 1;
          } else {
            if (1 == viableCount) {
              r = PC_None;
              goto finish;
            }
            commandStrings[i] = NULL;
            --viableCount;
          }
        }
      }
      c = mGetCOrEOF(f);
      if (EOF == c) {
        r = ferror(f->stream) ? PC_FileError : PC_None;
        goto finishEOF;
      }
      ++j;
    }
    finish:
    UNGETC;
    finishEOF:
    while (0 < j) {
      c = commandStrings[i][--j];
      UNGETC;
    }
    return r;
  }
}

