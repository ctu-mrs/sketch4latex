/* scene.c
   Copyright (C) 2005,2006,2007,2008,2009,2010,2011 Eugene K. Ressler, Jr.

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
#include <math.h>
#include "scene.h"
#include "emit.h"

DECLARE_DYNAMIC_2D_ARRAY_FUNCS(POINT_LIST_3D, POINT_3D, FLOAT,
			       point_list_3d, v, n_pts, NO_OTHER_INIT);
DECLARE_DYNAMIC_2D_ARRAY_FUNCS(TRANSFORM_LIST, TRANSFORM, FLOAT,
			       transform_list, xf, n_xfs, NO_OTHER_INIT);
DECLARE_DYNAMIC_ARRAY_FUNCS(SPECIAL_ARG_LIST, SPECIAL_ARG,
			    special_arg_list, arg, n_args, NO_OTHER_INIT);

// this must match the definition of OBJECT_TYPE
char *object_type_str[] = {
    "base",
    "tag",
    "option list",
    "scalar",
    "point",
    "vector",
    "transform",
    "dots",
    "line",
    "curve",
    "polygon",
    "special",
    "sweep",
    "repeat",
    "compound",
};

#define LAY_IN  0
#define LAY_OVER 1
#define LAY_UNDER -1

int lay_val(OPTS * opts, int lay_default)
{
    char *val = opt_val(opts, "lay");
    if (!val)
	return lay_default;
    if (strcmp(val, "over") == 0)
	return LAY_OVER;
    else if (strcmp(val, "under") == 0)
	return LAY_UNDER;
    else if (strcmp(val, "in") == 0)
	return LAY_IN;
    else {
	warn(no_line, "lay=%s has been ignored", val);
	return lay_default;
    }
}

OBJECT *new_tag_def(void)
{
    TAG_DEF *r = safe_malloc(sizeof *r);
    r->tag = O_TAG_DEF;
    r->sibling = NULL;
    return (OBJECT *) r;
}

OBJECT *new_opts_def_copy(OPTS * opts)
{
    OPTS_DEF *r = safe_malloc(sizeof *r);
    r->tag = O_OPTS_DEF;
    r->sibling = NULL;
    r->opts = opts;
    return (OBJECT *) r;
}

OBJECT *new_opts_def(char *opts_str, SRC_LINE line)
{
    return new_opts_def_copy(new_opts(opts_str, line));
}

OBJECT *new_scalar_def(FLOAT val)
{
    SCALAR_DEF *r = safe_malloc(sizeof *r);
    r->tag = O_SCALAR_DEF;
    r->sibling = NULL;
    r->val = val;
    return (OBJECT *) r;
}

OBJECT *new_point_def(POINT_3D p)
{
    POINT_DEF *r = safe_malloc(sizeof *r);
    r->tag = O_POINT_DEF;
    r->sibling = NULL;
    copy_pt_3d(r->p, p);
    return (OBJECT *) r;
}

OBJECT *new_vector_def(VECTOR_3D v)
{
    VECTOR_DEF *r = safe_malloc(sizeof *r);
    r->tag = O_VECTOR_DEF;
    r->sibling = NULL;
    copy_vec_3d(r->v, v);
    return (OBJECT *) r;
}

OBJECT *new_transform_def(TRANSFORM xf)
{
    TRANSFORM_DEF *r = safe_malloc(sizeof *r);
    r->tag = O_TRANSFORM_DEF;
    r->sibling = NULL;
    copy_transform(r->xf, xf);
    return (OBJECT *) r;
}

void translate_points(POINT_LIST_3D * dst, OBJECT * src_obj)
{
    POINT_DEF *sibling, *src = (POINT_DEF *) src_obj;

    while (src) {
	copy_pt_3d(pushed_point_list_3d_v(dst), src->p);
	sibling = (POINT_DEF *) src->sibling;
	safe_free(src);
	src = sibling;
    }
}

void translate_special_args(SPECIAL_ARG_LIST * dst, OBJECT * src)
{
    OBJECT *sibling;

    while (src) {
	SPECIAL_ARG *arg = pushed_special_arg_list_arg(dst);
	switch (src->tag) {
	case O_SCALAR_DEF:{
		SCALAR_DEF *scalar_def = (SCALAR_DEF *) src;
		arg->tag = SA_SCALAR;
		arg->val.flt = scalar_def->val;
	    }
	    break;
	case O_POINT_DEF:{
		POINT_DEF *pt_def = (POINT_DEF *) src;
		arg->tag = SA_POINT;
		copy_pt_3d(arg->val.pt, pt_def->p);
	    }
	    break;
	case O_VECTOR_DEF:{
		VECTOR_DEF *vec_def = (VECTOR_DEF *) src;
		arg->tag = SA_VECTOR;
		copy_vec_3d(arg->val.v, vec_def->v);
	    }
	    break;
	default:
	    die(no_line, "unexpected special arg object %s(%d)",
		object_type_str[src->tag], src->tag);
	}
	sibling = src->sibling;
	safe_free(src);
	src = sibling;
    }
}

DOTS_OBJECT *raw_dots(OPTS * opts)
{
    DOTS_OBJECT *r = safe_malloc(sizeof *r);
    r->tag = O_DOTS;
    r->sibling = NULL;
    r->opts = opts;
    init_point_list_3d(r->pts);
    return r;
}

OBJECT *new_dots(OPTS * opts, OBJECT * pts)
{
    DOTS_OBJECT *r = raw_dots(opts);
    translate_points(r->pts, pts);
    return (OBJECT *) r;
}

OBJECT *copy_dots(OBJECT * obj)
{
    DOTS_OBJECT *org = (DOTS_OBJECT *) obj, *r = raw_dots(org->opts);
    copy_point_list_3d(r->pts, org->pts);
    return (OBJECT *) r;
}

LINE_OBJECT *raw_line(OPTS * opts)
{
    LINE_OBJECT *r = safe_malloc(sizeof *r);
    r->tag = O_LINE;
    r->sibling = NULL;
    r->opts = opts;
    init_point_list_3d(r->pts);
    return r;
}

OBJECT *new_line(OPTS * opts, OBJECT * pts)
{
    LINE_OBJECT *r = raw_line(opts);
    translate_points(r->pts, pts);
    return (OBJECT *) r;
}

OBJECT *copy_line(OBJECT * obj)
{
    LINE_OBJECT *org = (LINE_OBJECT *) obj, *r = raw_line(org->opts);
    copy_point_list_3d(r->pts, org->pts);
    return (OBJECT *) r;
}

CURVE_OBJECT *raw_curve(OPTS * opts)
{
    CURVE_OBJECT *r = safe_malloc(sizeof *r);
    r->tag = O_CURVE;
    r->sibling = NULL;
    r->opts = opts;
    init_point_list_3d(r->pts);
    return r;
}

OBJECT *new_curve(OPTS * opts, OBJECT * pts)
{
    CURVE_OBJECT *r = raw_curve(opts);
    translate_points(r->pts, pts);
    return (OBJECT *) r;
}

OBJECT *copy_curve(OBJECT * obj)
{
    CURVE_OBJECT *org = (CURVE_OBJECT *) obj, *r = raw_curve(org->opts);
    copy_point_list_3d(r->pts, org->pts);
    return (OBJECT *) r;
}

POLYGON_OBJECT *raw_polygon(OPTS * opts)
{
    POLYGON_OBJECT *r = safe_malloc(sizeof *r);
    r->tag = O_POLYGON;
    r->sibling = NULL;
    r->opts = opts;
    init_point_list_3d(r->pts);
    r->border_p = 0;
    return r;
}

OBJECT *new_polygon(OPTS * opts, OBJECT * pts)
{
    POLYGON_OBJECT *r = raw_polygon(opts);
    translate_points(r->pts, pts);
    return (OBJECT *) r;
}

OBJECT *copy_polygon(OBJECT * obj)
{
    POLYGON_OBJECT *org = (POLYGON_OBJECT *) obj, *r =
	raw_polygon(org->opts);
    copy_point_list_3d(r->pts, org->pts);
    return (OBJECT *) r;
}

static SPECIAL_OBJECT *raw_special(OPTS * opts)
{
    SPECIAL_OBJECT *r = safe_malloc(sizeof *r);
    r->tag = O_SPECIAL;
    r->sibling = NULL;
    r->code = NULL;
    r->opts = opts;
    init_special_arg_list(r->args);
    return r;
}

/* The parser may put options anywhere in the arg list. We must
   flag options in any location except the first as an error. 
   In the first location, we unlink from args list and install. */
