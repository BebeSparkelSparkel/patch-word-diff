%{
#include <stdio.h>
#include <stdlib.h>
#include "diff_tree.h"
#include "queue_types.h"
#include "parser.h"

extern void yyerror(const char*s);
%}

%x S_GIT_HEADER S_GIT_INDEX S_FILE_DIFF_HEADER S_HUNK_HEADER S_HUNK S_HUNK_ONLY

  #ifdef DIFF
  #define START INITIAL
  #else
  #define YY_DECL int yylex_src(void)
  #define START S_HUNK_ONLY
  #endif

%%

  BEGIN(START);

<INITIAL,S_HUNK>^"diff --git" BEGIN(S_GIT_HEADER); return S_GIT_HEADER;
<S_GIT_HEADER>{
  "a/"[^ \n]+  |
  "b/".*       {
    if ((yylval.string = strdup(yytext + 2)) == NULL)
      yyerror("Could not duplicate path string.");
    BEGIN(S_GIT_INDEX);
    return PATH;
  }
}
<S_GIT_INDEX>{
  ^"index "  return S_GIT_INDEX;
  [0-7]+$ {
    yylval.integer = atoi(yytext);
    BEGIN(S_FILE_DIFF_HEADER);
    return FILE_MODE;
  }
  [0-9a-f]+ {
    if ((yylval.string = strdup(yytext)) == NULL)
      yyerror("Could not duplicate hash string.");
    return HASH;
  }
  ".."
}

<INITIAL,S_FILE_DIFF_HEADER,S_HUNK>^"--- a/".* {
  int pl = strlen("--- a/");
  if ((yylval.string = strndup(yytext + pl, yyleng - pl)) == NULL)
    yyerror("Could not duplicate path string.");
  return PATH;
}
<S_FILE_DIFF_HEADER>^"+++ b/".* {
  int pl = strlen("+++ b/");
  if ((yylval.string = strndup(yytext + pl, yyleng - pl)) == NULL)
    yyerror("Could not duplicate path string.");
  BEGIN(S_HUNK_HEADER);
  return PATH;
}

<INITIAL,S_HUNK_HEADER,S_HUNK>^"@@ -" BEGIN(S_HUNK_HEADER); return S_HUNK_HEADER;
<S_HUNK_HEADER>{
  [0-9]+  yylval.integer = atoi(yytext); return POSITION;
  ,
  " +"
  " @@"[ \n] BEGIN(S_HUNK);
}
  #else
  BEGIN(S_HUNK);
  #endif
<S_HUNK>{
  "[-" return REMOVAL_OPEN;
  "-]" return REMOVAL_CLOSE;
  "{+" return ADDITION_OPEN;
  "+}" return ADDITION_CLOSE;
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
