#ifndef QUEUE_TYPE
#error "QUEUE_TYPE must be defined before the import of queue.h"
#endif

#ifdef QUEUE_INCLUDE
#include QUEUE_INCLUDE
#undef QUEUE_INCLUDE
#endif


#ifndef QUEUE_SUFFIX
#define QUEUE_SUFFIX QUEUE_TYPE
#endif

#define MAKE_NAME_HELPER(x, y) x ## y
#define MAKE_NAME(x, y) MAKE_NAME_HELPER(x, y)

#define NODE MAKE_NAME(Node, QUEUE_SUFFIX)
typedef struct NODE {
  struct NODE *next;
  QUEUE_TYPE *x;
} NODE;

#define QUEUE MAKE_NAME(Queue, QUEUE_SUFFIX)
typedef struct QUEUE {
  NODE *head;
  NODE *tail;
} QUEUE;

QUEUE *MAKE_NAME(new, QUEUE) ();

QUEUE *MAKE_NAME(newOne, QUEUE)(QUEUE_TYPE *x);

void MAKE_NAME(push, QUEUE)(QUEUE *q, QUEUE_TYPE *x);

QUEUE_TYPE *MAKE_NAME(pop, QUEUE)(QUEUE *q);

