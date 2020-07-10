/* memutil.c
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
#include "memutil.h"

void *safe_malloc(unsigned size)
{
#undef malloc
    void *p = malloc(size);
    if (size > 0 && p == NULL) {
	fprintf(stderr, "\nsafe_malloc: out of memory\n");
	exit(1);
    }
    return p;
}

void *safe_realloc(void *p, unsigned size)
{
#undef realloc
    void *q = realloc(p, size);
    if (size > 0 && q == NULL) {
	fprintf(stderr, "\nsafe_realloc: out of memory\n");
	exit(1);
    }
    return q;
}

char *safe_strdup(char *str)
{
#undef strdup
    char *new_str = safe_malloc(strlen(str) + 1);
    strcpy(new_str, str);
    return new_str;
}

void safe_free(void *p)
{
#undef free
    free(p);
}