OBJECT *new_special(char *code, OBJECT * args, SRC_LINE line)
{
    SPECIAL_OBJECT *r;
    OPTS_DEF *d;
    OBJECT *p, *q;

    // use the first arg as options if that's what's there
    if (args && args->tag == O_OPTS_DEF) {
	d = (OPTS_DEF *) args;
	r = raw_special(d->opts);
	args = args->sibling;
	safe_free(d);
    } else {
	r = raw_special(NULL);
    }
    // ensure no options are left
    q = NULL;
    for (p = args; p != NULL; p = p->sibling) {
	if (p->tag == O_OPTS_DEF) {
	    err(line, "unexpected options found in special argument list");
	    // unlink
	    if (q == NULL)
		args = p->sibling;
	    else
		q->sibling = p->sibling;
	} else {
	    q = p;
	}
    }

    r->code = code;
    translate_special_args(r->args, args);
    // syntax check
    process_special(NULL, r, line);
    return (OBJECT *) r;
}

OBJECT *copy_special(OBJECT * obj)
{
    SPECIAL_OBJECT *org = (SPECIAL_OBJECT *) obj, *r =
	raw_special(org->opts);
    copy_special_arg_list(r->args, org->args);
    r->code = safe_strdup(org->code);
    return (OBJECT *) r;
}

SWEEP_OBJECT *raw_sweep(OPTS * opts)
{
    SWEEP_OBJECT *r = safe_malloc(sizeof *r);
    r->tag = O_SWEEP;
    r->sibling = NULL;
    r->n_slices = 0;
    r->closed_p = 0;
    init_transform_list(r->xforms);
    r->opts = opts;
    r->swept = NULL;
    return r;
}

void translate_transforms(TRANSFORM_LIST * dst, OBJECT * src_obj)
{
    TRANSFORM_DEF *sibling, *src = (TRANSFORM_DEF *) src_obj;
    while (src) {
	copy_transform(pushed_transform_list_xf(dst), src->xf);
	sibling = (TRANSFORM_DEF *) src->sibling;
	safe_free(src);
	src = sibling;
    }
}

OBJECT *new_sweep(OPTS * opts, int n_slices, int closed_p, OBJECT * xfs,
		  OBJECT * swept)
{
    SWEEP_OBJECT *r = raw_sweep(opts);
    r->n_slices = n_slices;
    r->closed_p = closed_p;
    translate_transforms(r->xforms, xfs);
    r->swept = swept;
    return (OBJECT *) r;
}

// this is a shallow copy
OBJECT *copy_sweep(OBJECT * obj)
{
    SWEEP_OBJECT *org = (SWEEP_OBJECT *) obj, *r = raw_sweep(org->opts);
    r->n_slices = org->n_slices;
    r->closed_p = org->closed_p;
    copy_transform_list(r->xforms, org->xforms);
    r->swept = org->swept;
    return (OBJECT *) r;
}

REPEAT_OBJECT *raw_repeat(void)
{
    REPEAT_OBJECT *r = safe_malloc(sizeof *r);
    r->tag = O_REPEAT;
    r->sibling = NULL;
    r->n = 0;
    init_transform_list(r->xforms);
    r->repeated = NULL;
    return r;
}

OBJECT *new_repeat(int n, OBJECT * xfs, OBJECT * repeated)
{
    REPEAT_OBJECT *r = raw_repeat();
    r->n = n;
    translate_transforms(r->xforms, xfs);
    r->repeated = repeated;
    return (OBJECT *) r;
}

OBJECT *copy_repeat(OBJECT * obj)
{
    REPEAT_OBJECT *org = (REPEAT_OBJECT *) obj, *r = raw_repeat();
    r->n = org->n;
    copy_transform_list(r->xforms, org->xforms);
    r->repeated = org->repeated;	// shallow copy
    return (OBJECT *) r;
}

OBJECT *new_compound(TRANSFORM xform, OBJECT * child)
{
    COMPOUND_OBJECT *r = safe_malloc(sizeof *r);
    r->tag = O_COMPOUND;
    r->sibling = NULL;
    copy_transform(r->xform, xform);
    r->child = child;
    return (OBJECT *) r;
}

// this is a shallow copy
OBJECT *copy_compound(OBJECT * obj)
{
    COMPOUND_OBJECT *org = (COMPOUND_OBJECT *) obj;
    return new_compound(org->xform, org->child);
}

typedef OBJECT *(*COPY_FUNC) (OBJECT *);

static COPY_FUNC copy_tbl[] = {
    NULL,			// O_BASE
    NULL,			// O_TAG_DEF
    NULL,			// O_OPTS_DEF
    NULL,			// O_SCALAR_DEF
    NULL,			// O_POINT_DEF
    NULL,			// O_VECTOR_DEF
    NULL,			// O_TRANSFORM_DEF
    copy_dots,
    copy_line,
    copy_curve,
    copy_polygon,
    copy_special,
    copy_sweep,
    copy_repeat,
    copy_compound,
};

OBJECT *copy_drawable_without_siblings(OBJECT * obj)
{
    return (*copy_tbl[obj->tag]) (obj);
}

OBJECT *copy_drawable(OBJECT * obj)
{
    OBJECT *r = NULL;
    while (obj) {
	if (copy_tbl[obj->tag]) {
	    OBJECT *copy = copy_drawable_without_siblings(obj);
	    copy->sibling = r;
	    r = copy;
	} else {
	    die(no_line, "copy_drawable: attempt to copy non-drawable %s",
		object_type_str[obj->tag]);
	}
	obj = obj->sibling;
    }
    return sibling_reverse(r);
}

OBJECT *sibling_reverse(OBJECT * obj)
{
    OBJECT *p, *q, *t;

    // pop from p and push onto q until p is empty
    p = obj;
    q = NULL;
    while (p) {
	t = p;
	p = p->sibling;		// pop
	t->sibling = q;
	q = t;			// push
    }
    return q;
}

OBJECT *object_from_expr(EXPR_VAL * val)
{
    switch (val->tag) {
    case E_FLOAT:
	return new_scalar_def(val->val.flt);
    case E_POINT:
	return new_point_def(val->val.pt);
    case E_VECTOR:
	return new_vector_def(val->val.vec);
    case E_TRANSFORM:
	return new_transform_def(val->val.xf);
    default:
	die(no_line, "object_from_expr: unknown value tag %d", val->tag);
    }
    return NULL;		// never occurs
}

