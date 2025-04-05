#include "element.h"

#define E_NULL(enumerator, infix, token) \
  token { $$ = newElement(enumerator); } infix
#define E_COUNT(enumerator, infix, token) \
  token { $$ = newElementCount(enumerator, $1); } infix
#define E_STRING(enumerator, infix, token) \
  token { $$ = newElementString(enumerator, $1); } infix

element: ELEMENT_MAP(E_NULL, E_COUNT, E_STRING, |, ;) ;
