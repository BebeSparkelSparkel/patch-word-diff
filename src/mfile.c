#include <string.h>
#include <assert.h>

#include "mfile.h"
#include "error.h"

#define _SELECT_COLUMNS_MFILE_TABLE_OpenInit_Field  _SELECT_COLUMNS_4_1
#define _SELECT_COLUMNS_MFILE_TABLE_CloseInit_Field _SELECT_COLUMNS_5_1
#define _SELECT_COLUMNS_MFILE_TABLE_Field_CloseInit _SELECT_COLUMNS_1_5

void streamFile(struct MFile CP f, FILE *stream, FP(char) path) {
  assert(f != NULL);
  assert(path != NULL);
  assert(stream != NULL);
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
  c = f->ungetI >= 0
    ? f->ungetBuf[f->ungetI--]
    : getc(f->stream);
  return updatePosition(c, f);
}

char *mGets(char *str, int size, struct MFile CP f) {
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

int mUngetc(const int c, struct MFile CP f) {
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
