%{
#include "diff_tree.h"
%}


%union {
  char *string;
  int integer;
}


%token GIT_HEADER

%token<string> PATH
%token<string> HASH
%token<integer> FILE_MODE

%token<integer> POSITION

%token HUNK_HEADER
%token REMOVAL_OPEN
%token REMOVAL_CLOSE
%token ADDITION_OPEN
%token ADDITION_CLOSE
%token<integer> '\n'
%token<integer> ' '
%token<string> WORD

%%
patch: header hunks | header hunks patch;



header: gitHeader fileDiffHeader
      | fileDiffHeader
      ;
gitHeader: GIT_HEADER PATH PATH GIT_INDEX HASH HASH FILE_MODE;
fileDiffHeader: PATH PATH;

hunks: hunk | hunk hunks;
hunk: hunkHeader hunkBody;
hunkHeader: HUNK_HEADER POSITION POSITION POSITION POSITION;
hunkBody: diff hunkBody | diff;
diff: string
    | REMOVAL_OPEN string REMOVAL_CLOSE
    | ADDITION_OPEN string ADDITION_CLOSE
    ;
string: stringElements string | stringElements;
elements: '\n' | ' ' | '[' | ']' | '{' | '}' | WORD;

%%

