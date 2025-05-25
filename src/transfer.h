#ifndef TRANSFER_H
#define TRANSFER_H

#include "error.h"

extern char parseBuf[];

/* not all transfer functions set this */
//extern int lastWrittenChar;

enum ErrorId advanceToLineCopy(struct MFile CP from, FILE CP to, FP(char) toPath, const int targetLine);

enum ErrorId matchAndCopy(struct MFile CP src, struct MFile CP patch, FILE CP to, FP(char) toPath, int (*charExclude)(int c));

enum ErrorId matchAndDiscardUntilClose(struct MFile CP src, struct MFile CP patch);

enum ErrorId copyUntilClose(struct MFile CP patch, FILE CP to, FP(char) toPath);

enum ErrorId copyWhiteSpace(struct MFile CP from, FILE CP to, FP(char) toPath);

enum ErrorId copyRest(struct MFile CP from, FILE CP to, FP(char) toPath);

#endif
