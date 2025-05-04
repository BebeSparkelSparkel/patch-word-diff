#ifndef TRANSFER_H
#define TRANSFER_H

#include "error.h"

extern char parseBuf[];

enum ErrorId advanceToLineCopy(struct MFile CP from, FILE CP to, const int targetLine);

enum ErrorId matchAndCopy(struct MFile CP src, struct MFile CP patch, FILE CP to);

enum ErrorId matchAndDiscardUntilClose(struct MFile CP src, struct MFile CP patch);

enum ErrorId copyUntilClose(struct MFile CP patch, FILE CP to);

enum ErrorId copyRest(struct MFile CP from, FILE CP to, FP(char) toPath);

#endif
