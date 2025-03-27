#ifndef NEW_H
#define NEW_H

#include "pp_utils.h"

/* When defining structs or unions define a <typeName>_FIELDS macro with the arguments:
 * (fieldType, field, fieldPtr, fieldList, fieldUnion, infix)
 *
 * fieldType(enumType, enumerator, infix)
 *   Defines the discriminator type for a union/struct
 *   - enumType: the enum type used as discriminator
 *   - enumerator: the specific enum value for this type
 *   - infix: optional separator for compound field names or macro chaining
 * 
 * field(type, name, infix)
 *   Declares a regular value field
 *   - type: data type of the field
 *   - name: field name
 *   - infix: optional separator for compound declarations
 * 
 * fieldPtr(type, name, infix)
 *   Declares a pointer field
 *   - type: data type pointed to
 *   - name: field name
 *   - infix: optional separator for compound declarations
 * 
 * fieldList(type, name, infix)
 *   Declares a linked list field
 *   - type: element type of the list
 *   - name: field name
 *   - infix: optional separator for compound declarations
 * 
 * fieldUnion(type, id, name, infix)
 *   Declares a union variant
 *   - type: variant's struct type
 *   - id: enum value that identifies this variant
 *   - name: field name for this variant
 *   - infix: optional separator for compound declarations
 * 
 * infix
 *   Optional argument passed to all field macros
 *   - Can be used for field separators, naming schemes, or to chain macro expansions
 *   - Often used to conditionally add commas or other syntax elements
 */

/* When defining an enum define a <typeName>_ENUM macro with the arguments:
 * (enumerator, infix)
 *
 * enumerator(name, infix)
 *   Defines the enumerator name
 *   - name: name of the enumerator
 *   - infix: optional separator for compound field names or macro chaining
 *
 * infix
 *   Optional argument passed to all field macros
 *   - Can be used for field separators, naming schemes, or to chain macro expansions
 *   - Often used to conditionally add commas or other syntax elements
 */


#define FIELD_TYPE(enumType, enumerator, ...) enumType type;
#define FIELD_FIELD(type, name, ...) type name;
#define FIELD_PTR(type, name, ...) type *name;
#define FIELD_SLL(type, name, ...) SLLNode_##type *name;
#define FIELD_UNION(type, enumerator, name, ...) type name;

#define TYPEDEF(name, specifier, fields) \
  typedef specifier { \
    fields(FIELD_TYPE, FIELD_FIELD, FIELD_PTR, FIELD_SLL, FIELD_UNION, ) \
  } name

#define TYPEDEF_ENUM(name, enumerators) \
  typedef enum { enumerators(CAT_APP, COMMA_F) } name

#define FWD_TYPE(...)
#define FWD_FIELD(type, name, default, infix) type name infix()
#define FWD_PTR(type, name, infix) type *name infix()
#define FWD_SLL(type, name, infix) SLLNode_##type *name infix()
#define FWD_UNION(type, enumerator, name, infix) type name infix()

#define NEW_FORWARD(type, fields) \
  type *new##type(fields(FWD_TYPE, FWD_FIELD, FWD_PTR, FWD_SLL, FWD_UNION, COMMA_F))

#define INIT_TYPE(enumType, enumerator, ...) x->type = enumerator;
#define INST_FIELD(type, name, ...) x->name = name;
#define INST_PTR(type, name, ...) x->name = name;
#define INST_SLL(type, name, ...) x->name = name;
#define INST_UNION(...)

#define NEW_INSTANCE(type, fields) \
  NEW_FORWARD(type, fields) { \
    type *x = malloc(sizeof(type)); \
    fields(INIT_TYPE, INST_FIELD, INST_PTR, INST_SLL, INST_UNION, ); \
    return x; \
  }

#define INIT_FORWARD(type) \
  void init##type(type *x)

#define INIT_FIELD(type, name, default, ...) \
  x->name = default;
#define INIT_PTR(type, name, ...) \
  x->name = NULL;
#define INIT_LIST(type, name, ...) \
  x->name = NULL;

#define INIT_INSTANCE(type, fields) \
  INIT_FORWARD(type) { \
    fields(INIT_TYPE, INIT_FIELD, INIT_PTR, INIT_LIST, EMPTY_F, ) \
  }

#endif
