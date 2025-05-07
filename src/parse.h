#ifndef PARSE_H
#define PARSE_H

#define PARSE_LINE(errorId, numberOfParsedParameters, formatString, errorMessage, ...) \
  parseBuf[PARSE_BUF_SIZE - 2] = '\0'; \
  s = mGets(parseBuf, PARSE_BUF_SIZE, patch); \
  ERROR_CONDITION(errorId, NULL == s, PLM(patch, "Failed to get line when parsing.")); \
  ERROR_CONDITION(ParseFail_BufferOverflow, '\0' != s[PARSE_BUF_SIZE - 2], errorArg.pathLine = ((struct PathLine){ patch->path, patch->line - 1 })); \
  i = sscanf(parseBuf, formatString, __VA_ARGS__); \
  ERROR_CONDITION(errorId, numberOfParsedParameters > i, PLM(patch, errorMessage))

#define PLM(f, msg) \
  errorArg.pathLineMsg = ((struct PathLineMsg){ f->path, f->line - 1, msg })

#endif
