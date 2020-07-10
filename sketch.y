/* sketch.y
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

%{

#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
#  include <malloc.h>
#  if !defined(alloca)
#    define alloca _alloca
#  endif
#  ifdef _MSC_VER
     // turn of warning about unused goto label in bison skeleton
#    pragma warning(disable:4102)
#  endif
#  define YYSTACK_USE_ALLOCA 1
#endif

#include "parse.h"
#include "expr.h"
#include "global.h"

int yylex(void);

void yyerror (char *s)  /* Called by yyparse on error */
{
  extern SRC_LINE line;
  err(line, "%s", s);
}

static SYMBOL_TABLE *sym_tab;

// exported parse tree and global environment
static OBJECT *objects;

%}

%union {
  char *str;
  FLOAT flt;
  POINT_3D pt;
  VECTOR_3D vec;
  TRANSFORM xf;
  EXPR_VAL exv;
  SYMBOL_NAME name;
  OBJECT *obj;
  OPTS *opts;
  int bool;
  int index;
}

%token <name>ID <name>PAREN_ID <name>BRACKET_ID 
%token <name>DBL_BRACKET_ID <name>CURLY_ID <name>ANGLE_ID
%token <flt>NUM 
%token <str>OPTS_STR <str>SPECIAL 
%token <index>TICK
%token THEN DEF EMPTY_ANGLE
%token DOTS LINE CURVE POLYGON REPEAT SWEEP PUT
%token TRANSLATE ROTATE SCALE PROJECT PERSPECTIVE VIEW
%token SQRT SIN ASIN COS ACOS ATAN2 UNIT INVERSE
%token GLOBAL SET PICTUREBOX FRAME CAMERA
%token LANGUAGE PSTRICKS TIKZ LaTeX ConTeXt

%type <name>  tagged_defs
%type <opts>  options option_id_list
%type <flt>   scalar scalar_expr opt_baseline
%type <pt>    point point_expr
%type <vec>   vector_literal vector_id vector vector_expr
%type <xf>    transform transform_expr
%type <exv>   expr
%type <obj>   defs_and_decls rev_defs_and_decls decl def_or_decl defable
%type <obj>   points rev_points transforms rev_transforms special_arg
%type <obj>   special_args rev_special_args
%type <bool>  opt_star
%type <index> output_language comma_macro_package graphics_language macro_package

%right THEN
%left '-' '+'
%left '*' '/' '.'
%left NEG     /* negation--unary minus */
%right '^'    /* exponentiation */
%left TICK    /* point and vector indexing */

%%

input                 : defs_and_decls global_decl_block { objects = $1; }

                      /* sets global_env as a side effect */
global_decl_block     : GLOBAL '{' global_decls '}'
                      | /* empty */
                      ;

global_decls          : global_decls global_decl 
                      | global_decl
                      ;

global_decl           : SET OPTS_STR 
                          { 
                            set_global_env_opts(global_env, $2, line);
                          }
                      | PICTUREBOX '[' scalar_expr ']'
                          { 
                            set_global_baseline(global_env, $3, line);
                          }
                      | PICTUREBOX opt_baseline point point
                          { 
                            set_global_baseline(global_env, $2, line);
                            set_global_env_extent(global_env, $3, $4, line);
                          }
                      | CAMERA transform_expr
                          {
                            set_global_env_camera(global_env, $2, line);
                          }
                      | FRAME 
                          { 
                            set_global_env_frame(global_env, NULL, line);
                          }
                      | FRAME OPTS_STR
                          { 
                            set_global_env_frame(global_env, $2, line);
                          }
                      | LANGUAGE output_language 
                          {
                            set_global_output_language(global_env, $2, line);
		          }
                      | def
                      ;

output_language       : graphics_language comma_macro_package { $$ = $1 | $2; }
                      ;

graphics_language     : PSTRICKS { $$ = GEOL_PSTRICKS; }
                      | TIKZ     { $$ = GEOL_TIKZ; }
		      ;

comma_macro_package   : ',' macro_package { $$ = $2; }
                      | /* empty */       { $$ = GEOL_LATEX; }
                      ;

