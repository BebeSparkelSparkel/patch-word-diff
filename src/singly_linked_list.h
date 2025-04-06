#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#define FIELD_CUSTOM(type, name, ...) SLL_##type *name;
#define INIT_CUSTOM(...) INIT_PTR(__VA_ARGS__)

#define SLL_BOX_FIELDS(type) \
  type *x; \
  bool possessed;

#define SLL_BOX_TYPEDEF(type) \
  typedef struct { \
    SLL_BOX_FIELDS(type) \
  } SLLBox_##type

#define SLL_TYPEDEF(type) \
  typedef struct SLL_##type { \
    SLL_BOX_FIELDS(type) \
    struct SLL_##type *next; \
  } SLL_##type

// #define SLL_NEW_NODE_FORWARD(type) \
//   SLL_##type *sslNewNode_##type (type *x)
// 
// #define SLL_NEW_NODE(type) \
//   SLL_NEW_NODE_FORWARD(type) { \
//     SLL_##type *n = malloc(sizeof(SLL_##type)); \
//     n->next = NULL; \
//     n->x = x; \
//     return n; \
//   }

#define SLLB_TYPEDEF(type) \
  typedef struct { \
    SLL_##type *head; \
    SLL_##type *tail; \
  } SLLB_##type

#define SLLB_INIT_FORWARD(type) \
  void sllInitBuilder_##type (SLLB_##type *b)

#define SLLB_INIT(type) \
  SLLB_INIT_FORWARD(type) { \
    b->head = NULL; \
    b->tail = NULL; \
  }

//#define SLL_INIT_ONE_BUILDER_FORWARD(type) \
//  void sllBuilderInitOne_##type (SLLB_##type *b, type *x)
//
//#define SLL_INIT_ONE_BUILDER(type) \
//  SLL_INIT_ONE_BUILDER_FORWARD(type) { \
//    SLL_##type *n = sslNewNode_##type(x); \
//    b->head = n; \
//    b->tail = n; \
//  }
//
//#define SLL_NEW_ONE_BUILDER_FORWARD(type) \
//  SLLB_##type *sllBuilderNewOne_##type (type *x)
//
//#define SLL_NEW_ONE_BUILDER(type) \
//  SLL_NEW_ONE_BUILDER_FORWARD(type) { \
//    SLLB_##type *b = malloc(sizeof(SLLB_##type)); \
//    sllBuilderInitOne_##type (b, x); \
//    return b; \
//  }

#define SLLB_APPEND_POSSESSED_FORWARD(type) \
  void sllbAppendPossessed_##type(SLLB_##type *b, type *x)

#define SLLB_APPEND_POSSESSED(type) \
  SLLB_APPEND_POSSESSED_FORWARD(type) { sllbAppend_##type(b, x, true); }

#define SLLB_APPEND_WILD_FORWARD(type) \
  void sllbAppendWild_##type(SLLB_##type *b, type *x)

#define SLLB_APPEND_WILD(type) \
  SLLB_APPEND_WILD_FORWARD(type) { sllbAppend_##type(b, x, false); }

#define SLLB_APPEND(type) \
  void sllbAppend_##type(SLLB_##type *b, type *x, bool possessed) { \
    SLL_##type *n = malloc(sizeof(SLL_##type)); \
    n->x = x; \
    n->possessed = possessed; \
    n->next = NULL; \
    if (b->head != NULL) \
      b->tail->next = n; \
    else \
      b->head = n; \
    b->tail = n; \
  }

#define SLLB_MATERIALIZE_FORWARD(type) \
  SLL_##type *sllMaterialize_##type (SLLB_##type *b)

#define SLLB_MATERIALIZE(type) \
  SLLB_MATERIALIZE_FORWARD(type) { \
    SLL_##type *n = b->head; \
    sllInitBuilder_##type (b); \
    return n; \
  }

//#define SLL_PUSH_POSSESSED_FORWARD(type) \
//  void sllPushPossessed_##type (SLL_##type **head, type *x)
//
//#define SLL_PUSH_WILD_FORWARD(type) \
//  void sllPushWild_##type (SLL_##type **head, type *x)
//
//#define SLL_PUSH(type) \
//  void sllPush##type(SLL_##type **, type *x, bool possessed { \
//      "implementation not defined" \
//  }

#define SLL_POP_FORWARD(type) \
  SLLBox_##type *sllPop_##type (SLL_##type **head)

#define SLL_POP(type) \
  SLL_POP_FORWARD(type) { \
    SLL_##type *n = *head; \
    if (n != NULL) { \
      *head = n->next; \
      n->next = NULL; \
    } \
    return (SLLBox_##type *)n; \
  }

#define SLL_FREE_FORWARD(type) \
  void sllFree_##type(SLL_##type **head)

#define SLL_FREE(type) \
  SLL_FREE_FORWARD(type) { \
    assert(head != NULL); \
    SLL_##type *n; \
    while (*head != NULL) { \
      n = *head; \
      if (n->possessed) \
        free_##type(n->x); \
      *head = n->next; \
      free(n); \
    } \
  }

#define SLL_H(type) \
  SLL_BOX_TYPEDEF(type); \
  SLL_TYPEDEF(type); \
  SLLB_TYPEDEF(type); \
  SLLB_INIT_FORWARD(type); \
  SLLB_APPEND_POSSESSED_FORWARD(type); \
  SLLB_APPEND_WILD_FORWARD(type); \
  SLLB_MATERIALIZE_FORWARD(type); \
  SLL_POP_FORWARD(type); \
  SLL_FREE_FORWARD(type);

#define SLL_C(type) \
  SLLB_INIT(type); \
  SLLB_APPEND(type); \
  SLLB_APPEND_POSSESSED(type); \
  SLLB_APPEND_WILD(type); \
  SLLB_MATERIALIZE(type); \
  SLL_POP(type); \
  SLL_FREE(type);

/* ------------ transforms ------------ */

//#define SLL_MAP_FORWARD(from, to) \
//  SLL_##to *sllMap_##from##2##to (SLL_##from *h, to *(*f)(from *))
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
  accumulator *sllFoldl_##from##2##accumulator (SLL_##from *n, accumulator *x, accumulator *(*f)(from *, accumulator *))

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
