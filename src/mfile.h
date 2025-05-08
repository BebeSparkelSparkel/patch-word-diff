#ifndef MFILE_H
#define MFILE_H

#include <limits.h>
#include <assert.h>

#include "error.h"
#include "cpp.h"
#include "patchControl.h"

#define SIZEOF(x) sizeof(x)

#define UNGET_BUF_SIZE PATCH_CONTROL_TABLE(TAIL, MAX, COMPOSE, SIZEOF, SND)

/* FILE Error Check */
#define EOF_FILE_CHECK(x, f, PATH) \
  if (x < 0) { \
    ERROR_CONDITION(FileError, ferror(f), errorArg.path = PATH); \
    return EOF; \
  }
#define EOF_MFILE_CHECK(x, f) EOF_FILE_CHECK(x, f->stream, f->path)

#define MF_ERROR_CHECK(f) \
  ferror((f)->stream)


#define MFILE_TABLE(cons, map, ...) \
  /*                  |        Field  |  Type | Array (Optional) |        OpenInit        |    CloseInit     | */ \
  cons(map(__VA_ARGS__,         stream, FILE *,                  ,          ASSIGN(STREAM),    VALUE(NULL) ), \
  cons(map(__VA_ARGS__,           line,    int,                  ,               ASSIGN(1), VALUE(INT_MIN) ), \
  cons(map(__VA_ARGS__,         column,    int,                  ,               ASSIGN(1), VALUE(INT_MIN) ), \
  cons(map(__VA_ARGS__,         ungetI,    int,                  ,           ASSIGN_NEG(1), VALUE(INT_MIN) ), \
  cons(map(__VA_ARGS__, ungetBufBackup,   char,  [UNGET_BUF_SIZE],                   EMPTY,   VALUE(EMPTY) ), \
       map(__VA_ARGS__,           path,   char,        [PATH_MAX], STRNCPY(path, PATH_MAX), VALUE_DEREF(0) ) \
      )))))

struct MFile {
  MFILE_TABLE(END_EXPRESSION_INTER, COMPOSE, EXPAND_ARG(CAT3), FLIP);
};

#define ASSERT_MFILE(f) \
  assert(f != NULL); \
  ASSERT_FILE(f->stream); \
  assert(f->line > 0); \
  assert(f->column > 0); \
  assert(f->ungetI >= -1)
#define ASSERT_FILE(f) \
  assert(f != NULL); \
  assert(!ferror(f)); \
  assert(!feof(f))

void initMFile(struct MFile CP f);

void streamFile(struct MFile CP f, FILE *stream, FP(char) path);

#define OPEN_READ(f, path) OPEN_MODE(f, path, "r")
/*
#define OPEN_WRITE(stream, PATH) \
  ERROR_CONDITION(BadPath, PATH == NULL || *PATH == '\0', ); \
  stream = fopen(PATH, "w"); \
  ERROR_CONDITION(UnsuccessfulWriteOpen, NULL == stream, errorArg.path = PATH);
  */
#define OPEN_MODE(f, PATH, mode) \
  { \
    FILE *stream; \
    assert(isClosed(f)); \
    ERROR_CONDITION(BadPath, NULL == PATH || '\0' == *PATH, ); \
    stream = fopen(PATH, mode); \
    ERROR_CONDITION(UnsuccessfulReadOpen, NULL == stream, errorArg.path = PATH); \
    streamFile(f, stream, PATH); \
  }

enum ErrorId closeFile(struct MFile CP f);

int isClosed(FP(struct MFile) f);


int mGetc(struct MFile CP f);

char *mGets(char *str, int size, struct MFile CP f);

int mUngetc(const int c, struct MFile CP f);


#endif
