#ifndef TRANSFER_H
#define TRANSFER_H

#include "error.h"

extern char parseBuf[];

ErrorId advanceToLineCopy(MFile CP from, FILE CP to, const int targetLine);

ErrorId matchAndCopy(MFile CP src, MFile CP patch, FILE CP to);

ErrorId matchAndDiscardUntilClose(MFile CP src, MFile CP patch);

ErrorId copyUntilClose(MFile CP patch, FILE CP to);

ErrorId copyRest(MFile CP from, FILE CP to);

#endif
