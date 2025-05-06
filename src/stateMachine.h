#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "error.h"

enum ErrorId stateMachine(struct MFile CP patch, struct MFile CP src, FILE * CP tmp, char *tmpPath);

#endif
