#include <assert.h>
#include <ctype.h>

#include "mfile.h"
#include "patchControl.h"

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

enum PatchControl parsePatchControl(struct MFile CP f) {
  static const int numPrefixes = PATCH_CONTROL_TABLE(TAIL, PLUS_INTER, HEAD, 1);
  int notNullCount = numPrefixes,
      i,
      c;
  const char *ps[] = { PATCH_CONTROL_TABLE(TAIL, COMMA_INTER, THIRD, ) };
  ASSERT_MFILE_EOF(f);
  do
    c = mGetCOrEOF(f);
  while (isspace(c));
  do {
    if (EOF == c)
      return ferror(f->stream) ? PC_FileError : PC_EOF;
    for(i = 0; i < numPrefixes; ++i) {
      if (NULL != ps[i]) {
        if (c == *ps[i]) {
          ++ps[i];
          if ('\0' == *ps[i])
            return i + 1;
        }
        else {
          --notNullCount;
          ps[i] = NULL;
        }
      }
    }
    c = mGetc(f);
  } while (notNullCount > 0);
  c = mUngetc(c, f);
  return EOF == c ? PC_FileError : PC_None;
}

