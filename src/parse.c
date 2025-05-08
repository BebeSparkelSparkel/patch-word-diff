#include <stdio.h>
#include <string.h>

#include "parse.h"
#include "mfile.h"

enum ErrorId parseHunkHeader(struct MFile CP patch, struct HunkHeader CP hh) {
  const char *s;
  const char *format;
  int i;
  LOAD_PARSE_LINE(ParseFail_HunkHeader);
  /* "@@ " already consumed by parsePatchControl */
  format = "-%d,%d +%d,%d @@";
  i = sscanf(parseBuf, format, &hh->minus.start, &hh->minus.end, &hh->plus.start, &hh->plus.end);
  if (4 == i)
    goto success;
  format = "-%d +%d @@";
  i = sscanf(parseBuf, format, &hh->minus.start, &hh->plus.start);
  if (2 == i) {
    hh->minus.end = 0 == hh->minus.start ? 0 : 1;
    hh->plus.end = 1;
    goto success;
  }
  format = "-%d,%d +%d @@";
  i = sscanf(parseBuf, format, &hh->minus.start, &hh->minus.end, &hh->plus.start);
  if (3 == i) {
    hh->plus.end = 1;
    goto success;
  }
  format = "-%d +%d,%d @@";
  i = sscanf(parseBuf, format, &hh->minus.start, &hh->plus.start, &hh->plus.end);
  if (3 == i) {
    hh->minus.end = 1;
    goto success;
  }
  ERROR_SET(ParseFail_HunkHeader, 
         errorArg.pathLineMsg = ((struct PathLineMsg){ 
           patch->path, 
           patch->line - 1, 
           "Hunk Format: Expected one of the following formats:\n"
           "  @@ -a,b +c,d @@    (standard format)\n"
           "  @@ -a +c @@        (single line format)\n"
           "  @@ -0,0 +c,d @@    (new file format)\n"
           "  @@ -a,b +c @@      (mixed format)\n"
           "  @@ -a +c,d @@      (mixed format)\n"
           "Context text after @@ is allowed in any format."
         }));
success:
  /* log(L_ParseSuccess, logArg.pathLineMsg = ((struct PathLineMsg){ patch->path, patch->line - 1, format })); */
  return Success;
}
