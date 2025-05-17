#ifndef LOGGING_H
#define LOGGING_H

#include "cpp.h"
#include "headers.h"
#include "parseState.h"
#include "patchControl.h"

#define LOG_LEVEL_TABLE(cons, map, ...) \
  cons(map(__VA_ARGS__, None,    = 0), \
  cons(map(__VA_ARGS__, Warning,    ), \
  cons(map(__VA_ARGS__, Info,       ), \
  cons(map(__VA_ARGS__, Verbose,    ), \
       map(__VA_ARGS__, Debug,      ) \
      ))))

#define DBUG_PREFIX "DBUG"

#define LOG_TABLE(cons, map, ...) \
  cons(map(__VA_ARGS__, L_None, None, \
           "" ), \
  cons(map(__VA_ARGS__, L_TooVerbose, Warning, \
           LOG_FORMAT("Logging only supports verbosity of %d but received %d\n", LOG_MAX, logLevel) ), \
  cons(map(__VA_ARGS__, L_EOF, Debug, \
           LOG_FORMAT("EOF: %s\n", logArg.path) ), \
  cons(map(__VA_ARGS__, L_PatchPath, Verbose, \
           LOG_FORMAT("Patch Path: %s\n", logArg.path) ), \
  cons(map(__VA_ARGS__, L_GitHeader, Debug, \
           LOG_FORMAT(FORMAT_GIT_HEADER(*logArg.gitHeader, DBUG_PREFIX ": ")) ), \
  cons(map(__VA_ARGS__, L_DiffHeader, Debug, \
           LOG_FORMAT(FORMAT_DIFF_HEADER(*logArg.diffHeader, DBUG_PREFIX ": ")) ), \
  cons(map(__VA_ARGS__, L_HunkHeader, Debug, \
           LOG_FORMAT(FORMAT_HUNK_HEADER(*logArg.hunkHeader)) ), \
  cons(map(__VA_ARGS__, L_SourcePath, Info, \
           LOG_FORMAT("Patching source file: %s\n", logArg.path) ), \
  cons(map(__VA_ARGS__, L_ParseState, Debug, \
           LOG_FORMAT("ParseState: %s\n", parseState2enumStr(logArg.parseState)) ), \
  cons(map(__VA_ARGS__, L_WhiteSpace, Debug, \
           LOG_FORMAT("WhiteSpace: " \
                      "Previous State = %s, " \
                      "WhiteSpace Character = %d, " \
                      "WhiteSpace Count = %d, " \
                      "Next Character = %d, " \
                      "Patch Control = %s\n", \
                      parseState2enumStr(logArg.whitespace.parseState), \
                      logArg.whitespace.character, \
                      logArg.whitespace.count, \
                      logArg.whitespace.nextCharacter, \
                      patchControl2enumStr(logArg.whitespace.patchControl) \
                      ) ), \
  cons(map(__VA_ARGS__, L_WarningMessage, Warning, \
           LOG_FORMAT("%s\n", logArg.message) ), \
  cons(map(__VA_ARGS__, L_InfoMessage, Info, \
           LOG_FORMAT("%s\n", logArg.message) ), \
  cons(map(__VA_ARGS__, L_VerboseMessage, Verbose, \
           LOG_FORMAT("%s\n", logArg.message) ), \
       map(__VA_ARGS__, L_DebugMessage, Debug, \
           LOG_FORMAT("%s\n", logArg.message) ) \
      )))))))))))))

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
    logOrigin = (struct LogOrigin){__FILE__, __func__, __LINE__}; \
    ERROR_CONDITION(WarningAsError, werror && logIdLevel(id) == LogWarning, ); \
    if (logIdLevel(id) <= logLevel) \
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
  struct WhiteSpaceLog {
    enum ParseState parseState;
    int character,
        count,
        nextCharacter;
    enum PatchControl patchControl;
  } whitespace;
};

extern union LogArg logArg;

struct LogOrigin {
  const char *path,
             *function;
  int line;
};

extern struct LogOrigin logOrigin;

#endif
