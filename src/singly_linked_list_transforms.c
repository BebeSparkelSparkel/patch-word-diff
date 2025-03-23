#include "singly_linked_list.h"
#include "singly_linked_list_transformers.h"

void *MAKE_NAME(sllMap, SLL_TYPE) (SLL_NODE *h, void *(*f)(SLL_TYPE *)) {
  SLL_NODE *n = h;
  void *y;
  while (n != NULL) {
    if ((y = f(n->x)) == NULL) return NULL;
    n->x = y;
    n = n->next;
  }
  return h;
}

void *MAKE_NAME(sllFoldr, SLL_TYPE) (SLL_NODE *n, void *x, void *(*f)(SLL_TYPE *, void *)) {
  while (n != NULL && x != NULL) {
    x = f(n->x, x);
    n = n->next;
  }
  return x;
}

