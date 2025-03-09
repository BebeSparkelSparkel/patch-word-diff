%{
#include <stdio.h>
#include <stdlib.h>
#include "ptypes.h"
#include "y.tab.h"


%}

  #ifdef DIFF
%x GIT_HEADER GIT_INDEX FILE_DIFF_HEADER HUNK_HEADER
  #else
  #define YY_DECL int yylex_src(void);
  #endif
%s HUNK

%%

#ifdef DIFF
<INITIAL,HUNK>^diff --git BEGIN(GIT_HEADER); return GIT_HEADER;
<GIT_HEADER>{
  "a/"[^ \n]+  |
  "b/".*       {
    if ((yylval.string = strdup(yytext + 2)) == NULL)
      yyerror("Could not duplicate path string.");
    BEGIN(GIT_INDEX);
    return PATH;
  }
}
<GIT_INDEX>{
  ^"index "  return GIT_INDEX;
  [0-7]+$ {
    yylval.integer = atoi(yytext);
    BEGIN(FILE_DIFF_HEADER);
    return FILE_MODE;
  }
  [0-9a-f]+ {
    if ((yylval.string = strdup(yytext)) == NULL)
      yyerror("Could not duplicate hash string.");
    return HASH;
  }
  ".."
}

<INITIAL,FILE_DIFF_HEADER,HUNK>^"--- a/".* {
  int pl = strlen("--- a/");
  if ((yylval.string = strndup(yytext + pl, yyleng - pl)) == NULL)
    yyerror("Could not duplicate path string.");
  return PATH;
}
<FILE_DIFF_HEADER>^"+++ b/".* {
  int pl = strlen("+++ b/");
  if ((yylval.string = strndup(yytext + pl, yyleng - pl)) == NULL)
    yyerror("Could not duplicate path string.");
  BEGIN(HUNK_HEADER);
  return PATH;
}

<INITIAL,HUNK_HEADER,HUNK>^"@@ -" BEGIN(HUNK_HEADER); return HUNK_HEADER;
<HUNK_HEADER>{
  [0-9]+  yylval.integer = atoi(yytext); return POSITION;
  ,
  " +"
  " @@"[ \n] BEGIN(HUNK);
}
#else
  BEGIN(HUNK);
#endif
<HUNK>{
  "[-"   return REMOVAL_OPEN;
  "-]"   return REMOVAL_CLOSE;
  "{+"  return ADDITION_OPEN;
  "+}"  return ADDITION_CLOSE;
  "["  return '[';
  "]"  return ']';
  "{"  return '{';
  "}"  return '}';
  \n+ yylval.integer = yyleng; return '\n';
  \ + yylval.integer = yyleng; return ' ';
  [^[\]{}\n ]+  {
    if ((yylval.string = strdup(yytext)) == NULL)
      yyerror("Could not duplicate hunk string.");
    return WORD;
  }
}

%%

