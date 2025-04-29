#include <assert.h>

#include "mfile.h"
#include "patchControl.h"

const char *patchControl2enumStr(PatchControl x) {
  switch (x) {
    PATCH_CONTROL_TABLE(END_EXPRESSION_INTER, END_EXPRESSION_INTER, COMPOSE, CASE_RETURN_STRINGIFIED, HEAD);
  }
}

const char *patchControl2commandStr(PatchControl x) {
  switch (x) {
    PATCH_CONTROL_TABLE(END_EXPRESSION_INTER, END_EXPRESSION_INTER, COMPOSE, IDENTITY, CASE_RETURN);
  }
}

PatchControl parsePatchControl(MFile CP f) {
  static const int numPrefixes = PATCH_CONTROL_TABLE(TAIL, PLUS_INTER, HEAD, 1);
  int notNullCount = numPrefixes,
      i,
      c;
  const char *ps[] = { PATCH_CONTROL_TABLE(TAIL, COMMA_INTER, THIRD, ) };
  ASSERT_MFILE(f);
  while (notNullCount > 0) {
    c = mGetc(f);
    if (EOF == c) return PC_EOF;
    for(i = 0; i < numPrefixes; ++i) {
      if (ps[i] != NULL) {
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
  }
  c = mUngetc(c, f);
  if (EOF == c)
    return PC_EOF;
  return PC_None;
}

