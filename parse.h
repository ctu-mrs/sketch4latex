/* parse.h
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

#ifndef __PARSE_H
#define __PARSE_H

#include "scene.h"
#include "error.h"
#include "symbol.h"

/* Skip yyunput() as we don't use it. */
#define YY_NO_UNPUT

extern int yydebug;
extern SRC_LINE line;
extern GLOBAL_ENV parsed_global_env[1];

void set_lexer_file(char *file_name, FILE * f);
int parse(SYMBOL_TABLE * st);
OBJECT *parsed_objects(void);

#endif