void
transform_points(POINT_LIST_3D * dst_pts, TRANSFORM xf,
		 POINT_LIST_3D * src_pts)
{
    int i;

    setup_point_list_3d(dst_pts, src_pts->n_pts);
    for (i = 0; i < src_pts->n_pts; i++)
	transform_pt_3d(dst_pts->v[i], xf, src_pts->v[i]);
    dst_pts->n_pts = src_pts->n_pts;
}

void
transform_special_args(SPECIAL_ARG_LIST * dst_args, TRANSFORM xf,
		       SPECIAL_ARG_LIST * src_args)
{
    int i;

    setup_special_arg_list(dst_args, src_args->n_args);
    for (i = 0; i < src_args->n_args; i++) {
	switch (dst_args->arg[i].tag = src_args->arg[i].tag) {
	case SA_SCALAR:
	    dst_args->arg[i].val.flt = src_args->arg[i].val.flt;
	    break;
	case SA_POINT:
	    transform_pt_3d(dst_args->arg[i].val.pt, xf,
			    src_args->arg[i].val.pt);
	    break;
	case SA_VECTOR:
	    transform_vec_3d(dst_args->arg[i].val.v, xf,
			     src_args->arg[i].val.v);
	    break;
	default:
	    die(no_line, "unexpected special arg type");
	}
    }
    dst_args->n_args = src_args->n_args;
}

static void
fill_transform_accum(TRANSFORM_LIST * accum, TRANSFORM_LIST * inc)
{
    int i;

    setup_transform_list(accum, inc->n_xfs);
    accum->n_xfs = inc->n_xfs;
    for (i = 0; i < inc->n_xfs; i++)
	set_ident(accum->xf[i]);
}

static void
advance_transform_accum(TRANSFORM_LIST * accum, TRANSFORM_LIST * inc)
{
    int i;
    for (i = 0; i < accum->n_xfs; i++)
	compose(accum->xf[i], accum->xf[i], inc->xf[i]);
}

static void
compose_transform_accum(TRANSFORM xf, TRANSFORM_LIST * accum,
			TRANSFORM model_view_xf)
{
    int i;

    if (accum->n_xfs <= 0)
	die(no_line, "zero size accumulator");
    copy_transform(xf, accum->xf[0]);
    // left-multiply because accumulator is in "then" order
    for (i = 1; i < accum->n_xfs; i++)
	compose(xf, accum->xf[i], xf);
    if (model_view_xf)
	compose(xf, model_view_xf, xf);
}

OBJECT *flat_dots(OBJECT * obj, TRANSFORM xf)
{
    DOTS_OBJECT *s = (DOTS_OBJECT *) obj, *dots = raw_dots(s->opts);
    transform_points(dots->pts, xf, s->pts);
    return (OBJECT *) dots;
}

OBJECT *flat_line(OBJECT * obj, TRANSFORM xf)
{
    LINE_OBJECT *s = (LINE_OBJECT *) obj, *line = raw_line(s->opts);
    check_opts(s->opts, OPT_INTERNAL | OPT_LINE,
	       "unknown line option %s=%s will be ignored",
	       global_env->output_language, no_line);
    transform_points(line->pts, xf, s->pts);
    return (OBJECT *) line;
}

OBJECT *flat_curve(OBJECT * obj, TRANSFORM xf)
{
    CURVE_OBJECT *s = (CURVE_OBJECT *) obj, *curve = raw_curve(s->opts);
    transform_points(curve->pts, xf, s->pts);
    return (OBJECT *) curve;
}

OBJECT *flat_polygon(OBJECT * obj, TRANSFORM xf)
{
    POLYGON_OBJECT *s = (POLYGON_OBJECT *) obj,
	*polygon = raw_polygon(s->opts);
    check_opts(s->opts, OPT_INTERNAL | OPT_POLYGON | OPT_LINE,
	       "unknown polygon option %s=%s will be ignored",
	       global_env->output_language, no_line);
    transform_points(polygon->pts, xf, s->pts);
    return (OBJECT *) polygon;
}

OBJECT *flat_special(OBJECT * obj, TRANSFORM xf)
{
    SPECIAL_OBJECT *s = (SPECIAL_OBJECT *) obj,
	*special = raw_special(s->opts);
    special->code = safe_strdup(s->code);
    transform_special_args(special->args, xf, s->args);
    return (OBJECT *) special;
}

#define MAX_WARP  1e-5

// return -1 if no split is necessary
// return 0 if best spilt is on the 0--2 line
// return 1 if best split is on the 1--3 line
static int
best_triangle_split(POINT_3D v0, POINT_3D v1, POINT_3D v2, POINT_3D v3)
{
    VECTOR_3D n, d0, d1, e, e_max;
    FLOAT e_len_sqr, e_max_len_sqr, warp;

    sub_vecs_3d(d0, v2, v0);
    sub_vecs_3d(d1, v3, v1);
    cross(n, d0, d1);

    // if the cross product is zero length, the polygon is degenerate and can
    // be considered flat; no need to traingulate
    if (!find_unit_vec_3d(n, n))
	return -1;

    // find the edge of maximum length; probably not necessary
    sub_vecs_3d(e_max, v1, v0);
    e_max_len_sqr = dot_3d(e_max, e_max);

    sub_vecs_3d(e, v2, v1);
    e_len_sqr = dot_3d(e, e);
    if (e_len_sqr > e_max_len_sqr) {
	e_max_len_sqr = e_len_sqr;
	copy_vec_3d(e_max, e);
    }

    sub_vecs_3d(e, v3, v2);
    e_len_sqr = dot_3d(e, e);
    if (e_len_sqr > e_max_len_sqr) {
	e_max_len_sqr = e_len_sqr;
	copy_vec_3d(e_max, e);
    }

    sub_vecs_3d(e, v0, v3);
    e_len_sqr = dot_3d(e, e);
    if (e_len_sqr > e_max_len_sqr) {
	e_max_len_sqr = e_len_sqr;
	copy_vec_3d(e_max, e);
    }
    // flat if projection of edge on normal is small, else split on shortest diagonal
    warp = dot_3d(e_max, n);
    return
	-MAX_WARP <= warp && warp <= MAX_WARP ? -1 :
	dot_3d(d0, d0) < dot_3d(d1, d1) ? 0 : 1;
}

