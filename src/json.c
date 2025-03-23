#ifndef JSON_LIST_OPEN
#define JSON_LIST_OPEN "["
#endif
#ifndef JSON_LIST_CLOSE
#define JSON_LIST_CLOSE "]"
#endif
#ifndef JSON_LIST_COMMA
#define JSON_LIST_COMMA ","
#endif
#ifndef JSON_OBJ_OPEN
#define JSON_OBJ_OPEN "{"
#endif
#ifndef JSON_OBJ_CLOSE
#define JSON_OBJ_CLOSE "}"
#endif
#ifndef JSON_OBJ_COLON
#define JSON_OBJ_COLON ":"
#endif
#ifndef JSON_OBJ_COMMA
#define JSON_OBJ_COMMA JSON_LIST_COMMA
#endif

#include "string_builder.h"
// #include "json.h"

#define SLL_TRANS_TYPE StringListBuilder
#define SLL_TRANS_SUFFIX MAKE_NAME(SLL_TYPE, 2SLB)
#include "singly_linked_list_transforms.h"

#define TO_JSON_HELPER MAKE_NAME(TO_JSON, _helper)
StringListBuilder *TO_JSON_HELPER(SLL_TYPE *x, StringListBuilder *l) {
  appendStringListBuilder(l, TO_JSON(x));
  return l;
}

StringWriter *MAKE_NAME(jsonList, SLL_TYPE) (SLL_NODE *xs) {
  StringBuilder b; initStringBuilder(&b);
  appendDataString(&b, JSON_LIST_OPEN);
  {
    StringListBuilder l; initStringListBuilder(&l);
    MAKE_NAME(sllFoldr, SLL_TRANS_SUFFIX) (xs, &l, &TO_JSON_HELPER);
    appendStringWriter(&b, intersperseDataString(&l, JSON_LIST_COMMA));
  }
  appendDataString(&b, JSON_LIST_CLOSE);
  return finalizeBuilder(&b);
}

#ifdef JSON_OBJ
#pragma push_macro("FIELD_PTR")
#pragma push_macro("FIELD_SLL")

#define FIELD_PTR(type, name) \
  appendDataString(&b, JSON_OBJ_COMMA #name JSON_OBJ_COLON); \
  appendStringWriter(&b, toJSON_##type(x->name));
#define FIELD_SLL(type, name) appendStringWriter(&b, jsonList##type(x->name));

#define STRINGIFY_VAR_HELPER(x) #x
#define STRINGIFY_VAR(x) STRINGIFY_VAR_HELPER(x)

StringWriter *TO_JSON (SLL_TYPE *x) {
  StringBuilder b; initStringBuilder(&b);
  appendDataString(&b, JSON_OBJ_OPEN
    "\"type\""
    JSON_OBJ_COLON
    STRINGIFY_VAR(SLL_TYPE)
  );
  JSON_OBJ
  appendDataString(&b, JSON_OBJ_CLOSE);
  return finalizeBuilder(&b);
}

#pragma pop_macro("FIELD_PTR")
#pragma pop_macro("FIELD_SLL")
#endif /* JSON_OBJ */

#ifdef JSON_UNION
#pragma push_macro("FIELD_UNION")

#define FIELD_UNION(type, enum, name) \
  case enum: return toJSON_##type((type *)x);

StringWriter *TO_JSON (SLL_TYPE *x) {
  switch(x->type) {
    JSON_UNION
  }
}

#pragma pop_macro("FIELD_UNION")
#endif /* JSON_UNION */

