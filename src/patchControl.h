#ifndef PATCHCONTROL_H
#define PATCHCONTROL_H

#include <stdio.h>

#include "cpp.h"

#define EXPECTED_CONTROL(EXPECTED) \
  ERROR_CONDITION( \
    ParseFail_ExpectedControl, \
    (control = parsePatchControl(patch)) != EXPECTED, \
    (errorArg.patchControlDiffers = (struct PatchControlDiffers){ patch, EXPECTED, control}) \
  )

#define UNEXPECTED_CONTROL(...) \
  ERROR_SET( \
    ParseFail_UnexpectedControl, \
    errorArg.unexpectedPatchControl = ((struct UnexpectedPatchControl){ patch, control, #__VA_ARGS__ }) \
  )
 
#define PATCH_CONTROL_TABLE(filt, cons, map, ...) \
  filt(map(__VA_ARGS__, PC_LineFeed   , "(PC_LineFeed)"   , = EOF - 5 ), \
  filt(map(__VA_ARGS__, PC_Tab        , "(PC_Tab"         , = EOF - 4 ), \
  filt(map(__VA_ARGS__, PC_Space      , "(PC_Space)"      , = EOF - 3 ), \
  filt(map(__VA_ARGS__, PC_WhiteSpace , "(PC_WhiteSpace)" , = EOF - 2 ), \
  filt(map(__VA_ARGS__, PC_FileError  , "(PC_FileError)"  , = EOF - 1 ), \
  filt(map(__VA_ARGS__, PC_EOF        , "(PC_EOF)"        , = EOF     ), \
  filt(map(__VA_ARGS__, PC_None       , "(PC_None)"       , = 0       ), \
  cons(map(__VA_ARGS__, PC_RmStart    , "[-"              ,           ), \
  cons(map(__VA_ARGS__, PC_RmEnd      , "-]"              ,           ), \
  cons(map(__VA_ARGS__, PC_AddStart   , "{+"              ,           ), \
  cons(map(__VA_ARGS__, PC_AddEnd     , "+}"              ,           ), \
  cons(map(__VA_ARGS__, PC_Hunk       , "@@ "             ,           ), \
  cons(map(__VA_ARGS__, PC_Git        , "diff --git "     ,           ), \
  cons(map(__VA_ARGS__, PC_Minus      , "--- "            ,           ), \
  cons(map(__VA_ARGS__, PC_Plus       , "+++ "            ,           ), \
       map(__VA_ARGS__, PC_Index      , "index "          ,           )  \
      )))))))))))))))

#define _SELECT_COLUMNS_PATCH_CONTROL_TABLE_Enumerator_Assignment _SELECT_COLUMNS_1_3

#define SIZEOF(x) sizeof(x)

#define UNGET_BUF_SIZE PATCH_CONTROL_TABLE(TAIL, MAX, COMPOSE, SIZEOF, SND)

enum PatchControl {
  PATCH_CONTROL_TABLE(COMMA_INTER, COMMA_INTER, COMPOSE, EXPAND_ARG(CAT), SELECT_COLUMNS(PATCH_CONTROL_TABLE, Enumerator, Assignment))
};

const char *patchControl2enumStr(enum PatchControl x);

const char *patchControl2commandStr(enum PatchControl x);

struct MFile;

enum PatchControl parsePatchControl(struct MFile CP f);

#endif
