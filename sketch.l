%{

/* sketch.l
   Copyright (C) 2005,2006,2007,2008 Eugene K. Ressler, Jr.

This file is part of Sketch, a small, simple system for making 
3d drawings with LaTeX and the PSTricks or TikZ package.

Sketch is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

Sketch is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Sketch; see the file COPYING.txt.  If not, see
http://www.gnu.org/copyleft */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "geometry.h"
#include "symbol.h"
#include "expr.h"
#include "parse.h"
#include "y.tab.h"
#include "error.h"
#include "opts.h"

#if defined(_WIN32) && !defined fileno
#define fileno(F) _fileno(F)
#pragma warning(disable:4102)
#endif

// undo the safety net in polygon.h so flex scanner can use these library functions
#undef malloc
#undef realloc
#undef free
#undef strdup

SRC_LINE line;

#define MAX_INPUT_DEPTH 10

struct {
 YY_BUFFER_STATE buffer_state;
 SRC_LINE line;
} input_stack[MAX_INPUT_DEPTH];
int input_stack_ptr = 0;

#define INIT_SPECIAL_BUF_SIZE 40

void update_line_number(void)
{
  int i;

  for (i = 0; yytext[i]; i++)
    if (yytext[i] == '\n')
      line.number++;
}

%}

%x inputdirective

Identifier [A-Za-z]([A-Za-z0-9_]*[A-Za-z0-9])?
WSnotNL [ \t\r]
WS [ \t\r\n]

%%

[%#].* /* comment; do nothing */

{WSnotNL}+ /* white space; do nothing */ 

"\n" { 
  line.number++; 
}

"["[^\]=]+"="[^\]]+"]" { /* [<stuff>=<stuff>] */
  // elide the brackets
  yylval.str = safe_malloc(yyleng + 1 - 2);
  str_slice(yylval.str, yyleng + 1 - 2, yytext, 1, -1);
  return OPTS_STR;
}

"language" {
	return LANGUAGE;
}

"pstricks" {
	return PSTRICKS;
}

"tikz" {
	return TIKZ;
}

"latex" {
        return LaTeX;
}

"context" {
        return ConTeXt;
} 

"input"{WS}* {
  update_line_number();
  BEGIN(inputdirective);
}

"curve" {
  return CURVE;
}

"camera" {
  return CAMERA;
}

"def" {
  return DEF;
}

"dots" {
  return DOTS;
}

"frame" {
  return FRAME;
}

"global" {
  return GLOBAL;
}

"line" {
  return LINE;
}

"polygon" {
  return POLYGON;
}

"put" {
  return PUT;
}

"repeat" {
  return REPEAT;
}

"set" {
  return SET;
}

"sweep" {
  return SWEEP;
}

"then" {
  return THEN;
}

"asin"{WS}*"(" {
  update_line_number();
  return ASIN;
}

"acos"{WS}*"(" {
  update_line_number();
  return ACOS;
}

"atan2"{WS}*"(" {
  update_line_number();
  return ATAN2;
}

"cos"{WS}*"(" {
  update_line_number();
  return COS;
}

"inverse"{WS}*"(" {
  update_line_number();
  return INVERSE;
}

"picturebox" {
  return PICTUREBOX;
}

"perspective"{WS}*"(" {
  update_line_number();
  return PERSPECTIVE;
}

"project"{WS}*"(" {
  update_line_number();
  return PROJECT;
}

"rotate"{WS}*"(" {
  update_line_number();
  return ROTATE;
}

"scale"{WS}*"(" {
  update_line_number();
  return SCALE;
}

"sin"{WS}*"(" {
  update_line_number();
  return SIN;
}

"special"{WS}* {  
  char *rtn;
  int max_rtn_len, delim, i, ch;

  update_line_number();

  max_rtn_len = INIT_SPECIAL_BUF_SIZE - 1;
  rtn = safe_malloc(max_rtn_len + 1);
  i = 0;

  delim = input();

  if (delim == EOF) {
    err(line, "expected special text, found end of file");
  }
  else {
    while ((ch = input()) != delim && ch != EOF) {
      if (ch == '\n')
        ++line.number;
      if (i == max_rtn_len) {
        max_rtn_len *= 2;
        rtn = safe_realloc(rtn, max_rtn_len + 1);
      }
      rtn[i++] = ch;
    }
    if (ch != delim)
      err(line, "missing end delimeter '%c' in special", delim);
  }
  rtn[i] = '\0';
  yylval.str = rtn;
  return SPECIAL;
}

"sqrt"{WS}*"(" {
  update_line_number();
  return SQRT;
}

"translate"{WS}*"(" {
  update_line_number();
  return TRANSLATE;
}

