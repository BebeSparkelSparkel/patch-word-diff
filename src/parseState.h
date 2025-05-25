#ifndef PARSESTATE_H
#define PARSESTATE_H

#include "cpp.h"

#define PARSE_STATE_TABLE(cons, map, ...) \
/* 0 */  cons(map(__VA_ARGS__, PS_ExitLoop,       = 0 ), \
/* 1 */  cons(map(__VA_ARGS__, PS_Start,              ), \
/* 2 */  cons(map(__VA_ARGS__, PS_EmptyPatch,         ), \
/* 3 */  cons(map(__VA_ARGS__, PS_WhiteSpacePatch,    ), \
/* 4 */  cons(map(__VA_ARGS__, PS_WhiteSpaceSource,   ), \
/* 5 */  cons(map(__VA_ARGS__, PS_Git,                ), \
/* 6 */  cons(map(__VA_ARGS__, PS_Diff,               ), \
/* 7 */  cons(map(__VA_ARGS__, PS_Hunk,               ), \
/* 8 */  cons(map(__VA_ARGS__, PS_Match,              ), \
/* 9 */  cons(map(__VA_ARGS__, PS_Remove,             ), \
/* A */  cons(map(__VA_ARGS__, PS_Add,                ), \
/* B */       map(__VA_ARGS__, PS_FinalizeSource,     ) \
/* F max */  )))))))))))

enum ParseState {
  PARSE_STATE_TABLE(COMMA_INTER, COMPOSE, IDENTITY, CAT)
};

const char *parseState2enumStr(enum ParseState x);

#endif
