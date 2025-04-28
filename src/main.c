#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <libgen.h>

#define IDENTITY(x) x
#define APPLY(x, ...) x(__VA_ARGS__)
#define EMPTY(...)
#define FLIP(x, y, ...) y, x, __VA_ARGS__
#define HEAD(x, ...) x
#define TAIL(_, ...) __VA_ARGS__
#define DROP(x) DROP_ ## x
#define DROP_2(_, __, ...) __VA_ARGS__
#define FIRST(f) FIRST_ ## f
#define SND(_, x, ...) x
#define SECOND(f) SECOND_ ## f
#define THIRD(_, __, x, ...) x
#define CAT(x, y, ...) x y
#define CAT3(x, y, z, ...) x y z
#define COMMA_INTER(x, y, ...) x, y
#define END_EXPRESSION(x, ...) x;
#define END_EXPRESSION_INTER(x, y, ...) END_EXPRESSION(x) y
#define AND_INTER(x, y, ...) (x) && (y)

#define COMPOSE(f, g, ...) f(g(__VA_ARGS__))
#define COMPOSE3(f, g, h, ...) f(g(h(__VA_ARGS__)))
#define COMPOSE4(f, g, h, i, ...) f(g(h(i(__VA_ARGS__))))
#define PLUS_INTER(x, y, ...) x + y

#ifdef CPP_DEV
#define MAX(a, b) max(x, b)
#else
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#define PREFIX(x) PREFIX_##x
#define PREFIX_Log(x) DEFER_STRING_CAT(Log, x)
#define DEFER_STRING_CAT(x, y) x ## y

#define SELECT_COLUMNS(table, x, y) SELECT_COLUMNS_ ## table ## _ ## x ## _ ## y
#define SELECT_COLUMNS_1_3(x, _, y, ...) x, y
#define SELECT_COLUMNS_4_1(x, _, __, y, ...) y, x
#define SELECT_COLUMNS_5_1(x, _, __, ___, y, ...) y, x


#define EXPAND_ARG(x, ...) EXPAND_ARG_ ## x
#define EXPAND_ARG_APPLY(...) APPLY(__VA_ARGS__)
#define EXPAND_ARG_CAT(...) CAT(__VA_ARGS__)
#define EXPAND_ARG_CAT3(...) CAT3(__VA_ARGS__)
#define EXPAND_ARG_HEAD(...) HEAD(__VA_ARGS__)
#define EXPAND_ARG_SND(...) SND(__VA_ARGS__)
#define EXPAND_ARG_SECOND(...) SECOND(__VA_ARGS__)
#define EXPAND_ARG_TAIL(...) TAIL(__VA_ARGS__)
#define EXPAND_ARG_DROP(x) EXPAND_ARG_DROP_ ## x
#define EXPAND_ARG_DROP_2(...) DROP(2)(__VA_ARGS__)


#define ADD_END "+}"
#define RM_END "-]"

#define PATCH_CONTROL_TABLE(filt, cons, map, ...) \
  filt(map(__VA_ARGS__, PC_EOF,      "(PC_EOF)"    , = EOF ), \
  filt(map(__VA_ARGS__, PC_None,     "(PC_None)"   , = 0   ), \
  cons(map(__VA_ARGS__, PC_RmStart,  "[-"          ,       ), \
  cons(map(__VA_ARGS__, PC_RmEnd,    RM_END        ,       ), \
  cons(map(__VA_ARGS__, PC_AddStart, "{+"          ,       ), \
  cons(map(__VA_ARGS__, PC_AddEnd,   ADD_END       ,       ), \
  cons(map(__VA_ARGS__, PC_Hunk,     "@@ "         ,       ), \
  cons(map(__VA_ARGS__, PC_Git,      "diff --git " ,       ), \
  cons(map(__VA_ARGS__, PC_Minus,    "--- "        ,       ), \
  cons(map(__VA_ARGS__, PC_Plus,     "+++ "        ,       ), \
       map(__VA_ARGS__, PC_Index,    "index "      ,       )  \
      ))))))))))

#define SELECT_COLUMNS_PATCH_CONTROL_TABLE_Enumerator_Assignment SELECT_COLUMNS_1_3

typedef enum {
  PATCH_CONTROL_TABLE(COMMA_INTER, COMMA_INTER, COMPOSE, EXPAND_ARG(CAT), SELECT_COLUMNS(PATCH_CONTROL_TABLE, Enumerator, Assignment))
} PatchControl;

#define CASE_RETURN_STRINGIFIED(x) case x: return #x
const char *patchControl2enumStr(PatchControl x) {
  switch (x) {
    PATCH_CONTROL_TABLE(END_EXPRESSION_INTER, END_EXPRESSION_INTER, COMPOSE, CASE_RETURN_STRINGIFIED, HEAD);
  }
}

#define CASE_RETURN(x, y, ...) case x: return y
const char *patchControl2commandStr(PatchControl x) {
  switch (x) {
    PATCH_CONTROL_TABLE(END_EXPRESSION_INTER, END_EXPRESSION_INTER, COMPOSE, IDENTITY, CASE_RETURN);
  }
}

