#include <string.h>
#include <assert.h>

#include "tmpfile.h"
#include "error.h"

ErrorId tmpFile(FILE **tmp, char CP srcMutable, char CP tmpPath, size_t tmpSize) {
#ifdef _WIN32
#error "Implement tmpFilePath for windows"
#else
  assert(NULL != srcMutable);
  assert(NULL != tmpPath);
  assert(PATH_MAX == tmpSize);
  char *baseName = NULL,
       *extension = NULL;
  int tmpFileIndex;
  size_t unindexedCharCount,
         indexedCharCount;
  ErrorId r;

  *tmpPath = '\0';

  baseName = strrchr(srcMutable, '/');
  if (NULL == baseName)
    baseName = srcMutable;
  else
    *baseName++ = '\0';

  extension = strrchr(baseName, '.');
  
  unindexedCharCount = snprintf(
    tmpPath,
    tmpSize,
    "%s/%s_%s.%s",
    /* dir_path */
     baseName == srcMutable ? "." : srcMutable,
    /* baseName */
    '\0' == *baseName ? "tmpfile" : baseName,
    /* extension */
    NULL == extension || extension == baseName
      ? "" : extension,
    /* tmp extesnion */
    "tmp"
    );
  if (unindexedCharCount >= tmpSize) {
    r = TmpPathBufferOverflow;
    goto cleanup;
  }
  tmpSize -= unindexedCharCount;
  for (tmpFileIndex = 0; tmpFileIndex <= INT_MAX; ++tmpFileIndex) {
    indexedCharCount = snprintf(tmpPath + unindexedCharCount, tmpSize, "%d", tmpFileIndex);
    if (indexedCharCount >= tmpSize) {
      r = TmpPathBufferOverflow;
      goto cleanup;
    }
    *tmp = fopen(tmpPath, "r");
    if (NULL == *tmp) {
      *tmp = fopen(tmpPath, "w");
      r = NULL == tmp ? UnsuccessfulWriteOpen : Success;
      goto cleanup;
    }
    else if (fclose(*tmp)) {
      r =  UnsuccessfulFileClose;
      goto cleanup;
    }
  }
  strcpy(tmpPath + indexedCharCount, "0");
  r = CouldNotOpenTmpFile;

cleanup:
  *srcMutable = '\0';
  if (r != Success)
    errorArg.path = tmpPath;
  return r;
#endif /* _WIN32 */
}

