#ifndef JSON_H
#define JSON_H

#include "pp_utils.h"

#define JSON_STRINGIFY(x) "\"" #x "\""

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

#define TO_JSON_LIST_FORWARD(type) \
  void toJSONList_##type (StringBuilder *b, SLLNode_##type *xs)

#define TO_JSON_LIST(type) \
  StringListBuilder *jsonListHelper_##type(type *x, StringListBuilder *l) { \
    StringBuilder b; \
    initStringBuilder(&b); \
    toJSON_##type(&b, x); \
    appendStringListBuilder(l, finalizeBuilder(&b)); \
    return l; \
  } \
  \
  TO_JSON_LIST_FORWARD(type) { \
    appendDataString(b, JSON_LIST_OPEN); \
    { \
      StringListBuilder l; initStringListBuilder(&l); \
      sllFoldl_##type##2StringListBuilder (xs, &l, &jsonListHelper_##type); \
      appendStringWriter(b, intersperseDataString(&l, JSON_LIST_COMMA)); \
    } \
    appendDataString(b, JSON_LIST_CLOSE); \
  }

#define JOF_TYPE(enumType, enumerator, ...)
#define JOF_FIELD(type, name, ...) \
  appendDataString(b, JSON_OBJ_COMMA #name JSON_OBJ_COLON); \
  toJSON_##type(b, &x->name);
#define JOF_PTR(type, name, ...) \
  appendDataString(b, JSON_OBJ_COMMA #name JSON_OBJ_COLON); \
  toJSON_##type(b, x->name);
#define JOF_SLL(type, name, ...) \
  toJSONList_##type(b, x->name);
#define JOF_UNION(type, enumerator, name, ...)

#define TO_JSON_SPLIT_FORWARD(nameSuffix, type) \
  void toJSON_##nameSuffix (StringBuilder *b, type *x)

#define TO_JSON_FORWARD(type) \
  TO_JSON_SPLIT_FORWARD(type, type)

#define TO_JSON_OBJ(type, fields) \
  TO_JSON_FORWARD(type) { \
    appendDataString(b, \
      JSON_OBJ_OPEN \
      "\"type\"" \
      JSON_OBJ_COLON \
      STRINGIFY_VAR(type) \
    ); \
    fields(JOF_TYPE, JOF_FIELD, JOF_PTR, JOF_SLL, JOF_UNION, , ) \
    appendDataString(b, JSON_OBJ_CLOSE); \
  }

#define JU_TYPE(...) x->type
#define JU_UNION(type, enumerator, name, ...) \
  case enumerator: return toJSON_##type(b, (type *)x);

#define TO_JSON_UNION(type, fields) \
  TO_JSON_FORWARD(type) { \
    switch (fields(JU_TYPE, EMPTY_F, EMPTY_F, EMPTY_F, EMPTY_F, , )) { \
      fields(EMPTY_F, EMPTY_F, EMPTY_F, EMPTY_F, JU_UNION, , ) \
    } \
  }

#define JE_ENUM(enumerator, ...) \
  case enumerator: appendDataString(b, STRINGIFY_VAR(enumerator)); break;

#define TO_JSON_ENUM(type, enumerators) \
  TO_JSON_FORWARD(type) { \
    switch (*x) { \
      enumerators(JE_ENUM, , ) \
    } \
  }

#define TO_JSON_SPLIT_HEAP_STRING(nameSuffix, type) \
  TO_JSON_SPLIT_FORWARD(nameSuffix, type) { \
    appendDataString(b, "\""); \
    appendHeapString(b, strlen((char *)x), (char *)x); \
    appendDataString(b, "\""); \
  }

#define TO_JSON_HEAP_STRING(type) TO_JSON_SPLIT_HEAP_STRING(type, type)

#define TO_JSON_INT(type) \
  TO_JSON_FORWARD(type) { \
    char *s; \
    size_t length; \
    length = asprintf(&s, "%d", *x); \
    appendHeapString(b, length, s); \
  }

#define TO_JSON_UNSIGNED(type) \
  TO_JSON_FORWARD(type) { \
    char *s; \
    size_t length; \
    length = asprintf(&s, "%u", *x); \
    appendHeapString(b, length, s); \
  }

#endif
