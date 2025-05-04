#ifndef TMPFILE_H
#define TMPFILE_H

#include "error.h"

ErrorId tmpFile(FILE * CP tmp, FP(char) srcPath, char CP tmpPath, FP(char) ext);

#endif