macro_package         : LaTeX     { $$ = GEOL_LATEX; } 
                      | ConTeXt   { $$ = GEOL_CONTEXT; }
                      ;
 
opt_baseline          : '[' scalar_expr ']' { $$ = $2; }
                      | /* empty */         { $$ = NO_BASELINE; }
                      ;
                      
defs_and_decls        : rev_defs_and_decls  { $$ = sibling_reverse($1); }
                      ;

rev_defs_and_decls    : rev_defs_and_decls def_or_decl  { $$ = cat_objects(sibling_reverse($2), $1); }
                      | def_or_decl                     { $$ = sibling_reverse($1); }
                      ;

def_or_decl           : def   { $$ = NULL; }
                      | decl  { $$ = $1; }
                      ;

                      /* slightly strange rules are to avoid inherited attributes */
def                   : DEF ID defable  { new_symbol(sym_tab, $2, 0, $3, line); }
                      | tagged_defs EMPTY_ANGLE defable { new_symbol(sym_tab, $1, 0, $3, line); }
                      | DEF ID EMPTY_ANGLE  { new_symbol(sym_tab, $2, 0, new_tag_def(), line); }
                      ;

tagged_defs           : DEF ID ANGLE_ID defable      { strcpy($$, new_symbol(sym_tab, $2, $3, $4, line) ? "" : $2); }
                      | tagged_defs ANGLE_ID defable { strcpy($$, new_symbol(sym_tab, $1, $2, $3, line) ? "" : $1); }
                      ;

defable               : expr     { $$ = object_from_expr(&$1); }
                      | decl     { $$ = $1; }
                      | OPTS_STR { $$ = new_opts_def($1, line); }
                      ;

decl                  : DOTS options points		  { $$ = new_dots($2, $3); }
                      | LINE options points     { $$ = new_line($2, $3); }
                      | CURVE options points    { $$ = new_curve($2, $3); }
                      | POLYGON options points  { $$ = new_polygon($2, $3); }
		      | SWEEP options '{' scalar_expr opt_star ',' transforms '}' point 
		          { 
                              $$ = new_sweep($2, $4, $5, $7, new_point_def($9));
                          }
                      | SWEEP options '{' scalar_expr opt_star ',' transforms '}' decl
			  {
                              $$ = new_sweep($2, $4, $5, $7, $9);
                          }
                      | REPEAT '{' scalar_expr ',' transforms '}' decl
                          {
                              $$ = new_repeat($3, $5, $7);
                          }
                      | PUT '{' transform_expr '}' decl { $$ = new_compound($3, $5); }
                      | SPECIAL special_args { $$ = new_special($1, $2, line); }
                      | CURLY_ID { look_up_drawable(sym_tab, &$$, line, $1); }
                      | '{'             { sym_tab = new_scope(sym_tab); }
                        defs_and_decls  { sym_tab = old_scope(sym_tab); }
                        '}'
                          {
                            if ($3 == NULL)
                              err(line, "no drawables in compound declaration");
                            $$ = $3;
                          }
                      ;

opt_star              : EMPTY_ANGLE { $$ = 1; }
                      | /* empty */ { $$ = 0; }
                      ;

option_id_list        : option_id_list ',' ID
                          { 
                            $$ = look_up_and_append_to_opts(sym_tab, &$1, line, $3);
                          }
                      | ID 
                          { 
                            $$ = NULL;
                            $$ = look_up_and_append_to_opts(sym_tab, &$$, line, $1);
			  }
                      ;

options               : OPTS_STR    { $$ = new_opts($1, line); }
                      | BRACKET_ID  { look_up_opts(sym_tab, &$$, line, $1); }
                      | '[' option_id_list ']' { $$ = $2; }
                      | /* empty */ { $$ = NULL; }
                      ;

points                : rev_points  { $$ = sibling_reverse($1); }
                      ;

rev_points            : rev_points point  { $$ = cat_objects(new_point_def($2), $1); }
                      | point             { $$ = new_point_def($1); }
                      ;

special_args          : rev_special_args { $$ = sibling_reverse($1); }
                      ;

rev_special_args      : rev_special_args special_arg { $$ = cat_objects($2, $1); }
                      | /* empty */                  { $$ = NULL; }
                      ;

