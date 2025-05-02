#include <stdio.h>

#include "logging.h"

#define LEVEL_PREFIX(level) _LEVEL_ ## level()
#define _LEVEL_None()    "NONE"
#define _LEVEL_Warning() "WARN"
#define _LEVEL_Info()    "INFO"
#define _LEVEL_Verbose() "VERB"
#define _LEVEL_Debug()   "DBUG"

#define CASE_LOG(_, id, level, format, ...) \
  case id: \
    fprintf(stderr, LEVEL_PREFIX(level) ":" format); \
    break;

void _log(LogId l) {
  switch (l) {
    LOG_TABLE(CAT, CASE_LOG)
  }
}

#define CASE_ID_2_LEVEL(_, id, level, ...) \
  case id: return Log ## level;

LogLevel logIdLevel(LogId x) {
  switch (x) {
    LOG_TABLE(CAT, CASE_ID_2_LEVEL);
  }
}

