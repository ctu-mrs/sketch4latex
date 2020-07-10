/* geometry.h
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

#ifndef __GEOMETRY_H
#define __GEOMETRY_H

// ---- memory -----------------------------------------------------------------

#include <float.h>		// floating point definitions
#include "dynarray.h"		// dynamic arrays

// bit N
#define bit(N)  (1u << (N))

// size of a static or auto declared array
#define ARRAY_SIZE(A) (sizeof (A) / sizeof (A)[0])

// checking memory allocators
void *safe_malloc(unsigned size);
void *safe_realloc(void *p, unsigned size);
char *safe_strdup(char *str);
void safe_free(void *p);
#ifdef _DEBUG
#define malloc(N)    __call_safe_malloc_instead()
#define realloc(P,N) __call_safe_alloc_instead()
#define strdup(S)    __call_safe_alloc_instead()
#define free(P)      __call_safe_free_instead()
#endif

// ---- numerics ---------------------------------------------------------------

// float declarations to ease compilation 
// with either single or double precision
typedef unsigned int SIZE, INDEX;
typedef double FLOAT;

#define FLOAT_SCAN_FMT "%lf"
#define FLOAT_EPS (8*DBL_EPSILON)
#define FLOAT_MIN FLT_MIN
#define FLOAT_MAX FLT_MAX

#ifdef _MSC_VER
// kill loss of precision warnings for case where FLOAT is float
#pragma warning(disable:4244 4305)
#endif

#define PI ((FLOAT)3.1415926535897932384626433832795028841971693993751)

// Max and min operators
FLOAT max_float(FLOAT x, FLOAT y);
FLOAT min_float(FLOAT x, FLOAT y);

// ---- points -----------------------------------------------------------------

// indices
#define X 0
#define Y 1
#define Z 2
#define W 3

// points
typedef FLOAT POINT_2D[2], POINT_3D[3];
void copy_pt_2d(POINT_2D r, POINT_2D s);
void copy_pt_3d(POINT_3D r, POINT_3D s);
void find_pt_3d_from_2d(POINT_3D r, POINT_2D pt);

// ---- polylines --------------------------------------------------------------

// polylines are just dynamic arrays of points

typedef struct polyline_2d_t {
    DYNAMIC_2D_ARRAY_FIELDS(POINT_2D, v, n_vertices);
    struct polyline_2d_t *next;
} POLYLINE_2D;

DECLARE_DYNAMIC_2D_ARRAY_PROTOS(POLYLINE_2D, POINT_2D, FLOAT, polyline_2d,
				v, n_vertices);
typedef struct polyline_3d_t {
    DYNAMIC_2D_ARRAY_FIELDS(POINT_3D, v, n_vertices);
    struct polyline_3d_t *next;
} POLYLINE_3D;

DECLARE_DYNAMIC_2D_ARRAY_PROTOS(POLYLINE_3D, POINT_3D, FLOAT, polyline_3d,
				v, n_vertices);
// ---- polygons ---------------------------------------------------------------
// polygons are just a dynamic arrays of points; chains represent complex polygons
typedef struct polygon_2d_t {
    DYNAMIC_2D_ARRAY_FIELDS(POINT_2D, v, n_sides);
    struct polygon_2d_t *next;
} POLYGON_2D;

DECLARE_DYNAMIC_2D_ARRAY_PROTOS(POLYGON_2D, POINT_2D, FLOAT, polygon_2d, v,
				n_sides);
typedef struct polygon_3d_t {
    DYNAMIC_2D_ARRAY_FIELDS(POINT_3D, v, n_sides);
    struct polygon_3d_t *next;
} POLYGON_3D;

DECLARE_DYNAMIC_2D_ARRAY_PROTOS(POLYGON_3D, POINT_3D, FLOAT, polygon_3d, v,
				n_sides);
// ---- vectors ----------------------------------------------------------------
typedef FLOAT *VECTOR;

// vectors of dynamic length
void init_vec(VECTOR * v);
void clear_vec(VECTOR * v);
void setup_vec(VECTOR * v, SIZE n);
void init_and_setup_vec(VECTOR * v, SIZE n);
void zero_vec(VECTOR r, SIZE n);
void copy_vec(VECTOR r, VECTOR v, SIZE n);

// vectors of useful static length.
typedef FLOAT VECTOR_2D[2], VECTOR_3D[3], VECTOR_4D[4];

FLOAT length_vec_2d(VECTOR_2D v);
FLOAT length_vec_3d(VECTOR_3D v);
FLOAT dist_2d(POINT_2D p1, POINT_2D p2);
FLOAT dist_3d(POINT_3D p1, POINT_3D p2);
FLOAT length_vec_2d_sqr(VECTOR_2D v);
FLOAT length_vec_3d_sqr(VECTOR_3D v);
FLOAT dist_2d_sqr(POINT_2D p1, POINT_2D p2);
FLOAT dist_3d_sqr(POINT_3D p1, POINT_3D p2);
void zero_vec_2d(VECTOR_2D v);
void zero_vec_3d(VECTOR_3D v);
void negate_vec_2d(VECTOR_2D r, VECTOR_2D v);
void negate_vec_3d(VECTOR_3D r, VECTOR_3D v);
void copy_vec_2d(VECTOR_2D r, VECTOR_2D s);
void copy_vec_3d(VECTOR_3D r, VECTOR_3D s);
void scale_vec_2d(VECTOR_2D r, VECTOR_2D v, FLOAT s);
void scale_vec_3d(VECTOR_3D r, VECTOR_3D v, FLOAT s);
int find_unit_vec_2d(VECTOR_2D r, VECTOR_2D v);
int find_unit_vec_3d(VECTOR_3D r, VECTOR_3D v);
void add_vecs_2d(VECTOR_2D r, VECTOR_2D a, VECTOR_2D b);
void add_vecs_3d(VECTOR_3D r, VECTOR_3D a, VECTOR_3D b);
void sub_vecs_2d(VECTOR_2D r, VECTOR_2D a, VECTOR_2D b);
void sub_vecs_3d(VECTOR_3D r, VECTOR_3D a, VECTOR_3D b);
void add_vec_to_pt_2d(POINT_2D r, POINT_2D pt, VECTOR_2D v);
void add_vec_to_pt_3d(POINT_3D r, POINT_3D pt, VECTOR_3D v);
void add_scaled_vec_to_pt_2d(POINT_2D r, POINT_2D pt, VECTOR_2D v,
			     FLOAT s);
void add_scaled_vec_to_pt_3d(POINT_3D r, POINT_3D pt, VECTOR_3D v,
			     FLOAT s);
void sub_pts_2d(VECTOR_2D r, POINT_2D a, POINT_2D b);
void sub_pts_3d(VECTOR_3D r, POINT_3D a, POINT_3D b);
void fold_min_pt_2d(POINT_2D min, POINT_2D new_pt);
void fold_min_pt_3d(POINT_3D min, POINT_3D new_pt);
void fold_max_pt_2d(POINT_2D max, POINT_2D new_pt);
void fold_max_pt_3d(POINT_3D max, POINT_3D new_pt);

FLOAT dot_2d(VECTOR_2D a, VECTOR_2D b);
FLOAT dot_3d(VECTOR_3D a, VECTOR_3D b);
void cross(VECTOR_3D r, VECTOR_3D a, VECTOR_3D b);

// linear interpolation operators
void lerp_2d(POINT_2D r, FLOAT t, POINT_2D p1, POINT_2D p2);
void lerp_3d(POINT_3D r, FLOAT t, POINT_3D p1, POINT_3D p2);

// find parameters of intersection point of two line segments
int line_intersect_2d(POINT_2D a, POINT_2D b, POINT_2D c, POINT_2D d,
		      FLOAT eps, FLOAT * t_ab, FLOAT * t_cd);

// ---- planes -----------------------------------------------------------------
typedef struct plane_t {
    VECTOR_3D n;
    POINT_3D p;
    FLOAT c;
} PLANE;

// return description of the plane of a polygon using Newell's method
void find_polygon_plane(PLANE * plane, POLYGON_3D * polygon);

#define S_IN      (1)
#define S_ON      (2)
#define S_OUT     (4)
#define S_IN_ON   (S_ON | 8)
#define S_OUT_ON  (S_ON | 16)
#define S_SPLIT   (32)

// #define PLANE_HALF_THICKNESS  (10.0 * FLOAT_EPS)
#define PLANE_HALF_THICKNESS  (.001/2)

// given a plane of thickness 2 * half_thickness, return:
//   S_IN or S_OUT if the point is resp. inside or outside the thickness of the plane
//   S_IN_ON or S_OUT_ON if the point is within half_thickness of the plane on the resp. side
//   S_ON if the point is precisely on the plane; no IN or OUT determination can be made
int pt_side_of_plane(PLANE * plane, POINT_3D p);

// given a polygon and a plane, return:
//   S_IN if all the verices are IN or ON the thickened plane
//   S_OUT if all the verices are OUTside or ON the thickened plane
//   S_ON if all vertice are ON the thickened plane
//   S_SPLIT otherwise
int polygon_side_of_plane(POLYGON_3D * polygon, PLANE * plane);

// given a polyline and a plane, return:
//   S_IN if all segments of the line are fully INside the thickened plane
//   S_OUT if all segments of the line are fully OUTside the thickened plane
//   S_ON if all vertice are ON the thickened plane
//   S_SPLIT otherwise
int polyline_side_of_plane(POLYLINE_3D * polyline, PLANE * plane);

// ---- boxes ------------------------------------------------------------------

typedef struct box_2d_t {
    POINT_2D min, max;
} BOX_2D;

typedef struct box_3d_t {
    POINT_3D min, max;
} BOX_3D;

void init_box_2d(BOX_2D * b);
void init_box_3d(BOX_3D * b);
void fold_min_max_pt_2d(BOX_2D * b, POINT_2D p);
void fold_min_max_pt_3d(BOX_3D * b, POINT_3D p);
void fold_min_max_polygon_2d(BOX_2D * b, POLYGON_2D * polygon);
void fold_min_max_polygon_3d(BOX_3D * b, POLYGON_3D * polygon);
void fold_min_max_polyline_2d(BOX_2D * b, POLYLINE_2D * polyline);
void fold_min_max_polyline_3d(BOX_3D * b, POLYLINE_3D * polyline);
void copy_box_2d(BOX_2D * r, BOX_2D * s);
void copy_box_3d(BOX_3D * r, BOX_3D * s);
int boxes_2d_intersect_p(BOX_2D * a, BOX_2D * b);
int boxes_3d_intersect_p(BOX_2D * a, BOX_2D * b);

// ---- transformations --------------------------------------------------------

// homogeneous transform stored in column major order
typedef FLOAT TRANSFORM[16];

// for initializations of identity transforms
#define IDENT_TRANSFORM  \
{ 1.0, 0.0, 0.0, 0.0, \
  0.0, 1.0, 0.0, 0.0, \
  0.0, 0.0, 1.0, 0.0, \
  0.0, 0.0, 0.0, 1.0 }

// ---- global contstants ------------------------------------------------------

extern TRANSFORM identity;
extern POINT_2D origin_2d;
extern POINT_3D origin_3d;
extern VECTOR_2D I_2d;
extern VECTOR_2D J_2d;
extern VECTOR_3D I_3d;
extern VECTOR_3D J_3d;
extern VECTOR_3D K_3d;

// row-column tranform indexing matches OpenGL convention: column major
#define IT(I,J) (4 * ((J) - 1) + ((I) - 1))

// copy source to result transform
void copy_transform(TRANSFORM r, TRANSFORM s);

// set the result transform to the identity
void set_ident(TRANSFORM r);

// create a rotation transform thru angle theta about axis u (must be unit vec)
void set_angle_axis_rot(TRANSFORM r, FLOAT theta, VECTOR_3D u);

// create a rotation transform thru angle theta
// u is optional axis which need not be a unit vector (default is [0,0,1])
// p is optional center of rotation (default is (0,0,0))
void set_angle_axis_rot_about_point(TRANSFORM r, FLOAT theta,
				    POINT_3D p, VECTOR_3D u);

// create a scale transform
void set_scale(TRANSFORM r, FLOAT sx, FLOAT sy, FLOAT sz);

// create a translation transform
void set_translation(TRANSFORM r, FLOAT dx, FLOAT dy, FLOAT dz);

// create a true perspective projection (depth = p for all projected points)
void set_perspective_projection(TRANSFORM r, FLOAT p);

// create a perspective transformation (depth is a pseudodepth)
void set_perspective_transform(TRANSFORM r, FLOAT p);

// create a true parallel projection (depth = 0 for all projected points)
void set_parallel_projection(TRANSFORM r);

// create an OpenGL-like view transformation matrix
void set_view_transform(TRANSFORM r, POINT_3D eye, VECTOR_3D vd,
			VECTOR_3D up);
void set_view_transform_with_look_at(TRANSFORM r, POINT_3D eye,
				     POINT_3D look_at, VECTOR_3D up);

// invert a given transform m; return its determinant; we give up if the 
// determinant is too small
void invert(TRANSFORM r, FLOAT * det_rtn, TRANSFORM m, FLOAT min_det);

// compose two transforms, but result cannot be the same as either operand
void compose_unsafe(TRANSFORM r, TRANSFORM a, TRANSFORM b);

// same as above, but safe to use either operand to hold result.
void compose(TRANSFORM r, TRANSFORM a, TRANSFORM b);

void transform_pt_3d(POINT_3D r, TRANSFORM m, POINT_3D p);
void transform_vec_3d(VECTOR_3D r, TRANSFORM m, VECTOR_3D p);

// ---- quaternions ------------------------------------------------------------

typedef FLOAT QUATERNION[4];

// for initializations of identity quaternions
#define IDENT_QUAT { 0.0, 0.0, 0.0, 1.0 }

void set_ident_quat(QUATERNION q);
void set_angle_axis_quat(QUATERNION q, FLOAT theta, VECTOR_3D axis);
void find_rot_from_quat(TRANSFORM r, QUATERNION q);
void find_quat_from_rot(QUATERNION q, TRANSFORM r);
void mult_quat(QUATERNION r, QUATERNION a, QUATERNION b);

// clear any storage for vertices in a polygon; after this,
// its state is the same as after init_polygon_2d()
void clear_polygon_2d(POLYGON_2D * poly);

// compute minkowski difference B - A with distinguished point p
void make_cso_polygon_2d(POLYGON_2D * r, POLYGON_2D * a, POINT_2D p,
			 POLYGON_2D * b);

// checks to see if p is left of or on all the edges of polygon a.
int point_inside_convex_polygon_2d_p(POINT_2D p, POLYGON_2D * a);

// checks to see if p is no more than eps right of all the edges of polygon a.
int point_near_convex_polygon_2d_p(POINT_2D p, POLYGON_2D * a, FLOAT eps);

#endif
