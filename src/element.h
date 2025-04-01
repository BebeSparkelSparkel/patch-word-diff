#ifndef ELEMENT_H
#define ELEMENT_H

#define ELEMENT_MAP(enull, ecount, estring, infix, end) \
  enull(OpenSquare, infix, '[') \
  enull(CloseSquare, infix, ']') \
  enull(OpenCurly, infix, '{') \
  enull(CloseCurly, infix, '}') \
  ecount(Newlines, infix, '\n') \
  ecount(Spaces, infix, ' ') \
  ecount(Backspaces, infix, '\b') \
  ecount(Formfeeds, infix, '\f') \
  ecount(CarriageReturns, infix, '\r') \
  ecount(HorizontalTabs, infix, '\t') \
  enull(ReverseSolidus, infix, '\\') \
  enull(DoubleQuote, infix, '"') \
  estring(Word, end, WORD)

#endif
