#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "cpp.h"
#include "headers.h"
#include "mfile.h"
#include "transfer.h"
#include "todo.h"
#include "parse.h"
#include "tmpfile.h"

#define PARSE_BUF_SIZE BUFSIZ + 2 * MAX(PATH_MAX, INDEX_MAX)
char parseBuf[PARSE_BUF_SIZE];

ErrorArg errorArg;
LogLevel logLevel = LogWarning;
LogId logId;
LogArg logArg;
char werror = 0;

#define IN_MAIN_FUNCTION
#include "error.h"

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
          case 'w':
            werror = 1;  // Set warnings as errors
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
        PARSE_GIT_HEADER;
        logDebug(L_GitHeader, logArg.gitHeader = &gh);
        EXPECTED_CONTROL(PC_Minus);
        ps = PS_Diff;
      case PS_Diff:
        PARSE_DIFF_HEADER;
        srcPathConst = dh.pathMinus;
        ERROR_CONDITION(
          DifferingSourceUpdatePaths,
          strncmp(srcPathConst, dh.pathPlus, PATH_MAX) || (*gh.pathA && (strncmp(srcPathConst, gh.pathA, PATH_MAX) || strncmp(srcPathConst, gh.pathB, PATH_MAX))),
          errorArg.pathsAB = ((PathsAB){ dh.pathMinus, dh.pathPlus })
          );
        srcPathMut = dh.pathPlus;
        logInfo(L_SourcePath, logArg.path = srcPathConst);
        OPEN_READ(src, srcPathConst);
        ERROR_CHECK(tmpFile(&tmp, srcPathMut, tmpPath, sizeof(tmpPath), "tmp"));
        EXPECTED_CONTROL(PC_Hunk);
        ps = PS_Hunk;
      case PS_Hunk:
        PARSE_HUNK_HEADER;
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
        ERROR_CHECK(copyRest(&src, tmp, tmpPath));
#ifdef _WIN32
        /* Windows requires the destination file to not exist */
        {
          char backupPath[PATH_MAX];
          strcpy(backupPath, tmpPath);
          strcpy(strrchr(backupPath, '.') + 1, "BAK"); /* set extension to backup */
          ERROR_CHECK(tmpFile(&tmp, srcPathMut, backupPath, sizeof(backupPath), "BAK"));
          ERROR_CONDITION(RenameFile, rename(srcPathConst, backupPath), errorArg.pathsAB = ((PathsAB){srcPathConst, backupPath}));
          ERROR_CONDITION(RenameFile, rename(tmpPath, srcPathConst), errorArg.pathsAB = ((PathsAB){tmpPath, srcPathConst}));
          ERROR_CONDITION(RemoveFile, remove(backupPath), errorArg.path = backupPath);
        }
#else
        ERROR_CONDITION(RenameFile, rename(tmpPath, srcPathConst), errorArg.pathsAB = ((PathsAB){tmpPath, srcPathConst}));
#endif /* _WIN32 */
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
#undef IN_MAIN_FUNCTION

