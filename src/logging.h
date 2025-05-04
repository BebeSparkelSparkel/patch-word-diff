#ifndef LOGGING_H
#define LOGGING_H

#include "cpp.h"
#include "headers.h"
#include "parseState.h"

#define LOG_LEVEL_TABLE(cons, map, ...) \
  cons(map(__VA_ARGS__, None,    = 0), \
  cons(map(__VA_ARGS__, Warning,    ), \
  cons(map(__VA_ARGS__, Info,       ), \
  cons(map(__VA_ARGS__, Verbose,    ), \
       map(__VA_ARGS__, Debug,      ) \
      ))))

#define LOG_TABLE(cons, map, ...) \
  cons(map(__VA_ARGS__, L_None, None, \
           "" ), \
  cons(map(__VA_ARGS__, L_TooVerbose, Warning, \
           LOG_FORMAT("Logging only supports verbosity of %d but received %d", LOG_MAX, logLevel) ), \
  cons(map(__VA_ARGS__, L_PatchPath, Verbose, \
           LOG_FORMAT("Patch Path: %s", logArg.path) ), \
  cons(map(__VA_ARGS__, L_GitHeader, Debug, \
           LOG_FORMAT( FORMAT_GIT_HEADER(*logArg.gitHeader) ) ), \
  cons(map(__VA_ARGS__, L_HunkHeader, Debug, \
           LOG_FORMAT( FORMAT_HUNK_HEADER(*logArg.hunkHeader) ) ), \
  cons(map(__VA_ARGS__, L_SourcePath, Info, \
           LOG_FORMAT("Patching source file: %s", logArg.path) ), \
  cons(map(__VA_ARGS__, L_ParseState, Debug, \
           LOG_FORMAT("ParseState: %s", parseState2enumStr(logArg.parseState)) ), \
       map(__VA_ARGS__, L_Message, Info, \
           LOG_FORMAT("%s", logArg.message) ) \
      )))))))

#define LOG_MAX LOG_LEVEL_TABLE(PLUS_INTER, HEAD, 1) - 1

enum LogLevel {
  LOG_LEVEL_TABLE(COMMA_INTER, COMPOSE, PREFIX(Log), CAT)
};


extern enum LogLevel logLevel; /* should probably default to LogWarning */

#define LOG_FORMAT(...) __VA_ARGS__

enum LogId {
  LOG_TABLE(COMMA_INTER, SND)
};

extern enum LogId logId;

extern char werror; /* if true warnings are errors */

enum LogLevel logIdLevel(enum LogId x);


#define log(id, argAssignments) \
  logIf(id, 1, argAssignments)

#define logIf(id, condition, argAssignments) \
  if (condition) { \
    argAssignments; \
    ERROR_CONDITION(WarningAsError, werror && logIdLevel(id) == LogWarning, ) \
    else if (logIdLevel(id) >= logLevel) \
      _log(id); \
  }

void _log(enum LogId l);

union LogArg {
  const char *message,
             *path;
  struct GitHeader *gitHeader;
  struct DiffHeader *diffHeader;
  struct HunkHeader *hunkHeader;
  enum ParseState parseState;
};

extern union LogArg logArg;

#endif
