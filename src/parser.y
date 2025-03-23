%{
#define INCLUDE #include <stdlib.h>
INCLUDE
#undef INCLUDE
#define INCLUDE #include <stdio.h>
INCLUDE
#undef INCLUDE
#define INCLUDE #include <stdint.h>
INCLUDE
#undef INCLUDE
#define INCLUDE #include "diff_tree.h"
INCLUDE
#undef INCLUDE

extern int yylex(void);
%}


%union {
  char *string;
  int integer;

  QueuePatch *patches;
  Patch *patch;
  PatchHeader patchHeader;
  QueueHunk *hunks;
  FileDiffHeader *fileDiffHeader;
  GitHeader *gitHeader;
  Hunk *hunk;
  HunkHeader *hunkHeader;
  QueueDiff *diffs;
  Diff *diff;
  QueueElement *elements;
  Element *element;
}


%token GIT_HEADER
%token GIT_INDEX

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

%type<patches> patches
%type<patch> patch
%type<patchHeader> patchHeader
%type<hunks> hunks
%type<fileDiffHeader> fileDiffHeader
%type<gitHeader> gitHeader
%type<hunk> hunk
%type<hunkHeader> hunkHeader
%type<diffs> diffs
%type<diff> diff
%type<elements> elements
%type<element> element

%%

patches: patch         { $$ = newOneQueuePatch($1); }
       | patches patch { pushQueuePatch($$, $2); }
       ;
patch: patchHeader hunks { $$ = newPatch($1, $2); };

patchHeader: gitHeader fileDiffHeader { $$.git->fileDiff = $2; }
      | fileDiffHeader { $$.fileDiff = $1; }
      ;
gitHeader: GIT_HEADER PATH PATH GIT_INDEX HASH HASH FILE_MODE
         { $$ = newGit($2, $3, $5, $6, $7); };
fileDiffHeader: PATH PATH { $$ = newFileDiff($1, $2); };

hunks: hunk { $$ = newOneQueueHunk($1); }
     | hunks hunk { pushQueueHunk($$, $2); }
     ;
hunk: hunkHeader diffs { $$ = newHunk($1, $2); };
hunkHeader: HUNK_HEADER POSITION POSITION POSITION POSITION
          { $$ = newHunkHeader($2, $3, $4, $5); };
diffs: diff { $$ = newOneQueueDiff($1); }
     | diffs diff { pushQueueDiff($$, $2); }
     ;
diff: element { $$ = match($1); }
    | REMOVAL_OPEN elements REMOVAL_CLOSE { $$ = removal($2); }
    | ADDITION_OPEN elements ADDITION_CLOSE { $$ = addition($2); }
    ;
elements: element { $$ = newOneQueueElement($1); }
        | elements element { pushQueueElement($$, $2); }
        ;

#include "element.h"
element: ELEMENT_GRAMMAR ;
//element: '['  { $$ = newElement( OpenSquare,     newElementNull()     ); }
//       | ']'  { $$ = newElement( CloseSquare,    newElementNull()     ); }
//       | '{'  { $$ = newElement( OpenCurly,      newElementNull()     ); }
//       | '}'  { $$ = newElement( CloseCurly,     newElementNull()     ); }
//       | '\n' { $$ = newElement( Newlines,       newElementCount($1)  ); }
//       | ' '  { $$ = newElement( Spaces,         newElementCount($1)  ); }
//       | '\b' { $$ = newElement( Backspaces,     newElementCount($1)  ); }
//       | '\f' { $$ = newElement( Formfeeds,      newElementCount($1)  ); }
//       | '\t' { $$ = newElement( HorizontalTabs, newElementCount($1)  ); }
//       | '\\' { $$ = newElement( ReverseSolidus, newElementNull()     ); }
//       | '"'  { $$ = newElement( DoubleQuote,    newElementNull()     ); }
//       | WORD { $$ = newElement( Word,           newElementString($1) ); }
//       ;

%%

void yyerror(char *msg) {
  fprintf(stderr, "Error: %s\n", msg);
  exit(1);
}
