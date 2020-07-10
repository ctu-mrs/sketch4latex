/* langver.h
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

#ifndef __LANGVER_H
#define __LANGVER_H

#include "error.h"

#define MAX_PST_VERSION_STRING_SIZE 10

typedef struct pst_version_t {
    char str[MAX_PST_VERSION_STRING_SIZE];
    char key[MAX_PST_VERSION_STRING_SIZE * 2];
} PST_VERSION;

int parse_pst_version(PST_VERSION * v, char *str, SRC_LINE line);
int pst_version_cmp(PST_VERSION * a, PST_VERSION * b);

/* Version 1.20 added linejoin option, where pstverb was 
   needed previously. */
#define PST_LINEJOIN_VERSION 1.20

/* This should be set to the latest version that 
   makes a difference for Sketch output. */
#define ASSUMED_PST_VERSION PST_LINEJOIN_VERSION

/* TikZ versions are just like PSTricks versions (except
   no letter qualifiers yet */
typedef PST_VERSION TIKZ_VERSION;

int parse_tikz_version(TIKZ_VERSION * v, char *str, SRC_LINE line);
int tikz_version_cmp(TIKZ_VERSION * a, TIKZ_VERSION * b);

#define TIKZ_DECORATIONS_VERSION 2.00

#define ASSUMED_TIKZ_VERSION TIKZ_DECORATIONS_VERSION

#endif
