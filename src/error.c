#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "mfile.h"
#include "error.h"
#include "todo.h"

#ifndef TRACE_SIZE
#define TRACE_SIZE 256
#endif

static struct ErrorOrigin errorOrigin[TRACE_SIZE];
static int count = -1;

static int traceBufferOverflowed(void) {
  return count >= TRACE_SIZE;
}

#define CASE_ERROR(_, e, __, action) \
  case e: action; break;

void error(enum ErrorId e, struct MFile CP src, struct MFile CP patch, FILE CP tmp) {
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

void _pushErrorOrigin(FP(char) file, FP(char) func, const int line) {
  assert(count >= -1 && "_pushErrorOrigin should not be called after popErrorOrigin");
  errorOrigin[++count % TRACE_SIZE] = (struct ErrorOrigin){ file, func, line };
}

const struct ErrorOrigin *popErrorOrigin(void) {
  static int end = -1;
  if (end == -1)
    end = MAX(0, count - TRACE_SIZE + 1);
  if (count < end) {
    count = INT_MIN;  /* Invalidate for future _pushErrorOrigin calls */
    return NULL;
  }
  return &errorOrigin[end++ % TRACE_SIZE];
}
