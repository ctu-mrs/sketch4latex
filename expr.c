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
#include <stdlib.h>
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
    r->val.inn.flt = val;
}

void print_float(FILE * f, EXPR_VAL * val)
{
    fprintf(f, F, val->val.inn.flt);
}

void set_point(EXPR_VAL * r, POINT_3D val)
{
    r->tag = E_POINT;
    copy_pt_3d(r->val.inn.pt, val);
}

void print_point(FILE * f, EXPR_VAL * val)
{
    FLOAT *p = val->val.inn.pt;
    fprintf(f, "(" F "," F "," F ")", p[X], p[Y], p[Z]);
}

void set_vector(EXPR_VAL * r, VECTOR_3D val)
{
    r->tag = E_VECTOR;
    copy_vec_3d(r->val.inn.vec, val);
}

void print_vector(FILE * f, EXPR_VAL * val)
{
    FLOAT *v = val->val.inn.vec;
    fprintf(f, "[" F "," F "," F "]", v[X], v[Y], v[Z]);
}

void set_transform(EXPR_VAL * r, TRANSFORM val)
{
    r->tag = E_TRANSFORM;
    copy_transform(r->val.inn.xf, val);
}

void print_transform(FILE * f, EXPR_VAL * val)
{
    FLOAT *xf = val->val.inn.xf;
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
	*val = r->val.inn.flt;
    } else {
	*val = 0;
	err(line, "expected float, found %s", expr_val_type_str[r->tag]);
    }
}

void coerce_to_point(EXPR_VAL * r, POINT_3D val, SRC_LINE line)
{
    if (r->tag == E_POINT) {
	copy_pt_3d(val, r->val.inn.pt);
    } else {
	val[X] = val[Y] = val[Z] = 0;
	err(line, "expected point, found %s", expr_val_type_str[r->tag]);
    }
}

void coerce_to_vector(EXPR_VAL * r, VECTOR_3D val, SRC_LINE line)
{
    if (r->tag == E_VECTOR) {
	copy_vec_3d(val, r->val.inn.vec);
    } else {
	val[X] = val[Y] = val[Z] = 0;
	err(line, "expected vector, found %s", expr_val_type_str[r->tag]);
    }
}

void coerce_to_transform(EXPR_VAL * r, TRANSFORM val, SRC_LINE line)
{
    if (r->tag == E_TRANSFORM) {
	copy_transform(val, r->val.inn.xf);
    } else {
	set_ident(val);
	err(line, "expected transform, found %s",
	    expr_val_type_str[r->tag]);
    }
}

void copy_expr_to_array_element(INNER_VAL * r, EXPR_VAL val){
  fprintf(stderr,"[Array expression]: Sanitize this - array should not be of multiple types.\n");
  if (!r) { 
    perror("[Array expression]: Array element is not allocated");
    abort();
  }
  if (val.tag != E_ARRAY){
    *r = val.val.inn;
  }
}

void copy_array(ARRAY * r, ARRAY * val)
{
  fprintf(stderr,"[Array expression]: Copying array.\n");
  r = safe_malloc(sizeof(ARRAY));
  if (!(r)) { 
    perror("[Array expression]: Error allocating array");
    abort();
  }
  r->length = val->length;
  r->data = safe_malloc(sizeof(EXPR_VAL) * val->length);
  if (!r->data) { 
    perror("[Array expression]: Error allocating memory");
    abort();
  }
  for (int i=0; i<val->length; i++){
    r->data[i] = val->data[i];
  }

}

void link_array(ARRAY ** r, ARRAY * val)
{
  fprintf(stderr,"[Array expression]: Linking array.\n");
  /* if (r){ */
  /*   free(r); */
  /* } */
  *r = val;
}

ARRAY* new_array_from_element(EXPR_VAL val){
  fprintf(stderr,"[Array expression]: Initiating with element: %f\n", val.val.inn.flt);
  ARRAY *arr = safe_malloc(sizeof(ARRAY));
  arr->data = safe_malloc(sizeof(EXPR_VAL));
  arr->length = 1;
  return arr;
}

ARRAY* append_array_element(ARRAY * prev_array, EXPR_VAL new_element){
  fprintf(stderr,"[Array expression]: Appending element: %f\n", new_element.val.inn.flt);
  prev_array->data = safe_realloc (prev_array->data, prev_array->length+1);
  copy_expr_to_array_element(&(prev_array->data[prev_array->length]), new_element);
  prev_array->length++;
  return prev_array;
}

ARRAY* new_array_ranged(FLOAT ini, FLOAT incr, FLOAT fini){
  fprintf(stderr,"[Array expression]: Ranged array declaration not yet implemented. Sorry\n");
  return (ARRAY*)NULL;
}

