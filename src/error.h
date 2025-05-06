/* ERROR_H check below */

#ifdef IN_MAIN_FUNCTION
/**
 * Main-specific error handling macros
 * These process errors since main cannot return them.
 */
#undef ERROR_CHECK
#define ERROR_CHECK(e) \
  error((e), &src, &patch, tmp)

#undef ERROR_SET
#define ERROR_SET(e, argAssignments) \
  argAssignments; ERROR_CHECK(e)

#undef ERROR_CONDITION
#define ERROR_CONDITION(e, condition, argAssignments) \
  if (condition) { ERROR_SET(e, argAssignments); }

#else
/**
 * Standard error handling macros - for functions that return ErrorId
 * These propagate errors up the call stack.
 */
#undef ERROR_CHECK
#define ERROR_CHECK(e) \
  if (Success != (e)) \
    return (e)

#undef ERROR_SET
#define ERROR_SET(e, argAssignments) \
  argAssignments; return(e)

#undef ERROR_CONDITION
#define ERROR_CONDITION(e, condition, argAssignments) \
  if (condition) { \
    SET_ORIGIN; \
    ERROR_SET(e, argAssignments); \
  }

#endif /* IN_MAIN_FUNCTION */

#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <errno.h>

#include "patchControl.h"
#include "parseState.h"
#include "logging.h"

#define ERROR_PRINT(...) \
  if (LogWarning >= logLevel) \
    fprintf(stderr, "ERROR: " __VA_ARGS__); \
  else { \
    const struct ErrorOrigin *origin; \
    origin = popErrorOrigin(); \
    assert(NULL != origin); \
    fprintf(stderr, "ERROR [at %s:%d in %s]: ", \
            NULL == origin ? "(null)" : origin->path, \
            NULL == origin ? INT_MIN : origin->line, \
            NULL == origin ? "(null)" : origin->function); \
    fprintf(stderr, __VA_ARGS__); \
    while (NULL != origin && LogDebug <= logLevel) { \
      fprintf(stderr, "  from %s:%d in function %s\n", origin->path, origin->line, origin->function); \
      origin = popErrorOrigin(); \
    } \
  }

