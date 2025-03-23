#ifndef SLL_TYPES_H
#define SLL_TYPES_H

#include "diff_tree.h"

#define SLL_TYPE Element
#include "singly_linked_list.h"
#undef SLL_TYPE

#define SLL_TYPE Diff
#include "singly_linked_list.h"
#undef SLL_TYPE

#define SLL_TYPE Hunk
#include "singly_linked_list.h"
#undef SLL_TYPE

#define SLL_TYPE Patch
#include "singly_linked_list.h"
#undef SLL_TYPE

#endif