void set_array(EXPR_VAL * r, ARRAY val)
{
    r->tag = E_ARRAY;
    copy_array(&(r->val.arr), &val);
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
	set_float(r, a->val.inn.flt + b->val.inn.flt);
	break;
    case HASH(E_POINT, E_VECTOR):
	r->tag = E_POINT;
	add_vec_to_pt_3d(r->val.inn.pt, a->val.inn.pt, b->val.inn.vec);
	break;
    case HASH(E_VECTOR, E_POINT):
	r->tag = E_POINT;
	add_vec_to_pt_3d(r->val.inn.pt, b->val.inn.pt, a->val.inn.vec);
	break;
    case HASH(E_VECTOR, E_VECTOR):
	r->tag = E_VECTOR;
	add_vecs_3d(r->val.inn.vec, a->val.inn.vec, b->val.inn.vec);
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
	set_float(r, a->val.inn.flt - b->val.inn.flt);
	break;
    case HASH(E_POINT, E_POINT):
	r->tag = E_VECTOR;
	sub_pts_3d(r->val.inn.vec, a->val.inn.pt, b->val.inn.pt);
	break;
    case HASH(E_POINT, E_VECTOR):
	r->tag = E_POINT;
	add_scaled_vec_to_pt_3d(r->val.inn.pt, a->val.inn.pt, b->val.inn.vec, -1);
	break;
    case HASH(E_VECTOR, E_VECTOR):
	r->tag = E_VECTOR;
	sub_vecs_3d(r->val.inn.vec, a->val.inn.vec, b->val.inn.vec);
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
	set_float(r, a->val.inn.flt * b->val.inn.flt);
	break;
    case HASH(E_VECTOR, E_FLOAT):
	r->tag = E_VECTOR;
	scale_vec_3d(r->val.inn.vec, a->val.inn.vec, b->val.inn.flt);
	break;
    case HASH(E_FLOAT, E_VECTOR):
	r->tag = E_VECTOR;
	scale_vec_3d(r->val.inn.vec, b->val.inn.vec, a->val.inn.flt);
	break;
    case HASH(E_VECTOR, E_VECTOR):
	r->tag = E_VECTOR;
	cross(r->val.inn.vec, a->val.inn.vec, b->val.inn.vec);
	break;
    case HASH(E_TRANSFORM, E_TRANSFORM):
	r->tag = E_TRANSFORM;
	compose(r->val.inn.xf, a->val.inn.xf, b->val.inn.xf);
	break;
    case HASH(E_TRANSFORM, E_POINT):
	r->tag = E_POINT;
	transform_pt_3d(r->val.inn.pt, a->val.inn.xf, b->val.inn.pt);
	break;
    case HASH(E_TRANSFORM, E_VECTOR):
	r->tag = E_VECTOR;
	transform_vec_3d(r->val.inn.vec, a->val.inn.xf, b->val.inn.vec);
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
	compose(r->val.inn.xf, b->val.inn.xf, a->val.inn.xf);
	break;
    case HASH(E_POINT, E_TRANSFORM):
	r->tag = E_POINT;
	transform_pt_3d(r->val.inn.pt, b->val.inn.xf, a->val.inn.pt);
	break;
    case HASH(E_VECTOR, E_TRANSFORM):
	r->tag = E_VECTOR;
	transform_vec_3d(r->val.inn.vec, b->val.inn.xf, a->val.inn.vec);
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
	set_float(r, safe_dvd(a->val.inn.flt, b->val.inn.flt, line));
	break;
    case HASH(E_VECTOR, E_FLOAT):
	r->tag = E_VECTOR;
	scale_vec_3d(r->val.inn.vec, a->val.inn.vec,
		     safe_dvd(1, b->val.inn.flt, line));
	break;
    case HASH(E_FLOAT, E_VECTOR):
	r->tag = E_VECTOR;
	scale_vec_3d(r->val.inn.vec, b->val.inn.vec,
		     safe_dvd(1, a->val.inn.flt, line));
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
	r->val.inn.flt = dot_3d(a->val.inn.vec, b->val.inn.vec);
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
	set_float(r, a->val.inn.vec[index]);
	break;
    case E_POINT:
	set_float(r, a->val.inn.pt[index]);
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
	set_float(r, pow(a->val.inn.flt, b->val.inn.flt));
	break;
    case HASH(E_TRANSFORM, E_FLOAT):
	if (to_integer(b->val.inn.flt, &n)) {
	    do_transform_power(xf_pwr, a->val.inn.xf, n, line);
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
	set_float(r, a->val.inn.flt >= 0 ? a->val.inn.flt : -a->val.inn.flt);
	break;
    case E_VECTOR:
	set_float(r, length_vec_3d(a->val.inn.vec));
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
	set_float(r, -a->val.inn.flt);
	break;
    case E_VECTOR:
	r->tag = E_VECTOR;
	negate_vec_3d(r->val.inn.vec, a->val.inn.vec);
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
	find_unit_vec_3d(r->val.inn.vec, a->val.inn.vec);
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
	if (a->val.inn.flt < 0)
	    err(line, "square root of negative number");
	set_float(r, sqrt(a->val.inn.flt));
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
	set_float(r, sin((PI / 180) * a->val.inn.flt));
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
	set_float(r, cos((PI / 180) * a->val.inn.flt));
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
	if (-1 < a->val.inn.flt || a->val.inn.flt > 1)
	    err(line, "asin operand is out of range [-1..1]");
	set_float(r, (180 / PI) * asin(a->val.inn.flt));
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
	if (-1 < a->val.inn.flt || a->val.inn.flt > 1)
	    err(line, "asin operand is out of range [-1..1]");
	set_float(r, (180 / PI) * acos(a->val.inn.flt));
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
	set_float(r, (180 / PI) * atan2(a->val.inn.flt, b->val.inn.flt));
	break;
    default:
	err(line, "operands of atan2 (types %s, %s) must be float",
	    expr_val_type_str[a->tag], expr_val_type_str[b->tag]);
	break;
    }
}
