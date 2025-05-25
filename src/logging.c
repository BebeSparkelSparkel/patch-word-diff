#include <stdio.h>

#include "logging.h"
#include "ctype.h"
#include "stdlib.h"

#define LEVEL_PREFIX(level) _LEVEL_ ## level()
#define _LEVEL_None()    "NONE"
#define _LEVEL_Warning() "WARN"
#define _LEVEL_Info()    "INFO"
#define _LEVEL_Verbose() "VERB"
#define _LEVEL_Debug()   DBUG_PREFIX

#define CASE_LOG(_, id, level, format, prepare, cleanup, ...) \
  case id: \
    prepare; \
    if (LogDebug > logLevel) \
      fprintf(stderr, LEVEL_PREFIX(level) ": " format); \
    else { \
      fprintf(stderr, LEVEL_PREFIX(level) " [at %s:%3d in %s]: ", logOrigin.path, logOrigin.line, logOrigin.function); \
      fprintf(stderr, format); \
    } \
    cleanup; \
    break;

static inline void visualizeWhitespace(char *buffer);
static inline void restoreWhitespace(char *buffer);

void _log(enum LogId l) {
  switch (l) {
    LOG_TABLE(CAT, CASE_LOG)
  }
}

#define CASE_ID_2_LEVEL(_, id, level, ...) \
  case id: return Log ## level;

enum LogLevel logIdLevel(enum LogId x) {
  switch (x) {
    LOG_TABLE(CAT, CASE_ID_2_LEVEL);
  }
}


static inline void visualizeWhitespace(char *buffer) {
  for (int i = 0; buffer[i] != '\0'; i++) {
    switch (buffer[i]) {
      case ' ':  buffer[i] = 's'; break;  /* space -> s */
      case '\t': buffer[i] = 't'; break;  /* tab -> t */
      case '\n': buffer[i] = 'n'; break;  /* newline -> n */
      case '\r': buffer[i] = 'r'; break;  /* carriage return -> r */
      case '\f': buffer[i] = 'f'; break;  /* form feed -> f */
      case '\v': buffer[i] = 'v'; break;  /* vertical tab -> v */
      default:
        {
          char printable[] = "( ) ";
          printable[1] = buffer[i];
          fprintf(stderr, "ERROR: Non-whitespace character 0x%02X %sfound at index %d in whitespace buffer\n",
                  (unsigned char)buffer[i], isprint(buffer[i]) ? printable : "", i);
          exit(1);
        }
    }
  }
}

static inline void restoreWhitespace(char *buffer) {
  for (int i = 0; buffer[i] != '\0'; i++) {
    switch (buffer[i]) {
      case 's': buffer[i] = ' '; break;   /* s -> space */
      case 't': buffer[i] = '\t'; break;  /* t -> tab */
      case 'n': buffer[i] = '\n'; break;  /* n -> newline */
      case 'r': buffer[i] = '\r'; break;  /* r -> carriage return */
      case 'f': buffer[i] = '\f'; break;  /* f -> form feed */
      case 'v': buffer[i] = '\v'; break;  /* v -> vertical tab */
      default:
        {
          char printable[] = "( ) ";
          printable[1] = buffer[i];
          fprintf(stderr, "ERROR: Invalid visual character 0x%02X %sfound at index %d during whitespace restoration\n",
                  (unsigned char)buffer[i], isprint(buffer[i]) ? printable : "", i);
          exit(1);
        }
    }
  }
}
