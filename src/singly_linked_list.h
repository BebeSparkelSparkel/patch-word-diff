#ifndef SLL_TYPE
#error "SLL_TYPE must be defined before including singly_linked_list.h"
#endif

#ifdef SLL_INCLUDE
#include SLL_INCLUDE
#undef SLL_INCLUDE
#endif

#ifndef SLL_SUFFIX
#define SLL_SUFFIX SLL_TYPE
#endif

#define HELPER_MAKE_NAME(x, y) x ## y
#define MAKE_NAME(x, y) HELPER_MAKE_NAME(x, y)

#define SLL_NODE MAKE_NAME(Node, SLL_SUFFIX)
typedef struct SLL_NODE {
  struct SLL_NODE *next;
  SLL_TYPE *x;
} SLL_NODE;

#define SLL_NEW_NODE MAKE_NAME(new, SLL_NODE)
SLL_NODE *SLL_NEW_NODE (SLL_TYPE *x);

#define SLL_BUILDER MAKE_NAME(SLLBuilder, SLL_SUFFIX)
typedef struct SLL_BUILDER {
  SLL_NODE *head;
  SLL_NODE *tail;
} SLL_BUILDER;

void MAKE_NAME(init, SLL_BUILDER) (SLL_BUILDER *);

void MAKE_NAME(initOne, SLL_BUILDER) (SLL_BUILDER *, SLL_TYPE *);

SLL_BUILDER *MAKE_NAME(new, SLL_BUILDER) ();

SLL_BUILDER *MAKE_NAME(newOne, SLL_BUILDER) (SLL_TYPE *);

void MAKE_NAME(sllBuildAppend, SLL_SUFFIX) (SLL_BUILDER *, SLL_TYPE *);

SLL_NODE *MAKE_NAME(sllMaterialize, SLL_SUFFIX) (SLL_BUILDER *);

void MAKE_NAME(sllPush, SLL_SUFFIX) (SLL_NODE **, SLL_TYPE *);

SLL_TYPE *MAKE_NAME(sllPop, SLL_SUFFIX) (SLL_NODE **);

int MAKE_NAME(sllForEach, SLL_SUFFIX) (SLL_NODE *, int (*f)(SLL_TYPE *));
