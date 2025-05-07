#include <string.h>
#include <assert.h>

#include "mfile.h"
#include "error.h"

#define _SELECT_COLUMNS_MFILE_TABLE_OpenInit_Field  _SELECT_COLUMNS_4_1
#define _SELECT_COLUMNS_MFILE_TABLE_CloseInit_Field _SELECT_COLUMNS_5_1
#define _SELECT_COLUMNS_MFILE_TABLE_Field_CloseInit _SELECT_COLUMNS_1_5

void streamFile(struct MFile CP f, FILE *stream, FP(char) path) {
  assert(NULL != f);
  assert(NULL != path);
  assert(PATH_MAX > strlen(path));
  assert(NULL != stream);
  assert(!feof(stream));
  assert(!ferror(stream));
#define _ASSIGN_STREAM(x, ...) x = STREAM
#define STREAM stream
  MFILE_TABLE(END_EXPRESSION_INTER, COMPOSE3, EXPAND_ARG(APPLY), EXPAND_ARG(SECOND(DREF_FROM(f))), SELECT_COLUMNS(MFILE_TABLE, OpenInit, Field));
#undef STREAM
}

static void markClosed(struct MFile CP f) {
#define VALUE ASSIGN
#define VALUE_DEREF ASSIGN_DEREF
  MFILE_TABLE(END_EXPRESSION_INTER, COMPOSE3, EXPAND_ARG(APPLY), EXPAND_ARG(SECOND(DREF_FROM(f))), SELECT_COLUMNS(MFILE_TABLE, CloseInit, Field));
#undef VALUE
#undef VALUE_DEREF
}

enum ErrorId closeFile(struct MFile CP f) {
  ASSERT_MFILE(f);
  ERROR_CONDITION(UnsuccessfulFileClose, fclose(f->stream), errorArg.path = f->path);
  markClosed(f);
  return Success;
}

int isClosed(FP(struct MFile) f) {
#define VALUE EQUIVALENT
#define VALUE_DEREF EQUIVALENT_DEREF
  return MFILE_TABLE(AND_INTER, COMPOSE3, EXPAND_ARG(APPLY), EXPAND_ARG(SECOND(DREF_FROM(f))), SELECT_COLUMNS(MFILE_TABLE, CloseInit, Field));
#undef VALUE
#undef VALUE_DEREF
}

static int updatePosition(int c, struct MFile CP f) {
  if ('\n' == c) {
    ++f->line;
    f->column = 1;
  }
  else EOF_MFILE_CHECK(c, f)
  else ++f->column;
  return c;
}

int mGetc(struct MFile CP f) {
  int c;
  ASSERT_MFILE(f);
  c = 0 <= f->ungetI
    ? f->ungetBufBackup[f->ungetI--]
    : getc(f->stream);
  return updatePosition(c, f);
}

char *mGets(char CP str, const int size, struct MFile CP f) {
  int i = 0;
  char *e;
  while (0 <= f->ungetI && i < size - 1) {
    int c = f->ungetBufBackup[f->ungetI--];
    assert(0 < c);
    updatePosition(c, f);
    str[i++] = c;
  }
  str[i] = '\0';
  if (size - 1 - i > 0) {
    e = fgets(&str[i], size - i, f->stream);
    if (NULL == e && ferror(f->stream)) {
      while (0 < i && mUngetc(str[--i], f) > 0);
      return NULL;
    }
    while ('\0' != str[i]) {
      int r;
      r = updatePosition(str[i++], f);
      assert(0 <= r);
    }
  }
  return 0 < i ? str : NULL;
}

static int ungetBackup(int c, struct MFile CP f) {
  if (UNGET_BUF_SIZE > f->ungetI) {
    --f->column;
    return f->ungetBufBackup[f->ungetI++] = c;
  }
  {
    /* attempted recovery if buffer full */
    int i, r;
    for (i = 0; i <= f->ungetI; ++i) {
      r = ungetc(f->ungetBufBackup[i], f->stream);
      if (EOF == r)
        break;
    }
    /*
    Example Logic
    Indicies:             | 0 | 1 | 2 | 3 |
    3 chars:              | A | B | C |   |
    f->ungetI = 2:                  X
    i ungets 2 so i == 2: | X | X |   |   |
    f->ungetI + 1 - i == 1 chars need to be shifted to the beginning (f->ungetBufBackup) of the buffer
      starting from index i
    */
    if (0 < i) {
      if (f->ungetI >= i)
        memmove(f->ungetBufBackup, f->ungetBufBackup + i, (f->ungetI + 1 - i) * sizeof(char));
      f->ungetI -= i;
      assert(-1 <= f->ungetI);
      return mUngetc(c, f);
    }
  }
  return EOF;
}

int mUngetc(const int c, struct MFile CP f) {
  int r;
  ASSERT_MFILE(f);
  assert('\n' != c);
  assert(EOF != c);
  assert('!' <= c);
  assert('~' >= c);
  if (0 <= c) {
    if (0 > f->ungetI) {
      r = ungetc(c, f->stream);
      if (EOF == r)
        return ungetBackup(c, f);
      --f->column;
      return r;
    }
    return ungetBackup(c, f);
  }
  return c;
}
