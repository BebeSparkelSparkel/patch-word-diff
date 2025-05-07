#include <assert.h>
#include <ctype.h>
#include <unistd.h>

#include "transfer.h"
#include "mfile.h"

/* Used for copying the unmodified source until the beginning of the hunk */
enum ErrorId advanceToLineCopy(struct MFile CP from, FILE CP to, FP(char) toPath, const int targetLine) {
  int c, r;
  ASSERT_MFILE(from);
  ASSERT_FILE(to);
  assert(targetLine > 0);
  assert(from->line <= targetLine);
  ERROR_CONDITION(CannotFindLineInUpdateFile, from->line > targetLine, errorArg.pathLine = ((struct PathLine){ from->path, targetLine }));
  while (from->ungetI >= 0 && from->line < targetLine) {
    c = from->ungetBufBackup[from->ungetI--];
    if ('\n' == c) ++from->line;
    r = putc(c, to);
    EOF_FILE_CHECK(r, to, toPath);
  }
  while (from->line < targetLine) {
    c = getc(from->stream);
    EOF_MFILE_CHECK(c, from);
    if ('\n' == c) ++from->line;
    r = putc(c, to);
    EOF_FILE_CHECK(r, to, toPath);
  }
  from->column = 1;
  return Success;
}

/* Used to copy unmodified source and ensuring it matches the text specified by the patch */
enum ErrorId matchAndCopy(struct MFile CP src, struct MFile CP patch, FILE CP to, FP(char) toPath) {
  int sc, pc, e;
  ASSERT_MFILE(src);
  ASSERT_MFILE(patch);
  ASSERT_FILE(to);
  while(1) {
    sc = mGetc(src);
    pc = mGetc(patch);
    if (EOF == sc || EOF == pc)
      goto eofHandler;
    if (sc != pc) {
      if (isspace(sc) && isspace(pc)) {
        do {
          e = putc(sc, to);
          ERROR_CONDITION(FileError, EOF == e, errorArg.path = toPath; mUngetc(sc, src));
          sc = mGetc(src);
        } while(isspace(sc));
        ERROR_CONDITION(FileError, EOF == sc && MF_ERROR_CHECK(src), errorArg.path = src->path);
        do {
          pc = mGetc(patch);
        } while(isspace(pc));
        ERROR_CONDITION(FileError, EOF == pc && MF_ERROR_CHECK(patch), errorArg.path = patch->path);
        if (EOF == sc || EOF == pc)
          goto eofHandler;
      }
      sc = mUngetc(sc, src);
      pc = mUngetc(pc, patch);
      /* error checks should be after both ungets so each is attempted */
      ERROR_CONDITION(FileError, EOF == sc, errorArg.path = src->path);
      ERROR_CONDITION(FileError, EOF == pc, errorArg.path = patch->path);
      return Success;
    }
    e = putc(sc, to);
    ERROR_CONDITION(FileError, EOF == e, errorArg.path = toPath; mUngetc(sc, src));
  }
  ERROR_SET(UndefinedBehavior, errorArg.msg = "matchAndCopy loop unexpectedly exited");
  eofHandler:
  if (EOF == sc && EOF == pc) {
    ERROR_CONDITION(FileError, MF_ERROR_CHECK(src), errorArg.path = src->path);
    ERROR_CONDITION(FileError, MF_ERROR_CHECK(patch), errorArg.path = patch->path);
    return EOF;
  }
  if (EOF == sc) {
    e = mUngetc(pc, patch);
    ERROR_CONDITION(FileError, EOF == e, errorArg.path = patch->path);
    ERROR_CONDITION(FileError, MF_ERROR_CHECK(src), errorArg.path = src->path);
    return EOF;
  }
  if (EOF == pc) {
    e = mUngetc(sc, src);
    ERROR_CONDITION(FileError, EOF == e, errorArg.path = src->path);
    ERROR_CONDITION(FileError, MF_ERROR_CHECK(patch), errorArg.path = patch->path);
    return EOF;
  }
  ERROR_SET(UndefinedBehavior, errorArg.msg = "matchAndCopy eofHandler did not find EOF");
}

