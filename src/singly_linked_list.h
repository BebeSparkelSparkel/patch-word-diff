#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#define SLL_NODE(type) \
  typedef struct SLLNode_##type { \
    struct SLLNode_##type *next; \
    type *x; \
  } SLLNode_##type

#define NEW_NODE_FORWARD(type) \
  SLLNode_##type *sslNewNode_##type (type *x)

#define NEW_NODE(type) \
  NEW_NODE_FORWARD(type) { \
    SLLNode_##type *n = malloc(sizeof(SLLNode_##type)); \
    n->next = NULL; \
    n->x = x; \
    return n; \
  }

#define SLL_BUILDER(type) \
  typedef struct { \
    SLLNode_##type *head; \
    SLLNode_##type *tail; \
  } SLLBuilder_##type

#define SLL_INIT_BUILDER_FORWARD(type) \
  void sllInitBuilder_##type (SLLBuilder_##type *b)

#define SLL_INIT_BUILDER(type) \
  SLL_INIT_BUILDER_FORWARD(type) { \
    b->head = NULL; \
    b->tail = NULL; \
  }

#define SLL_INIT_ONE_BUILDER_FORWARD(type) \
  void sllBuilderInitOne__##type (SLLBuilder_##type *b, type *x)

#define SLL_INIT_ONE_BUILDER(type) \
  SLL_INIT_ONE_BUILDER_FORWARD(type) { \
    SLLNode_##type *n = sslNewNode_##type(x); \
    b->head = n; \
    b->tail = n; \
  }

#define SLL_APPEND_FORWARD(type) \
  void sllBuildAppend_##type (SLLBuilder_##type *b, type *x)

#define SLL_APPEND(type) \
  SLL_APPEND_FORWARD(type) { \
    SLLNode_##type *n = sslNewNode_##type(x); \
    if (b->head != NULL) \
      b->tail->next = n; \
    else \
      b->head = n; \
    b->tail = n; \
  }

#define SLL_MATERIALIZE_FORWARD(type) \
  SLLNode_##type *sllMaterialize_##type (SLLBuilder_##type *b)

#define SLL_MATERIALIZE(type) \
  SLL_MATERIALIZE_FORWARD(type) { \
    SLLNode_##type *n = b->head; \
    sllInitBuilder_##type (b); \
    return n; \
  }

#define SLL_PUSH_FORWARD(type) \
  void sllPush_##type (SLLNode_##type **head, type *x)

#define SLL_PUSH(type) \
  SLL_PUSH_FORWARD(type) { \
    SLLNode_##type *n = sslNewNode_##type(x); \
    if (*head != NULL) \
      n->next = *head; \
    *head = n; \
  }

#define SLL_POP_FORWARD(type) \
  type *sllPop_##type (SLLNode_##type **head)

#define SLL_POP(type) \
  SLL_POP_FORWARD(type) { \
    SLLNode_##type *n = *head; \
    if (*head != NULL) { \
      type *x = n->x; \
      *head = n->next; \
      free(n); \
      return x; \
    } \
    return NULL; \
  }

#define SLL_H(type) \
  SLL_NODE(type); \
  SLL_BUILDER(type); \
  NEW_NODE_FORWARD(type); \
  SLL_INIT_BUILDER_FORWARD(type); \
  SLL_INIT_ONE_BUILDER_FORWARD(type); \
  SLL_APPEND_FORWARD(type); \
  SLL_MATERIALIZE_FORWARD(type); \
  SLL_PUSH_FORWARD(type); \
  SLL_POP_FORWARD(type);

#define SLL_C(type) \
  NEW_NODE(type) \
  SLL_INIT_BUILDER(type) \
  SLL_INIT_ONE_BUILDER(type) \
  SLL_APPEND(type) \
  SLL_MATERIALIZE(type) \
  SLL_PUSH(type) \
  SLL_POP(type)

/* ------------ transforms ------------ */

//#define SLL_MAP_FORWARD(from, to) \
//  SLLNode_##to *sllMap_##from##2##to (SLLNode_##from *h, to *(*f)(from *))
//
//#define SLL_MAP(from, to) \
//  SLL_MAP_FORWARD(from, to) { \
//    void *n = h; \
//    void *y; \
//    while (n != NULL) { \
//      if ((y = f(n->x)) == NULL) return NULL; \
//      n->x = y; \
//      n = n->next; \
//    } \
//    return h; \
//  }

#define SLL_FOLDL_FORWARD(from, accumulator) \
  accumulator *sllFoldl_##from##2##accumulator (SLLNode_##from *n, accumulator *x, accumulator *(*f)(from *, accumulator *))

#define SLL_FOLDL(from, accumulator) \
  SLL_FOLDL_FORWARD(from, accumulator) { \
    while (n != NULL && x != NULL) { \
      x = f(n->x, x); \
      n = n->next; \
    } \
    return x; \
  }

#define SLL_TRANS_H(from, to) \
  SLL_FOLDL_FORWARD(from, to);

#define SLL_TRANS_C(from, to) \
  SLL_FOLDL(from, to)

#endif
