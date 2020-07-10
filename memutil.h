/* memutil.h
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

#ifndef __MEMUTIL_H
#define __MEMUTIL_H

#include <stdlib.h>
#include <string.h>

// booleans
typedef int bool;

// bit N
#define bit(N)  (1u << (N))

// size of a static or auto declared array
#define ARRAY_SIZE(A) (sizeof (A) / sizeof (A)[0])

// zero the memory of struct at *P to zero
#define SET_STRUCT_ZERO(P) memset((P), 0, sizeof *(P))

// zero the array A to zero
#define SET_ARRAY_ZERO(A)  memset((A), 0, sizeof (A))

// checking memory allocators
void *safe_malloc(unsigned size);
void *safe_realloc(void *p, unsigned size);
char *safe_strdup(char *str);
void safe_free(void *p);

#define MALLOC_STRUCT(P)  (P) = safe_malloc(sizeof *(P))

// deliberately cause compilation errors if caller uses
// system memory routines
#ifdef _DEBUG
#define malloc(N)    __call_safe_malloc_instead()
#define realloc(P,N) __call_safe_alloc_instead()
#define strdup(S)    __call_safe_alloc_instead()
#define free(P)      __call_safe_free_instead()
#endif

#endif
