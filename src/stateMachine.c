#include <string.h>
#include <ctype.h>

#include "todo.h"
#include "parse.h"
#include "mfile.h"
#include "error.h"
#include "headers.h"
#include "stateMachine.h"
#include "tmpfile.h"
#include "transfer.h"

char parseBuf[PARSE_BUF_SIZE];

#define TRANSITION_TABLE(firstOrder, secondOrder, lagged) \
  firstOrder(PS_Start, PC_EOF, PS_EmptyPatch) \
  firstOrder(PS_Start, PC_WhiteSpace, PS_EmptyPatch) \
  firstOrder(PS_Start, PC_Tab, PS_EmptyPatch) \
  firstOrder(PS_Start, PC_Space, PS_EmptyPatch) \
  firstOrder(PS_Start, PC_LineFeed, PS_EmptyPatch) \
  firstOrder(PS_Start, PC_Git, PS_Git) \
  firstOrder(PS_Git, PC_Minus, PS_Diff) \
  firstOrder(PS_Diff, PC_Hunk, PS_Hunk) \
  firstOrder(PS_Hunk, PC_AddStart, PS_Add) \
  firstOrder(PS_WhiteSpace, PC_AddStart, PS_Add) \
  firstOrder(PS_Hunk, PC_Space, PS_WhiteSpace) \
  firstOrder(PS_Add, PC_LineFeed, PS_Match) \
  firstOrder(PS_Add, PC_Space, PS_WhiteSpace) \
  secondOrder(PS_Add, PS_WhiteSpace, PC_None, PS_WriteWhiteSpace) \
  firstOrder(PS_WriteWhiteSpace, PC_None, PS_Match) \
  firstOrder(PS_Match, PC_LineFeed, PS_WhiteSpace) \
  firstOrder(PS_Match, PC_EOF, PS_FinalizeSource) \
  firstOrder(PS_WhiteSpace, PC_EOF, PS_FinalizeSource) \
  firstOrder(PS_FinalizeSource, PC_EOF, PS_ExitLoop) \

#define FIRST_ORDER(state, control, newState) \
  case TUPLE(state, control): setState(newState); break;

#define SECOND_ORDER(previousState, currentState, control, newState) \
  case TUPLE3(previousState, currentState, control): setState(newState); break;

#define LAGGED(previousState, control, newState) \
  case TUPLE(previousState, control): setState(newState); break;

#define setState(s) (prevState = state, state = (s))

#define WRITE_WHITESPACE \
  { \
    c = fputs(parseBuf, *tmp); \
    ERROR_CONDITION(FileError, EOF == c, errorArg.path = tmpPath); \
    *parseBuf = '\0'; \
  }

enum ErrorId stateMachine(struct MFile CP patch, struct MFile CP src, FILE * CP tmp, char *tmpPath) {
  enum PatchControl control;
  enum ParseState state     = PS_Start,
                  prevState = PS_Start;
  //enum WhiteSpace whitespace;
  enum ErrorId e;
  static struct GitHeader gitHeader;
  static struct DiffHeader diffHeader;
  static struct HunkHeader hunkHeader;
  int c, i;
  const char *s;
  int constTmpPath = tmpPath[0];
  const char *srcPath;