#define PARSE_STATE_TABLE(cons, map, ...) \
  cons(map(__VA_ARGS__, PS_ExitLoop,      = 0 ), \
  cons(map(__VA_ARGS__, PS_Start,             ), \
  cons(map(__VA_ARGS__, PS_End,               ), \
  cons(map(__VA_ARGS__, PS_Git,               ), \
  cons(map(__VA_ARGS__, PS_Diff,              ), \
  cons(map(__VA_ARGS__, PS_Hunk,              ), \
  cons(map(__VA_ARGS__, PS_Match,             ), \
  cons(map(__VA_ARGS__, PS_Remove,            ), \
  cons(map(__VA_ARGS__, PS_Add,               ), \
  cons(map(__VA_ARGS__, PS_FinalizeSource,    ), \
       map(__VA_ARGS__, PS_EOF,               ) \
      ))))))))))

typedef enum {
  PARSE_STATE_TABLE(COMMA_INTER, COMPOSE, IDENTITY, CAT)
} ParseState;

const char *parseState2enumStr(ParseState x) {
  switch (x) {
    PARSE_STATE_TABLE(END_EXPRESSION_INTER, COMPOSE, CASE_RETURN_STRINGIFIED, HEAD);
  }
}

#define SIZEOF(x) sizeof(x)

#define UNGET_BUF_SIZE PATCH_CONTROL_TABLE(TAIL, MAX, COMPOSE, SIZEOF, SND)

#define FP(x) const x *const
#define CP *const

typedef struct MFile MFile;

#define ERROR_CHECK(e) \
  if (Success != (e)) \
    return (e)
#define ERROR_SET(e, argAssignments) \
  argAssignments; return(e)
#define ERROR_CONDITION(e, condition, argAssignments) \
  if (condition) { \
    SET_ORIGIN; \
    ERROR_SET(e, argAssignments); \
  }

#define ERROR_PRINT(...) \
  if (LogWarning >= logLevel) \
    fprintf(stderr, "ERROR: " __VA_ARGS__); \
  else { \
    const ErrorOrigin *origin; \
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
	     	   TODO("werror pring")), \
       map(__VA_ARGS__, UnknownError, , \
           ERROR_PRINT("Unknown error\n")) \
      ))))))))))))))))))))))))))))

typedef enum {
  ERROR_TABLE(COMMA_INTER, COMPOSE, IDENTITY, CAT)
} ErrorId;

typedef struct {
  char c;
  const char *argString;
} CharInString;

typedef struct {
  const char *path;
  int line;
} PathLine;

typedef struct {
  const char *path;
  int line;
  const char *msg;
} PathLineMsg;

typedef struct {
  MFile *f;
  ParseState state;
  PatchControl control;
} StateControl;

typedef struct {
  const char *pathA,
             *pathB;
} PathsAB;

typedef struct {
  MFile *f;
  PatchControl expected,
               received;
} PatchControlDiffers;

typedef struct {
  MFile *f;
  PatchControl unexpected;
  const char *expected;
} UnexpectedPatchControl;

typedef struct {
  MFile *src,
        *patch;
} SourceAndPatch;

typedef union {
  const char *msg,
             *path;
  CharInString undefinedFlag;
  PathLine pathLine;
  PathLineMsg pathLineMsg;
  PathsAB pathsAB;
  PatchControlDiffers patchControlDiffers;
  UnexpectedPatchControl unexpectedPatchControl;
  StateControl stateControl;
  SourceAndPatch sourceAndPatch;
} ErrorArg;
ErrorArg errorArg;


#define SET_ORIGIN_EOF(value) SET_ORIGIN_CONDITIONALLY(value, EQUIVALENT(EOF))
#define SET_ORIGIN_NE0(value) SET_ORIGIN_CONDITIONALLY(value, !EQUIVALENT(0))
#define SET_ORIGIN_CONDITIONALLY(value, condition) if (condition(value)) { SET_ORIGIN }
#define SET_ORIGIN errorOrigin[++eoc % TRACE_SIZE] = (ErrorOrigin){ __FILE__, __func__, __LINE__ }
typedef struct {
  const char *path,
             *function;
  int line;
} ErrorOrigin;
#ifndef TRACE_SIZE
#define TRACE_SIZE 3
#endif
int eoc = -1;
ErrorOrigin errorOrigin[TRACE_SIZE];
const ErrorOrigin *popErrorOrigin(void) {
  static int end = -1;
  assert(eoc >= 0);
  if (end == -1)
    end = MAX(0, eoc - TRACE_SIZE + 1);
  if (eoc < end)
    return NULL;
  return &errorOrigin[eoc-- % TRACE_SIZE];
}

#define LOG_LEVEL_TABLE(cons, map, ...) \
  cons(map(__VA_ARGS__, None,    = 0), \
  cons(map(__VA_ARGS__, Warning,    ), \
  cons(map(__VA_ARGS__, Info,       ), \
  cons(map(__VA_ARGS__, Verbose,    ), \
       map(__VA_ARGS__, Debug,      ) \
      ))))
#define LOG_MAX LOG_LEVEL_TABLE(PLUS_INTER, HEAD, 1) - 1


typedef enum {
  LOG_LEVEL_TABLE(COMMA_INTER, COMPOSE, PREFIX(Log), CAT)
} LogLevel;
LogLevel logLevel = LogWarning;