special_arg           : scalar                 { $$ = new_scalar_def($1); }
                      | point                  { $$ = new_point_def($1); }
                      | vector_literal         { $$ = new_vector_def($1); }
                      | OPTS_STR               { $$ = new_opts_def($1, line); }
                      | BRACKET_ID             { look_up_vector_or_opts(sym_tab, &$$, line, $1); }
                      ;

transforms            : rev_transforms { $$ = sibling_reverse($1); }
                      ;

rev_transforms        : rev_transforms ',' transform_expr { $$ = cat_objects(new_transform_def($3), $1); } 
                      | transform_expr                    { $$ = new_transform_def($1); } 
                      ;

expr                  : scalar                      { set_float(&$$, $1); }
                      | point                       { set_point(&$$, $1); }
                      | vector                      { set_vector(&$$, $1); }
                      | transform                   { set_transform(&$$, $1); }
                      | expr '+' expr               { do_add(&$$, &$1, &$3, line); }
                      | expr '-' expr               { do_sub(&$$, &$1, &$3, line); }
                      | expr '*' expr               { do_mul(&$$, &$1, &$3, line); }
                      | expr '/' expr               { do_dvd(&$$, &$1, &$3, line); }
                      | expr '.' expr               { do_dot(&$$, &$1, &$3, line); }
                      | expr THEN expr              { do_thn(&$$, &$1, &$3, line); }
                      | '|' expr '|'                { do_mag(&$$, &$2, line); }
                      | '-' expr %prec NEG          { do_neg(&$$, &$2, line); }
                      | expr '^' expr               { do_pwr(&$$, &$1, &$3, line); }
                      | '(' expr ')'                { $$ = $2; }
                      | UNIT  expr ')'              { do_unit(&$$, &$2, line); }
                      | SQRT  expr ')'              { do_sqrt(&$$, &$2, line); }
                      | SIN   expr ')'              { do_sin(&$$, &$2, line); }
                      | COS   expr ')'              { do_cos(&$$, &$2, line); }
                      | ASIN  expr ')'              { do_asin(&$$, &$2, line); }
                      | ACOS  expr ')'              { do_acos(&$$, &$2, line); }
                      | ATAN2 expr ',' expr ')'     { do_atan2(&$$, &$2, &$4, line); }
                      | expr TICK                   { do_index(&$$, &$1, $2, line); }
                      ;

scalar                : NUM { $$ = $1; }
                      | ID  { look_up_scalar(sym_tab, &$$, line, $1); }
                      ;

scalar_expr           : expr  { coerce_to_float(&$1, &$$, line); }
                      ;

point                 : '(' scalar_expr ',' scalar_expr ',' scalar_expr ')'  
                          { 
                            $$[X] = $2; $$[Y] = $4; $$[Z] = $6;
                          }
                      | '(' scalar_expr ',' scalar_expr ')'
                          { 
                            $$[X] = $2; $$[Y] = $4; $$[Z] = 0;
                          }
                      | PAREN_ID { look_up_point(sym_tab, $$, line, $1); }
                      ;

point_expr            : expr  { coerce_to_point(&$1, $$, line); }
                      ;

vector                : vector_literal { copy_vec_3d($$, $1); }
                      | vector_id      { copy_vec_3d($$, $1); }
                      ;

vector_literal        : '[' scalar_expr ',' scalar_expr ',' scalar_expr ']'
                          { 
                            $$[X] = $2; $$[Y] = $4; $$[Z] = $6;
                          }
                      | '[' scalar_expr ',' scalar_expr ']'
                          { 
                            $$[X] = $2; $$[Y] = $4; $$[Z] = 0;
                          }
                      ;

vector_id             : BRACKET_ID { look_up_vector(sym_tab, $$, line, $1); }
                      ;

vector_expr           : expr  { coerce_to_vector(&$1, $$, line); }
                      ;

