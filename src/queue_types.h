#ifndef QUEUE_TYPEs_H
#define QUEUE_TYPEs_H

#include "diff_tree.h"

#define QUEUE_TYPE Element
#include "queue.h"

#define QUEUE_TYPE Diff
#include "queue.h"

#define QUEUE_TYPE Hunk
#include "queue.h"

#define QUEUE_TYPE Patch
#include "queue.h"

#endif
