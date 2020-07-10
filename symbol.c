/* symbol.c
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
#include <string.h>
#include "error.h"
#include "geometry.h"
#include "symbol.h"

// dragon book hash function due to as&u.
unsigned hash(char *s)
{
    unsigned h = 0, g;

    while (*s) {
	h = (h << 4) + *s++;
	if ((g = h & 0xf0000000L) != 0) {
	    h ^= (g >> 24);
	    h ^= g;
	}
    }
    return h;
}

// chain a new scope onto an existing symbol table (or NULL)
// and return the new table
SYMBOL_TABLE *new_scope(SYMBOL_TABLE * sym_tab)
{
    int i;
    SYMBOL_TABLE *new_sym_tab = safe_malloc(sizeof *new_sym_tab);
    new_sym_tab->enclosing = sym_tab;
    for (i = 0; i < ARRAY_SIZE(new_sym_tab->head); i++)
	new_sym_tab->head[i] = NULL;
    return new_sym_tab;
}

// unchain the inner scope from an existing symbol table
// and return the next outer scope or NULL if this was
// the outermost scope; as a side effect, warns of unused
// symbols
//
// we are not worrying about freeing storage; the def nodes
// are needed for object names anyway
SYMBOL_TABLE *old_scope(SYMBOL_TABLE * sym_tab)
{
    int i;
    SYMBOL *sym, *sym_next;
    SYMBOL_TABLE *sym_tab_enclosing;

    // warn of unreferenced symbols
    for (i = 0; i < ARRAY_SIZE(sym_tab->head); i++) {
	for (sym = sym_tab->head[i]; sym; sym = sym_next) {
	    sym_next = sym->next;
	    if (sym->n_references == 0 && !sym->def_line.include_p) {
		if (sym->obj)
		    warn(sym->def_line, "%s '%s' is never referenced",
			 object_type_str[sym->obj->tag], sym->name);
		else
		    warn(sym->def_line, "'%s' is never referenced",
			 sym->name);
	    }
	    safe_free(sym);
	}
    }
    sym_tab_enclosing = sym_tab->enclosing;
    safe_free(sym_tab);
    return sym_tab_enclosing;
}

static SYMBOL *lookup_in_inner_scope(SYMBOL_TABLE * sym_tab, char *name,
				     unsigned index)
{
    SYMBOL *sym;

    for (sym = sym_tab->head[index]; sym; sym = sym->next)
	if (strncmp(name, sym->name, sizeof sym->name - 1) == 0)
	    return sym;
    return NULL;
}

SYMBOL *lookup(SYMBOL_TABLE * sym_tab, char *name)
{
    SYMBOL *sym;
    unsigned index;

    index = hash(name) % ARRAY_SIZE(sym_tab->head);
    do {
	sym = lookup_in_inner_scope(sym_tab, name, index);
	if (sym) {
	    sym->n_references++;
	    return sym;
	}
	sym_tab = sym_tab->enclosing;
    }
    while (sym_tab);
    return NULL;
}

static OBJECT *lookup_with_type_check(SYMBOL_TABLE * sym_tab,
				      OBJECT_TYPE tag, SRC_LINE line,
				      char *name)
{
    SYMBOL *sym = lookup(sym_tab, name);
    if (sym) {
	if (sym->obj) {
	    if (sym->obj->tag == tag)
		return sym->obj;
	    else
		err(line, "expected %s to be a %s and instead it's a %s",
		    name, object_type_str[tag],
		    object_type_str[sym->obj->tag]);
	} else {
	    err(line, "%s has a null definition", name);
	}
    } else {
	err(line, "found undefined identifier %s while looking for %s",
	    name, object_type_str[tag]);
    }
    return NULL;
}

void
look_up_tag(SYMBOL_TABLE * sym_tab, int *exists_p, SRC_LINE line,
	    char *name)
{
    SYMBOL *sym;

    if (name) {
	sym = lookup(sym_tab, name);
	*exists_p = sym && sym->obj && sym->obj->tag == O_TAG_DEF;
    } else {
	*exists_p = 0;
    }
}

int tag_exists_p(SYMBOL_TABLE * sym_tab, char *name)
{
    int exists_p;
    look_up_tag(sym_tab, &exists_p, no_line, name);
    if (!exists_p)
	err(no_line, "undefined tag %s", name);
    return exists_p;
}

void
look_up_opts(SYMBOL_TABLE * sym_tab, OPTS ** r, SRC_LINE line, char *name)
{
    OPTS_DEF *def =
	(OPTS_DEF *) lookup_with_type_check(sym_tab, O_OPTS_DEF, line,
					    name);
    *r = def ? def->opts : NULL;
}

OPTS *look_up_and_append_to_opts(SYMBOL_TABLE * sym_tab, OPTS ** r,
				 SRC_LINE line, char *name)
{
    OPTS *opts;
    look_up_opts(sym_tab, &opts, line, name);
    if (opts) {
	if (*r == NULL)
	    *r = raw_opts();
	cat_opts(*r, opts);
    }
    return *r;
}

void
look_up_scalar(SYMBOL_TABLE * sym_tab, FLOAT * r, SRC_LINE line,
	       char *name)
{
    SCALAR_DEF *def =
	(SCALAR_DEF *) lookup_with_type_check(sym_tab, O_SCALAR_DEF, line,
					      name);
    *r = def ? def->val : 0;
}

void
look_up_point(SYMBOL_TABLE * sym_tab, POINT_3D r, SRC_LINE line,
	      char *name)
{
    POINT_DEF *def =
	(POINT_DEF *) lookup_with_type_check(sym_tab, O_POINT_DEF, line,
					     name);
    if (def)
	copy_pt_3d(r, def->p);
    else
	r[X] = r[Y] = r[Z] = 0;
}

void
look_up_vector(SYMBOL_TABLE * sym_tab, VECTOR_3D r, SRC_LINE line,
	       char *name)
{
    VECTOR_DEF *def =
	(VECTOR_DEF *) lookup_with_type_check(sym_tab, O_VECTOR_DEF, line,
					      name);
    if (def)
	copy_vec_3d(r, def->v);
    else
	zero_vec_3d(r);
}

void
look_up_transform(SYMBOL_TABLE * sym_tab, TRANSFORM r, SRC_LINE line,
		  char *name)
{
    TRANSFORM_DEF *def =
	(TRANSFORM_DEF *) lookup_with_type_check(sym_tab, O_TRANSFORM_DEF,
						 line, name);
    if (def)
	copy_transform(r, def->xf);
    else
	set_ident(r);
}

void
look_up_drawable(SYMBOL_TABLE * sym_tab, OBJECT ** r, SRC_LINE line,
		 char *name)
{
    SYMBOL *sym = lookup(sym_tab, name);
    *r = NULL;
    if (sym) {
	if (sym->obj) {
	    if (is_drawable(sym->obj))
		*r = copy_drawable(sym->obj);	// copy needed so concat of lookup result is ok
	    else {
		err(line,
		    "expected %s to be a drawable object and instead it's a %s",
		    name, object_type_str[sym->obj->tag]);
	    }
	} else {
	    err(line, "%s contains no drawable objects", name);
	}
    } else {
	err(line,
	    "found undefined identifier %s while looking for a drawable object",
	    name);
    }
}

void look_up_vector_or_opts(SYMBOL_TABLE * sym_tab, OBJECT ** r,
			    SRC_LINE line, char *name)
{
    SYMBOL *sym = lookup(sym_tab, name);
    *r = NULL;
    if (sym) {
	if (sym->obj) {
	    switch (sym->obj->tag) {
	    case O_VECTOR_DEF:{
		    VECTOR_DEF *d = (VECTOR_DEF *) sym->obj;
		    *r = new_vector_def(d->v);
		}
		break;
	    case O_OPTS_DEF:{
		    OPTS_DEF *d = (OPTS_DEF *) sym->obj;
		    *r = new_opts_def_copy(d->opts);
		}
		break;
	    default:
		err(line, "expected %s to be special options or argument",
		    name);
		break;
	    }
	}
    } else {
	err(line,
	    "found undefined identifier %s while looking for special options or argument",
	    name);
    }
}

OBJECT *remove_from_inner_scope(SYMBOL_TABLE * sym_tab, char *name,
				unsigned index)
{
    OBJECT *r;
    SYMBOL *sym, *prev_sym;

    for (prev_sym = NULL, sym = sym_tab->head[index]; sym;
	 prev_sym = sym, sym = sym->next)
	if (strncmp(name, sym->name, sizeof sym->name - 1) == 0) {
	    if (prev_sym)
		prev_sym->next = sym->next;
	    else
		sym_tab->head[index] = sym->next;
	    r = sym->obj;
	    safe_free(sym);
	    return r;
	}
    return NULL;
}

OBJECT *remove_symbol(SYMBOL_TABLE * sym_tab, char *name, SRC_LINE line)
{
    unsigned index;
    OBJECT *obj;

    index = hash(name) % ARRAY_SIZE(sym_tab->head);
    do {
	obj = remove_from_inner_scope(sym_tab, name, index);
	if (obj)
	    return obj;
	sym_tab = sym_tab->enclosing;
    }
    while (sym_tab);
    return NULL;
}

SYMBOL *new_symbol(SYMBOL_TABLE * sym_tab, char *name, char *tag,
		   OBJECT * obj, SRC_LINE def_line)
{
    int exists_p;
    unsigned index;
    SYMBOL *sym;

    if (!name || !name[0])
	return NULL;

    index = hash(name) % ARRAY_SIZE(sym_tab->head);

    // def has a tag that is undefined, so ignore it
    if (tag) {
	look_up_tag(sym_tab, &exists_p, def_line, tag);
	if (!exists_p)
	    return NULL;
    }

    sym = lookup_in_inner_scope(sym_tab, name, index);

    // ok to redefine if tag is given and the existing definition 
    // is default and no uses have yet occurred
    if (sym && !(tag && !sym->tag[0] && sym->n_references == 0)) {
	// symbol already defined in the inner scope; this is an error
	if (is_no_line_p(sym->def_line))
	    err(def_line, "name %s is already defined", name);
	else
	    err(def_line, "name %s is already defined on line %d",
		name, sym->def_line.number);
	return NULL;
    }
    if (sym)
	warn(def_line, "def of '%s' with tag '%s' after default", name,
	     tag);

    // create a new symbol and fill it in 
    sym = safe_malloc(sizeof *sym);

    // copy up chars that fit then make sure the last is \0
    strncpy(sym->name, name, sizeof sym->name);
    sym->name[sizeof sym->name - 1] = '\0';

    if (strlen(name) > sizeof sym->name - 1)
	warn(def_line, "long identifier shortened to '%s'", sym->name);

    if (tag) {
	strncpy(sym->tag, tag, sizeof sym->tag);
	sym->tag[sizeof sym->tag - 1] = '\0';
    } else
	sym->tag[0] = '\0';

    // other fields
    sym->def_line = def_line;
    sym->n_references = 0;
    sym->obj = obj;

    // push onto hash table list
    sym->next = sym_tab->head[index];
    sym_tab->head[index] = sym;

    return sym;
}
