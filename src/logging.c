#include <stdio.h>

#include "logging.h"

#define CASE_LOG(_, id, __, action) \
  case id: \
    action; \
    break;

void _log(LogId l) {
  switch (l) {
    LOG_TABLE(CAT, CASE_LOG)
  }
}

