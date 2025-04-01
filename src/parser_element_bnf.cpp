#include "element.h"

#define E_NULL(enumerator, infix, token) \
  token { $$ = newElement(enumerator); } infix
#define E_COUNT(enumerator, infix, token) \
  token { $$ = newElementCount(enumerator, $1); } infix
#define E_STRING(enumerator, infix, token) \
  token { $$ = newElementString(enumerator, $1); } infix

element: ELEMENT_MAP(E_NULL, E_COUNT, E_STRING, |, ;) ;
/*element: '['  { $$ = newElement( OpenSquare,     newElementNull()     ); }
 *       | ']'  { $$ = newElement( CloseSquare,    newElementNull()     ); }
 *       | '{'  { $$ = newElement( OpenCurly,      newElementNull()     ); }
 *       | '}'  { $$ = newElement( CloseCurly,     newElementNull()     ); }
 *       | '\n' { $$ = newElement( Newlines,       newElementCount($1)  ); }
 *       | ' '  { $$ = newElement( Spaces,         newElementCount($1)  ); }
 *       | '\b' { $$ = newElement( Backspaces,     newElementCount($1)  ); }
 *       | '\f' { $$ = newElement( Formfeeds,      newElementCount($1)  ); }
 *       | '\t' { $$ = newElement( HorizontalTabs, newElementCount($1)  ); }
 *       | '\\' { $$ = newElement( ReverseSolidus, newElementNull()     ); }
 *       | '"'  { $$ = newElement( DoubleQuote,    newElementNull()     ); }
 *       | WORD { $$ = newElement( Word,           newElementString($1) ); }
 *       ;
 */

