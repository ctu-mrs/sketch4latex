/* emit.h
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

#ifndef __EMIT_H
#define __EMIT_H

#include <stdio.h>
#include "scene.h"
#include "global.h"

char *flt_str(char *buf, double f);
void process_special(FILE * f, SPECIAL_OBJECT * special, SRC_LINE line);

char *doc_template_from_file(char *file_name, int output_language);
extern char standard_us_doc_template_file_name_flag[];
extern char standard_euro_doc_template_file_name_flag[];

void emit(FILE * f, OBJECT * obj, GLOBAL_ENV * env,
	  char *doc_template_file_name);

#endif
