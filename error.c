/* error.c
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
#include <stdarg.h>
#include "error.h"

SRC_LINE no_line = { NULL, 0 };

typedef enum error_type_t {
    E_REMARK,
    E_WARNING,
    E_ERROR,
    E_FATAL_ERROR,
    E_N				// keep last; this is the number of error types
} ERROR_TYPE;

char *error_type_str[] = {
    "remark",
    "warning",
    "error",
    "fatal",
};

static int n_errors[E_N];	// assumes C initialization to zero

static void
print_error(ERROR_TYPE type, SRC_LINE line, char *fmt, va_list arg_list)
{
    n_errors[type]++;
    if (line.file_name)
	fprintf(stderr, "%s", line.file_name);
    if (line.number > 0)
	fprintf(stderr, "(%u) : ", line.number);
    fprintf(stderr, "%s, ", error_type_str[type]);
    vfprintf(stderr, fmt, arg_list);
    fprintf(stderr, "\n");
}

void remark(SRC_LINE line, char *fmt, ...)
{
    va_list arg_list;

    va_start(arg_list, fmt);
    print_error(E_REMARK, line, fmt, arg_list);
    va_end(arg_list);
}

void warn(SRC_LINE line, char *fmt, ...)
{
    va_list arg_list;

    va_start(arg_list, fmt);
    print_error(E_WARNING, line, fmt, arg_list);
    va_end(arg_list);
}

void err(SRC_LINE line, char *fmt, ...)
{
    va_list arg_list;

    va_start(arg_list, fmt);
    print_error(E_ERROR, line, fmt, arg_list);
    va_end(arg_list);
}

void die(SRC_LINE line, char *fmt, ...)
{
    va_list arg_list;

    va_start(arg_list, fmt);
    print_error(E_FATAL_ERROR, line, fmt, arg_list);
    va_end(arg_list);
    report_errors();
}

int trouble_p(void)
{
    return n_errors[E_ERROR] > 0 || n_errors[E_FATAL_ERROR] > 0;
}

void report_errors(void)
{
    int i, n_total = 0;

    // start printing summary
    fprintf(stderr, "summary: ");
    for (i = 0; i < E_N; i++) {
	if (n_errors[i] > 0) {
	    if (n_total > 0)
		fprintf(stderr, ", ");
	    fprintf(stderr, "%d %s", n_errors[i], error_type_str[i]);
	    n_total++;
	}
    }

    // see if we need to abort
    if (trouble_p()) {
	fprintf(stderr, "\nquitting...\n");
	exit(1);
    }
    // not aborting, so finish up the summary
    if (n_total == 0) {
	fprintf(stderr, "no errors");
    }
    fprintf(stderr, "\n");

    // zero the error counters
    for (i = 0; i < E_N; i++)
	n_errors[i] = 0;
}
