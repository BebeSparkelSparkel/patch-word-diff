#ifndef CLI_H
#define CLI_H

#include "error.h"

struct CLIParameters {
  struct MFile CP patch;
  enum LogLevel CP logLevel;
  char CP werror;
  char * CP tmpPath;
};

enum ErrorId cli(const int argc, FP(char) CP argv, FP(struct CLIParameters) params);

#endif