// add triangular or quadrilateral faces to object list depending on flatness
static void
make_faces(OBJECT ** r, OPTS * opts, TRANSFORM xf,
	   POINT_3D v0, POINT_3D v1, POINT_3D v2, POINT_3D v3, int split_p)
{
    POLYGON_OBJECT *new_polygon;

    if (!split_p)
	goto no_split;

    switch (best_triangle_split(v0, v1, v2, v3)) {
    case -1:
      no_split:
	new_polygon = raw_polygon(opts);
	setup_point_list_3d(new_polygon->pts, 4);
	transform_pt_3d(new_polygon->pts->v[0], xf, v0);
	transform_pt_3d(new_polygon->pts->v[1], xf, v1);
	transform_pt_3d(new_polygon->pts->v[2], xf, v2);
	transform_pt_3d(new_polygon->pts->v[3], xf, v3);
	new_polygon->pts->n_pts = 4;
	new_polygon->sibling = *r;
	*r = (OBJECT *) new_polygon;
	break;

    case 0:
	new_polygon = raw_polygon(opts);
	setup_point_list_3d(new_polygon->pts, 3);
	transform_pt_3d(new_polygon->pts->v[0], xf, v0);
	transform_pt_3d(new_polygon->pts->v[1], xf, v1);
	transform_pt_3d(new_polygon->pts->v[2], xf, v2);
	new_polygon->pts->n_pts = 3;
	new_polygon->sibling = *r;
	*r = (OBJECT *) new_polygon;
	new_polygon = raw_polygon(opts);
	setup_point_list_3d(new_polygon->pts, 3);
	transform_pt_3d(new_polygon->pts->v[0], xf, v2);
	transform_pt_3d(new_polygon->pts->v[1], xf, v3);
	transform_pt_3d(new_polygon->pts->v[2], xf, v0);
	new_polygon->pts->n_pts = 3;
	new_polygon->sibling = *r;
	*r = (OBJECT *) new_polygon;
	break;

    case 1:
	new_polygon = raw_polygon(opts);
	setup_point_list_3d(new_polygon->pts, 3);
	transform_pt_3d(new_polygon->pts->v[0], xf, v1);
	transform_pt_3d(new_polygon->pts->v[1], xf, v2);
	transform_pt_3d(new_polygon->pts->v[2], xf, v3);
	new_polygon->pts->n_pts = 3;
	new_polygon->sibling = *r;
	*r = (OBJECT *) new_polygon;
	new_polygon = raw_polygon(opts);
	setup_point_list_3d(new_polygon->pts, 3);
	transform_pt_3d(new_polygon->pts->v[0], xf, v3);
	transform_pt_3d(new_polygon->pts->v[1], xf, v0);
	transform_pt_3d(new_polygon->pts->v[2], xf, v1);
	new_polygon->pts->n_pts = 3;
	new_polygon->sibling = *r;
	*r = (OBJECT *) new_polygon;
	break;
    }
}

OBJECT *flat_sweep(OBJECT * obj, TRANSFORM xf)
{
    int i, j, jj, split_p;
    POINT_LIST_3D *a, *b, *t;
    OBJECT *swept, *r;
    TRANSFORM sweep_xf;
    POINT_LIST_3D pts_1[1], pts_2[1];
    TRANSFORM_LIST sweep_accum[1];

    SWEEP_OBJECT *s = (SWEEP_OBJECT *) obj;

    init_point_list_3d(pts_1);
    init_point_list_3d(pts_2);
    init_transform_list(sweep_accum);

    split_p = bool_opt_p(s->opts, "split", 1)
	&& bool_opt_p(global_env->opts, "split", 1);

    r = NULL;

#define ADD_TO_OUTPUT(O)  do { \
  (O)->sibling = r; \
  r = (OBJECT*)(O); \
} while (0)

    // handle definitions first; a point becomes a single line or a polygon
    if (s->swept->tag == O_POINT_DEF) {
	fill_transform_accum(sweep_accum, s->xforms);
	for (swept = s->swept; swept; swept = swept->sibling) {
	    POINT_DEF *pd = (POINT_DEF *) swept;
	    if (s->closed_p) {
		POLYGON_OBJECT *polygon = raw_polygon(s->opts);
		for (i = 0; i < s->n_slices; i++) {
		    compose_transform_accum(sweep_xf, sweep_accum, xf);
		    transform_pt_3d(pushed_point_list_3d_v(polygon->pts),
				    sweep_xf, pd->p);
		    advance_transform_accum(sweep_accum, s->xforms);
		}
		ADD_TO_OUTPUT(polygon);
	    } else {
		LINE_OBJECT *line = raw_line(s->opts);
		for (i = 0; i < s->n_slices + 1; i++) {
		    compose_transform_accum(sweep_xf, sweep_accum, xf);
		    transform_pt_3d(pushed_point_list_3d_v(line->pts),
				    sweep_xf, pd->p);
		    advance_transform_accum(sweep_accum, s->xforms);
		}
		ADD_TO_OUTPUT(line);
	    }
	}
    } else {

	// it's drawable; recursively flatten swept object in its own coordinates
	for (swept = flat_scene(s->swept, NULL); swept;
	     swept = swept->sibling) {

	    // refill with identity for each swept object
	    fill_transform_accum(sweep_accum, s->xforms);

	    // now the different flavors of sweep depend on what's being swept and
	    // the setting of the closure tag
	    if (swept->tag == O_LINE) {
		// a line becomes a surface represented by a sequence of 4-sided polygons
		LINE_OBJECT *line = (LINE_OBJECT *) swept;

		// a is the trail buffer and b the lead
		a = pts_1;
		b = pts_2;
		copy_point_list_3d(a, line->pts);

		if (s->closed_p) {
		    POLYGON_OBJECT *e1 = raw_polygon(s->opts);
		    POLYGON_OBJECT *e2 = raw_polygon(s->opts);
		    OPTS *face_opts = line->opts ? line->opts : s->opts;

		    // set up in advance; e1 is filled in forward, e2 in reverse
		    setup_point_list_3d(e1->pts, s->n_slices);
		    e1->pts->n_pts = s->n_slices;
		    setup_point_list_3d(e2->pts, s->n_slices);
		    e2->pts->n_pts = s->n_slices;
		    for (i = 0; i < s->n_slices - 1; i++) {
			advance_transform_accum(sweep_accum, s->xforms);
			compose_transform_accum(sweep_xf, sweep_accum, 0);	// apply mv transform in make_faces
			transform_points(b, sweep_xf, line->pts);
			// copy first and last points for 'end caps'
			transform_pt_3d(e1->pts->v[i], xf,
					b->v[b->n_pts - 1]);
			transform_pt_3d(e2->pts->v[s->n_slices - 1 - i],
					xf, b->v[0]);
			for (jj = 0, j = 1; j < a->n_pts; jj = j++)
			    make_faces(&r, face_opts, xf, b->v[jj],
				       b->v[j], a->v[j], a->v[jj],
				       split_p);
			t = a;
			a = b;
			b = t;	// swap a and b for next pass
		    }
		    // closure: add last point of original line. first to ends, then as faces
		    transform_pt_3d(e1->pts->v[i], xf,
				    line->pts->v[line->pts->n_pts - 1]);
		    transform_pt_3d(e2->pts->v[0], xf, line->pts->v[0]);
		    for (jj = 0, j = 1; j < a->n_pts; jj = j++)
			make_faces(&r, face_opts, xf, line->pts->v[jj],
				   line->pts->v[j], a->v[j], a->v[jj],
				   split_p);

		    // add ends to output
		    ADD_TO_OUTPUT(e1);
		    ADD_TO_OUTPUT(e2);
		} else {
		    for (i = 0; i < s->n_slices; i++) {
			advance_transform_accum(sweep_accum, s->xforms);
			compose_transform_accum(sweep_xf, sweep_accum, 0);
			transform_points(b, sweep_xf, line->pts);
			for (jj = 0, j = 1; j < a->n_pts; jj = j++)
			    make_faces(&r, s->opts, xf, b->v[jj], b->v[j],
				       a->v[j], a->v[jj], split_p);
			t = a;
			a = b;
			b = t;	// swap a and b for next pass
		    }
		}
	    } else if (swept->tag == O_POLYGON) {
		// a polygon becomes a surface represented by a sequence of 4-sided polygons (with "end caps")
		POLYGON_OBJECT *new_polygon, *polygon =
		    (POLYGON_OBJECT *) swept;
		OPTS *end_opts = polygon->opts ? polygon->opts : s->opts;

		if (s->closed_p)
		    warn(no_line,
			 "closure tag on polygon sweep ignored (sorry, no line number)");

		a = pts_1;
		b = pts_2;
		copy_point_list_3d(a, polygon->pts);

		// initial end cap
		new_polygon = raw_polygon(end_opts);
		transform_points(new_polygon->pts, xf, a);
		ADD_TO_OUTPUT(new_polygon);

		for (i = 0; i < s->n_slices; i++) {
		    advance_transform_accum(sweep_accum, s->xforms);
		    compose_transform_accum(sweep_xf, sweep_accum, 0);
		    transform_points(b, sweep_xf, polygon->pts);
		    for (jj = a->n_pts - 1, j = 0; j < a->n_pts; jj = j++)
			make_faces(&r, s->opts, xf, b->v[jj], b->v[j],
				   a->v[j], a->v[jj], split_p);
		    t = a;
		    a = b;
		    b = t;	// swap a and b for next pass
		}

		// final end cap is copy of a (note reverse point order)
		new_polygon = raw_polygon(end_opts);
		reverse_copy_point_list_3d(new_polygon->pts, a);
		transform_points(new_polygon->pts, xf, new_polygon->pts);
		ADD_TO_OUTPUT(new_polygon);
	    } else {
		warn(no_line,
		     "cannot sweep a %s; object ignored (sorry, no line number)",
		     object_type_str[swept->tag]);
	    }
	}
    }

    clear_point_list_3d(pts_1);
    clear_point_list_3d(pts_2);
    clear_transform_list(sweep_accum);
    return r;