"unit"{WS}*"(" {
  update_line_number();
  return UNIT;
}

"view"{WS}*"(" {
  update_line_number();
  return VIEW;
}

{Identifier} { 
  str_slice(yylval.name, sizeof yylval.name, yytext, 0, sizeof yylval.name);
  return ID; 
}

"<"{Identifier}">" { 
  str_slice(yylval.name, sizeof yylval.name, yytext, 1, -1);
  return ANGLE_ID; 
}

"<>" {
  return EMPTY_ANGLE;
}

"'"{WS}*"x" {
  update_line_number();
  yylval.index = 0;
  return TICK;
}

"'"{WS}*"y" {
  update_line_number();
  yylval.index = 1;
  return TICK;
}

"'"{WS}*"z" {
  update_line_number();
  yylval.index = 2;
  return TICK;
}

"("{Identifier}")" { 
  str_slice(yylval.name, sizeof yylval.name, yytext, 1, -1);
  return PAREN_ID; 
}

"["{Identifier}"]" { 
  str_slice(yylval.name, sizeof yylval.name, yytext, 1, -1);
  return BRACKET_ID; 
}

"[["{Identifier}"]]" { 
  str_slice(yylval.name, sizeof yylval.name, yytext, 2, -2);
  return DBL_BRACKET_ID; 
}

"{"{Identifier}"}" { 
  str_slice(yylval.name, sizeof yylval.name, yytext, 1, -1);
  return CURLY_ID; 
}

(([0-9]+"."[0-9]*)|("."[0-9]+)|([0-9]+))([eE][-+]?[0-9]+)? { 
  if (sscanf(yytext, FLOAT_SCAN_FMT, &yylval.flt) != 1) {
    err(line, "numeric constant %s could not be converted (probably out of range)", yytext);
    yylval.flt = 0;
  }
  return NUM;
}

"("{WS}*{Identifier}{WS}*")" {
  warn(line, "found parenthesized identifier '%s'; delete space if you meant a point ref", yytext);
  yyless(1);
  return '(';
}

"["{WS}*{Identifier}{WS}*"]" {
  warn(line, "found id in brackets '%s'; delete space if you meant a vector ref", yytext);
  yyless(1);
  return '[';
}

"{"{WS}*{Identifier}{WS}*"}" {
  warn(line, "found id in braces '%s'; delete space if you meant a drawable ref", yytext);
  yyless(1);
  return '{';
}

"[["{WS}*{Identifier}{WS}*"]]" {
  warn(line, "found id in double-brackets '%s'; delete space if you meant a transform ref", yytext);
  yyless(1);
  return '[';
}

"<"{WS}*{Identifier}{WS}*">" {
  warn(line, "found id in angle-brackets '%s'; delete space if you meant a tag ref", yytext);
  yyless(1);
  return '<';
}

[-+*/^|.()\[\]{}=,]	{ 
  return yytext[0];	
}

<inputdirective>"{"[^}]*"}" {

  FILE *f;
  char buf[1024];
  int i;

  // Copy path from current sketch source file name, if any.
  i = str_last_occurance(line.file_name, "/\\");
  if (i >= 0) {
    str_slice(buf, sizeof buf, line.file_name, 0, i + 1);
  }
  else {
    buf[0] = '\0';
  }

  i = strlen(buf);
  str_slice(&buf[i], sizeof buf - i, yytext, 1, -1);

  if (input_stack_ptr >= MAX_INPUT_DEPTH) {
    err(line, "inputs nested too deeply (file '%s' ignored)", buf);
  }
  else {
    f = fopen(buf, "r");
    if (f) {
      yyin = f;
      input_stack[input_stack_ptr].buffer_state = YY_CURRENT_BUFFER;
      input_stack[input_stack_ptr].line = line;
      input_stack_ptr++;
      yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
      line.number = 1;
      line.include_p = 1;
      line.file_name = safe_strdup(buf);
    }
    else
      err(line, "can't open input '%s'", buf);
  }
  BEGIN(INITIAL);
}

<<EOF>> {
  if (input_stack_ptr == 0) {
    yyterminate();
  }
  else {
    yy_delete_buffer(YY_CURRENT_BUFFER);
    // don't free: refs may still exist for def locs in the symbol table
    // safe_free(line.file_name);
    --input_stack_ptr;
    yy_switch_to_buffer(input_stack[input_stack_ptr].buffer_state);
    line = input_stack[input_stack_ptr].line;
  }
}

.	{
  die(line, "bad char '%c' (%d)\n", yytext[0], (int)yytext[0]);
}

%%

void set_lexer_file(char *file_name, FILE *f)
{
  line.file_name = safe_strdup(file_name);
  line.include_p = 0;
  line.number = 1;
  yyin = f;
}