transform             : '[' 
                        '[' scalar_expr ',' scalar_expr ',' scalar_expr',' scalar_expr ']'
                        '[' scalar_expr ',' scalar_expr ',' scalar_expr',' scalar_expr ']'
                        '[' scalar_expr ',' scalar_expr ',' scalar_expr',' scalar_expr ']'
                        '[' scalar_expr ',' scalar_expr ',' scalar_expr',' scalar_expr ']'
                        ']'
                          { // transform is column major while elements are row major
                            $$[0] = $3;  $$[4] = $5;  $$[8]  = $7;  $$[12] = $9;
                            $$[1] = $12; $$[5] = $14; $$[9]  = $16; $$[13] = $18;
                            $$[2] = $21; $$[6] = $23; $$[10] = $25; $$[14] = $27;
                            $$[3] = $30; $$[7] = $32; $$[11] = $34; $$[15] = $36;
                          }
                      | ROTATE scalar_expr ')'
                          {
                            set_angle_axis_rot_about_point($$, $2 * (PI/180), 0, 0);
                          }
                      | ROTATE scalar_expr ',' expr ')'
                          {
                            if (EXPR_TYPE_IS(&$4, E_POINT))
                              set_angle_axis_rot_about_point($$, $2 * (PI/180), $4.val.pt, 0);
                            else if (EXPR_TYPE_IS(&$4, E_VECTOR))
                              set_angle_axis_rot_about_point($$, $2 * (PI/180), 0, $4.val.vec);
                            else
                              err(line, "expected point or vector rotation parameter, and it's a %s",
                                expr_val_type_str[$4.tag]);
                          }
                      | ROTATE scalar_expr ',' point_expr ',' vector_expr ')'
                          {
                            set_angle_axis_rot_about_point($$, $2 * (PI/180), $4, $6);
                          }
                      | TRANSLATE vector_expr ')'
                          {
                            set_translation($$, $2[X], $2[Y], $2[Z]);
                          }
                      | SCALE expr ')'
                          {
                            if ($2.tag == E_FLOAT) {
                              FLOAT s = $2.val.flt;
                              set_scale($$, s, s, s);
                            }
                            else if ($2.tag == E_VECTOR) {
                              VECTOR v = $2.val.vec;
                              set_scale($$, v[X], v[Y], v[Z]);
                            }
                            else {
                              err(line, 
                                "expected scalar or vector scale parameter, and it's a %s",
                                expr_val_type_str[$2.tag]);
                              set_ident($$);
                            }
                          }
                      | PROJECT ')'                 { set_parallel_projection($$); }
                      | PROJECT scalar_expr ')'     { set_perspective_projection($$, $2); }
                      | PERSPECTIVE scalar_expr ')' { set_perspective_transform($$, $2); }
                      | VIEW point_expr ',' expr ',' vector_expr ')'
                          {
                            if ($4.tag == E_VECTOR)
                              set_view_transform($$, $2, $4.val.vec, $6);
                            else if ($4.tag == E_POINT)
                              set_view_transform_with_look_at($$, $2, $4.val.pt, $6);
                            else
                              err(line, "expected point or vector view parameter, and it's a %s",
                                expr_val_type_str[$4.tag]);
                          }
                      | VIEW point_expr ',' expr ')'
                          {
                            if ($4.tag == E_VECTOR)
                              set_view_transform($$, $2, $4.val.vec, NULL);
                            else if ($4.tag == E_POINT)
                              set_view_transform_with_look_at($$, $2, $4.val.pt, NULL);
                            else
                              err(line, "expected point or vector view parameter, and it's a %s",
                                expr_val_type_str[$4.tag]);
                          }
                      | VIEW point_expr ')'
                          {
                            set_view_transform($$, $2, NULL, NULL);
                          }

                      | INVERSE transform_expr ')'  { do_inverse($$, $2, line); }
                      | DBL_BRACKET_ID              { look_up_transform(sym_tab, $$, line, $1); }
                      ;

transform_expr        : expr  { coerce_to_transform(&$1, $$, line); }
                      ;

%%

int parse(SYMBOL_TABLE *st)
{
  int ret;

  objects = NULL;
  sym_tab = st;
  ret = yyparse();

  // should set sym_tab back to NULL
  sym_tab = old_scope(sym_tab);
  if (sym_tab)
    die(no_line, "zombie symbol table");

  return ret;
}

OBJECT *parsed_objects(void)
{
	return objects;
}