#undef ADD_TO_OUTPUT
}

// forward declaration
static OBJECT *rev_transformed_flat_scene(OBJECT * obj, TRANSFORM xf);

OBJECT *flat_repeat(OBJECT * obj, TRANSFORM xf)
{
    int i;
    REPEAT_OBJECT *s = (REPEAT_OBJECT *) obj;
    OBJECT *flat_repeated, *r;
    TRANSFORM_LIST repeat_accum[1];
    TRANSFORM child_xf;

    init_transform_list(repeat_accum);

    if (s->n <= 0)
	return NULL;

    // recursively flatten repeated object in its own coordinates
    flat_repeated = flat_scene(s->repeated, NULL);

    fill_transform_accum(repeat_accum, s->xforms);
    r = NULL;
    for (i = 0; i < s->n; i++) {
	compose_transform_accum(child_xf, repeat_accum, xf);
	r = cat_objects(rev_transformed_flat_scene
			(flat_repeated, child_xf), r);
	advance_transform_accum(repeat_accum, s->xforms);
    }
    // flat_repeated is a memory leak
    return r;
}

OBJECT *flat_compound(OBJECT * obj, TRANSFORM xf)
{
    COMPOUND_OBJECT *compound = (COMPOUND_OBJECT *) obj;
    TRANSFORM child_xf;
    compose(child_xf, xf, compound->xform);
    return rev_transformed_flat_scene(compound->child, child_xf);
}

typedef OBJECT *(*FLATTEN_FUNC) (OBJECT *, TRANSFORM);

static FLATTEN_FUNC flatten_tbl[] = {
    NULL,			// O_BASE
    NULL,			// O_TAG_DEF
    NULL,			// O_OPTS_DEF
    NULL,			// O_SCALAR_DEF
    NULL,			// O_POINT_DEF
    NULL,			// O_VECTOR_DEF
    NULL,			// O_TRANSFORM_DEF
    flat_dots,
    flat_line,
    flat_curve,
    flat_polygon,
    flat_special,
    flat_sweep,
    flat_repeat,
    flat_compound,
};

OBJECT *cat_objects(OBJECT * lft, OBJECT * rgt)
{
    OBJECT *p;

    if (!lft)
	return rgt;
    for (p = lft; p->sibling; p = p->sibling)
	/* skip */ ;
    p->sibling = rgt;
    return lft;
}

static OBJECT *rev_transformed_flat_scene(OBJECT * obj, TRANSFORM xf)
{
    OBJECT *r = NULL;
    while (obj) {
	// flatten the object
	if (flatten_tbl[obj->tag] == NULL)
	    die(no_line, "rev_transformed_flat_scene: bad tag %d",
		obj->tag);

	// join scene sibling lists
	r = cat_objects((*flatten_tbl[obj->tag]) (obj, xf), r);

	// on to next object
	obj = obj->sibling;
    }
    return r;
}

// call with null env omits camera transformation
OBJECT *flat_scene(OBJECT * obj, GLOBAL_ENV * env)
{
    FLOAT *camera = env
	&& global_env_is_set_p(env, GE_CAMERA) ? env->camera : identity;
    return sibling_reverse(rev_transformed_flat_scene(obj, camera));
}

// ---- overlay/underlay/depth sort flag --------------------------------------

static int dots_lay_val(OBJECT * obj)
{
    return lay_val(((DOTS_OBJECT *) obj)->opts, LAY_IN);
}

static int line_lay_val(OBJECT * obj)
{
    return lay_val(((LINE_OBJECT *) obj)->opts, LAY_IN);
}

static int curve_lay_val(OBJECT * obj)
{
    return lay_val(((CURVE_OBJECT *) obj)->opts, LAY_IN);
}

static int polygon_lay_val(OBJECT * obj)
{
    return lay_val(((POLYGON_OBJECT *) obj)->opts, LAY_IN);
}

static int special_lay_val(OBJECT * obj)
{
    return lay_val(((SPECIAL_OBJECT *) obj)->opts, LAY_OVER);
}

typedef int (*LAY_VAL_FUNC) (OBJECT *);

static LAY_VAL_FUNC lay_val_tbl[] = {
    NULL,			// O_BASE
    NULL,			// O_TAG_DEF
    NULL,			// O_OPTS_DEF
    NULL,			// O_SCALAR_DEF
    NULL,			// O_POINT_DEF
    NULL,			// O_VECTOR_DEF
    NULL,			// O_TRANSFORM_DEF
    dots_lay_val,
    line_lay_val,
    curve_lay_val,
    polygon_lay_val,
    special_lay_val,
    NULL,			// O_SWEEP (flattened)
    NULL,			// O_REPEAT (flattened)
    NULL,			// O_COMPOUND (flattened)
};

int object_lay_val(OBJECT * obj)
{
    if (!lay_val_tbl[obj->tag])
	die(no_line, "bad tag in object_lay_val");
    return (*lay_val_tbl[obj->tag]) (obj);
}

// ---- binary space partition ------------------------------------------------

static void add_dots_object_to_bsp_pass_1(OBJECT * obj, BSP_TREE * bsp)
{
}

static void add_line_object_to_bsp_pass_1(OBJECT * obj, BSP_TREE * bsp)
{
}

static void add_curve_object_to_bsp_pass_1(OBJECT * obj, BSP_TREE * bsp)
{
}

