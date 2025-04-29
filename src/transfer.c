#include <assert.h>

#include "transfer.h"
#include "mfile.h"

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

