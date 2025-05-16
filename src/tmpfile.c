#include <string.h>
#include "assert.h"

#include "tmpfile.h"
#include "error.h"

enum ErrorId tmpFile(FILE * CP tmp, const char srcPath[PATH_MAX], char tmpPath[PATH_MAX], FP(char) ext) {
#ifdef _WIN32
#error "Implement tmpFilePath for windows"
#else
  char *extension;
  int i;
  size_t tmpPathLen,
         count;
  FILE *f;

  assert(NULL == tmp || NULL == *tmp);
  assert(NULL != srcPath);
  assert(NULL != tmpPath);
  assert('\0' == *tmpPath);
  assert(NULL != ext);
  assert(PATH_MAX > strlen(srcPath));

  strncpy(tmpPath, srcPath, PATH_MAX);
  extension = strrchr(tmpPath, '.');
  if (NULL != extension)
    *extension = '_';
  tmpPathLen = strlen(tmpPath);

  count = strlen(ext) + 1;
  ERROR_CONDITION( TmpPathBufferOverflow
                 , PATH_MAX - 1 - tmpPathLen < count
                 , errorArg.path = tmpPath
                 );
  tmpPath[tmpPathLen] = '.';
  strcpy(&tmpPath[tmpPathLen + 1], ext);
  tmpPathLen += count;

  for (i = 0; i < INT_MAX; ++i) {
    count = snprintf(tmpPath + tmpPathLen, PATH_MAX - 1 - tmpPathLen, "%d", i);
    ERROR_CONDITION( TmpPathBufferOverflow
                   , count > PATH_MAX - 1 - tmpPathLen
                   , errorArg.path = tmpPath
                   );
    f = fopen(tmpPath, "r");
    if (NULL == f) {
      f = fopen(tmpPath, "w");
      ERROR_CONDITION(UnsuccessfulWriteOpen, NULL == f, errorArg.path = tmpPath);
      if (NULL != tmp)
        *tmp = f;
      else ERROR_CONDITION(UnsuccessfulFileClose, fclose(f), errorArg.path = tmpPath);
      return Success;
    }
    else ERROR_CONDITION(UnsuccessfulFileClose, fclose(f), errorArg.path = tmpPath);
  }
  ERROR_SET( CouldNotOpenTmpFile
           , errorArg.path = strncat(tmpPath + tmpPathLen, "0", PATH_MAX - 1 - tmpPathLen)
           );

#endif /* _WIN32 */
}

