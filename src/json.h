#include "singly_linked_list.h"

StringWriter *MAKE_NAME(jsonList, SLL_TYPE) (SLL_NODE *);

#define TO_JSON MAKE_NAME(toJSON_, SLL_TYPE)
StringWriter *TO_JSON (SLL_TYPE *);

#define JSON_STRINGIFY(x) "\"" #x "\""
