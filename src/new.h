#ifndef NEW_H
#define NEW_H

#define FIELD_TYPE(TYPE, id, footer) TYPE type;
#define FIELD_HEADER(type, footer) type header;
#define FIELD(type, name, footer) type name;
#define FIELD_PTR(type, name, footer) type *name;
#define FIELD_SLL(type, name, footer) Node##type *name;
#define FIELD_UNION(type, id, name, footer) type name;

#define TYPEDEF(name, specifier, fields) \
  typedef specifier { \
    fields(FIELD_TYPE, FIELD_HEADER, FIELD, FIELD_PTR, FIELD_SLL, FIELD_UNION, ) \
  } name;

#define FWD_TYPE(TYPE, id, footer)
#define FWD_HEADER(type, footer) type header footer()
#define FWD(type, name, footer) type name footer()
#define FWD_PTR(type, name, footer) type *name footer()
#define FWD_SLL(type, name, footer) Node##type *name footer()
#define FWD_UNION(type, id, name, footer) type name footer()
#define EMPTY()
#define COMMA() ,

#define NEW_FORWARD(type, fields) \
  type *new##type(fields(FWD_TYPE, FWD_HEADER, FWD, FWD_PTR, FWD_SLL, FWD_UNION, COMMA))

#define INST_TYPE(TYPE, id, footer) x->type = id;
#define INST_HEADER(type, footer)
#define INST(type, name, footer) x->name = name;
#define INST_PTR(type, name, footer) x->name = name;
#define INST_SLL(type, name, footer) x->name = name;
#define INST_UNION(type, id, name, footer)

#define NEW_INSTANCE(type, fields) \
  NEW_FORWARD(type, fields) { \
    type *x = malloc(sizeof(type)); \
    fields(INST_TYPE, INST_HEADER, INST, INST_PTR, INST_SLL, INST_UNION, ) \
    return x; \
  }

#endif
