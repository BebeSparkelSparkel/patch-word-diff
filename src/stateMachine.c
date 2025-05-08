#include <string.h>

#include "parse.h"
#include "mfile.h"
#include "error.h"
#include "headers.h"
#include "stateMachine.h"
#include "tmpfile.h"
#include "transfer.h"

char parseBuf[PARSE_BUF_SIZE];

enum ErrorId stateMachine(struct MFile CP patch, struct MFile CP src, FILE * CP tmp, char *tmpPath) {
  int i;
  const char *s;
  enum ErrorId e;
  enum PatchControl pc;
  enum ParseState ps = PS_Start;
  static struct GitHeader  gh = {0};
  static struct DiffHeader dh = {0};
  static struct HunkHeader hh = {0};
  const char *srcPath;
  const int constTmpPath = tmpPath[0];

  while (ps) {
    switch (ps) {
      case PS_Start:
        log(L_ParseState, logArg.parseState = ps);
        pc = parsePatchControl(patch);
        switch (pc) {
          case PC_Git: ps = PS_Git; break;
          case PC_Minus: ps = PS_Diff; break;
          case PC_EOF:
            ERROR_SET(EmptyPatchFile, errorArg.path = patch->path);
            break;
          case PC_FileError:
            ERROR_SET(FileError, errorArg.path = patch->path);
            break;
          default:
            UNEXPECTED_CONTROL(PC_Git, PC_Minus);
            break;
        }
        break;
      case PS_Git:
        log(L_ParseState, logArg.parseState = ps);
        PARSE_GIT_HEADER;
        log(L_GitHeader, logArg.gitHeader = &gh);
        EXPECTED_CONTROL(PC_Minus);
        ps = PS_Diff;
        FALLTHROUGH;
      case PS_Diff:
        log(L_ParseState, logArg.parseState = ps);
        PARSE_DIFF_HEADER;
        srcPath = dh.pathMinus;
        ERROR_CONDITION(
          DifferingSourceUpdatePaths,
          strncmp(srcPath, dh.pathPlus, PATH_MAX) || (*gh.pathA && (strncmp(srcPath, gh.pathA, PATH_MAX) || strncmp(srcPath, gh.pathB, PATH_MAX))),
          errorArg.pathsAB = ((struct PathsAB){ dh.pathMinus, dh.pathPlus })
          );
        log(L_SourcePath, logArg.path = srcPath);
        OPEN_READ(src, srcPath);
        if (constTmpPath) {
          *tmp = fopen(tmpPath, "w");
          ERROR_CONDITION(UnsuccessfulWriteOpen, NULL == *tmp, errorArg.path = tmpPath);
        } else
          ERROR_CHECK(tmpFile(tmp, srcPath, tmpPath, "tmp"));
        EXPECTED_CONTROL(PC_Hunk);
        ps = PS_Hunk;
        FALLTHROUGH;
      case PS_Hunk:
        log(L_ParseState, logArg.parseState = ps);
        ERROR_CHECK(parseHunkHeader(patch, &hh));
        log(L_HunkHeader, logArg.hunkHeader = &hh);
        e = advanceToLineCopy(src, *tmp, tmpPath, hh.minus.start);
        if (EOF == e) {
          ps = PS_EOF;
          break;
        }
        ERROR_CHECK(e);
        ps = PS_Match;
        FALLTHROUGH;
      case PS_Match:
        log(L_ParseState, logArg.parseState = ps);
        e = matchAndCopy(src, patch, *tmp, tmpPath);
        switch (e) {
          case Success:
            pc = parsePatchControl(patch);
            switch (pc) {
              case PC_AddStart: ps = PS_Add;    break;
              case PC_RmStart:  ps = PS_Remove; break;
              case PC_Hunk:     ps = PS_Hunk;   break;
              case PC_Git:      ps = PS_Git;    break;
              case PC_EOF:      ps = PS_EOF;    break;
              case PC_FileError:
                ERROR_SET(FileError, errorArg.path = patch->path);
                break;
              case PC_None:
                ERROR_SET(MissMatch, (errorArg.sourceAndPatch = (struct SourceAndPatch){src, patch}));
                break;
              case PC_RmEnd:
              case PC_AddEnd:
              case PC_Minus:
              case PC_Plus:
              case PC_Index:
                ERROR_SET(ParseFail_InvalidControlInState, (errorArg.stateControl = (struct StateControl){ patch, ps, pc }));
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
        log(L_ParseState, logArg.parseState = ps);
        e = copyUntilClose(patch, *tmp, tmpPath);
        if (EOF == e)
          ps = PS_EOF;
        else {
          ERROR_CHECK(e);
          ps = PS_Match;
        }
        break;
      case PS_Remove:
        log(L_ParseState, logArg.parseState = ps);
        e = matchAndDiscardUntilClose(src, patch);
        if (EOF == e)
          ps = PS_EOF;
        else {
          ERROR_CHECK(e);
          ps = PS_Match;
        }
       break;
      case PS_EOF:
        log(L_ParseState, logArg.parseState = ps);
        if (EOF == pc) {
          ps = PS_FinalizeSource;
          break;
        } else if (feof(src->stream)) {
          pc = parsePatchControl(patch);
          switch (pc) {
            case PC_EOF:   ps = PS_FinalizeSource; break;
            case PC_Git:   ps = PS_Git; break;
            case PC_Minus: ps = PS_Diff; break;
            case PC_FileError:
              ERROR_SET(FileError, errorArg.path = patch->path);
              break;
            default: UNEXPECTED_CONTROL(PC_EOF, PC_Git, PC_Minus);
          }
        } else {
          ERROR_SET(UndefinedBehavior, errorArg.msg = "receiving state of PS_EOF when neither file handles for source nor patch are EOF");
        }
        break;
      case PS_FinalizeSource:
        log(L_ParseState, logArg.parseState = ps);
        ERROR_CHECK(copyRest(src, *tmp, tmpPath));
#ifdef _WIN32
        /* Windows requires the destination file to not exist */
        {
          char backupPath[PATH_MAX];
          ERROR_CHECK(tmpFile(NULL, srcPath, backupPath, "BAK"));
          ERROR_CONDITION(RenameFile, rename(srcPath, backupPath), errorArg.pathsAB = ((struct PathsAB){srcPath, backupPath}));
          ERROR_CONDITION(RenameFile, rename(tmpPath, srcPath), errorArg.pathsAB = ((struct PathsAB){tmpPath, srcPath}));
          ERROR_CONDITION(RemoveFile, remove(backupPath), errorArg.path = backupPath);
        }
#else
        ERROR_CONDITION(RenameFile, rename(tmpPath, srcPath), errorArg.pathsAB = ((struct PathsAB){tmpPath, srcPath}));
#endif /* _WIN32 */
        ps = PS_End;
        FALLTHROUGH;
      case PS_End:
        log(L_ParseState, logArg.parseState = ps);
        ERROR_CHECK(closeFile(patch));
        ps = PS_ExitLoop;
        break;
      case PS_ExitLoop:
        ERROR_SET(UndefinedBehavior, errorArg.msg = "PS_ExitLoop should never be matched");
        break;
    }
  }
  return Success;
}