#define ERROR_TABLE(cons, map, ...) \
  cons(map(__VA_ARGS__, UpdatePosition,  = EOF - 2, \
           ERROR_PRINT("Failed to update position\n")), \
  cons(map(__VA_ARGS__, FileError,  = EOF - 1, \
           ERROR_PRINT("File I/O error with file: %s\n", errorArg.path)), \
  cons(map(__VA_ARGS__, eof,  = EOF, \
           ERROR_PRINT("Unexpected EOF\n")), \
  cons(map(__VA_ARGS__, Success,  = 0, \
           return), \
  cons(map(__VA_ARGS__, UndefinedFlag, , \
           ERROR_PRINT("Undefined flag '%c' in argument %s\n", errorArg.undefinedFlag.c, errorArg.undefinedFlag.argString)), \
  cons(map(__VA_ARGS__, MissingPatchFileCommandArgument, , \
           ERROR_PRINT("Missing patch file argument\n")), \
  cons(map(__VA_ARGS__, BadPath, , \
           ERROR_PRINT("Bad path\n")), \
  cons(map(__VA_ARGS__, BadPatchFilePath, , \
           ERROR_PRINT("Bad patch file path: %s\n", errorArg.path)), \
  cons(map(__VA_ARGS__, TmpPathBufferOverflow, , \
           ERROR_PRINT("Temporary file path buffer overflow\n")), \
  cons(map(__VA_ARGS__, CouldNotOpenTmpFile, , \
           ERROR_PRINT("Failed to open temporary file: %s\n", errorArg.path)), \
  cons(map(__VA_ARGS__, UnsuccessfulReadOpen, , \
           ERROR_PRINT("Failed to open file as read only: %s\n", errorArg.path)), \
  cons(map(__VA_ARGS__, UnsuccessfulWriteOpen, , \
           ERROR_PRINT("Failed to open file write only: %s\n", errorArg.path)), \
  cons(map(__VA_ARGS__, UnsuccessfulFileClose, , \
           ERROR_PRINT("Failed to close file: %s\n", errorArg.path)), \
  cons(map(__VA_ARGS__, DifferingSourceUpdatePaths, , \
           ERROR_PRINT("Differing source update paths of %s and %s\n", errorArg.pathsAB.pathA, errorArg.pathsAB.pathB)), \
  cons(map(__VA_ARGS__, RenameFile, , \
           ERROR_PRINT("Failed to rename file from '%s' to '%s': %s\n", errorArg.pathsAB.pathA, errorArg.pathsAB.pathB, strerror(errno))), \
  cons(map(__VA_ARGS__, RemoveFile, , \
           ERROR_PRINT("Failed to remove file '%s': %s\n", errorArg.path, strerror(errno))), \
  cons(map(__VA_ARGS__, CannotFindLineInUpdateFile, , \
           ERROR_PRINT("Cannot find line %d in update file %s\n", errorArg.pathLine.line, errorArg.pathLine.path)), \
  cons(map(__VA_ARGS__, ParseFail_BufferOverflow, , \
           ERROR_PRINT("Buffer overflow at line %d in file %s\n", errorArg.pathLine.line, errorArg.pathLine.path)), \
  cons(map(__VA_ARGS__, ParseFail_UnexpectedControl, , \
           ERROR_PRINT("Expected one of %s but received %s `%s` in patch file %s line %d column %d\n", \
                       errorArg.unexpectedPatchControl.expected, \
                       patchControl2enumStr(errorArg.unexpectedPatchControl.unexpected), \
                       patchControl2commandStr(errorArg.unexpectedPatchControl.unexpected), \
                       errorArg.unexpectedPatchControl.f->path, \
                       errorArg.unexpectedPatchControl.f->line, \
                       errorArg.unexpectedPatchControl.f->column - (int)strlen(patchControl2commandStr(errorArg.unexpectedPatchControl.unexpected))) ), \
  cons(map(__VA_ARGS__, ParseFail_ExpectedControl, , \
           ERROR_PRINT("Expected patch control %s `%s` but received %s `%s` in patch file %s line %d column %d\n", \
                       patchControl2enumStr(errorArg.patchControlDiffers.expected), \
                       patchControl2commandStr(errorArg.patchControlDiffers.expected), \
                       patchControl2enumStr(errorArg.patchControlDiffers.received), \
                       patchControl2commandStr(errorArg.patchControlDiffers.received), \
                       errorArg.patchControlDiffers.f->path, \
                       errorArg.patchControlDiffers.f->line, \
                       errorArg.patchControlDiffers.f->column - (int)strlen(patchControl2commandStr(errorArg.patchControlDiffers.received))) ), \
  cons(map(__VA_ARGS__, ParseFail_GitHeader, , \
           ERROR_PRINT("Failed to parse Git header: %s (line %d in %s)\n", errorArg.pathLineMsg.msg, errorArg.pathLineMsg.line, errorArg.pathLineMsg.path)), \
  cons(map(__VA_ARGS__, ParseFail_GitHeader_Index, , \
           ERROR_PRINT("Failed to parse Git header index: %s (line %d in %s)\n", errorArg.pathLineMsg.msg, errorArg.pathLineMsg.line, errorArg.pathLineMsg.path)), \
  cons(map(__VA_ARGS__, ParseFail_DiffHeader_PathMinus, , \
           ERROR_PRINT("Failed to parse Diff header path (minus): %s (line %d in %s)\n", errorArg.pathLineMsg.msg, errorArg.pathLineMsg.line, errorArg.pathLineMsg.path)), \
  cons(map(__VA_ARGS__, ParseFail_DiffHeader_PathPlus, , \
           ERROR_PRINT("Failed to parse Diff header path (plus): %s (line %d in %s)\n", errorArg.pathLineMsg.msg, errorArg.pathLineMsg.line, errorArg.pathLineMsg.path)), \
  cons(map(__VA_ARGS__, ParseFail_HunkHeader, , \
           ERROR_PRINT("Failed to parse hunk header: %s (line %d in %s)\n", errorArg.pathLineMsg.msg, errorArg.pathLineMsg.line, errorArg.pathLineMsg.path)), \
  cons(map(__VA_ARGS__, ParseFail_InvalidControlInState, , \
           ERROR_PRINT("Found invalid parse control %s \"%s\" in state %s (line %d column %d in %s)\n", \
                       patchControl2enumStr(errorArg.stateControl.control), \
                       patchControl2commandStr(errorArg.stateControl.control), \
                       parseState2enumStr(errorArg.stateControl.state), \
                       errorArg.stateControl.f->line, \
                       errorArg.stateControl.f->column, \
                       errorArg.stateControl.f->path)), \
  cons(map(__VA_ARGS__, PrintFail_GitHeader, , \
           ERROR_PRINT("Failed to print Git header\n")), \
  cons(map(__VA_ARGS__, MissMatch, , \
           ERROR_PRINT("Characters from source and patch do not match at line %d col %d in %s and line %d col %d in %s\n", \
                        errorArg.sourceAndPatch.src->line, errorArg.sourceAndPatch.src->column, errorArg.sourceAndPatch.src->path, \
                        errorArg.sourceAndPatch.patch->line, errorArg.sourceAndPatch.patch->column, errorArg.sourceAndPatch.patch->path)), \
  cons(map(__VA_ARGS__, UndefinedBehavior, , \
           ERROR_PRINT("Undefined behavior of %s\n", errorArg.msg)), \
  cons(map(__VA_ARGS__, WarningAsError, , \
           switch(logId) { LOG_TABLE(CAT, CASE_LOG_ERROR) } ), \
       map(__VA_ARGS__, UnknownError, , \
           ERROR_PRINT("Unknown error\n")) \
      ))))))))))))))))))))))))))))))

