%{
#include <stdio.h>
#include <stdlib.h>
#include "diff_tree.h"
#include "parser.h"

extern void yyerror(const char*s);

void yylex_src_only();
%}

%option nounput
%option noinput
%option noyywrap

%x S_GIT_HEADER S_GIT_INDEX S_FILE_DIFF_HEADER S_HUNK_HEADER S_HUNK S_HUNK_ONLY

%%

<INITIAL,S_HUNK>^"diff --git" BEGIN(S_GIT_HEADER); return S_GIT_HEADER;
<S_GIT_HEADER>{
  "a/"[^ \n]+  |
  "b/".*       {
    yylval.string = newStringCopy(yyleng - 2, yytext + 2);
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
    yylval.string = newStringCopy(yyleng, yytext);
    return HASH;
  }
  ".."
}

<INITIAL,S_FILE_DIFF_HEADER,S_HUNK>^"--- a/".* {
  int pl = strlen("--- a/");
  yylval.string = newStringCopy(yyleng - pl, yytext + pl);
  return PATH;
}
<S_FILE_DIFF_HEADER>^"+++ b/".* {
  int pl = strlen("+++ b/");
  yylval.string = newStringCopy(yyleng - pl, yytext + pl);
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
  \b+ yylval.integer = yyleng; return '\b';
  \f+ yylval.integer = yyleng; return '\f';
  \t+ yylval.integer = yyleng; return '\t';
  \\  return '\\';
  \"  return '"';
  [^[\]{}\n \\\"\b\f\t]+  {
    yylval.string = newStringCopy(yyleng, yytext);
    return WORD;
  }
}

%%

void yylex_src_only() { BEGIN(S_HUNK_ONLY); }