static void add_polygon_object_to_bsp_pass_1(OBJECT * obj, BSP_TREE * bsp)
{
    int i;
    POLYGON_3D *polygon;
    POLYGON_OBJECT *polygon_obj = (POLYGON_OBJECT *) obj;
    PLANE plane[1];

    // copy point list to new polygon
    polygon = new_polygon_3d(polygon_obj->pts->n_pts);
    polygon->n_sides = polygon_obj->pts->n_pts;
    for (i = 0; i < polygon->n_sides; i++)
	copy_pt_3d(polygon->v[i], polygon_obj->pts->v[i]);

    find_polygon_plane(plane, polygon);

    // backface elimination
    // put the new polygon in the tree
    if (plane->n[Z] >= -FLOAT_EPS ||
	!bool_opt_p(polygon_obj->opts, "cull", 1) ||
	!bool_opt_p(global_env->opts, "cull", 1)) {

	add_polygon_to_bsp(bsp, polygon, obj);
    } else {
	delete_polygon_3d(polygon);
    }
}

static void add_special_object_to_bsp_pass_1(OBJECT * obj, BSP_TREE * bsp)
{
}

typedef void (*BSP_INSERT_FUNC) (OBJECT *, BSP_TREE *);

static BSP_INSERT_FUNC insert_in_bsp_pass_1_tbl[] = {
    NULL,			// O_BASE
    NULL,			// O_TAG_DEF
    NULL,			// O_OPTS_DEF
    NULL,			// O_SCALAR_DEF
    NULL,			// O_POINT_DEF
    NULL,			// O_VECTOR_DEF
    NULL,			// O_TRANSFORM_DEF
    add_dots_object_to_bsp_pass_1,
    add_line_object_to_bsp_pass_1,
    add_curve_object_to_bsp_pass_1,
    add_polygon_object_to_bsp_pass_1,
    add_special_object_to_bsp_pass_1,
    NULL,			// O_SWEEP (flattened)
    NULL,			// O_REPEAT (flattened)
    NULL,			// O_COMPOUND (flattened)
};

static void add_dots_object_to_bsp_pass_2(OBJECT * obj, BSP_TREE * bsp)
{
    int i;
    DOTS_OBJECT *dots_obj = (DOTS_OBJECT *) obj;
    // insert each dot as a polyline with only one vertex
    for (i = 0; i < dots_obj->pts->n_pts; i++) {
	POLYLINE_3D *dot = new_polyline_3d(1);
	dot->n_vertices = 1;
	copy_pt_3d(dot->v[0], dots_obj->pts->v[i]);
	add_polyline_to_bsp(bsp, dot, obj);
    }
}

static void add_line_object_to_bsp_pass_2(OBJECT * obj, BSP_TREE * bsp)
{
    int i;
    POLYLINE_3D *polyline;
    LINE_OBJECT *line_obj = (LINE_OBJECT *) obj;

    // copy point list to new polyline
    polyline = new_polyline_3d(line_obj->pts->n_pts);
    polyline->n_vertices = line_obj->pts->n_pts;
    for (i = 0; i < line_obj->pts->n_pts; i++)
	copy_pt_3d(polyline->v[i], line_obj->pts->v[i]);

    // fprintf(stderr, "adding to bsp [%p(%d)]\n", line_obj->opts, line_obj->opts->list->n_elts); // DEBUG

    // put the new polyline in the tree
    add_polyline_to_bsp(bsp, polyline, obj);
}

static void add_curve_object_to_bsp_pass_2(OBJECT * obj, BSP_TREE * bsp)
{
}

static void add_polygon_object_to_bsp_pass_2(OBJECT * obj, BSP_TREE * bsp)
{
}

static int special_first_point_index(SPECIAL_OBJECT * special)
{
    int i;
    for (i = 0; i < special->args->n_args; i++)
	if (special->args->arg[i].tag == SA_POINT)
	    return i;
    return -1;
}

// Really returninga location of a POINT_3D here
static FLOAT *special_first_point(SPECIAL_OBJECT * special)
{
    int i = special_first_point_index(special);
    return (i >= 0) ? special->args->arg[i].val.pt : origin_3d;
}

static void add_special_object_to_bsp_pass_2(OBJECT * obj, BSP_TREE * bsp)
{
    SPECIAL_OBJECT *special_obj = (SPECIAL_OBJECT *) obj;
    POLYLINE_3D *special = new_polyline_3d(1);
    special->n_vertices = 1;
    // Find the first point.
    copy_pt_3d(special->v[0], special_first_point(special_obj));
    add_polyline_to_bsp(bsp, special, obj);
}

static BSP_INSERT_FUNC insert_in_bsp_pass_2_tbl[] = {
    NULL,			// O_BASE
    NULL,			// O_TAG_DEF  
    NULL,			// O_OPTS_DEF
    NULL,			// O_SCALAR_DEF
    NULL,			// O_POINT_DEF
    NULL,			// O_VECTOR_DEF
    NULL,			// O_TRANSFORM_DEF
    add_dots_object_to_bsp_pass_2,
    add_line_object_to_bsp_pass_2,
    add_curve_object_to_bsp_pass_2,
    add_polygon_object_to_bsp_pass_2,
    add_special_object_to_bsp_pass_2,
    NULL,			// O_SWEEP (flattened)
    NULL,			// O_REPEAT (flattened)
    NULL,			// O_COMPOUND (flattened)
};

// table functions are called in 
// OBJECT *hsr_scene_with_bsp(OBJECT *scene);

static void
get_dots_from_polyline(OBJECT * src, OBJECT ** output,
		       BSP_POLYLINE_NODE * polyline_node)
{
    DOTS_OBJECT *dots_src = (DOTS_OBJECT *) src;
    DOTS_OBJECT *new_obj = raw_dots(dots_src->opts);
    copy_point_list_3d(new_obj->pts,
		       (POINT_LIST_3D *) polyline_node->polyline);
    new_obj->sibling = *output;
    *output = (OBJECT *) new_obj;
}

static void
get_line_from_polyline(OBJECT * src, OBJECT ** output,
		       BSP_POLYLINE_NODE * polyline_node)
{
    LINE_OBJECT *line_src = (LINE_OBJECT *) src;
    LINE_OBJECT *new_obj = raw_line(copy_line_opts(line_src->opts,
						   polyline_node->first_p,
						   polyline_node->last_p,
						   global_env->output_language));
    copy_point_list_3d(new_obj->pts,
		       (POINT_LIST_3D *) polyline_node->polyline);
    new_obj->sibling = *output;
    *output = (OBJECT *) new_obj;
}

static void
get_curve_from_polyline(OBJECT * src, OBJECT ** output,
			BSP_POLYLINE_NODE * polyline_node)
{
}

static void
get_polygon_border_from_polyline(OBJECT * src,
				 OBJECT ** output,
				 BSP_POLYLINE_NODE * polyline_node)
{
    // no longer used
    POLYGON_OBJECT *polygon_src = (POLYGON_OBJECT *) src;
    LINE_OBJECT *new_obj = raw_line(copy_opts(polygon_src->opts, OPT_LINE,
					      global_env->
					      output_language));
    copy_point_list_3d(new_obj->pts,
		       (POINT_LIST_3D *) polyline_node->polyline);
    new_obj->sibling = *output;
    *output = (OBJECT *) new_obj;
}

static void
get_special_from_polyline(OBJECT * src, OBJECT ** output,
			  BSP_POLYLINE_NODE * polyline_node)
{
    SPECIAL_OBJECT *special_src = (SPECIAL_OBJECT *) src;
    SPECIAL_OBJECT *new_obj =
	raw_special(copy_opts(special_src->opts, OPT_INTERNAL,
			      global_env->output_language));
    // go back to original special since we didn't split
    copy_special_arg_list(new_obj->args, special_src->args);
    new_obj->code = safe_strdup(special_src->code);
    new_obj->sibling = *output;
    *output = (OBJECT *) new_obj;
}

