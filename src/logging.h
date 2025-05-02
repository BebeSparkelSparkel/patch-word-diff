#ifndef LOGGING_H
#define LOGGING_H

#include "cpp.h"
#include "headers.h"

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
           WARN_FORMAT("Logging only supports verbosity of %d but received %d", LOG_MAX, logLevel) ), \
  cons(map(__VA_ARGS__, L_PatchPath, Verbose, \
           VERB_FORMAT("Patch Path: %s", logArg.path) ), \
  cons(map(__VA_ARGS__, L_GitHeader, Debug, \
           DBUG_FORMAT( FORMAT_GIT_HEADER(*logArg.gitHeader) ) ), \
  cons(map(__VA_ARGS__, L_HunkHeader, Debug, \
           DBUG_FORMAT( FORMAT_HUNK_HEADER(*logArg.hunkHeader) ) ), \
  cons(map(__VA_ARGS__, L_SourcePath, Info, \
           INFO_FORMAT("Patching source file: %s", logArg.path) ), \
       map(__VA_ARGS__, L_Message, Info, \
           INFO_FORMAT("%s", logArg.message) ) \
      ))))))

#define LOG_MAX LOG_LEVEL_TABLE(PLUS_INTER, HEAD, 1) - 1

typedef enum {
  LOG_LEVEL_TABLE(COMMA_INTER, COMPOSE, PREFIX(Log), CAT)
} LogLevel;


extern LogLevel logLevel; /* should probably default to LogWarning */

#define WARN_FORMAT(...) PROTO_LOG_FORMAT(WARN, __VA_ARGS__)
#define INFO_FORMAT(...) PROTO_LOG_FORMAT(INFO, __VA_ARGS__)
#define VERB_FORMAT(...) PROTO_LOG_FORMAT(VERB, __VA_ARGS__)
#define DBUG_FORMAT(...) PROTO_LOG_FORMAT(DBUG, __VA_ARGS__)

#define PROTO_LOG_FORMAT(level, format, ...) \
  #level ": " format "\n", __VA_ARGS__

typedef enum LogId {
  LOG_TABLE(COMMA_INTER, SND)
} LogId;

extern LogId logId;

extern char werror; /* if true warnings are errors */

#define logWarningIf(id, condition, argAssignments) \
  if (condition) { \
		argAssignments; \
    ERROR_CONDITION(WarningAsError, werror, ) \
    else logProto(Warning, id, 1, ) \
  }
#define logInfo(id, argAssignments) logProto(Info, id, 1, argAssignments)
#define logVerbose(id, argAssignments) logProto(Verbose, id, 1, argAssignments)
#define logDebug(id, argAssignments) logProto(Debug, id, 1, argAssignments)
#define logProto(level, id, condition, argAssignments) \
  if (Log##level <= logLevel && condition) { \
    argAssignments; \
    _log(id); \
  }

void _log(LogId l);

typedef union {
  const char *message,
             *path;
  GitHeader *gitHeader;
  DiffHeader *diffHeader;
  HunkHeader *hunkHeader;
} LogArg;

extern LogArg logArg;

#endif
