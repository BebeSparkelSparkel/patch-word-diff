#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "mfile.h"
#include "error.h"
#include "todo.h"

#ifndef TRACE_SIZE
#define TRACE_SIZE 3
#endif

static ErrorOrigin errorOrigin[TRACE_SIZE];
static int count = -1;

#define CASE_ERROR(_, e, __, action) \
  case e: action; break;

void error(ErrorId e, MFile CP src, MFile CP patch, FILE CP tmp) {
  switch (e) {
    ERROR_TABLE(CAT, CASE_ERROR);
    default:
      fprintf(stderr, "ERROR: Unknown error code: %d\n", e);
      e = UnknownError;
      break;
  }
  if (src && src->stream != NULL) fclose(src->stream);
  if (patch && patch->stream != NULL) fclose(patch->stream);
  if (tmp != NULL) fclose(tmp);
  exit(e);
}

void pushErrorOrigin(FP(char) file, FP(char) func, const int line) {
  errorOrigin[++count % TRACE_SIZE] = (ErrorOrigin){ __FILE__, __func__, __LINE__ };
}

const ErrorOrigin *popErrorOrigin(void) {
  static int end = -1;
  assert(count >= 0);
  if (end == -1)
    end = MAX(0, count - TRACE_SIZE + 1);
  if (count < end)
    return NULL;
  return &errorOrigin[count-- % TRACE_SIZE];
}
