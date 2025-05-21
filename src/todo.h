#ifndef TODO_H
#define TODO_H

#ifdef DEVELOPMENT

#include <stdio.h>
#include <stdlib.h>

#include "error.h"

#define TODO(...) TODO_STUB((exit(Todo), NULL), __VA_ARGS__)

#define TODO_DEFER(...) TODO_STUB(NULL, __VA_ARGS__)

#define TODO_STUB(assumedValue, ...) \
  ( \
    fprintf(stderr, "TODO: " __VA_ARGS__), \
    fprintf(stderr, ". Function: %s, File: %s, Line: %d\n", __func__, __FILE__, __LINE__), \
    assumedValue \
  )

#endif /* DEVELOPMENT */

#endif
