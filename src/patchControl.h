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
 
#define PATCH_CONTROL_TABLE_ALL(cons, map, ...) \
  PATCH_CONTROL_TABLE(cons, cons, map, __VA_ARGS__) \

#define PATCH_CONTROL_TABLE(filt, cons, map, ...) \
/* 0 */ filt(map(__VA_ARGS__, PC_LineFeed   , "(PC_LineFeed)"   , = EOF - 5 ), \
/* 1 */ filt(map(__VA_ARGS__, PC_Tab        , "(PC_Tab"         , = EOF - 4 ), \
/* 2 */ filt(map(__VA_ARGS__, PC_Space      , "(PC_Space)"      , = EOF - 3 ), \
/* 3 */ filt(map(__VA_ARGS__, PC_WhiteSpace , "(PC_WhiteSpace)" , = EOF - 2 ), \
/* 4 */ filt(map(__VA_ARGS__, PC_FileError  , "(PC_FileError)"  , = EOF - 1 ), \
/* 5 */ filt(map(__VA_ARGS__, PC_EOF        , "(PC_EOF)"        , = EOF     ), \
/* 6 */ filt(map(__VA_ARGS__, PC_None       , "(PC_None)"       , = 0       ), \
/* 7 */ cons(map(__VA_ARGS__, PC_RmStart    , "[-"              ,           ), \
/*      cons(map(__VA_ARGS__, PC_RmEnd      , "-]"              ,           ), */ \
/* 8 */ cons(map(__VA_ARGS__, PC_AddStart   , "{+"              ,           ), \
/*      cons(map(__VA_ARGS__, PC_AddEnd     , "+}"              ,           ), */ \
/* 9 */ cons(map(__VA_ARGS__, PC_Hunk       , "@@ "             ,           ), \
/* A */ cons(map(__VA_ARGS__, PC_Git        , "diff --git "     ,           ), \
/* B */ cons(map(__VA_ARGS__, PC_Minus      , "--- "            ,           ), \
/*      cons(map(__VA_ARGS__, PC_Plus       , "+++ "            ,           ), */ \
/* C */      map(__VA_ARGS__, PC_Index      , "index "          ,           )  \
/* F max */ ))))))))))))

#define _SELECT_COLUMNS_PATCH_CONTROL_TABLE_Enumerator_Assignment _SELECT_COLUMNS_1_3

#define SIZEOF(x) sizeof(x)

#define UNGET_BUF_SIZE PATCH_CONTROL_TABLE(TAIL, MAX, COMPOSE, SIZEOF, SND)

#define MIN_PATCH_CONTROL PATCH_CONTROL_TABLE_ALL(MIN, SND)

/* [ EOF - 5, ..., EOF, 0, ...           ]
 * [     -10, ...,  -5, 0, ...           ] EOF = -5
 * |   EOF - 5 = -10  |  EOF - (EOF - 5) |
 * [       0, ...,   5, 5, ...           ] incorrect off by one
 */
#define NORMALIZE_CONTROL(c) \
  ((c) <= PC_EOF ? (c) - minPatchControl : (c) + PC_EOF - minPatchControl + 1)

#define DENORMALIZE_CONTROL(n) \
  ((n) <= PC_EOF - minPatchControl ? (n) + minPatchControl : (n) - PC_EOF + minPatchControl - 1)

#define GET_CONTROL(state) ((enum PatchControl)(DENORMALIZE_CONTROL(FST4(state))))

enum PatchControl {
  PATCH_CONTROL_TABLE_ALL(COMMA_INTER, COMPOSE, EXPAND_ARG(CAT), SELECT_COLUMNS(PATCH_CONTROL_TABLE, Enumerator, Assignment))
};

extern const enum PatchControl minPatchControl;

const char *patchControl2enumStr(enum PatchControl x);

const char *patchControl2commandStr(enum PatchControl x);

struct MFile;

enum PatchControl parsePatchControl(struct MFile CP f);

#endif
