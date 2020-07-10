/* expr.c
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
#include <math.h>
#include "expr.h"
#include "error.h"

#define F "%.3f"

char *expr_val_type_str[] = {
    "float",
    "point",
    "vector",
    "transform",
};

// set expression value to given type and value
void set_float(EXPR_VAL * r, FLOAT val)
{
    r->tag = E_FLOAT;
    r->val.flt = val;
}

void print_float(FILE * f, EXPR_VAL * val)
{
    fprintf(f, F, val->val.flt);
}

void set_point(EXPR_VAL * r, POINT_3D val)
{
    r->tag = E_POINT;
    copy_pt_3d(r->val.pt, val);
}

void print_point(FILE * f, EXPR_VAL * val)
{
    FLOAT *p = val->val.pt;
    fprintf(f, "(" F "," F "," F ")", p[X], p[Y], p[Z]);
}

void set_vector(EXPR_VAL * r, VECTOR_3D val)
{
    r->tag = E_VECTOR;
    copy_vec_3d(r->val.vec, val);
}

void print_vector(FILE * f, EXPR_VAL * val)
{
    FLOAT *v = val->val.vec;
    fprintf(f, "[" F "," F "," F "]", v[X], v[Y], v[Z]);
}

void set_transform(EXPR_VAL * r, TRANSFORM val)
{
    r->tag = E_TRANSFORM;
    copy_transform(r->val.xf, val);
}

void print_transform(FILE * f, EXPR_VAL * val)
{
    FLOAT *xf = val->val.xf;
    int i, j;

    fprintf(f, "[");
    for (i = 0; i < 4; i++) {
	fprintf(f, "[");
	for (j = 0; j < 16; j += 4)
	    fprintf(f, "%s" F, (j == 0) ? "" : ",", xf[i + j]);
	fprintf(f, "]");
    }
    fprintf(f, "]");
}

// coerce an expression value to given type 
// generate error message if it can't be done
void coerce_to_float(EXPR_VAL * r, FLOAT * val, SRC_LINE line)
{
    if (r->tag == E_FLOAT) {
	*val = r->val.flt;
    } else {
	*val = 0;
	err(line, "expected float, found %s", expr_val_type_str[r->tag]);
    }
}

void coerce_to_point(EXPR_VAL * r, POINT_3D val, SRC_LINE line)
{
    if (r->tag == E_POINT) {
	copy_pt_3d(val, r->val.pt);
    } else {
	val[X] = val[Y] = val[Z] = 0;
	err(line, "expected point, found %s", expr_val_type_str[r->tag]);
    }
}

void coerce_to_vector(EXPR_VAL * r, VECTOR_3D val, SRC_LINE line)
{
    if (r->tag == E_VECTOR) {
	copy_vec_3d(val, r->val.vec);
    } else {
	val[X] = val[Y] = val[Z] = 0;
	err(line, "expected vector, found %s", expr_val_type_str[r->tag]);
    }
}

void coerce_to_transform(EXPR_VAL * r, TRANSFORM val, SRC_LINE line)
{
    if (r->tag == E_TRANSFORM) {
	copy_transform(val, r->val.xf);
    } else {
	set_ident(val);
	err(line, "expected transform, found %s",
	    expr_val_type_str[r->tag]);
    }
}

typedef void (*PRINT_FUNC) (FILE *, EXPR_VAL *);

static PRINT_FUNC print_expr_val_tbl[] = {
    print_float,
    print_point,
    print_vector,
    print_transform,
};

void print_expr_val(FILE * f, EXPR_VAL * r)
{
    (*print_expr_val_tbl[r->tag]) (f, r);
}

#define HASH(A, B) (((A) << 2) | (B))

void do_add(EXPR_VAL * r, EXPR_VAL * a, EXPR_VAL * b, SRC_LINE line)
{
    switch (HASH(a->tag, b->tag)) {
    case HASH(E_FLOAT, E_FLOAT):
	set_float(r, a->val.flt + b->val.flt);
	break;
    case HASH(E_POINT, E_VECTOR):
	r->tag = E_POINT;
	add_vec_to_pt_3d(r->val.pt, a->val.pt, b->val.vec);
	break;
    case HASH(E_VECTOR, E_POINT):
	r->tag = E_POINT;
	add_vec_to_pt_3d(r->val.pt, b->val.pt, a->val.vec);
	break;
    case HASH(E_VECTOR, E_VECTOR):
	r->tag = E_VECTOR;
	add_vecs_3d(r->val.vec, a->val.vec, b->val.vec);
	break;
    default:
	err(line, "operands of + (types %s and %s) cannot be added",
	    expr_val_type_str[a->tag], expr_val_type_str[b->tag]);
	set_float(r, 0);
	break;
    }
}

void do_sub(EXPR_VAL * r, EXPR_VAL * a, EXPR_VAL * b, SRC_LINE line)
{
    switch (HASH(a->tag, b->tag)) {
    case HASH(E_FLOAT, E_FLOAT):
	set_float(r, a->val.flt - b->val.flt);
	break;
    case HASH(E_POINT, E_POINT):
	r->tag = E_VECTOR;
	sub_pts_3d(r->val.vec, a->val.pt, b->val.pt);
	break;
    case HASH(E_POINT, E_VECTOR):
	r->tag = E_POINT;
	add_scaled_vec_to_pt_3d(r->val.pt, a->val.pt, b->val.vec, -1);
	break;
    case HASH(E_VECTOR, E_VECTOR):
	r->tag = E_VECTOR;
	sub_vecs_3d(r->val.vec, a->val.vec, b->val.vec);
	break;
    default:
	err(line, "operands of - (types %s and %s) cannot be subtracted",
	    expr_val_type_str[a->tag], expr_val_type_str[b->tag]);
	set_float(r, 0);
	break;
    }
}

void do_mul(EXPR_VAL * r, EXPR_VAL * a, EXPR_VAL * b, SRC_LINE line)
{
    switch (HASH(a->tag, b->tag)) {
    case HASH(E_FLOAT, E_FLOAT):
	set_float(r, a->val.flt * b->val.flt);
	break;
    case HASH(E_VECTOR, E_FLOAT):
	r->tag = E_VECTOR;
	scale_vec_3d(r->val.vec, a->val.vec, b->val.flt);
	break;
    case HASH(E_FLOAT, E_VECTOR):
	r->tag = E_VECTOR;
	scale_vec_3d(r->val.vec, b->val.vec, a->val.flt);
	break;
    case HASH(E_VECTOR, E_VECTOR):
	r->tag = E_VECTOR;
	cross(r->val.vec, a->val.vec, b->val.vec);
	break;
    case HASH(E_TRANSFORM, E_TRANSFORM):
	r->tag = E_TRANSFORM;
	compose(r->val.xf, a->val.xf, b->val.xf);
	break;
    case HASH(E_TRANSFORM, E_POINT):
	r->tag = E_POINT;
	transform_pt_3d(r->val.pt, a->val.xf, b->val.pt);
	break;
    case HASH(E_TRANSFORM, E_VECTOR):
	r->tag = E_VECTOR;
	transform_vec_3d(r->val.vec, a->val.xf, b->val.vec);
	break;
    default:
	err(line, "operands of * (types %s and %s) cannot be multiplied",
	    expr_val_type_str[a->tag], expr_val_type_str[b->tag]);
	set_float(r, 0);
	break;
    }
}

void do_thn(EXPR_VAL * r, EXPR_VAL * a, EXPR_VAL * b, SRC_LINE line)
{
    switch (HASH(a->tag, b->tag)) {
    case HASH(E_TRANSFORM, E_TRANSFORM):
	r->tag = E_TRANSFORM;
	compose(r->val.xf, b->val.xf, a->val.xf);
	break;
    case HASH(E_POINT, E_TRANSFORM):
	r->tag = E_POINT;
	transform_pt_3d(r->val.pt, b->val.xf, a->val.pt);
	break;
    case HASH(E_VECTOR, E_TRANSFORM):
	r->tag = E_VECTOR;
	transform_vec_3d(r->val.vec, b->val.xf, a->val.vec);
	break;
    default:
	err(line,
	    "operands of 'then' (types %s and %s) cannot be multiplied",
	    expr_val_type_str[a->tag], expr_val_type_str[b->tag]);
	set_float(r, 0);
	break;
    }
}

static FLOAT safe_dvd(FLOAT a, FLOAT b, SRC_LINE line)
{
    if (-FLOAT_EPS < b && b < FLOAT_EPS) {
	err(line, "attempt to divide " F " by zero", a);
	return 0;
    }
    return a / b;
}

void do_dvd(EXPR_VAL * r, EXPR_VAL * a, EXPR_VAL * b, SRC_LINE line)
{
    switch (HASH(a->tag, b->tag)) {
    case HASH(E_FLOAT, E_FLOAT):
	set_float(r, safe_dvd(a->val.flt, b->val.flt, line));
	break;
    case HASH(E_VECTOR, E_FLOAT):
	r->tag = E_VECTOR;
	scale_vec_3d(r->val.vec, a->val.vec,
		     safe_dvd(1, b->val.flt, line));
	break;
    case HASH(E_FLOAT, E_VECTOR):
	r->tag = E_VECTOR;
	scale_vec_3d(r->val.vec, b->val.vec,
		     safe_dvd(1, a->val.flt, line));
	break;
    default:
	err(line, "operands of / (types %s and %s) cannot be divided",
	    expr_val_type_str[a->tag], expr_val_type_str[b->tag]);
	set_float(r, 0);
	break;
    }
}

void do_dot(EXPR_VAL * r, EXPR_VAL * a, EXPR_VAL * b, SRC_LINE line)
{
    switch (HASH(a->tag, b->tag)) {
    case HASH(E_VECTOR, E_VECTOR):
	r->tag = E_FLOAT;
	r->val.flt = dot_3d(a->val.vec, b->val.vec);
	break;
    case HASH(E_FLOAT, E_FLOAT):
    case HASH(E_VECTOR, E_FLOAT):
    case HASH(E_FLOAT, E_VECTOR):
    case HASH(E_TRANSFORM, E_TRANSFORM):
    case HASH(E_TRANSFORM, E_POINT):
    case HASH(E_TRANSFORM, E_VECTOR):
	do_mul(r, a, b, line);
	break;
    default:
	err(line, "operands of dot (types %s and %s) cannot be multiplied",
	    expr_val_type_str[a->tag], expr_val_type_str[b->tag]);
	set_float(r, 0);
	break;
    }
}

void do_index(EXPR_VAL * r, EXPR_VAL * a, int index, SRC_LINE line)
{
    switch (a->tag) {
    case E_VECTOR:
	set_float(r, a->val.vec[index]);
	break;
    case E_POINT:
	set_float(r, a->val.pt[index]);
	break;
    default:
	err(line,
	    "operand of 'index is a %s and should be a point or a vector",
	    expr_val_type_str[a->tag]);
	set_float(r, 0);
	break;
    }
}

void do_inverse(TRANSFORM inv, TRANSFORM xf, SRC_LINE line)
{
    FLOAT det;
    invert(inv, &det, xf, 1e-4);
    if (det == 0) {
	err(line, "inverse of singular transform");
	set_ident(inv);
    }
}

// put a^n into r;  r and a can't both be the same storage
// exploits a^(2n) = (a^n)^2 to reduce work
void do_transform_power(TRANSFORM r, TRANSFORM a, int n, SRC_LINE line)
{
    if (n < 0) {
	TRANSFORM inv;
	do_inverse(inv, a, line);
	do_transform_power(r, inv, -n, line);
    } else if (n == 0) {
	set_ident(r);
    } else {
	int m = (int) bit(30);
	while ((m & n) == 0)
	    m >>= 1;
	copy_transform(r, a);
	for (m >>= 1; m; m >>= 1) {
	    compose(r, r, r);
	    if (m & n)
		compose(r, r, a);
	}
    }
}

int to_integer(FLOAT x, int *n)
{
    double frac_part, int_part;

    frac_part = modf(x, &int_part);
    if (-1e9 <= int_part && int_part <= 1e9) {
	*n = (int) int_part;
	return 1;
    }
    return 0;
}

void do_pwr(EXPR_VAL * r, EXPR_VAL * a, EXPR_VAL * b, SRC_LINE line)
{
    TRANSFORM xf_pwr;
    int n;

    switch (HASH(a->tag, b->tag)) {
    case HASH(E_FLOAT, E_FLOAT):
	set_float(r, pow(a->val.flt, b->val.flt));
	break;
    case HASH(E_TRANSFORM, E_FLOAT):
	if (to_integer(b->val.flt, &n)) {
	    do_transform_power(xf_pwr, a->val.xf, n, line);
	} else {
	    err(line, "transform power out of domain (integer -1e9..1e9)");
	    set_ident(xf_pwr);
	}
	set_transform(r, xf_pwr);
	break;
    default:
	err(line, "operands of ^ (types %s and %s) must be type float",
	    expr_val_type_str[a->tag], expr_val_type_str[b->tag]);
	set_float(r, 0);
	break;
    }
}

void do_mag(EXPR_VAL * r, EXPR_VAL * a, SRC_LINE line)
{
    switch (a->tag) {
    case E_FLOAT:
	set_float(r, a->val.flt >= 0 ? a->val.flt : -a->val.flt);
	break;
    case E_VECTOR:
	set_float(r, length_vec_3d(a->val.vec));
	break;
    default:
	err(line, "operand of magnitude operator (type %s) must be vector",
	    expr_val_type_str[a->tag]);
	*r = *a;
	break;
    }
}

void do_neg(EXPR_VAL * r, EXPR_VAL * a, SRC_LINE line)
{
    switch (a->tag) {
    case E_FLOAT:
	set_float(r, -a->val.flt);
	break;
    case E_VECTOR:
	r->tag = E_VECTOR;
	negate_vec_3d(r->val.vec, a->val.vec);
	break;
    default:
	err(line, "operand of unary minus (type %s) cannot be negated",
	    expr_val_type_str[a->tag]);
	*r = *a;
	break;
    }
}

void do_unit(EXPR_VAL * r, EXPR_VAL * a, SRC_LINE line)
{
    if (a->tag == E_VECTOR) {
	r->tag = E_VECTOR;
	find_unit_vec_3d(r->val.vec, a->val.vec);
    } else {
	static VECTOR_3D k = { 0, 0, 1 };
	err(line, "operand of unit operator (type %s) must be vector",
	    expr_val_type_str[a->tag]);
	set_vector(r, k);
    }
}

void do_sqrt(EXPR_VAL * r, EXPR_VAL * a, SRC_LINE line)
{
    switch (a->tag) {
    case E_FLOAT:
	if (a->val.flt < 0)
	    err(line, "square root of negative number");
	set_float(r, sqrt(a->val.flt));
	break;
    default:
	err(line, "operand of sqrt (type %s) must be float",
	    expr_val_type_str[a->tag]);
	break;
    }
}

void do_sin(EXPR_VAL * r, EXPR_VAL * a, SRC_LINE line)
{
    switch (a->tag) {
    case E_FLOAT:
	set_float(r, sin((PI / 180) * a->val.flt));
	break;
    default:
	err(line, "operand of sin (type %s) must be float",
	    expr_val_type_str[a->tag]);
	break;
    }
}

void do_cos(EXPR_VAL * r, EXPR_VAL * a, SRC_LINE line)
{
    switch (a->tag) {
    case E_FLOAT:
	set_float(r, cos((PI / 180) * a->val.flt));
	break;
    default:
	err(line, "operand of cos (type %s) must be float",
	    expr_val_type_str[a->tag]);
	break;
    }
}

void do_asin(EXPR_VAL * r, EXPR_VAL * a, SRC_LINE line)
{
    switch (a->tag) {
    case E_FLOAT:
	if (-1 < a->val.flt || a->val.flt > 1)
	    err(line, "asin operand is out of range [-1..1]");
	set_float(r, (180 / PI) * asin(a->val.flt));
	break;
    default:
	err(line, "operand of asin (type %s) must be float",
	    expr_val_type_str[a->tag]);
	break;
    }
}

void do_acos(EXPR_VAL * r, EXPR_VAL * a, SRC_LINE line)
{
    switch (a->tag) {
    case E_FLOAT:
	if (-1 < a->val.flt || a->val.flt > 1)
	    err(line, "asin operand is out of range [-1..1]");
	set_float(r, (180 / PI) * acos(a->val.flt));
	break;
    default:
	err(line, "operand of acos (type %s) must be float",
	    expr_val_type_str[a->tag]);
	break;
    }
}

void do_atan2(EXPR_VAL * r, EXPR_VAL * a, EXPR_VAL * b, SRC_LINE line)
{
    switch (HASH(a->tag, b->tag)) {
    case HASH(E_FLOAT, E_FLOAT):
	set_float(r, (180 / PI) * atan2(a->val.flt, b->val.flt));
	break;
    default:
	err(line, "operands of atan2 (types %s, %s) must be float",
	    expr_val_type_str[a->tag], expr_val_type_str[b->tag]);
	break;
    }
}
