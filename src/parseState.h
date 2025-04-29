#ifndef PARSESTATE_H
#define PARSESTATE_H

#include "cpp.h"

#define PARSE_STATE_TABLE(cons, map, ...) \
  cons(map(__VA_ARGS__, PS_ExitLoop,      = 0 ), \
  cons(map(__VA_ARGS__, PS_Start,             ), \
  cons(map(__VA_ARGS__, PS_End,               ), \
  cons(map(__VA_ARGS__, PS_Git,               ), \
  cons(map(__VA_ARGS__, PS_Diff,              ), \
  cons(map(__VA_ARGS__, PS_Hunk,              ), \
  cons(map(__VA_ARGS__, PS_Match,             ), \
  cons(map(__VA_ARGS__, PS_Remove,            ), \
  cons(map(__VA_ARGS__, PS_Add,               ), \
  cons(map(__VA_ARGS__, PS_FinalizeSource,    ), \
       map(__VA_ARGS__, PS_EOF,               ) \
      ))))))))))

typedef enum {
  PARSE_STATE_TABLE(COMMA_INTER, COMPOSE, IDENTITY, CAT)
} ParseState;

const char *parseState2enumStr(ParseState x);

#endif
