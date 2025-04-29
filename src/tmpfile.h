#ifndef TMPFILE_H
#define TMPFILE_H

#include "error.h"

/** srcMutable string is mutated and set to a zero length string */
ErrorId tmpFile(FILE **tmp, char CP srcMutable, char CP tmpPath, size_t tmpSize);

#endif
