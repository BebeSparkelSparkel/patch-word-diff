#ifndef TMPFILE_H
#define TMPFILE_H

#include "error.h"

enum ErrorId tmpFile(FILE * CP tmp, FP(char) srcPath, char CP tmpPath, FP(char) ext);

#endif
