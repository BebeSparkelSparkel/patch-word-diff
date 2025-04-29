#ifndef TODO_H
#define TODO_H

#ifdef DEVELOPMENT
#define TODO(msg) \
      (fprintf(stderr, "TODO: %s. Function: %s, File: %s, Line: %d\n", msg, __func__, __FILE__, __LINE__), \
      exit(1))
#endif /* DEVELOPMENT */

#endif
