/* opts.h
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

#ifndef __OPTS_H
#define __OPTS_H

#include <stdio.h>
#include "dynarray.h"
#include "error.h"

#define OPT_NONE 0x00

// type flags
#define OPT_INTERNAL 0x01
#define OPT_LINE     0x02
#define OPT_POLYGON  0x04

// default class flags
#define OPT_LINE_STYLE	0x08
#define OPT_FILL_STYLE	0x10
#define OPT_FILL_COLOR	0x20
#define OPT_DEFAULTS (OPT_LINE_STYLE|OPT_FILL_STYLE|OPT_FILL_COLOR)

// key doesn't have type information; use val
#define OPT_TYPE_IN_VAL	0x40

// sufficient to emit value without key
#define OPT_EMIT_VAL		0x80

// some dynamic array types
typedef struct opt_t {
    char *key, *val;
} OPT;

typedef struct opt_list_t {
    DYNAMIC_ARRAY_FIELDS(OPT, elt, n_elts);
} OPT_LIST;

DECLARE_DYNAMIC_ARRAY_PROTOS(OPT_LIST, OPT, opt_list, elt, n_elts);

#define OPTS_LAY 0
#define LAY_IN  0
#define LAY_OVER 1
#define LAY_UNDER -1

typedef struct opts_t {
    OPT_LIST list[1];

    // Cached values.
    unsigned long set_p_mask;
    int lay;
} OPTS;

void init_opts(OPTS * opts);
void setup_opts(OPTS * opts, char *opt_str, SRC_LINE line);
OPTS *raw_opts(void);
OPTS *new_opts(char *opts_str, SRC_LINE line);
void clear_opts(OPTS * opts);
char *opt_val(OPTS * opts, char *opt);
int bool_opt_p(OPTS * opts, char *opt, int default_p);
int opt_type(OPT * opt, int default_type, int lang);
void add_no_edges_default_opt(OPTS ** opts_ptr, int lang);
void add_solid_white_default_opt(OPTS ** opts_ptr, int lang);
// selective copy for splitting option lists by type
OPTS *copy_opts(OPTS * opts, int type_mask, int lang);
OPTS *cat_opts(OPTS * dst, OPTS * src);
// selective copy for splitting out line options and modifying arrows
OPTS *copy_line_opts(OPTS * opts, int first_p, int last_p, int lang);
void emit_opts_raw(FILE * f, OPTS * opts, int lang);
void emit_opts(FILE * f, OPTS * opts, int lang);
void emit_opts_with_exceptions(FILE * f, OPTS * opts, char **exceptions,
			       int lang);
void check_opts(OPTS * opts, int allowed, char *allowed_msg, int lang,
		SRC_LINE line);

// slice src into dest using Perl/Python conventions
char *str_slice(char *dst, int dst_size, char *src, int beg, int end);
#define SLICE_TO_END  ((int)(~0u >> 1))

// find last occurance of aachar in set in src; return index or -1 if none
int str_last_occurance(char *src, char *set);

// strtok with a state variable instead of static
char *istrtok(int *p, char *s, char sep);

#endif
