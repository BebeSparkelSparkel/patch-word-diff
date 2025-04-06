#ifndef NEW_H
#define NEW_H

#include "pp_utils.h"

/* When defining structs or unions define a <typeName>_FIELDS macro with the arguments:
 * (fieldType, fieldHeader, field, fieldPtr, fieldCustom, fieldUnion, infix, end)
 *
 * fieldType(type, enumerator, infix)
 *   Defines the discriminator type for a union/struct
 *   - type: the enum type used as discriminator
 *   - enumerator: the specific enum value for this type
 *   - infix: optional separator for compound field names or macro chaining
 *
 * fieldHeader(type, infix, ...)
 *   Defines the struct header
 *   - headerType: the header type
 *   - infix: optional separator for compound field names or macro chaining
 *   - __VA_ARGS__: values to pass to the initWith##type function to initialize the header
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
 * fieldCustom(type, name, infix)
 *   Declares a custom field.
 *   If utilized in type definitions, define the macros
 *    - `FIELD_CUSTOM(type, name, ...)` for the struct or union type member defintion
 *    - `INIT_CUSTOM(type, name, ...)` optional, for the default value initialization of the field
 *   Args:
 *    - type: abstract type
 *    - name: field name
 *    - infix: optional separator for compound declarations
 * 
 * fieldUnion(type, enumerator, name, infix)
 *   Declares a union variant
 *   - type: variant's type
 *   - enumerator: enum value that identifies this variant
 *   - name: field name for this variant
 *   - infix: optional separator for compound declarations
 * 
 * infix
 *   Optional argument passed to all but the last field macros
 *   - Can be used for field separators, naming schemes, or to chain macro expansions
 *   - Often used to conditionally add commas or other syntax elements
 *
 * end
 *   Optional argument similar to infux but only passed to the last field argument
 */

/* When defining an enum define a <typeName>_ENUM macro with the arguments:
 * (enumerator, infix, end)
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
 *
 * end
 *   Optional argument similar to infux but only passed to the last field argument
 */


/* --- Define a Type --- */

#define FIELD_TYPE(TYPE, enumerator, ...) TYPE type;
#define FIELD_HEADER(type, ...) type header;
#define FIELD_FIELD(type, name, ...) type name;
#define FIELD_PTR(type, name, ...) type *name;
#ifndef FIELD_CUSTOM
#define FIELD_CUSTOM "ERROR: The macro `#define FIELD_CUSTOM(type, name, ...)` needes to be defined for the struct or union type member defintion. See new.h for details."
#endif
#define FIELD_UNION(type, enumerator, name, ...) type name;

#define TYPEDEF(name, specifier, fields) \
  typedef specifier { \
    fields(FIELD_TYPE, FIELD_HEADER, FIELD_FIELD, FIELD_PTR, FIELD_CUSTOM, FIELD_UNION, , ) \
  } name

#define TYPEDEF_ENUM(type, enumerators) \
  typedef enum { enumerators(CAT_APP, COMMA_F, EMPTY_F) } type

/* --- Function to Initalize the Type --- */

#define INITWITH_FIELD_FWD(type, name, infix) type name infix()
#define INITWITH_PTR_FWD(type, name, infix) type *name infix()
#define INITWITH_SLL_FWD(type, name, infix) SLLNode_##type *name infix()
#define INITWITH_UNION_FWD(type, _, name, infix) type name infix()
#define INITWITH_TYPED_ARGS(fields) fields(EMPTY_F, EMPTY_F, INITWITH_FIELD_FWD, INITWITH_PTR_FWD, INITWITH_SLL_FWD, INITWITH_UNION_FWD, COMMA_F, EMPTY_F)

#define INITWITH_FORWARD(type, fields) \
  void initWith##type(type *x, INITWITH_TYPED_ARGS(fields))

#define INIT_TYPE(_, enumerator, ...) x->type = enumerator;
#define INIT_HEADER(type, _, ...) initWith##type((type *)x, __VA_ARGS__);
#define INITWITH_FIELD(_, name, ...) x->name = name;
#define INITWITH_PTR(_, name, ...) x->name = name;
#define INITWITH_SLL(_, name, ...) x->name = name;

#define INITWITH(type, fields) \
  INITWITH_FORWARD(type, fields) { \
    fields(INIT_TYPE, INIT_HEADER, INITWITH_FIELD, INITWITH_PTR, INITWITH_SLL, EMPTY_F, , ); \
  }

/* --- Function to Dynamically Allocate and Initalize the Type --- */

#define NEW_FORWARD(type, fields) \
  type *new##type(INITWITH_TYPED_ARGS(fields))