  do {
    log(L_DebugMessage, logArg.message = "---- State Loop Start ----");
    log(L_ParseState, logArg.parseState = state);
    control = parsePatchControl(patch);
    log(L_PatchControl, logArg.patchControl = control);

    /* Stateful Control */
    switch (TUPLE3(prevState, state, control)) {
      TRANSITION_TABLE(EMPTY, SECOND_ORDER, EMPTY);
      default:
        switch (TUPLE(state, control)) {
          TRANSITION_TABLE(FIRST_ORDER, EMPTY, EMPTY);
          default:
            switch (TUPLE(prevState, control)) {
              TRANSITION_TABLE(EMPTY, EMPTY, LAGGED);
              default:
                TODO("Transition Table\n"
                     "\tPrevious State: %s\n"
                     "\tCurrent  State: %s\n"
                     "\tControl:        %s\n"
                     "\tL %d, C %d\n\t",
                     parseState2enumStr(prevState),
                     parseState2enumStr(state),
                     patchControl2enumStr(control),
                     patch->line,
                     patch->column
                    );
            }
        }
    }

    /* State Action */
    log(L_ParseState, logArg.parseState = state);
    switch (state) {
      case PS_EmptyPatch:
        ERROR_SET(EmptyPatchFile, errorArg.path = patch->path);
        break;
      case PS_DropWhiteSpace:
        ERROR_CHECK(mSkipWhitespace(patch));
        break;
      case PS_Git:
        PARSE_GIT_HEADER;
        break;
      case PS_Diff:
        PARSE_DIFF_HEADER;
        srcPath = diffHeader.pathMinus;
        OPEN_READ(src, srcPath);
        if (constTmpPath) {
          *tmp = fopen(tmpPath, "w");
          ERROR_CONDITION(UnsuccessfulWriteOpen, NULL == *tmp, errorArg.path = tmpPath);
        } else
          ERROR_CHECK(tmpFile(tmp, srcPath, tmpPath, "tmp"));
        log(L_TmpPath, logArg.path = tmpPath);
        break;
      case PS_Hunk:
        ERROR_CHECK(parseHunkHeader(patch, &hunkHeader));
        ERROR_CHECK(matchAndCopy(src, patch, *tmp, tmpPath));
        *parseBuf = '\0';
        break;
      case PS_Match:
        ERROR_CHECK(matchAndCopy(src, patch, *tmp, tmpPath));
        break;
      case PS_Add:
        WRITE_WHITESPACE;
        ERROR_CHECK(copyUntilClose(patch, *tmp, tmpPath));
        break;
      case PS_WriteWhiteSpace:
        WRITE_WHITESPACE;
        break;
      case PS_WhiteSpace:
        i = 0;
        do {
          c = mGetCOrEOF(patch);
          parseBuf[i++] = c;
        } while (isspace(c) && i < sizeof(parseBuf));
        ERROR_CONDITION(ParseFail_BufferOverflow, sizeof(parseBuf) <= i, );
        parseBuf[i - 1] = '\0';
        if (EOF != c) {
          c = mUngetc(c, patch);
          ERROR_CONDITION(FileError, EOF == c, errorArg.path = patch->path);
        }
        break;
      case PS_FinalizeSource:
        WRITE_WHITESPACE;
        if (!isEOF(src))
          ERROR_CHECK(copyRest(src, *tmp, tmpPath));
        ERROR_CHECK(closeFile(src));
        //if ('\n' != lastWrittenChar)
        //  ERROR_CONDITION(FileError, '\n' != fputc('\n', *tmp), errorArg.path = tmpPath);
        ERROR_CONDITION(UnsuccessfulFileClose, fclose(*tmp), errorArg.path = tmpPath);
        //lastWrittenChar = EOF;
#ifdef _WIN32
        /* Windows requires the destination file to not exist */
        {
          char backupPath[PATH_MAX];
          ERROR_CHECK(tmpFile(NULL, srcPath, backupPath, "BAK"));
          ERROR_CONDITION(RenameFileFail, rename(srcPath, backupPath), errorArg.pathsAB = ((struct PathsAB){srcPath, backupPath}));
          ERROR_CONDITION(RenameFileFail, rename(tmpPath, srcPath), errorArg.pathsAB = ((struct PathsAB){tmpPath, srcPath}));
          ERROR_CONDITION(RemoveFileFail, remove(backupPath), errorArg.path = backupPath);
        }
#else
        ERROR_CONDITION(RenameFileFail, rename(tmpPath, srcPath), errorArg.pathsAB = ((struct PathsAB){tmpPath, srcPath}));
#endif /* _WIN32 */
        break;
      case PS_ExitLoop:
        ERROR_CHECK(closeFile(patch));
        break;
      default:
        TODO("state %s", parseState2enumStr(state)); break;
    }

    ///* Historical State */
    //switch (TUPLE(prevState, state)) {
    //  TRANSITION(PS_Start, PS_DropWhiteSpace, PS_Start);
    //  NO_TRANSITION(PS_Start, PS_Git);
    //  NO_TRANSITION(PS_Git,   PS_Diff);
    //  NO_TRANSITION(PS_Diff,  PS_Hunk);
    //  NO_TRANSITION(PS_Hunk,  PS_Add);
    //  NO_TRANSITION(PS_Hunk,  PS_WhiteSpace);
    //  NO_TRANSITION(PS_Match, PS_WhiteSpace);
    //  NO_TRANSITION(PS_Add,   PS_Match);
    //  NO_TRANSITION(PS_Match, PS_Match);
    //  NO_TRANSITION(PS_Add,   PS_FinalizeSource);
    //  NO_TRANSITION(PS_WriteWhiteSpace,   PS_FinalizeSource);
    //  NO_TRANSITION(PS_WhiteSpace, PS_Add);
    //  NO_TRANSITION(PS_WhiteSpace, PS_WriteWhiteSpace);
    //  NO_TRANSITION(PS_FinalizeSource, PS_ExitLoop);
    //  default:
    //    TODO("History Transition Table\n"
    //         "\tPrevious State: %s\n"
    //         "\tCurrent State:  %s\n\t",
    //         parseState2enumStr(prevState),
    //         parseState2enumStr(state)
    //        );
    //}

  } while (state);
  return Success;
}

