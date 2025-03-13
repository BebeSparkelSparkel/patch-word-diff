#define QUEUE_TYPE Element
#include "queue.h"
#undef QUEUE_TYPE

#define QUEUE_TYPE Diff
#include "queue.h"
#undef QUEUE_TYPE

#define QUEUE_TYPE Hunk
#include "queue.h"
#undef QUEUE_TYPE

#define QUEUE_TYPE Patch
#include "queue.h"
#undef QUEUE_TYPE

