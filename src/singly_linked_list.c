#include <stdlib.h>
#include "singly_linked_list.h"

SLL_NODE *SLL_NEW_NODE (SLL_TYPE *x) {
  SLL_NODE *n = malloc(sizeof(SLL_NODE));
  n->next = NULL;
  n->x = x;
  return n;
}

void MAKE_NAME(init, SLL_BUILDER) (SLL_BUILDER *b) {
  b->head = NULL;
  b->tail = NULL;
}

void MAKE_NAME(initOne, SLL_BUILDER) (SLL_BUILDER *b, SLL_TYPE *x) {
  SLL_NODE *n = SLL_NEW_NODE(x);
  b->head = n;
  b->tail = n;
}

SLL_BUILDER *MAKE_NAME(new, SLL_BUILDER) () {
  SLL_BUILDER *b = malloc(sizeof(SLL_BUILDER));
  MAKE_NAME(init, SLL_BUILDER) (SLL_BUILDER *b) (b);
  return b;
}

SLL_BUILDER *MAKE_NAME(newOne, SLL_BUILDER) (SLL_TYPE *x) {
  SLL_BUILDER *b = malloc(sizeof(SLL_BUILDER));
  MAKE_NAME(initOne, SLL_BUILDER) (SLL_BUILDER *b) (b, x);
  return b;
}

void MAKE_NAME(sllBuildAppend, SLL_TYPE) (SLL_BUILDER *b, SLL_TYPE *x) {
  SLL_NODE *n = SLL_NEW_NODE(x);
  if (b->head != NULL)
    b->tail->next = n;
  else
    b->head = n;
  b->tail = n;
}

SLL_NODE *MAKE_NAME(sllMaterialize, SLL_TYPE) (SLL_BUILDER *b) {
  SLL_NODE *n = b->head;
  return n;
}

void MAKE_NAME(sllPush, SLL_TYPE) (SLL_NODE **head, SLL_TYPE *x) {
  SLL_NODE *n = SLL_NEW_NODE(x);
  if (*head != NULL)
    n->next = *head;
  *head = n;
}

SLL_TYPE *MAKE_NAME(sllPop, SLL_TYPE) (SLL_NODE **head) {
  SLL_NODE *n = *head;
  if (*head != NULL) {
    SLL_TYPE *x = n->x;
    *head = n->next;
    free(n);
    return x;
  }
  return NULL;
}

int MAKE_NAME(sllForEach, SLL_TYPE) (SLL_NODE *n, int (*f)(SLL_TYPE *)) {
  int y;
  while (n != NULL) {
    if ((y = f(n->x)) != 0) return y;
    n = n->next;
  }
  return 0;
}
