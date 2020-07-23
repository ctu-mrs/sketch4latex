/* expr.h
   Copyright (C) 2005,2006,2007,2008 Eugene K. Ressler, Jr.

   Edited by Viktor Walter

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

#ifndef __EXPR_H
#define __EXPR_H

#include <stdio.h>
#include "geometry.h"

// used by parser for dynamically typed expression evaluation
// must match expr_val_type_str[] in expr.c
typedef enum expr_typa_t {
    E_FLOAT,
    E_POINT,
    E_VECTOR,
    E_TRANSFORM,
    E_ARRAY,
} EXPR_TYPE;


extern char *expr_val_type_str[];

typedef union inner_val_t {
  FLOAT flt;
  POINT_3D pt;
  VECTOR_3D vec;
  TRANSFORM xf;
} INNER_VAL;

typedef struct array_t {
  unsigned int length;
  INNER_VAL *data;
} ARRAY ;

typedef struct expr_val_t {
    EXPR_TYPE tag;
    union {
      INNER_VAL inn;
      ARRAY arr;
    }
    val;
} EXPR_VAL;


#define EXPR_TYPE_IS(E, T)  ((E)->tag == (T))

// set expression value to given type and value
void set_float(EXPR_VAL * r, FLOAT val);
void set_point(EXPR_VAL * r, POINT_3D val);
void set_vector(EXPR_VAL * r, VECTOR_3D val);
void set_transform(EXPR_VAL * r, TRANSFORM val);
void set_array(EXPR_VAL * r, ARRAY val);

// coerce an expression value to given type 
// generate error message if it can't be done
void coerce_to_float(EXPR_VAL * r, FLOAT * val, SRC_LINE line);
void coerce_to_point(EXPR_VAL * r, POINT_3D val, SRC_LINE line);
void coerce_to_vector(EXPR_VAL * r, VECTOR_3D val, SRC_LINE line);
void coerce_to_transform(EXPR_VAL * r, TRANSFORM val, SRC_LINE line);

void copy_expr(EXPR_VAL * r, EXPR_VAL val);
void copy_array(ARRAY * r, ARRAY * val);
void link_array(ARRAY ** r, ARRAY * val);
ARRAY* new_array_from_element(EXPR_VAL val);
ARRAY* append_array_element(ARRAY * prev_array, EXPR_VAL new_element);
ARRAY* new_array_ranged(FLOAT ini, FLOAT incr, FLOAT fini);

void print_expr_val(FILE * f, EXPR_VAL * r);

void do_add(EXPR_VAL * r, EXPR_VAL * a, EXPR_VAL * b, SRC_LINE line);
void do_sub(EXPR_VAL * r, EXPR_VAL * a, EXPR_VAL * b, SRC_LINE line);
void do_mul(EXPR_VAL * r, EXPR_VAL * a, EXPR_VAL * b, SRC_LINE line);
void do_thn(EXPR_VAL * r, EXPR_VAL * a, EXPR_VAL * b, SRC_LINE line);
void do_dvd(EXPR_VAL * r, EXPR_VAL * a, EXPR_VAL * b, SRC_LINE line);
void do_dot(EXPR_VAL * r, EXPR_VAL * a, EXPR_VAL * b, SRC_LINE line);
void do_pwr(EXPR_VAL * r, EXPR_VAL * a, EXPR_VAL * b, SRC_LINE line);
void do_mag(EXPR_VAL * r, EXPR_VAL * a, SRC_LINE line);
void do_neg(EXPR_VAL * r, EXPR_VAL * a, SRC_LINE line);
void do_unit(EXPR_VAL * r, EXPR_VAL * a, SRC_LINE line);
void do_sqrt(EXPR_VAL * r, EXPR_VAL * a, SRC_LINE line);
void do_sin(EXPR_VAL * r, EXPR_VAL * a, SRC_LINE line);
void do_cos(EXPR_VAL * r, EXPR_VAL * a, SRC_LINE line);
void do_asin(EXPR_VAL * r, EXPR_VAL * a, SRC_LINE line);
void do_acos(EXPR_VAL * r, EXPR_VAL * a, SRC_LINE line);
void do_index(EXPR_VAL * r, EXPR_VAL * a, int index, SRC_LINE line);
void do_atan2(EXPR_VAL * r, EXPR_VAL * a, EXPR_VAL * b, SRC_LINE line);
void do_inverse(TRANSFORM inv, TRANSFORM xf, SRC_LINE line);

#endif