#define CASE_LOG_ERROR(_, id, __, format, ...) \
  case id: ERROR_PRINT(format); break;

enum ErrorId {
  ERROR_TABLE(COMMA_INTER, COMPOSE, IDENTITY, CAT)
};

struct CharInString {
  char c;
  const char *argString;
};

struct PathLine {
  const char *path;
  int line;
};

struct PathLineMsg {
  const char *path;
  int line;
  const char *msg;
};

struct StateControl {
  struct MFile *f;
  enum ParseState state;
  enum PatchControl control;
};

struct PathsAB {
  const char *pathA,
             *pathB;
};

struct PatchControlDiffers {
  struct MFile *f;
  enum PatchControl expected,
                    received;
};

struct UnexpectedPatchControl {
  struct MFile *f;
  enum PatchControl unexpected;
  const char *expected;
};

struct SourceAndPatch {
  struct MFile *src,
               *patch;
};

union ErrorArg {
  const char *msg,
             *path;
  struct CharInString undefinedFlag;
  struct PathLine pathLine;
  struct PathLineMsg pathLineMsg;
  struct PathsAB pathsAB;
  struct PatchControlDiffers patchControlDiffers;
  struct UnexpectedPatchControl unexpectedPatchControl;
  struct StateControl stateControl;
  struct SourceAndPatch sourceAndPatch;
  enum LogId logId;
};

extern union ErrorArg errorArg;


#define SET_ORIGIN_EOF(value) SET_ORIGIN_CONDITIONALLY(value, EQUIVALENT(EOF))
#define SET_ORIGIN_NE0(value) SET_ORIGIN_CONDITIONALLY(value, !EQUIVALENT(0))
#define SET_ORIGIN_CONDITIONALLY(value, condition) if (condition(value)) { SET_ORIGIN }
#define SET_ORIGIN _pushErrorOrigin(__FILE__, __func__, __LINE__)

struct ErrorOrigin {
  const char *path,
             *function;
  int line;
};

void _pushErrorOrigin(FP(char) file, FP(char) func, const int line);

const struct ErrorOrigin *popErrorOrigin(void);

void error(enum ErrorId e, struct MFile CP src, struct MFile CP patch, FILE CP tmp);

#endif /* ERROR_H */