//enum ErrorId stateMachine(struct MFile CP patch, struct MFile CP src, FILE * CP tmp, char *tmpPath) {
//  enum PatchControl control;
//  enum ParseState state     = PS_Start,
//                  lastState = PS_Start;
//  //enum WhiteSpace whitespace;
//  enum ErrorId e;
//  static struct GitHeader gitHeader;
//  static struct DiffHeader diffHeader;
//  static struct HunkHeader hunkHeader;
//  char c;
//  const char *s;
//  int i;
//  int constTmpPath = tmpPath[0];
//  const char *srcPath;
//  do {
//    switch (state) {
//      default: TODO("state %s", parseState2enumStr(state)); break;
//      case PS_Start:
//        log(L_ParseState, logArg.parseState = state);
//        control = parsePatchControl(patch);
//        log(L_PatchControl, logArg.patchControl = control);
//        switch (control) {
//          case PC_Git:        setState(PS_Git); break;
//          case PC_Minus:      setState(PS_Diff); break;
//          case PC_WhiteSpace: ERROR_CHECK(mSkipWhitespace(patch)); break;
//          case PC_EOF:        ERROR_SET(EmptyPatchFile, errorArg.path = patch->path);
//          default:            TODO("PS_Start %s", patchControl2enumStr(control)); break;
//        }
//        break;
//      case PS_Git:
//        log(L_ParseState, logArg.parseState = state);
//        PARSE_GIT_HEADER;
//        EXPECTED_CONTROL(PC_Minus);
//        setState(PS_Diff);
//        FALLTHROUGH;
//      case PS_Diff:
//        log(L_ParseState, logArg.parseState = state);
//        PARSE_DIFF_HEADER;
//        EXPECTED_CONTROL(PC_Hunk);
//        {
//          /* open files */
//          srcPath = diffHeader.pathMinus;
//          OPEN_READ(src, srcPath);
//          if (constTmpPath) {
//            *tmp = fopen(tmpPath, "w");
//            ERROR_CONDITION(UnsuccessfulWriteOpen, NULL == *tmp, errorArg.path = tmpPath);
//          } else
//            ERROR_CHECK(tmpFile(tmp, srcPath, tmpPath, "tmp"));
//        }
//        setState(PS_Hunk);
//        FALLTHROUGH;
//      case PS_Hunk:
//        log(L_ParseState, logArg.parseState = state);
//        ERROR_CHECK(parseHunkHeader(patch, &hunkHeader));
//        setState(PS_Match);
//        FALLTHROUGH;
//      case PS_Match:
//        log(L_ParseState, logArg.parseState = state);
//        ERROR_CHECK(matchAndCopy(src, patch, *tmp, tmpPath));
//        setState(PS_HunkTransition);
//        break;
//      case PS_Add:
//        log(L_ParseState, logArg.parseState = state);
//        ERROR_CHECK(copyUntilClose(patch, *tmp, tmpPath));
//        setState(PS_HunkTransition);
//        break;
//      case PS_HunkTransition:
//        control = parsePatchControl(patch);
//        switch (TUPLE(lastState, control)) {
//          case TUPLE(PS_Match, PC_AddStart):
//            setState(PS_Add);
//            break;
//          case TUPLE(PS_Add, PC_WhiteSpace):
//            ERROR_CHECK(copyWhiteSpace(patch, *tmp, tmpPath));
//            break;
//          case TUPLE(PS_Add, PC_EOF):
//            setState(PS_FinalizeSource);
//            break;
//          default:
//            TODO("PS_HunkTransition: Last State = %s, Control = %s", parseState2enumStr(lastState), patchControl2enumStr(control));
//            break;
//        }
//        break;
//      case PS_FinalizeSource:
//        TODO("Copy rest of source");
//        TODO("Handle state transition based on control");
//        if (PC_EOF == control)
//          setState(PS_PatchEOF);
//        break;
//      case PS_PatchEOF:
//        TODO("PS_PatchEOF");
//        
//        break;
//    }
//  } while (PS_ExitLoop != state);
//  return Success;
//}

