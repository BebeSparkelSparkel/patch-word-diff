#include "parseState.h"

const char *parseState2enumStr(ParseState x) {
  switch (x) {
    PARSE_STATE_TABLE(END_EXPRESSION_INTER, COMPOSE, CASE_RETURN_STRINGIFIED, HEAD);
  }
}
