/* error.h
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

#ifndef __ERROR_H
#define __ERROR_H

typedef struct source_line_t {
    char *file_name;
    int include_p;
    int number;
} SRC_LINE;

extern SRC_LINE no_line;
#define is_no_line_p(L) ((L).number <= 0)

void remark(SRC_LINE line, char *fmt, ...);
void warn(SRC_LINE line, char *fmt, ...);
void err(SRC_LINE line, char *fmt, ...);
void die(SRC_LINE line, char *fmt, ...);
int trouble_p(void);
void report_errors(void);

#endif