//enum ErrorId stateMachine(struct MFile CP patch, struct MFile CP src, FILE * CP tmp, char *tmpPath) {
//  enum PatchControl control;
//  enum ParseState state     = PS_Start,
//                  lastState = PS_Start;
//  //enum WhiteSpace whitespace;
//  enum ErrorId e;
//  static struct GitHeader gitHeader;
//  static struct DiffHeader diffHeader;
//  static struct HunkHeader hunkHeader;
//  char c;
//  const char *s;
//  int i;
//  int constTmpPath = tmpPath[0];
//  const char *srcPath;
//  do {
//    control = parsePatchControl(patch);
//    switch (TUPLE3(lastState, control, state)) {
//      case TUPLE3(PS_Start, PC_EOF, PS_Start):
//        log(L_DebugMessage, logArg.message = "case TUPLE3(PS_Start, PC_EOF, PS_Start):");
//        ERROR_SET(EmptyPatchFile, errorArg.path = patch->path);
//      case TUPLE3(PS_Start, PC_WhiteSpace, PS_Start):
//        log(L_DebugMessage, logArg.message = "case TUPLE3(PS_Start, PC_WhiteSpace, PS_Start):");
//        ERROR_CHECK(mSkipWhitespace(patch));
//        break;
//      case TUPLE3(PS_Start, PC_Git, PS_Start):
//        log(L_DebugMessage, logArg.message = "case TUPLE3(PS_Start, PC_Git, PS_Start):");
//        PARSE_GIT_HEADER;
//        EXPECTED_CONTROL(PC_Minus);
//        FALLTHROUGH;
//      case TUPLE3(PS_Start, PC_Minus, PS_Start):
//        log(L_DebugMessage, logArg.message = "case TUPLE3(PS_Start, PC_Minus, PS_Start):");
//        PARSE_DIFF_HEADER;
//        TODO_DEFER("Check path equivalance");
//        EXPECTED_CONTROL(PC_Hunk);
//        {
//          /* open files */
//          srcPath = diffHeader.pathMinus;
//          OPEN_READ(src, srcPath);
//          if (constTmpPath) {
//            *tmp = fopen(tmpPath, "w");
//            ERROR_CONDITION(UnsuccessfulWriteOpen, NULL == *tmp, errorArg.path = tmpPath);
//          } else
//            ERROR_CHECK(tmpFile(tmp, srcPath, tmpPath, "tmp"));
//        }
//        FALLTHROUGH;
//      case TUPLE3(PS_Start, PC_Hunk, PS_Start):
//        log(L_DebugMessage, logArg.message = "case TUPLE3(PS_Start, PC_Hunk, PS_Start):");
//        ERROR_CHECK(parseHunkHeader(patch, &hunkHeader));
//        TODO_DEFER("Copy source to hunk start");
//        setState(PS_Match);
//        e = matchAndCopy(src, patch, *tmp, tmpPath);
//        switch (e) {
//          case EOF:
//          case Success:
//            break;
//          default:
//            ERROR_CHECK(e);
//            abort();
//        }
//        break;
//        //while (1) {
//        //  switch (state) {
//        //    case PS_Match:
//        //      e = matchAndCopy(src, patch, *tmp, tmpPath);
//        //      switch (e) {
//        //        case EOF:
//        //          if (isEOF(src) && isEOF(patch)) {
//        //            TODO("PS_Match both src and patch EOF");
//        //          } else if (isEOF(src)) {
//        //            setState(PS_SourceEOF);
//        //          } else if (isEOF(patch)) {
//        //            TODO("PS_Match patch EOF");
//        //          } else
//        //            ERROR_SET(UndefinedBehavior, errorArg.msg = "Unknown EOF");
//        //          break;
//        //        default:
//        //          TODO("matchAndCopy %s", errorId2enumStr(e));
//        //      }
//        //    default:
//        //      TODO("Unhandled hunk state of %s", parseState2enumStr(state));
//        //      break;
//        //  }
//        //}
//        //endHunk:
//        //break;
//      case TUPLE3(PS_Add, PC_WhiteSpace, PS_Match):
//        log(L_DebugMessage, logArg.message = "case TUPLE3(PS_Add, PC_WhiteSpace, PS_Match):");
//        //c = mGetc(patch);
//        //assert(isspace(c));
//        //switch (c) {
//        //  case '\n':
//        //    break;
//        //  case ' ':
//        //    e = fputc(c, *tmp);
//        //    ERROR_CONDITION(FileError, EOF == e, errorArg.path = tmpPath);
//        //    break;
//        //  case EOF:
//        //    ERROR_SET(FileError, errorArg.path = patch->path);
//        //  default:
//        //    TODO("Whitespace %d", c);
//        //    break;
//        //}
//        e = matchAndCopy(src, patch, *tmp, tmpPath);
//        if (Success != e && EOF != e)
//          ERROR_CHECK(e);
//        break;
//      case TUPLE3(PS_Start, PC_AddStart, PS_Match):
//        log(L_DebugMessage, logArg.message = "case TUPLE3(PS_Start, PC_AddStart, PS_Match):");
//        ERROR_CHECK(copyUntilClose(patch, *tmp, tmpPath));
//        setState(PS_Add);
//        break;
//      case TUPLE3(PS_Match, PC_WhiteSpace, PS_Add):
//        log(L_DebugMessage, logArg.message = "case TUPLE3(PS_Match, PC_WhiteSpace, PS_Add):");
//        //copyWhiteSpace(patch, *tmp, tmpPath);
//        setState(PS_Match);
//        break;
//      case TUPLE3(PS_Add, PC_EOF, PS_Match):
//      //case TUPLE3(PS_Match, PC_EOF, PS_Add):
//        log(L_DebugMessage, logArg.message = "case TUPLE3(PS_Match, PC_EOF, PS_Add):");
//        setState(PS_FinalizeSource);
//        break;
//      case TUPLE3(PS_Add, PC_EOF, PS_FinalizeSource):
//        log(L_DebugMessage, logArg.message = "case TUPLE3(PS_Add, PC_EOF, PS_FinalizeSource):");
//
//      //case PS_FinalizeSource:
//        log(L_ParseState, logArg.parseState = state);
//        if (!isEOF(src))
//          ERROR_CHECK(copyRest(src, *tmp, tmpPath));
//        ERROR_CHECK(closeFile(src));
//        //if ('\n' != lastWrittenChar)
//        //  ERROR_CONDITION(FileError, '\n' != fputc('\n', *tmp), errorArg.path = tmpPath);
//        ERROR_CONDITION(UnsuccessfulFileClose, fclose(*tmp), errorArg.path = tmpPath);
//        lastWrittenChar = EOF;
//#ifdef _WIN32
//        /* Windows requires the destination file to not exist */
//        {
//          char backupPath[PATH_MAX];
//          ERROR_CHECK(tmpFile(NULL, srcPath, backupPath, "BAK"));
//          ERROR_CONDITION(RenameFileFail, rename(srcPath, backupPath), errorArg.pathsAB = ((struct PathsAB){srcPath, backupPath}));
//          ERROR_CONDITION(RenameFileFail, rename(tmpPath, srcPath), errorArg.pathsAB = ((struct PathsAB){tmpPath, srcPath}));
//          ERROR_CONDITION(RemoveFileFail, remove(backupPath), errorArg.path = backupPath);
//        }
//#else
//        ERROR_CONDITION(RenameFileFail, rename(tmpPath, srcPath), errorArg.pathsAB = ((struct PathsAB){tmpPath, srcPath}));
//#endif /* _WIN32 */
//
//        setState(PS_ExitLoop);
//        break;
//      default:
//        TODO("Unhandled case TUPLE3(%s, %s, %s)", parseState2enumStr(lastState), patchControl2enumStr(control), parseState2enumStr(state));
//        break;
//    }
//  } while (PS_ExitLoop != state);
//  return Success;
//}

