#ifndef PARSE_H
#define PARSE_H

#define PARSE_LINE(errorId, numberOfParsedParameters, formatString, errorMessage, ...) \
  s = mGets(parseBuf, PARSE_BUF_SIZE, patch); \
  if (NULL == s) { \
    PLM(patch, "Failed to get line when parsing."); \
    ERROR_CHECK(FileError); \
  } \
  if (*(s - 1) != '\n') { \
    errorArg.pathLine = (struct PathLine){ patch->path, patch->line - 1 }; \
    ERROR_CHECK(ParseFail_BufferOverflow); \
  } \
  i = sscanf(parseBuf, formatString, __VA_ARGS__); \
  if (i < numberOfParsedParameters) { \
    PLM(patch, errorMessage); \
    ERROR_CHECK(errorId); \
  }

#define PLM(f, msg) \
  errorArg.pathLineMsg = (struct PathLineMsg){ f->path, f->line - 1, msg }

#endif
