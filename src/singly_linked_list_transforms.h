#ifndef SLL_TYPE
#error "SLL_TYPE must be defined before including singly_linked_list.h"
#endif

#ifndef SLL_TRANS_TYPE
#error "SLL_TRANS_TYPE must be defined before including singly_linked_list_transforms.h"
#endif

#ifndef MAKE_NAME
#error "MAKE_NAME must be defined before including singly_linked_list_transforms.h"
#endif

#ifndef SLL_TRANS_SUFFIX
#define SLL_TRANS_SUFFIX MAKE_NAME(SLL_TYPE, SLL_TRANS_TYPE)
#endif

SLL_TRANS_TYPE *MAKE_NAME(sllMap, SLL_TRANS_SUFFIX) (SLL_NODE *, SLL_TRANS_TYPE *(*f)(SLL_TYPE *));

SLL_TRANS_TYPE *MAKE_NAME(sllFoldr, SLL_TRANS_SUFFIX) (SLL_NODE *, SLL_TRANS_TYPE *, SLL_TRANS_TYPE *(*f)(SLL_TYPE *, SLL_TRANS_TYPE *));


