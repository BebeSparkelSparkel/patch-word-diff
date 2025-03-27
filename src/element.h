#ifndef ELEMENT_H
#define ELEMENT_H

#define ELEMENT_MAP(enull, ecount, estring, infix, f) \
  f('[', OpenSquare, enull) infix \
  f(']', CloseSquare, enull) infix \
  f('{', OpenCurly, enull) infix \
  f('}', CloseCurly, enull) infix \
  f('\n', Newlines, ecount) infix \
  f(' ', Spaces, ecount) infix \
  f('\b', Backspaces, ecount) infix \
  f('\f', Formfeeds, ecount) infix \
  f('\r', CarriageReturns, ecount) infix \
  f('\t', HorizontalTabs, ecount) infix \
  f('\\', ReverseSolidus, enull) infix \
  f('"', DoubleQuote, enull) infix \
  f(WORD, Word, estring)

#define ELEMENT_GRAMMAR_F(pattern, enumId, valueConstructor) pattern { $$ = newElement( enumId, valueConstructor ); }
#define ELEMENT_GRAMMAR ELEMENT_MAP(newElementNull(), newElementCount($1), newElementString($1), |, ELEMENT_GRAMMAR_F) 

#define ELEMENT_ENUM_F(pattern, enumId, valueConstructor) enumId
#define ELEMENT_ENUM ELEMENT_MAP(, , , COMMA, ELEMENT_ENUM_F) 

#endif