#define LOG_TABLE(cons, map, ...) \
  cons(map(__VA_ARGS__, L_None, None, \
           return ), \
  cons(map(__VA_ARGS__, L_TooVerbose, Warning, \
           WARN_PRINT("Logging only supports verbosity of %d but received %d", LOG_MAX, logLevel) ), \
  cons(map(__VA_ARGS__, L_PatchPath, Verbose, \
           VERB_PRINT("Patch Path: %s", logArg.path) ), \
  cons(map(__VA_ARGS__, L_GitHeader, Debug, \
           DBUG_PRINT( FORMAT_GIT_HEADER(*logArg.gitHeader) ) ), \
  cons(map(__VA_ARGS__, L_HunkHeader, Debug, \
           DBUG_PRINT( FORMAT_HUNK_HEADER(*logArg.hunkHeader) ) ), \
  cons(map(__VA_ARGS__, L_SourcePath, Info, \
           INFO_PRINT("Patching source file: %s", logArg.path) ), \
       map(__VA_ARGS__, L_Message, Info, \
           INFO_PRINT("%s", logArg.message) ) \
      ))))))
#define WARN_PRINT(...) fprintf(stderr, PROTO_LOG_FORMAT(WARN, __VA_ARGS__))
#define INFO_PRINT(...) fprintf(stderr, PROTO_LOG_FORMAT(INFO, __VA_ARGS__))
#define VERB_PRINT(...) fprintf(stderr, PROTO_LOG_FORMAT(VERB, __VA_ARGS__))
#define DBUG_PRINT(...) fprintf(stderr, PROTO_LOG_FORMAT(DBUG, __VA_ARGS__))

