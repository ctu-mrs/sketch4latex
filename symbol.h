/* symbol.h
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

#ifndef __SYMBOL_H
#define __SYMBOL_H

#include "error.h"
#include "scene.h"

typedef char SYMBOL_NAME[32];

typedef struct symbol_t {
    struct symbol_t *next;
    SYMBOL_NAME name, tag;
    SRC_LINE def_line;
    int n_references;
    OBJECT *obj;
} SYMBOL;

#define SYMBOL_HASH_SIZE  79

typedef struct symbol_table_t {
    struct symbol_table_t *enclosing;
    SYMBOL *head[SYMBOL_HASH_SIZE];
} SYMBOL_TABLE;

// chain a new scope onto an existing symbol table (or NULL)
// and return the new table
SYMBOL_TABLE *new_scope(SYMBOL_TABLE * sym_tab);

// unchain the inner scope from an existing symbol table
// and return the next outer scope or NULL if this was
// the outermost scope
SYMBOL_TABLE *old_scope(SYMBOL_TABLE * sym_tab);

// look up a symbol of the given name in all the scopes of the
// provided symbol table
SYMBOL *lookup(SYMBOL_TABLE * sym_tab, char *name);

// look up a symbol with type checking and value extraction
void look_up_tag(SYMBOL_TABLE * sym_tab, int *exists_p, SRC_LINE line,
		 char *name);
void look_up_opts(SYMBOL_TABLE * sym_tab, OPTS ** opts, SRC_LINE line,
		  char *name);
void look_up_scalar(SYMBOL_TABLE * sym_tab, FLOAT * r, SRC_LINE line,
		    char *name);
void look_up_point(SYMBOL_TABLE * sym_tab, POINT_3D r, SRC_LINE line,
		   char *name);
void look_up_vector(SYMBOL_TABLE * sym_tab, VECTOR_3D r, SRC_LINE line,
		    char *name);
void look_up_transform(SYMBOL_TABLE * sym_tab, TRANSFORM r, SRC_LINE line,
		       char *name);
void look_up_drawable(SYMBOL_TABLE * sym_tab, OBJECT ** r, SRC_LINE line,
		      char *name);
void look_up_vector_or_opts(SYMBOL_TABLE * sym_tab, OBJECT ** r,
			    SRC_LINE line, char *name);

// predicated for tag existence; raises error if not
int tag_exists_p(SYMBOL_TABLE * sym_tab, char *name);

// insert a given name in the symbol table
SYMBOL *new_symbol(SYMBOL_TABLE * sym_tab, char *name, char *tag,
		   OBJECT * obj, SRC_LINE def_line);
OBJECT *remove_symbol(SYMBOL_TABLE * sym_tab, char *name, SRC_LINE line);

// look up the name, which must be an option
// if the lookup succeeds, append the option to options at *r (creating a new raw option
// structure in the case where *r is null)
// return the options *r
OPTS *look_up_and_append_to_opts(SYMBOL_TABLE * sym_tab, OPTS ** r,
				 SRC_LINE line, char *name);

#endif
