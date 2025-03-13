#include <stdlib.h>
#include "queue.h"

QUEUE *MAKE_NAME(new, QUEUE) () {
  QUEUE *q = malloc(sizeof(QUEUE));
  q->head = NULL;
  return q;
}

QUEUE *MAKE_NAME(newOne, QUEUE)(QUEUE_TYPE *x) {
  QUEUE *q = malloc(sizeof(QUEUE));
  NODE *n = malloc(sizeof(NODE));
  q->head = q->tail = n;
  return q;
}

void MAKE_NAME(push, QUEUE)(QUEUE *q, QUEUE_TYPE *x) {
  NODE *n = malloc(sizeof(NODE));
  n->x = x;
  if (q->head == NULL)
    q->head = q->tail = n;
  else {
    NODE *t = q->tail;
    t->next = n;
    q->tail = n;
  }
}

QUEUE_TYPE *MAKE_NAME(pop, QUEUE)(QUEUE *q) {
  if (q->head == NULL) return NULL;
  NODE *h = q->head;
  QUEUE_TYPE *x = h->x;
  if (h == q->tail) q->head = NULL;
  else q->head = h->next;
  free(h);
  return x;
}