//char parseBuf[PARSE_BUF_SIZE];
//
//int lastWrittenChar = EOF;
//
//
//enum ErrorId stateMachine(struct MFile CP patch, struct MFile CP src, FILE * CP tmp, char *tmpPath) {
//  int i, c, c2;
//  const char *s;
//  enum ErrorId e;
//  enum PatchControl pc;
//  enum ParseState state = PS_Start,
//                  lastState = PS_Start;
//  static struct GitHeader  gh = {0};
//  static struct DiffHeader dh = {0};
//  static struct HunkHeader hh = {0};
//  const char *srcPath;
//  const int constTmpPath = tmpPath[0];
//
//  while (state) {
//    {
//      /* whitespace */
//      log(L_DebugMessage, logArg.message = "whitespace");
//      i = 0;
//      while (1) {
//        c2 = mGetc(patch);
//        if (c2 != c)
//          break;
//        i++;
//      }
//      ERROR_CONDITION(FileError, EOF == c2 && MF_ERROR_CHECK(patch), errorArg.path = patch->path);
//      if (EOF == c2 && '\n' == c) {
//        setState(PS_EOF);
//      }
//      else {
//        c2 = mUngetc(c2, patch);
//        ERROR_CONDITION(FileError, EOF == c2, errorArg.path = patch->path);
//        pc = parsePatchControl(patch);
//        log(L_WhiteSpace, logArg.whitespace = ((struct WhiteSpaceLog){state, c, i, c2, pc}));
//        switch (TUPLE(state, pc)) {
//          case TUPLE(PS_Match, PC_AddStart):
//            for (; 0 < i; --i) {
//              c2 = fputc(c, *tmp);
//              ERROR_CONDITION(FileError, EOF == c2, errorArg.path = tmpPath);
//            }
//            setState(PS_Add);
//            break;
//          //case TUPLE(PS_Match, PC_None):
//          //  for (; 0 < i; --i) {
//          //    c2 = fputc(c, *tmp);
//          //    ERROR_CONDITION(FileError, EOF == c2, errorArg.path = tmpPath);
//          //  }
//          //  setState(PS_Match);
//          //  break;
//          default:
//            TODO("Whitespace following state %s with patch control %s", parseState2enumStr(state), patchControl2enumStr(pc));
//            break;
//          //switch (pc) {
//          //  case PC_WhiteSpace:
//          //    TODO("Mixed whitespace");
//          //    break;
//          //  case PC_AddStart:
//          //    TODO("Whitespace preceeding an addition");
//          //    break;
//          //  case PC_RmStart:
//          //    TODO("Whitespace preceeding a removal");
//          //    break;
//          //  default:
//          //    TODO("Other cases following whitespace control");
//          //}
//        }
//      }
//    }
//    switch (state) {
//      case PS_Start:
//        log(L_ParseState, logArg.parseState = state);
//        pc = parsePatchControl(patch);
//        switch (pc) {
//          case PC_Git: setState(PS_Git); break;
//          case PC_Minus: setState(PS_Diff); break;
//          case PC_WhiteSpace:
//            ERROR_CHECK(mSkipWhitespace(patch));
//            break;
//          case PC_EOF:
//            ERROR_SET(EmptyPatchFile, errorArg.path = patch->path);
//            break;
//          case PC_FileError:
//            ERROR_SET(FileError, errorArg.path = patch->path);
//            break;
//          default:
//            UNEXPECTED_CONTROL(PC_Git, PC_Minus);
//            break;
//        }
//        break;
//      case PS_Git:
//        log(L_ParseState, logArg.parseState = state);
//        PARSE_GIT_HEADER;
//        log(L_GitHeader, logArg.gitHeader = &gh);
//        EXPECTED_CONTROL(PC_Minus);
//        setState(PS_Diff);
//        FALLTHROUGH;
//      case PS_Diff:
//        log(L_ParseState, logArg.parseState = state);
//        PARSE_DIFF_HEADER;
//        log(L_DiffHeader, logArg.diffHeader = &dh);
//        srcPath = dh.pathMinus;
//        ERROR_CONDITION(
//          DifferingSourceUpdatePaths,
//          strncmp(srcPath, dh.pathPlus, PATH_MAX) || (*gh.pathA && (strncmp(srcPath, gh.pathA, PATH_MAX) || strncmp(srcPath, gh.pathB, PATH_MAX))),
//          errorArg.pathsAB = ((struct PathsAB){ dh.pathMinus, dh.pathPlus })
//          );
//        log(L_SourcePath, logArg.path = srcPath);
//        OPEN_READ(src, srcPath);
//        if (constTmpPath) {
//          *tmp = fopen(tmpPath, "w");
//          ERROR_CONDITION(UnsuccessfulWriteOpen, NULL == *tmp, errorArg.path = tmpPath);
//        } else
//          ERROR_CHECK(tmpFile(tmp, srcPath, tmpPath, "tmp"));
//        EXPECTED_CONTROL(PC_Hunk);
//        setState(PS_Hunk);
//        FALLTHROUGH;
//      case PS_Hunk:
//        log(L_ParseState, logArg.parseState = state);
//        ERROR_CHECK(parseHunkHeader(patch, &hh));
//        log(L_HunkHeader, logArg.hunkHeader = &hh);
//        e = advanceToLineCopy(src, *tmp, tmpPath, hh.minus.start);
//        if (EOF == e) {
//          setState(PS_EOF);
//          break;
//        }
//        ERROR_CHECK(e);
//        setState(PS_Match);
//        FALLTHROUGH;
//      case PS_Match:
//        log(L_ParseState, logArg.parseState = state);
//        e = matchAndCopy(src, patch, *tmp, tmpPath);
//        switch (e) {
//          case Success:
//            pc = parsePatchControl(patch);
//            switch (pc) {
//              case PC_AddStart: setState(PS_Add);    break;
//              case PC_RmStart:  setState(PS_Remove); break;
//              case PC_Hunk:     setState(PS_Hunk);   break;
//              case PC_Git:      setState(PS_Git);    break;
//              case PC_EOF:      setState(PS_EOF);    break;
//              case PC_FileError:
//                ERROR_SET(FileError, errorArg.path = patch->path);
//                break;
//              case PC_None:
//                ERROR_SET(MissMatch, (errorArg.sourceAndPatch = (struct SourceAndPatch){src, patch}));
//                break;
//              case PC_RmEnd:
//              case PC_AddEnd:
//              case PC_Minus:
//              case PC_Plus:
//              case PC_Index:
//                ERROR_SET(ParseFail_InvalidControlInState, (errorArg.stateControl = (struct StateControl){ patch, state, pc }));
//                break;
//            }
//            break;
//          case EOF:
//            setState(PS_EOF);
//            break;
//          default:
//            ERROR_CHECK(e);
//            break;
//        }
//        break;
//      case PS_Remove:
//        log(L_ParseState, logArg.parseState = state);
//        ERROR_CHECK(matchAndDiscardUntilClose(src, patch));
//        setState(PS_Match);
//        break;
//      case PS_Add:
//        log(L_ParseState, logArg.parseState = state);
//        ERROR_CHECK(copyUntilClose(patch, *tmp, tmpPath));
//        pc = parsePatchControl(patch);
//        setState(PS_Match);
//        break;
//      case PS_Append:
//        log(L_ParseState, logArg.parseState = state);
//        ERROR_CHECK(copyUntilClose(patch, *tmp, tmpPath));
//        setState(PS_EOF);
//        FALLTHROUGH;
//      case PS_EOF:
//        log(L_ParseState, logArg.parseState = state);
//        if (EOF == pc) {
//          log(L_EOF, logArg.path = patch->path);
//          setState(PS_FinalizeSource);
//          break;
//        } else if (isEOF(src)) {
//          log(L_EOF, logArg.path = src->path);
//          ERROR_CHECK(mSkipWhitespace(patch));
//          pc = parsePatchControl(patch);
//          switch (pc) {
//            case PC_EOF:      setState(PS_FinalizeSource); break;
//            case PC_Git:      setState(PS_Git);            break;
//            case PC_Minus:    setState(PS_Diff);           break;
//            case PC_AddStart: setState(PS_Append);         break;
//            case PC_FileError:
//              ERROR_SET(FileError, errorArg.path = patch->path);
//              break;
//            default: UNEXPECTED_CONTROL(PC_EOF, PC_Git, PC_Minus);
//          }
//        } else {
//          ERROR_SET(UndefinedBehavior, errorArg.msg = "receiving state of PS_EOF when neither file handles for source nor patch are EOF");
//        }
//        break;
//      case PS_FinalizeSource:
//        log(L_ParseState, logArg.parseState = state);
//        if (!isEOF(src))
//          ERROR_CHECK(copyRest(src, *tmp, tmpPath));
//        ERROR_CHECK(closeFile(src));
//        if ('\n' != lastWrittenChar)
//          ERROR_CONDITION(FileError, '\n' != fputc('\n', *tmp), errorArg.path = tmpPath);
//        ERROR_CONDITION(UnsuccessfulFileClose, fclose(*tmp), errorArg.path = tmpPath);
//        lastWrittenChar = EOF;
//#ifdef _WIN32
//        /* Windows requires the destination file to not exist */
//        {
//          char backupPath[PATH_MAX];
//          ERROR_CHECK(tmpFile(NULL, srcPath, backupPath, "BAK"));
//          ERROR_CONDITION(RenameFileFail, rename(srcPath, backupPath), errorArg.pathsAB = ((struct PathsAB){srcPath, backupPath}));
//          ERROR_CONDITION(RenameFileFail, rename(tmpPath, srcPath), errorArg.pathsAB = ((struct PathsAB){tmpPath, srcPath}));
//          ERROR_CONDITION(RemoveFileFail, remove(backupPath), errorArg.path = backupPath);
//        }
//#else
//        ERROR_CONDITION(RenameFileFail, rename(tmpPath, srcPath), errorArg.pathsAB = ((struct PathsAB){tmpPath, srcPath}));
//#endif /* _WIN32 */
//        setState(PS_End);
//        FALLTHROUGH;
//      case PS_End:
//        log(L_ParseState, logArg.parseState = state);
//        ERROR_CHECK(closeFile(patch));
//        setState(PS_ExitLoop);
//        break;
//      case PS_ExitLoop:
//        ERROR_SET(UndefinedBehavior, errorArg.msg = "PS_ExitLoop should never be matched");
//        break;
//    }
//  }
//  return Success;
//}