utilize PROTO_LOG_FORMAT in the error messages
add LogId to ErrorArg for error typing
#define PROTO_LOG_FORMAT(level, format, ...) \
  fprintf(stderr, #level ": " format "\n", __VA_ARGS__)

typedef enum LogId {
  LOG_TABLE(COMMA_INTER, SND)
} LogId;

int werror = 0;
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

#ifdef DEVELOPMENT
#define TODO(msg) \
      (fprintf(stderr, "TODO: %s. Function: %s, File: %s, Line: %d\n", msg, __func__, __FILE__, __LINE__), \
      exit(1))
#endif /* DEVELOPMENT */

/* FILE Error Check */
#define EOF_FILE_CHECK(x, f, PATH) \
  if (x < 0) { \
    ERROR_CONDITION(FileError, ferror(f), errorArg.path = PATH); \
    return EOF; \
  }
#define EOF_MFILE_CHECK(x, f) EOF_FILE_CHECK(x, f->stream, f->path)

#define MFILE_TABLE(cons, map, ...) \
  /*                  |  Field  |  Type | Array (Optional) |        OpenInit        |    CloseInit     | */ \
  cons(map(__VA_ARGS__,   stream, FILE *,                  ,          ASSIGN(STREAM),    VALUE(NULL) ), \
  cons(map(__VA_ARGS__,     line,    int,                  ,               ASSIGN(1), VALUE(INT_MIN) ), \
  cons(map(__VA_ARGS__,   column,    int,                  ,               ASSIGN(1), VALUE(INT_MIN) ), \
  cons(map(__VA_ARGS__,   ungetI,    int,                  ,           ASSIGN_NEG(1), VALUE(INT_MIN) ), \
  cons(map(__VA_ARGS__, ungetBuf,   char,  [UNGET_BUF_SIZE],                   EMPTY,   VALUE(EMPTY) ), \
       map(__VA_ARGS__,     path,   char,        [PATH_MAX], STRNCPY(path, PATH_MAX), VALUE_DEREF(0) ) \
      )))))

#define ASSIGN(x, ...) ASSIGN_##x
#define ASSIGN_1(x, ...) x = 1

#define ASSIGN_NEG(x, ...) ASSIGN_ ## NEG_ ## x
#define ASSIGN_NEG_1(x, ...) x = -1

#define ASSIGN_NULL(x, ...) x = NULL

#define ASSIGN_EMPTY(...)

#define ASSIGN_DEREF(x, ...) ASSIGN_DEREF_##x
#define ASSIGN_DEREF_0(x, ...) *x = 0

#define ASSIGN_INT_MIN(x, ...) x = INT_MIN

#define ASSIGN_STREAM(x, ...) x = STREAM

#define STRNCPY(from, n) STRNCPY_ ## from ## _ ## n
#define STRNCPY_path_PATH_MAX(to, ...) strncpy(to, path, PATH_MAX - 1)

#define SELECT_COLUMNS_MFILE_TABLE_OpenInit_Field SELECT_COLUMNS_4_1
#define SELECT_COLUMNS_MFILE_TABLE_CloseInit_Field SELECT_COLUMNS_5_1
#define SELECT_COLUMNS_MFILE_TABLE_Field_CloseInit SELECT_COLUMNS_1_5

typedef struct MFile {
  MFILE_TABLE(END_EXPRESSION_INTER, COMPOSE, EXPAND_ARG(CAT3), FLIP);
} MFile;

#define ASSERT_FILE(f) \
  assert(f != NULL); \
  assert(!ferror(f)); \
  assert(!feof(f))
#define ASSERT_MFILE(f) \
  assert(f != NULL); \
  ASSERT_FILE(f->stream); \
  assert(f->line > 0); \
  assert(f->column > 0); \
  assert(f->ungetI >= -1)


#define DREF_FROM(x) DREF_FROM_ ## x
#define DREF_FROM_f(x) f-> x
#define SECOND_DREF_FROM(f) SECOND_DREF_FROM_f
#define SECOND_DREF_FROM_f(...) EXPAND_ARG(HEAD)(__VA_ARGS__), DREF_FROM_f(EXPAND_ARG(SND)(__VA_ARGS__)), EXPAND_ARG(DROP(2))(__VA_ARGS__)

void streamFile(MFile CP f, FILE *stream, FP(char) path) {
  assert(f != NULL);
  assert(path != NULL);
  assert(stream != NULL);
  assert(!feof(stream));
  assert(!ferror(stream));

#define STREAM stream
  MFILE_TABLE(END_EXPRESSION_INTER, COMPOSE3, EXPAND_ARG(APPLY), EXPAND_ARG(SECOND(DREF_FROM(f))), SELECT_COLUMNS(MFILE_TABLE, OpenInit, Field));
#undef STREAM
  //strncpy(f->path, path, PATH_MAX - 1);
  //f->stream = stream;
  //f->line = 1;
  //f->column = 1;
  //f->ungetI = -1;
}

#define OPEN_READ(f, path) OPEN_MODE(f, path, "r")
#define OPEN_MODE(f, PATH, mode) \
  { \
    FILE *stream; \
    assert(isClosed(&f)); \
    ERROR_CONDITION(BadPath, NULL == PATH || '\0' == *PATH, ); \
    stream = fopen(PATH, mode); \
    ERROR_CONDITION(UnsuccessfulReadOpen, NULL == stream, errorArg.path = PATH); \
    streamFile(&f, stream, PATH); \
  }

/*
#define OPEN_WRITE(stream, PATH) \
  ERROR_CONDITION(BadPath, PATH == NULL || *PATH == '\0', ); \
  stream = fopen(PATH, "w"); \
  ERROR_CONDITION(UnsuccessfulWriteOpen, NULL == stream, errorArg.path = PATH);
  */

#define EQUIVALENT(x) EQUIVALENT_ ## x
#define EQUIVALENT_DEREF(x) EQUIVALENT_DEREF_ ## x

#define EQUIVALENT_NULL(x, ...) NULL == (x)
#define EQUIVALENT_INT_MIN(x, ...) INT_MIN == (x)
#define EQUIVALENT_EMPTY(...) NULL == NULL

#define EQUIVALENT_DEREF_0(x, ...) 0 == *(x)

void markClosed(MFile CP f) {
#define VALUE ASSIGN
#define VALUE_DEREF ASSIGN_DEREF
  MFILE_TABLE(END_EXPRESSION_INTER, COMPOSE3, EXPAND_ARG(APPLY), EXPAND_ARG(SECOND(DREF_FROM(f))), SELECT_COLUMNS(MFILE_TABLE, CloseInit, Field));
#undef VALUE
#undef VALUE_DEREF
}

int closeFile(MFile CP f) {
  int r;
  ASSERT_MFILE(f);
  ERROR_CONDITION(UnsuccessfulFileClose, fclose(f->stream), errorArg.path = f->path);
  markClosed(f);
  return Success;
}



int isClosed(FP(MFile) f) {
#define VALUE EQUIVALENT
#define VALUE_DEREF EQUIVALENT_DEREF
  return MFILE_TABLE(AND_INTER, COMPOSE3, EXPAND_ARG(APPLY), EXPAND_ARG(SECOND(DREF_FROM(f))), SELECT_COLUMNS(MFILE_TABLE, CloseInit, Field));
#undef VALUE
#undef VALUE_DEREF
}

ErrorId advanceToLineCopy(MFile CP from, FILE CP to, const int targetLine) {
  int c, r;
  ASSERT_MFILE(from);
  ASSERT_FILE(to);
  assert(targetLine > 0);
  assert(from->line <= targetLine);
  ERROR_CONDITION(CannotFindLineInUpdateFile, from->line > targetLine, errorArg.pathLine = ((PathLine){ from->path, targetLine }));
  while (from->ungetI >= 0 && from->line < targetLine) {
    c = from->ungetBuf[from->ungetI--];
    if ('\n' == c) ++from->line;
    r = putc(c, to);
    EOF_FILE_CHECK(r, to, "tmpfile");
  }
  while (from->line < targetLine) {
    c = getc(from->stream);
    EOF_MFILE_CHECK(c, from);
    if ('\n' == c) ++from->line;
    r = putc(c, to);
    EOF_FILE_CHECK(r, to, "tmpfile");
  }
  from->column = 1;
  return Success;
}

int updatePosition(int c, MFile CP f) {
  if ('\n' == c) {
    ++f->line;
    f->column = 1;
  }
  else EOF_MFILE_CHECK(c, f)
  else ++f->column;
  return c;
}

int mGetc(MFile CP f) {
  int c;
  ASSERT_MFILE(f);
  c = f->ungetI >= 0
    ? f->ungetBuf[f->ungetI--]
    : getc(f->stream);
  return updatePosition(c, f);
}

char *mGets(char *str, int size, MFile CP f) {
  while (f->ungetI >= 0 && size > 1) {
    *str++ = f->ungetBuf[f->ungetI--];
  }
  *str = '\0';
  if (size > 0) {
    str = fgets(str, size, f->stream);
    if (NULL == str)
      return NULL;
    while (*str != '\0') {
      int r;
      r = updatePosition(*str++, f);
      assert(r >= 0);
    }
  }
  return str;
}

int mUngetc(const int c, MFile CP f) {
  int r;
  ASSERT_MFILE(f);
  assert(c != '\n');
  assert(c != EOF);
  assert(c >= 0);
  r = ungetc(c, f->stream);
  if (EOF == r) {
    assert(f->ungetI < UNGET_BUF_SIZE);
    if (f->ungetI < UNGET_BUF_SIZE)
      return EOF;
    r = f->ungetBuf[f->ungetI++] = c;
  }
  --f->column;
  return r;
}

ErrorId matchAndCopy(MFile CP src, MFile CP patch, FILE CP to) {
  int sc, pc;
  ASSERT_MFILE(src);
  ASSERT_MFILE(patch);
  ASSERT_FILE(to);
  while(1) {
    sc = mGetc(src);
    pc = mGetc(patch);
    if (EOF == sc && EOF == pc) {
      ERROR_CONDITION(FileError, ferror(src->stream) || ferror(patch->stream), );
      return EOF;
    }
    if (EOF == sc) {
      pc = mUngetc(pc, patch);
      ERROR_CONDITION(FileError, EOF == pc || ferror(src->stream), );
      return EOF;
    }
    if (EOF == pc) {
      sc = mUngetc(sc, src);
      ERROR_CONDITION(FileError, EOF == sc || ferror(patch->stream), );
      return EOF;
    }
    if (sc != pc) {
      sc = mUngetc(sc, src);
      pc = mUngetc(pc, patch);
      ERROR_CONDITION(FileError, EOF == sc || EOF == pc, );
      return Success;
    }
    pc = putc(sc, to);
    ERROR_CONDITION(FileError, EOF == pc, );
  }
}

ErrorId matchAndDiscardUntilClose(MFile CP src, MFile CP patch) {
  int sc, pc;
  ASSERT_MFILE(src);
  ASSERT_MFILE(patch);
  while(1) {
    sc = mGetc(src);
    pc = mGetc(patch);
    if (EOF == sc && EOF == pc) {
      ERROR_CONDITION(FileError, ferror(src->stream) || ferror(patch->stream), );
      return EOF;
    }
    if (EOF == sc) {
      pc = mUngetc(pc, patch);
      ERROR_CONDITION(FileError, EOF == pc || ferror(src->stream), );
      return EOF;
    }
    if (EOF == pc) {
      sc = mUngetc(sc, src);
      ERROR_CONDITION(FileError, EOF == sc || ferror(patch->stream), );
      return EOF;
    }
    if (sc != pc) {
      sc = mUngetc(sc, src);
      ERROR_CONDITION(FileError, EOF == sc, mUngetc(pc, patch));
      if ('-' == pc) {
        pc = mGetc(patch);
        if (']' == pc)
          return Success;
        if (EOF == pc) {
          pc = mUngetc('-', patch);
          ERROR_CONDITION(FileError, EOF == pc || ferror(patch->stream), );
          return EOF;
        }
        pc = mUngetc(pc, patch);
        ERROR_CONDITION(FileError, EOF == pc, );
        pc = '-';
      }
      pc = mUngetc(pc, patch);
      ERROR_CONDITION(FileError, EOF == pc, );
      ERROR_SET(MissMatch, errorArg.sourceAndPatch = ((SourceAndPatch){src, patch}));
    }
  }
}

ErrorId copyUntilClose(MFile CP patch, FILE CP to) {
  int c, r;
  ASSERT_MFILE(patch);
  ASSERT_FILE(to);
  while(1) {
    c = mGetc(patch);
    if (EOF == c) {
      ERROR_CONDITION(FileError, ferror(patch->stream), );
      return EOF;
    }
    if ('+' == c) {
      c = mGetc(patch);
      if ('}' == c)
        return Success;
      if (EOF == c) {
        r = putc('+', to);
        ERROR_CONDITION(FileError, EOF == r, mUngetc('+', patch));
        return EOF;
      }
      c = '+';
    }
    r = putc(c, to);
    ERROR_CONDITION(FileError, EOF == r, mUngetc(c, patch));
  }
}

//int fputPatchControl(PatchControl x, FILE * f) {
//  const char *s;
//  switch (x) {
//#define SET_S(_, enumerator, string, ...) case enumerator: s = string; break;
//    PATCH_CONTROL_TABLE(TAIL, CAT, SET_S);
//    case PC_EOF: s = "ERROR: EOF"; break;
//    case PC_None: s = "ERROR: No command recognized"; break;
//#undef SET_S
//  }
//  return fputs(s, f);
//}

#define EXPECTED_CONTROL(EXPECTED) \
  ERROR_CONDITION(ParseFail_ExpectedControl, (pc = parsePatchControl(&patch)) != EXPECTED, (errorArg.patchControlDiffers = (PatchControlDiffers){ &patch, EXPECTED, pc}))

#define UNEXPECTED_CONTROL(...) \
  ERROR_SET(ParseFail_UnexpectedControl, (errorArg.unexpectedPatchControl = (UnexpectedPatchControl){ &patch, pc, #__VA_ARGS__ }))
    
PatchControl parsePatchControl(MFile CP f) {
  static const int numPrefixes = PATCH_CONTROL_TABLE(TAIL, PLUS_INTER, HEAD, 1);
  int notNullCount = numPrefixes,
      i,
      c;
  const char *ps[] = { PATCH_CONTROL_TABLE(TAIL, COMMA_INTER, THIRD, ) };
  ASSERT_MFILE(f);
  while (notNullCount > 0) {
    c = mGetc(f);
    if (EOF == c) return PC_EOF;
    for(i = 0; i < numPrefixes; ++i) {
      if (ps[i] != NULL) {
        if (c == *ps[i]) {
          ++ps[i];
          if ('\0' == *ps[i])
            return i + 1;
        }
        else {
          --notNullCount;
          ps[i] = NULL;
        }
      }
    }
  }
  c = mUngetc(c, f);
  if (EOF == c)
    return PC_EOF;
  return PC_None;
}

#ifndef INDEX_MAX
#define INDEX_MAX 256
#endif
typedef struct {
  char pathA[PATH_MAX],
       pathB[PATH_MAX];
  char indexA[INDEX_MAX],
       indexB[INDEX_MAX];
  uint32_t mode;
} GitHeader;

#define FORMAT_GIT_HEADER(gh, ...) \
  "diff --git a/%s b/%s\nindex %s..%s %d\n", (gh).pathA, (gh).pathB, (gh).indexA, (gh).indexB, (gh).mode

typedef struct {
  char pathMinus[PATH_MAX],
       pathPlus[PATH_MAX];
} DiffHeader;

#define FORMAT_DIFF_HEADER(dh, ...) \
  "--- a/%s\n+++ b/%s\n", (dh).pathMinus, (dh).pathPlus

#define PLM(f, msg) \
  errorArg.pathLineMsg = (PathLineMsg){ f.path, f.line - 1, msg }

#define PARSE_BUF_SIZE BUFSIZ + 2 * MAX(PATH_MAX, INDEX_MAX)
static char parseBuf[PARSE_BUF_SIZE];

#define PARSE_LINE(errorId, numberOfParsedParameters, formatString, errorMessage, ...) \
  s = mGets(parseBuf, PARSE_BUF_SIZE, &patch); \
  if (NULL == s) { \
    PLM(patch, "Failed to get line when parsing."); \
    ERROR_CHECK(FileError); \
  } \
  if (*(s - 1) != '\n') { \
    errorArg.pathLine = (PathLine){ patch.path, patch.line - 1 }; \
    ERROR_CHECK(ParseFail_BufferOverflow); \
  } \
  i = sscanf(parseBuf, formatString, __VA_ARGS__); \
  if (i < numberOfParsedParameters) { \
    PLM(patch, errorMessage); \
    ERROR_CHECK(errorId); \
  }

typedef struct {
  int start,
      end;
} StartEnd;

typedef struct {
  StartEnd minus,
           plus;
} HunkHeader;

#define FORMAT_HUNK_HEADER(h) \
  "@@ -%d,%d +%d,%d @@\n", (h).minus.start, (h).minus.end, (h).plus.start, (h).plus.end

ErrorId copyRest(MFile CP from, FILE CP to) {
  size_t r, w;
  ASSERT_MFILE(from);
  ASSERT_FILE(to);
  do {
    r = fread(parseBuf, sizeof(char), BUFSIZ, from->stream);
    w = fwrite(parseBuf, sizeof(char), r, to);
    ERROR_CONDITION(FileError, r != w, );
  } while (BUFSIZ == r);
  ERROR_CONDITION(FileError, ferror(from->stream) || ferror(to), );
  return Success;
}

typedef union {
  const char *message,
             *path;
  GitHeader *gitHeader;
  DiffHeader *diffHeader;
  HunkHeader *hunkHeader;
} LogArg;
LogArg logArg;

#define CASE_LOG(_, id, __, action) \
  case id: \
    action; \
    break;
void _log(LogId l) {
  switch (l) {
    LOG_TABLE(CAT, CASE_LOG)
    default:
      WARN_PRINT("Unknown log id: %d\n", l);
      break;
  }
}

ErrorId tmpFile(FILE **tmp, char CP srcMutable, char CP tmpPath, size_t tmpSize) {
#ifdef _WIN32
#error "Implement tmpFilePath for windows"
#else
  assert(srcMutable != NULL);
  assert(tmpPath != NULL);
  assert(PATH_MAX == tmpSize);
  char *baseName = NULL,
       *extension = NULL;
  int tmpFileIndex;
  size_t unindexedCharCount,
         indexedCharCount;
  ErrorId r;

  *tmpPath = '\0';

  baseName = strrchr(srcMutable, '/');
  if (NULL == baseName)
    baseName = srcMutable;
  else
    *baseName++ = '\0';

  extension = strrchr(baseName, '.');
  
  unindexedCharCount = snprintf(
    tmpPath,
    tmpSize,
    "%s/%s_%s.%s",
    /* dir_path */
     baseName == srcMutable ? "." : srcMutable,
    /* baseName */
    '\0' == *baseName ? "tmpfile" : baseName,
    /* extension */
    NULL == extension || extension == baseName
      ? "" : extension,
    /* tmp extesnion */
    "tmp"
    );
  if (unindexedCharCount >= tmpSize) {
    r = TmpPathBufferOverflow;
    goto cleanup;
  }
  tmpSize -= unindexedCharCount;
  for (tmpFileIndex = 0; tmpFileIndex <= INT_MAX; ++tmpFileIndex) {
    indexedCharCount = snprintf(tmpPath + unindexedCharCount, tmpSize, "%d", tmpFileIndex);
    if (indexedCharCount >= tmpSize) {
      r = TmpPathBufferOverflow;
      goto cleanup;
    }
    *tmp = fopen(tmpPath, "r");
    if (NULL == *tmp) {
      *tmp = fopen(tmpPath, "w");
      r = NULL == tmp ? UnsuccessfulWriteOpen : Success;
      goto cleanup;
    }
    else if (fclose(*tmp)) {
      r =  UnsuccessfulFileClose;
      goto cleanup;
    }
  }
  strcpy(tmpPath + indexedCharCount, "0");
  r = CouldNotOpenTmpFile;

cleanup:
  *srcMutable = '\0';
  if (r != Success)
    errorArg.path = tmpPath;
  return r;
#endif /* _WIN32 */
}

#ifndef MAIN
#define MAIN

#undef ERROR_CHECK
#define ERROR_CHECK(e) \
  error((e), &src, &patch, tmp)
#undef ERROR_SET
#define ERROR_SET(e, argAssignments) \
  argAssignments; ERROR_CHECK(e)
#undef ERROR_CONDITION
#define ERROR_CONDITION(e, condition, argAssignments) \
  if (condition) { ERROR_SET(e, argAssignments); }
#define CASE_ERROR(_, e, __, action) \
  case e: action; break;

void error(ErrorId e, MFile CP src, MFile CP patch, FILE CP tmp) {
  switch (e) {
    ERROR_TABLE(CAT, CASE_ERROR);
    default:
      fprintf(stderr, "ERROR: Unknown error code: %d\n", e);
      e = UnknownError;
      break;
  }
  if (src && src->stream != NULL) fclose(src->stream);
  if (patch && patch->stream != NULL) fclose(patch->stream);
  if (tmp != NULL) fclose(tmp);
  exit(e);
}
int main(const int argc, const char **argv) {
  int i;
  const char *s;
  ErrorId e;
  const char *patchPath = NULL;
  MFile src = {0},
        patch = {0};
  const char *srcPathConst;
  char       *srcPathMut;
  char tmpPath[PATH_MAX];
  FILE *tmp = NULL;
  PatchControl pc;
  ParseState ps;
  GitHeader gh = {0};
  DiffHeader dh = {0};
  HunkHeader hh = {0};

  for(i = 1; i < argc - 1; ++i) {
    s = argv[i];
    if ('-' == *s++) {
      for (; *s != '\0'; s++)
        switch (*s) {
          case 'v':
            logLevel++;
            break;
          case 'q':
            logLevel = LogNone;
            break;
          default:
            ERROR_SET(UndefinedFlag, errorArg.undefinedFlag.c = *s; errorArg.undefinedFlag.argString = argv[i]);
            break;
        }
    }
  }
  logWarningIf(L_TooVerbose, logLevel > LOG_MAX, );
  /* Patch path is the last argument.
   * If `-` then stdin is used. */
  if (argc - 1 == i) {
    patchPath = argv[i];
  }
  ERROR_CONDITION(MissingPatchFileCommandArgument, NULL == patchPath, )
  else if (!strcmp(patchPath, "-"))
    streamFile(&patch, stdin, "stdin");
  else OPEN_READ(patch, patchPath);
  //else ERROR_CONDITION(BadPatchFilePath, openFile(&patch, patchPath), errorArg.path = patchPath);
  logVerbose(L_PatchPath, logArg.path = patch.path);

  while (ps) {
    TODO("Add logging");
    switch (ps) {
      case PS_Start:
        pc = parsePatchControl(&patch);
        switch (pc) {
          case PC_Git: ps = PS_Git; break;
          case PC_Minus: ps = PS_Diff; break;
          default:
            UNEXPECTED_CONTROL(PC_Git, PC_Minus);
            break;
        }
        break;
      case PS_Git:
        PARSE_LINE(ParseFail_GitHeader, 2, "a/%s b/%s\n", "Expected format: `diff --git a/<pathA> b/<pathB>\\n`", gh.pathA, gh.pathB);
        PARSE_LINE(ParseFail_GitHeader_Index, 3, "index %[0-9a-f]..%[0-9a-f] %d\n", "Expected format: `index <indexA>..<indexB> <mode>\\n`", gh.indexA, gh.indexB, &gh.mode);
        logDebug(L_GitHeader, logArg.gitHeader = &gh);
        EXPECTED_CONTROL(PC_Minus);
        ps = PS_Diff;
      case PS_Diff:
        PARSE_LINE(ParseFail_DiffHeader_PathMinus, 1, "--- a/%s\n", "Expected format: `--- a/<path>\\n`", dh.pathMinus);
        PARSE_LINE(ParseFail_DiffHeader_PathPlus, 1, "+++ b/%s\n", "Expected format: `--- b/<path>\\n`", dh.pathPlus);
        srcPathConst = dh.pathMinus;
        ERROR_CONDITION(
          DifferingSourceUpdatePaths,
          strncmp(srcPathConst, dh.pathPlus, PATH_MAX) || (*gh.pathA && (strncmp(srcPathConst, gh.pathA, PATH_MAX) || strncmp(srcPathConst, gh.pathB, PATH_MAX))),
          errorArg.pathsAB = ((PathsAB){ dh.pathMinus, dh.pathPlus })
          );
        srcPathMut = dh.pathPlus;
        logInfo(L_SourcePath, logArg.path = srcPathConst);
        OPEN_READ(src, srcPathConst);
        ERROR_CHECK(tmpFile(&tmp, srcPathMut, tmpPath, sizeof(tmpPath)));
        EXPECTED_CONTROL(PC_Hunk);
        ps = PS_Hunk;
      case PS_Hunk:
        PARSE_LINE(ParseFail_HunkHeader, 4, "-%d,%d +%d,%d @@\n", "Hunk Format: `@@ -<old line start>,<old line end> +<new line start>,<new line end> @@\\n", &hh.minus.start, &hh.minus.end, &hh.plus.start, &hh.plus.end);
        logDebug(L_HunkHeader, logArg.hunkHeader = &hh);
        e = advanceToLineCopy(&src, tmp, hh.minus.start);
        if (EOF == e) {
          ps = PS_EOF;
          break;
        }
        ERROR_CHECK(e);
        ps = PS_Match;
      case PS_Match:
        e = matchAndCopy(&src, &patch, tmp);
        switch (e) {
          case Success:
            pc = parsePatchControl(&patch);
            switch (pc) {
              case PC_AddStart:
                ps = PS_Add;
                break;
              case PC_RmStart:
                ps = PS_Remove;
                break;
              case PC_Hunk:
                ps = PS_Hunk;
              case PC_Git:
                ps = PS_Git;
                break;
              case PC_EOF:
                ps = PS_EOF;
                break;
              case PC_None:
                ERROR_SET(MissMatch, (errorArg.sourceAndPatch = (SourceAndPatch){&src, &patch}));
                break;
              case PC_RmEnd:
              case PC_AddEnd:
              case PC_Minus:
              case PC_Plus:
              case PC_Index:
                ERROR_SET(ParseFail_InvalidControlInState, (errorArg.stateControl = (StateControl) { &patch, ps, pc }));
                break;
            }
            break;
          case EOF:
            ps = PS_EOF;
            break;
          default:
            ERROR_CHECK(e);
            break;
        }
        break;
      case PS_Add:
        e = copyUntilClose(&patch, tmp);
        if (EOF == e)
          ps = PS_EOF;
        else {
          ERROR_CHECK(e);
          ps = PS_Match;
        }
        break;
      case PS_Remove:
        e = matchAndDiscardUntilClose(&src, &patch);
        if (EOF == e)
          ps = PS_EOF;
        else {
          ERROR_CHECK(e);
          ps = PS_Match;
        }
       break;
      case PS_EOF:
        if (EOF == pc) {
					TODO("PS_EOF and EOF == pc");
          ps = PS_FinalizeSource;
        } else if (feof(src.stream)) {
          TODO("PS_EOF src is EOF. "
               "Check if patch is done matching on src. "
               "If done, move tmp to src, "
               "otherwise error. "
              );
          pc = parsePatchControl(&patch);
          switch (pc) {
            case PC_EOF: ps = PS_EOF; break;
            case PC_Git: ps = PS_Git; break;
            case PC_Minus: ps = PS_Diff; break;
            default: UNEXPECTED_CONTROL(PC_EOF, PC_Git, PC_Minus);
          }
        } else {
          ERROR_SET(UndefinedBehavior, errorArg.msg = "receiving state of PS_EOF when neither file handles for source nor patch are EOF");
        }
        break;
      case PS_FinalizeSource:
        ERROR_CHECK(copyRest(&src, tmp));
        ERROR_CHECK(closeFile(&src));
        ERROR_CONDITION(UnsuccessfulFileClose, fclose(tmp), errorArg.path = tmpPath);
#ifdef _WIN32
          /* Windows requires the destination file to not exist */
          ERROR_CONDITION(FileError, remove(srcPathConst) && errno != ENOENT, errorArg.path = srcPathConst);
#endif /* _WIN32 */
        ERROR_CONDITION(FileError, rename(tmpPath, srcPathConst), errorArg.path = srcPathConst);
        TODO("PS_FinalizeSource set ending state");
        //ps = TODO;
        break;
      case PS_End:
        ERROR_CHECK(closeFile(&patch));
        ps = PS_ExitLoop;
        break;
      case PS_ExitLoop:
        ERROR_SET(UndefinedBehavior, errorArg.msg = "PS_ExitLoop should never be matched");
        break;
    }
  }

  printf("Done");
  return 0;
}
#endif /* MAIN */

