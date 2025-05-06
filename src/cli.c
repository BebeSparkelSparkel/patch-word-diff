#include <string.h>

#include "error.h"
#include "cli.h"
#include "mfile.h"

enum ErrorId cli(const int argc, FP(char) CP argv, FP(struct CLIParameters) params) {
  int i;
  const char *s;
  const char *patchPath = NULL;

  for(i = 1; i < argc - 1; ++i) {
    s = argv[i];
    if (!strcmp("-tmpPath", s)) {
      char CP tmpPath = *params->tmpPath,
           CP endChar = tmpPath + PATH_MAX - 1;
      ++i;
      ERROR_CONDITION(MissingFollowingCommandArgument, i >= argc, errorArg.optionType.type = "file path"; errorArg.optionType.option = "-tmpPath");
      
      *endChar = '\0';
      strncpy(tmpPath, argv[i], PATH_MAX);
      ERROR_CONDITION(TmpPathBufferOverflow, '\0' != *endChar, );
    }
    else if ('-' == *s++) {
      for (; *s != '\0'; s++)
        switch (*s) {
          case 'v':
            (*params->logLevel)++;
            break;
          case 'q':
            *params->logLevel = LogNone;
            break;
          case 'w':
            *params->werror = 1;  // Set warnings as errors
            break;
          default:
            ERROR_SET(UndefinedFlag, errorArg.undefinedFlag.c = *s; errorArg.undefinedFlag.argString = argv[i]);
            break;
        }
    }
  }
  logIf(L_TooVerbose, logLevel > LOG_MAX, );

  /* Patch path is the last argument.
   * If `-` then stdin is used. */
  if (argc - 1 == i)
    patchPath = argv[i];
  ERROR_CONDITION(MissingPatchFileCommandArgument, NULL == patchPath, )
  else if (!strcmp(patchPath, "-"))
    streamFile(params->patch, stdin, "stdin");
  else OPEN_READ(params->patch, patchPath);
  log(L_PatchPath, logArg.path = params->patch->path);

  return Success;
}

