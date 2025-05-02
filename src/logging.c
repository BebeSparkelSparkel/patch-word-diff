#include <stdio.h>

#include "logging.h"

#define CASE_LOG(_, id, __, format) \
  case id: \
    fprintf(stderr, format); \
    break;

void _log(LogId l) {
  switch (l) {
    LOG_TABLE(CAT, CASE_LOG)
  }
}

