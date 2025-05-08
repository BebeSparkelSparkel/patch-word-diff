#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "cpp.h"
#include "todo.h"
#include "tmpfile.h"
#include "cli.h"
#include "stateMachine.h"
#include "mfile.h"

union ErrorArg errorArg;
enum LogLevel logLevel = LogWarning;
enum LogId logId;
union LogArg logArg;
char werror = 0;

#define IN_MAIN_FUNCTION
#include "error.h"

int main(const int argc, FP(char) CP argv) {
  struct MFile src,
               patch;
  char tmpPath[PATH_MAX] = {'\0'};
  FILE *tmp = NULL;

  initMFile(&src);
  initMFile(&patch);

  {
    const struct CLIParameters cliParameters = {
        &patch,
        &logLevel,
        &werror,
        tmpPath,
      };
    ERROR_CHECK(cli(argc, argv, &cliParameters));
  }

  ERROR_CHECK(stateMachine(&patch, &src, &tmp, tmpPath));
  return Success;
}
#undef IN_MAIN_FUNCTION