typedef void (*GET_OBJ_FROM_POLYLINE_FUNC) (OBJECT * src, OBJECT ** output,
					    BSP_POLYLINE_NODE *
					    polyline_node);

static GET_OBJ_FROM_POLYLINE_FUNC get_obj_from_polyline_tbl[] = {
    NULL,			// O_BASE
    NULL,			// O_TAG_DEF  
    NULL,			// O_OPTS_DEF
    NULL,			// O_SCALAR_DEF
    NULL,			// O_POINT_DEF
    NULL,			// O_VECTOR_DEF
    NULL,			// O_TRANSFORM_DEF
    get_dots_from_polyline,
    get_line_from_polyline,
    get_curve_from_polyline,
    get_polygon_border_from_polyline,
    get_special_from_polyline,
    NULL,			// O_SWEEP (flattened)
    NULL,			// O_REPEAT (flattened)
    NULL,			// O_COMPOUND (flattened)
};

void get_objects_from_bsp_node(BSP_NODE * bsp, void *env)
{
    int i, j, k, broken_border_p;
    OBJECT **output = (OBJECT **) env;

    if (bsp == NULL)
	return;

    if (bsp->tag == BSP_POLYGON) {
	OPTS *opts;
	LINE_OBJECT *new_line_obj = NULL;
	BSP_POLYGON_NODE *polygon_node = (BSP_POLYGON_NODE *) bsp;
	POLYGON_OBJECT *src = bsp->attr, *new_polygon_obj;

	broken_border_p = 0;
	for (i = 0; i < polygon_node->polygon->n_sides; i++) {
	    if (!polygon_node->polygon_attr->elt[i].border_p) {
		broken_border_p = 1;
		break;
	    }
	}
	if (broken_border_p) {

	    //  add these options if user didn't specify them
	    opts =
		copy_opts(src->opts, OPT_POLYGON,
			  global_env->output_language);
	    add_no_edges_default_opt(&opts, global_env->output_language);
	    add_solid_white_default_opt(&opts,
					global_env->output_language);

	    new_polygon_obj = raw_polygon(opts);
	    copy_point_list_3d(new_polygon_obj->pts,
			       (POINT_LIST_3D *) polygon_node->polygon);
	    new_polygon_obj->sibling = *output;
	    *output = (OBJECT *) new_polygon_obj;

	    // create the border from edges that did not result from splitting
	    //
	    // find a break in the border if there is one
	    for (j = polygon_node->polygon->n_sides - 1, i = 0;
		 i < polygon_node->polygon->n_sides; j = i++) {
		if (!polygon_node->polygon_attr->elt[j].border_p)
		    break;
	    }
	    if (i == polygon_node->polygon->n_sides)
		i = 0;
	    // j->i now isn't inside a border edge section, which is what we want
	    for (k = 0;
		 k < polygon_node->polygon->n_sides;
		 j = i, i = (i + 1) % polygon_node->polygon->n_sides, k++)
	    {
		if (polygon_node->polygon_attr->elt[j].border_p) {
		    if (new_line_obj == NULL) {
			opts =
			    copy_opts(src->opts, OPT_LINE,
				      global_env->output_language);
			new_line_obj = raw_line(opts);
			copy_pt_3d(pushed_point_list_3d_v
				   (new_line_obj->pts),
				   polygon_node->polygon->v[j]);
		    }
		    copy_pt_3d(pushed_point_list_3d_v(new_line_obj->pts),
			       polygon_node->polygon->v[i]);
		} else if (new_line_obj) {
		    new_line_obj->sibling = *output;
		    *output = (OBJECT *) new_line_obj;
		    new_line_obj = NULL;
		}
	    }
	    if (new_line_obj) {
		new_line_obj->sibling = *output;
		*output = (OBJECT *) new_line_obj;
		new_line_obj = NULL;
	    }
	} else {
	    opts =
		copy_opts(src->opts, OPT_POLYGON | OPT_LINE,
			  global_env->output_language);
	    add_solid_white_default_opt(&opts,
					global_env->output_language);

	    new_polygon_obj = raw_polygon(opts);
	    new_polygon_obj->border_p = 1;
	    copy_point_list_3d(new_polygon_obj->pts,
			       (POINT_LIST_3D *) polygon_node->polygon);
	    new_polygon_obj->sibling = *output;
	    *output = (OBJECT *) new_polygon_obj;
	}
    } else {			// BSP_POLYLINE
	OBJECT *src = bsp->attr;
	(*get_obj_from_polyline_tbl[src->tag]) (src, output,
						(BSP_POLYLINE_NODE *) bsp);
    }
}

static void add_default_fill_opts(POLYGON_OBJECT * src)
{
    OPTS *opts = copy_opts(src->opts, OPT_POLYGON | OPT_LINE,
			   global_env->output_language);
    add_solid_white_default_opt(&opts, global_env->output_language);
    src->opts = opts;
}

static void add_default_fill_opts_to_polygons(OBJECT * src)
{
    OBJECT *obj;

    for (obj = src; obj; obj = obj->sibling)
	if (obj->tag == O_POLYGON)
	    add_default_fill_opts((POLYGON_OBJECT *) obj);
}

OBJECT *hsr_scene_with_bsp(OBJECT * scene)
{
    OBJECT *p, *t, *underlay, *overlay, *sorted;
    BSP_TREE bsp;

    // two passes are needed to serve the bsp requirement
    // that polylines be inserted after all polygons are
    // already there
    // also take care of underlays and ovelays in the first pass
    bsp = NULL;
    underlay = overlay = sorted = NULL;
    for (p = scene; p; p = p->sibling) {
	switch (object_lay_val(p)) {
	case LAY_UNDER:
	    t = copy_drawable_without_siblings(p);
	    t->sibling = underlay;
	    underlay = t;
	    break;
	case LAY_IN:
	    (*insert_in_bsp_pass_1_tbl[p->tag]) (p, &bsp);
	    break;
	case LAY_OVER:
	    t = copy_drawable_without_siblings(p);
	    t->sibling = overlay;
	    overlay = t;
	    break;
	default:
	    die(no_line, "bad lay value in hsr_scene_with_bsp");
	    break;
	}
    }
    for (p = scene; p; p = p->sibling)
	if (object_lay_val(p) == LAY_IN)
	    (*insert_in_bsp_pass_2_tbl[p->tag]) (p, &bsp);
    traverse_bsp(bsp, get_objects_from_bsp_node, &sorted);
    // Overlay and underlay polygons still need fill options added.
    add_default_fill_opts_to_polygons(overlay);
    add_default_fill_opts_to_polygons(underlay);
    // All three sublists are now in reverse order, so cat overlay then underlay
    sorted = cat_objects(overlay, sorted);
    sorted = cat_objects(sorted, underlay);
    // Final reverse makes everything right.
    sorted = sibling_reverse(sorted);
    return sorted;
}

// ---- depth sort ------------------------------------------------------------

static void add_dots_object_to_sort(OBJECT * obj, BSP_TREE * bsp)
{
    int i;
    DOTS_OBJECT *dots_obj = (DOTS_OBJECT *) obj;
    POLYLINE_3D dot[1];

    init_polyline_3d(dot);
    setup_polyline_3d(dot, 1);
    dot->n_vertices = 1;

    // insert each dot as a polyline with only one vertex
    for (i = 0; i < dots_obj->pts->n_pts; i++) {
	copy_pt_3d(dot->v[0], dots_obj->pts->v[i]);
	add_polyline_to_sort(bsp, dot, obj);
    }
    clear_polyline_3d(dot);
}

