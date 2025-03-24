#include "singly_linked_list.h"

void MAKE_NAME(jsonList, SLL_TYPE) (StringBuilder *, SLL_NODE *);

#define TO_JSON MAKE_NAME(toJSON_, SLL_TYPE)
void TO_JSON (StringBuilder *, SLL_TYPE *);

#define JSON_STRINGIFY(x) "\"" #x "\""