/* Used to remove text from source as specified by the patch */
enum ErrorId matchAndDiscardUntilClose(struct MFile CP src, struct MFile CP patch) {
  int sc, pc;
  ASSERT_MFILE(src);
  ASSERT_MFILE(patch);
  while(1) {
    sc = mGetc(src);
    pc = mGetc(patch);
    if (EOF == sc && EOF == pc) {
      ERROR_CONDITION(FileError, MF_ERROR_CHECK(src), errorArg.path = src->path);
      ERROR_CONDITION(FileError, MF_ERROR_CHECK(patch), errorArg.path = patch->path);
      return EOF;
    }
    if (EOF == sc) {
      pc = mUngetc(pc, patch);
      ERROR_CONDITION(FileError, EOF == pc, errorArg.path = patch->path);
      ERROR_CONDITION(FileError, MF_ERROR_CHECK(src), errorArg.path = src->path);
      return EOF;
    }
    if (EOF == pc) {
      sc = mUngetc(sc, src);
      ERROR_CONDITION(FileError, EOF == sc, errorArg.path = src->path);
      ERROR_CONDITION(FileError, MF_ERROR_CHECK(patch), errorArg.path = patch->path);
      return EOF;
    }
    if (sc != pc) {
      sc = mUngetc(sc, src);
      ERROR_CONDITION(FileError, EOF == sc, errorArg.path = src->path; mUngetc(pc, patch));
      if ('-' == pc) {
        pc = mGetc(patch);
        if (']' == pc)
          return Success;
        if (EOF == pc) {
          pc = mUngetc('-', patch);
          ERROR_CONDITION(FileError, EOF == pc || MF_ERROR_CHECK(patch), errorArg.path = patch->path);
          return EOF;
        }
        pc = mUngetc(pc, patch);
        ERROR_CONDITION(FileError, EOF == pc, errorArg.path = patch->path);
        pc = '-';
      }
      pc = mUngetc(pc, patch);
      ERROR_CONDITION(FileError, EOF == pc, errorArg.path = patch->path);
      ERROR_SET(MissMatch, errorArg.sourceAndPatch = ((struct SourceAndPatch){src, patch}));
    }
  }
}

/* Used for addtions to the source as specfied by the patch */
enum ErrorId copyUntilClose(struct MFile CP patch, FILE CP to, FP(char) toPath) {
  int c, r;
  ASSERT_MFILE(patch);
  ASSERT_FILE(to);
  while(1) {
    c = mGetc(patch);
    if (EOF == c) {
      ERROR_CONDITION(FileError, MF_ERROR_CHECK(patch), errorArg.path = patch->path);
      return EOF;
    }
    if ('+' == c) {
      c = mGetc(patch);
      if ('}' == c)
        return Success;
      r = putc('+', to);
      ERROR_CONDITION(FileError, EOF == r, errorArg.path = toPath; mUngetc(c, patch); mUngetc('+', patch));
      if (EOF == c)
        return EOF;
    }
    r = putc(c, to);
    ERROR_CONDITION(FileError, EOF == r, errorArg.path = toPath; mUngetc(c, patch));
  }
}

enum ErrorId copyRest(struct MFile CP from, FILE CP to, FP(char) toPath) {
  ASSERT_MFILE(from);
  ASSERT_FILE(to);
  int src_fd, to_fd, e;
  long src_pos;
  if ( (src_fd = fileno(from->stream)) != -1
    && (to_fd = fileno(to)) != -1
    && (src_pos = ftell(from->stream)) != -1
    && lseek(src_fd, src_pos, SEEK_SET) != -1
    ) {
    /* Unbufferd file transfer */
    size_t r;
    ssize_t w;
    e = fflush(to);
    ERROR_CONDITION(FileError, e, errorArg.path = toPath);
    do {
      r = read(src_fd, parseBuf, BUFSIZ);
      if (r > 0) {
        w = write(to_fd, parseBuf, r);
        ERROR_CONDITION(FileError, (size_t)w != r, errorArg.path = toPath);
      }
    } while (r == BUFSIZ);
    ERROR_CONDITION(FileError, r == (size_t)-1 || MF_ERROR_CHECK(from), errorArg.path = from->path);
    ERROR_CONDITION(FileError, ferror(to), errorArg.path = toPath);
  } else {
    /* Fallback to original buffered I/O approach without seeking */
    size_t r, w;
    do {
      r = fread(parseBuf, sizeof(char), BUFSIZ, from->stream);
      w = fwrite(parseBuf, sizeof(char), r, to);
      ERROR_CONDITION(FileError, r != w, errorArg.path = from->path);
    } while (BUFSIZ == r);
    ERROR_CONDITION(FileError, MF_ERROR_CHECK(from), errorArg.path = from->path);
    ERROR_CONDITION(FileError, ferror(to), errorArg.path = toPath);
  }
  ERROR_CHECK(closeFile(from));
  ERROR_CONDITION(UnsuccessfulFileClose, fclose(to), errorArg.path = toPath);
  return Success;
}