static void add_line_object_to_sort(OBJECT * obj, BSP_TREE * bsp)
{
    LINE_OBJECT *line_obj = (LINE_OBJECT *) obj;
    // DANGER: assumes point list in polyline object is congruent to a geometry.h polyline
    add_polyline_to_sort(bsp, (POLYLINE_3D *) line_obj->pts, obj);
}

static void add_curve_object_to_sort(OBJECT * obj, BSP_TREE * bsp)
{
}

static void add_polygon_object_to_sort(OBJECT * obj, BSP_TREE * bsp)
{
    POLYGON_OBJECT *polygon_obj = (POLYGON_OBJECT *) obj;
    PLANE plane[1];

    // backface elimination
    // put the new polygon in the tree
    find_polygon_plane(plane, (POLYGON_3D *) polygon_obj->pts);
    if (plane->n[Z] >= -FLOAT_EPS ||
	!bool_opt_p(polygon_obj->opts, "cull", 1) ||
	!bool_opt_p(global_env->opts, "cull", 1))
	add_polygon_to_sort(bsp, (POLYGON_3D *) polygon_obj->pts, obj);
}

static void add_special_object_to_sort(OBJECT * obj, BSP_TREE * bsp)
{
    SPECIAL_OBJECT *special_obj = (SPECIAL_OBJECT *) obj;
    POLYLINE_3D *special = new_polyline_3d(1);
    special->n_vertices = 1;
    copy_pt_3d(special->v[0], special_first_point(special_obj));
    add_polyline_to_sort(bsp, special, obj);
}

typedef void (*ADD_TO_DS_FUNC) (OBJECT *, BSP_TREE *);

static ADD_TO_DS_FUNC add_to_sort_tbl[] = {
    NULL,			// O_BASE
    NULL,			// O_TAG_DEF
    NULL,			// O_OPTS_DEF
    NULL,			// O_SCALAR_DEF
    NULL,			// O_POINT_DEF
    NULL,			// O_VECTOR_DEF
    NULL,			// O_TRANSFORM_DEF
    add_dots_object_to_sort,
    add_line_object_to_sort,
    add_curve_object_to_sort,
    add_polygon_object_to_sort,
    add_special_object_to_sort,
    NULL,			// O_SWEEP (flattened)
    NULL,			// O_REPEAT (flattened)
    NULL,			// O_COMPOUND (flattened)
};

OBJECT *hsr_scene_with_depth_sort(OBJECT * scene)
{
    OBJECT *p, *t, *underlay, *overlay, *sorted;
    BSP_TREE bsp;

    // two passes are needed to serve the bsp requirement
    // that polylines be inserted after all polygons are
    // already there
    // also take care of underlays and ovelays in the first pass
    bsp = NULL;
    underlay = overlay = sorted = NULL;
    for (p = scene; p; p = p->sibling) {
	switch (object_lay_val(p)) {
	case LAY_UNDER:
	    t = copy_drawable_without_siblings(p);
	    t->sibling = underlay;
	    underlay = t;
	    break;
	case LAY_IN:
	    (*add_to_sort_tbl[p->tag]) (p, &bsp);
	    break;
	case LAY_OVER:
	    t = copy_drawable_without_siblings(p);
	    t->sibling = overlay;
	    overlay = t;
	    break;
	default:
	    die(no_line, "bad lay value in hsr_scene_with_depth_sort");
	    break;
	}
    }
    sort_by_depth(&bsp);
    traverse_depth_sort(bsp, get_objects_from_bsp_node, &sorted);
    // Overlay and underlay polygons still need fill options added.
    add_default_fill_opts_to_polygons(overlay);
    add_default_fill_opts_to_polygons(underlay);
    // all three sublists are now in reverse order, so cat overlay to start
    sorted = cat_objects(overlay, sorted);
    sorted = cat_objects(sorted, underlay);
    // final reverse makes everything good.
    sorted = sibling_reverse(sorted);
    return sorted;
}

// ---- extent finding --------------------------------------------------------

static void get_extent_of_points(POINT_LIST_3D * pts, BOX_3D * e)
{
    int i;

    for (i = 0; i < pts->n_pts; i++)
	fold_min_max_pt_3d(e, pts->v[i]);
}

static void get_extent_of_dots(OBJECT * obj, BOX_3D * e)
{
    DOTS_OBJECT *dots = (DOTS_OBJECT *) obj;
    get_extent_of_points(dots->pts, e);
}

static void get_extent_of_line(OBJECT * obj, BOX_3D * e)
{
    LINE_OBJECT *line = (LINE_OBJECT *) obj;
    get_extent_of_points(line->pts, e);
}

static void get_extent_of_curve(OBJECT * obj, BOX_3D * e)
{
    CURVE_OBJECT *curve = (CURVE_OBJECT *) obj;
    get_extent_of_points(curve->pts, e);
}

static void get_extent_of_polygon(OBJECT * obj, BOX_3D * e)
{
    POLYGON_OBJECT *polygon = (POLYGON_OBJECT *) obj;
    get_extent_of_points(polygon->pts, e);
}

static void get_extent_of_special(OBJECT * obj, BOX_3D * e)
{
    SPECIAL_OBJECT *special = (SPECIAL_OBJECT *) obj;
    fold_min_max_pt_3d(e, special_first_point(special));
}

typedef void (*EXTENT_FUNC) (OBJECT *, BOX_3D *);

static EXTENT_FUNC extent_tbl[] = {
    NULL,			// O_BASE
    NULL,			// O_TAG_DEF
    NULL,			// O_OPTS_DEF
    NULL,			// O_SCALAR_DEF
    NULL,			// O_POINT_DEF
    NULL,			// O_VECTOR_DEF
    NULL,			// O_TRANSFORM_DEF
    get_extent_of_dots,
    get_extent_of_line,
    get_extent_of_curve,
    get_extent_of_polygon,
    get_extent_of_special,
    NULL,			// O_SWEEP (flattened)
    NULL,			// O_REPEAT (flattened)
    NULL,			// O_COMPOUND (flattened)
};

void get_extent(OBJECT * obj, BOX_3D * e, int *n_obj)
{
    if (obj) {
	int n = 0;
	init_box_3d(e);
	while (obj) {
	    if (extent_tbl[obj->tag] == NULL)
		die(no_line, "get_extent: bad tag %d", obj->tag);
	    (*extent_tbl[obj->tag]) (obj, e);
	    obj = obj->sibling;
	    ++n;
	}
	*n_obj = n;
    } else {
	// reasonable empty box
	e->min[X] = e->min[Y] = e->min[Z] = 0;
	e->max[X] = e->max[Y] = e->max[Z] = 1;
	*n_obj = 0;
    }
}

int xy_overlap_p(OBJECT * obj, BOX_3D * e)
{
    BOX_3D e_obj[1];

    init_box_3d(e_obj);
    (*extent_tbl[obj->tag]) (obj, e_obj);
    return
	!(e_obj->max[X] < e->min[X] ||
	  e_obj->min[X] > e->max[X] ||
	  e_obj->max[Y] < e->min[Y] || e_obj->min[Y] > e->max[Y]);
}
