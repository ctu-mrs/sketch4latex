/* scene.h
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

#ifndef __SCENE_H
#define __SCENE_H

#include "opts.h"
#include "error.h"
#include "geometry.h"
#include "expr.h"
#include "bsp.h"
#include "dynarray.h"
#include "global.h"

typedef struct point_list_3d_t {
    DYNAMIC_2D_ARRAY_FIELDS(POINT_3D, v, n_pts);
} POINT_LIST_3D;

DECLARE_DYNAMIC_2D_ARRAY_PROTOS(POINT_LIST_3D, POINT_3D, FLOAT,
				point_list_3d, v, n_pts);
typedef struct transform_list_t {
    DYNAMIC_2D_ARRAY_FIELDS(TRANSFORM, xf, n_xfs);
} TRANSFORM_LIST;

DECLARE_DYNAMIC_2D_ARRAY_PROTOS(TRANSFORM_LIST, TRANSFORM, FLOAT,
				transform_list, xf, n_xfs);

// object_type_str[] definition in scene.c must match!
typedef enum object_type_t {
    O_BASE,
    O_TAG_DEF,
    O_OPTS_DEF,
    O_SCALAR_DEF,
    O_POINT_DEF,
    O_VECTOR_DEF,
    O_TRANSFORM_DEF,
    O_DOTS,
    O_LINE,
    O_CURVE,
    O_POLYGON,
    O_SPECIAL,
    O_SWEEP,
    O_REPEAT,
    O_COMPOUND,
} OBJECT_TYPE;

#define is_drawable(P)  ((P)->tag >= O_DOTS)

extern char *object_type_str[];

#define BASE_OBJECT_FIELDS  \
  struct object_t *sibling; \
  OBJECT_TYPE tag

typedef struct object_t {
    BASE_OBJECT_FIELDS;
} OBJECT;

typedef OBJECT TAG_DEF;
OBJECT *new_tag_def(void);

typedef struct opts_def_t {
    BASE_OBJECT_FIELDS;
    OPTS *opts;
} OPTS_DEF;

OBJECT *new_opts_def(char *opts_str, SRC_LINE line);
OBJECT *new_opts_def_copy(OPTS * opts);

typedef struct scalar_def_t {
    BASE_OBJECT_FIELDS;
    FLOAT val;
} SCALAR_DEF;

OBJECT *new_scalar_def(FLOAT val);

typedef struct point_def_t {
    BASE_OBJECT_FIELDS;
    POINT_3D p;
} POINT_DEF;

OBJECT *new_point_def(POINT_3D p);

typedef struct vector_def_t {
    BASE_OBJECT_FIELDS;
    VECTOR_3D v;
} VECTOR_DEF;

OBJECT *new_vector_def(VECTOR_3D v);

typedef struct transform_def_t {
    BASE_OBJECT_FIELDS;
    TRANSFORM xf;
} TRANSFORM_DEF;

OBJECT *new_transform_def(TRANSFORM xf);

typedef struct dots_t {
    BASE_OBJECT_FIELDS;
    OPTS *opts;
    POINT_LIST_3D pts[1];
} DOTS_OBJECT;

OBJECT *new_dots(OPTS * opts, OBJECT * pts);

typedef struct line_object_t {
    BASE_OBJECT_FIELDS;
    OPTS *opts;
    POINT_LIST_3D pts[1];
} LINE_OBJECT;

OBJECT *new_line(OPTS * opts, OBJECT * pts);

typedef struct curve_t {
    BASE_OBJECT_FIELDS;
    OPTS *opts;
    POINT_LIST_3D pts[1];
} CURVE_OBJECT;

OBJECT *new_curve(OPTS * opts, OBJECT * pts);

typedef struct polygon_t {
    BASE_OBJECT_FIELDS;
    OPTS *opts;
    int border_p;
    POINT_LIST_3D pts[1];
} POLYGON_OBJECT;

OBJECT *new_polygon(OPTS * opts, OBJECT * pts);

typedef enum {
    SA_SCALAR,
    SA_POINT,
    SA_VECTOR,
} SPECIAL_ARG_TYPE;

typedef struct special_arg_t {
    SPECIAL_ARG_TYPE tag;
    union val_u {
	POINT_3D pt;
	VECTOR_3D v;
	FLOAT flt;
    } val;
} SPECIAL_ARG;

typedef struct special_arg_list_t {
    DYNAMIC_ARRAY_FIELDS(SPECIAL_ARG, arg, n_args);
} SPECIAL_ARG_LIST;

DECLARE_DYNAMIC_ARRAY_PROTOS(SPECIAL_ARG_LIST, SPECIAL_ARG,
			     special_arg_list, arg, n_args);

typedef struct special_t {
    BASE_OBJECT_FIELDS;
    char *code;
    OPTS *opts;
    SPECIAL_ARG_LIST args[1];
} SPECIAL_OBJECT;

OBJECT *new_special(char *code, OBJECT * pts, SRC_LINE line);

typedef struct sweep_t {
    BASE_OBJECT_FIELDS;
    OPTS *opts;
    int n_slices;
    int closed_p;
    TRANSFORM_LIST xforms[1];
    OBJECT *swept;
} SWEEP_OBJECT;

OBJECT *new_sweep(OPTS * opts, int n_slices, int closed_p, OBJECT * xfs,
		  OBJECT * swept);

typedef struct repeat_t {
    BASE_OBJECT_FIELDS;
    int n;
    TRANSFORM_LIST xforms[1];
    OBJECT *repeated;
} REPEAT_OBJECT;

OBJECT *new_repeat(int n, OBJECT * xfs, OBJECT * repeated);

typedef struct compound_t {
    BASE_OBJECT_FIELDS;
    TRANSFORM xform;
    OBJECT *child;
} COMPOUND_OBJECT;

OBJECT *new_compound(TRANSFORM xform, OBJECT * child);

OBJECT *copy_drawable(OBJECT * obj);
OBJECT *cat_objects(OBJECT * lft, OBJECT * rgt);
OBJECT *sibling_reverse(OBJECT * obj);
OBJECT *object_from_expr(EXPR_VAL * val);

OBJECT *flat_scene(OBJECT * obj, GLOBAL_ENV * env);
OBJECT *hsr_scene_with_bsp(OBJECT * scene);
OBJECT *hsr_scene_with_depth_sort(OBJECT * scene);
void get_extent(OBJECT * obj, BOX_3D * e, int *n_obj);
int xy_overlap_p(OBJECT * obj, BOX_3D * e);

#endif