#define NAME_FIELD(_, name, infix) name infix()
#define NAME_PTR(_, name, infix) name infix()
#define NAME_SLL(_, name, infix) name infix()
#define NAME_UNION(_, __, name, infix) name infix()
#define INITWITH_ARGS(fields) fields(EMPTY_F, EMPTY_F, NAME_FIELD, NAME_PTR, NAME_SLL, NAME_UNION, COMMA_F, EMPTY_F)

#define NEW_INSTANCE(type, fields) \
  NEW_FORWARD(type, fields) { \
    type *x = malloc(sizeof(type)); \
    initWith##type(x, INITWITH_ARGS(fields)); \
    return x; \
  }

/* --- Function to Initalize a Type with Default Values --- */

#define INIT_FORWARD(type) \
  void init##type(type *x)

#define INIT_FIELD(__, name, type) DEFAULT_##type##_##name(x->name);
#define INIT_PTR(type, name, ...) x->name = NULL;
#ifndef INIT_CUSTOM
#define INIT_CUSTOM "ERROR: The macro `#define INIT_CUSTOM(type, name, ...)` needs to be defined for the default value initialization of the field. See new.h for details."
#endif

#define INIT_INSTANCE(type, fields) \
  INIT_FORWARD(type) { \
    fields(INIT_TYPE, INIT_HEADER, INIT_FIELD, INIT_PTR, INIT_CUSTOM, EMPTY_F, type, type) \
  }

/* --- Functions to Safely Convert Type Pointers to a Union Pointer --- */

#define TU_FWD(type, unionType) \
  unionType *type##2##unionType(type *x)

#define TU_UNION_FWD(type, _, __, unionType) \
  TU_FWD(type, unionType);

#define TO_UNION_FORWARD(type, fields) \
  fields(EMPTY_F, EMPTY_F, EMPTY_F, EMPTY_F, EMPTY_F, TU_UNION_FWD, type, type)

#define TU_UNION_ACCESS(TYPE, enumerator, unionType, accessor) \
  TU_FWD(TYPE, unionType) { \
    assert(accessor == enumerator); \
    return (unionType *)x; \
  }
#define TU_UNION_TYPED(TYPE, enumerator, _, unionType) \
  TU_UNION_ACCESS(TYPE, enumerator, unionType, x->type)
#define TU_UNION_HEADED(TYPE, enumerator, _, unionType) \
  TU_UNION_ACCESS(TYPE, enumerator, unionType, x->header.type)
#define TU_CHOOSE_TYPED(...) TU_UNION_TYPED
#define TU_CHOOSE_HEADED(...) TU_UNION_HEADED
#define TU_UNION(fields) fields(TU_CHOOSE_TYPED, TU_CHOOSE_HEADED, EMPTY_F, EMPTY_F, EMPTY_F, EMPTY_F, EMPTY_F, EMPTY_F)

#define TO_UNION(type, fields) \
  fields(EMPTY_F, EMPTY_F, EMPTY_F, EMPTY_F, EMPTY_F, TU_UNION(fields), type, type)

/* --- Functions to Safely Convert Union Pointer to Component Type Pointers --- */

#define FU_FWD(type, unionType) \
  type *unionType##2##type(unionType *x)

#define FU_HEADER_FWD(type, unionType, ...) FU_FWD(type, unionType);
#define FU_UNION_FWD(type, _, __, unionType) FU_FWD(type, unionType);

#define FROM_UNION_FORWARD(type, fields) \
  fields(EMPTY_F, FU_HEADER_FWD, EMPTY_F, EMPTY_F, EMPTY_F, FU_UNION_FWD, type, type)

#define FU_ASSERTS(type, unionType, asserts) \
  FU_FWD(type, unionType) { \
    assert(x != NULL); \
    asserts; \
    return (type *)x; \
  }
#define FU_HEADER(type, unionType, ...) \
  FU_ASSERTS(type, unionType, )
#define FU_UNION_TYPED(TYPE, enumerator, _, unionType) \
    FU_ASSERTS(TYPE, unionType, assert(x->type == enumerator))
#define FU_UNION_HEADED(TYPE, enumerator, _, unionType) \
    FU_ASSERTS(TYPE, unionType, assert(x->header.type == enumerator))
#define FU_CHOOSE_TYPED(...) FU_UNION_TYPED
#define FU_CHOOSE_HEADED(...) FU_UNION_HEADED
#define FU_UNION(fields) fields(FU_CHOOSE_TYPED, FU_CHOOSE_HEADED, EMPTY_F, EMPTY_F, EMPTY_F, EMPTY_F, EMPTY_F, EMPTY_F)

#define FROM_UNION(type, fields) \
  fields(EMPTY_F, FU_HEADER, EMPTY_F, EMPTY_F, EMPTY_F, FU_UNION(fields), type, type)

#endif
