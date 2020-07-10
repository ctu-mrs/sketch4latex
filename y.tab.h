/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ID = 258,
     PAREN_ID = 259,
     BRACKET_ID = 260,
     DBL_BRACKET_ID = 261,
     CURLY_ID = 262,
     ANGLE_ID = 263,
     NUM = 264,
     OPTS_STR = 265,
     SPECIAL = 266,
     TICK = 267,
     THEN = 268,
     DEF = 269,
     EMPTY_ANGLE = 270,
     DOTS = 271,
     LINE = 272,
     CURVE = 273,
     POLYGON = 274,
     REPEAT = 275,
     SWEEP = 276,
     PUT = 277,
     TRANSLATE = 278,
     ROTATE = 279,
     SCALE = 280,
     PROJECT = 281,
     PERSPECTIVE = 282,
     VIEW = 283,
     SQRT = 284,
     SIN = 285,
     ASIN = 286,
     COS = 287,
     ACOS = 288,
     ATAN2 = 289,
     UNIT = 290,
     INVERSE = 291,
     GLOBAL = 292,
     SET = 293,
     PICTUREBOX = 294,
     FRAME = 295,
     CAMERA = 296,
     LANGUAGE = 297,
     PSTRICKS = 298,
     TIKZ = 299,
     LaTeX = 300,
     ConTeXt = 301,
     NEG = 302
   };
#endif
/* Tokens.  */
#define ID 258
#define PAREN_ID 259
#define BRACKET_ID 260
#define DBL_BRACKET_ID 261
#define CURLY_ID 262
#define ANGLE_ID 263
#define NUM 264
#define OPTS_STR 265
#define SPECIAL 266
#define TICK 267
#define THEN 268
#define DEF 269
#define EMPTY_ANGLE 270
#define DOTS 271
#define LINE 272
#define CURVE 273
#define POLYGON 274
#define REPEAT 275
#define SWEEP 276
#define PUT 277
#define TRANSLATE 278
#define ROTATE 279
#define SCALE 280
#define PROJECT 281
#define PERSPECTIVE 282
#define VIEW 283
#define SQRT 284
#define SIN 285
#define ASIN 286
#define COS 287
#define ACOS 288
#define ATAN2 289
#define UNIT 290
#define INVERSE 291
#define GLOBAL 292
#define SET 293
#define PICTUREBOX 294
#define FRAME 295
#define CAMERA 296
#define LANGUAGE 297
#define PSTRICKS 298
#define TIKZ 299
#define LaTeX 300
#define ConTeXt 301
#define NEG 302




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 57 "sketch.y"
typedef union YYSTYPE {
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
} YYSTYPE;
/* Line 1447 of yacc.c.  */
#line 146 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



