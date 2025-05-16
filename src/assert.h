#ifndef ASSERT_H
#define ASSERT_H

#ifdef assert
#error "assert already defined remove standard #include <assert.h>"
#endif

#ifdef NDEBUG
  #define assert(_) ((void)0)
#else
  #include <stdlib.h>
  #include <stdio.h>
  #include "error.h"
  #define assert(condition) \
    do { \
      if (!(condition)) { \
        fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", #condition, __FILE__, __LINE__); \
        exit(AssertFalse); \
      } \
    } while (0)

  #define assert_trace(condition) \
    ERROR_CONDITION(AssertFalse, condition, errorArg.condition = #condition)
#endif

#endif
