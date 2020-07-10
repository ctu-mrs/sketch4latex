/* geometry.c
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
#include <string.h>
#include <math.h>
#include "geometry.h"
#include "error.h"
#include "memutil.h"

// global constants 
POINT_2D origin_2d = { 0, 0 };
POINT_3D origin_3d = { 0, 0, 0 };
VECTOR_2D I_2d = { 1, 0 };
VECTOR_2D J_2d = { 0, 1 };
VECTOR_3D I_3d = { 1, 0, 0 };
VECTOR_3D J_3d = { 0, 1, 0 };
VECTOR_3D K_3d = { 0, 0, 1 };

TRANSFORM identity = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

// numerics 

FLOAT max_float(FLOAT x, FLOAT y)
{
    return x > y ? x : y;
}

FLOAT min_float(FLOAT x, FLOAT y)
{
    return x < y ? x : y;
}

// points

void copy_pt_2d(POINT_2D r, POINT_2D s)
{
    r[X] = s[X];
    r[Y] = s[Y];
}

void copy_pt_3d(POINT_3D r, POINT_3D s)
{
    r[X] = s[X];
    r[Y] = s[Y];
    r[Z] = s[Z];
}

void find_pt_3d_from_2d(POINT_3D r, POINT_2D pt)
{
    r[X] = pt[X];
    r[Y] = pt[Y];
    r[Z] = 0;
}

// polyline initialization and cleanup

#define SET_NEXT_NULL a->next = NULL;

DECLARE_DYNAMIC_2D_ARRAY_FUNCS(POLYLINE_2D, POINT_2D, FLOAT, polyline_2d,
			       v, n_vertices, SET_NEXT_NULL);
DECLARE_DYNAMIC_2D_ARRAY_FUNCS(POLYLINE_3D, POINT_3D, FLOAT, polyline_3d,
			       v, n_vertices, SET_NEXT_NULL);
// polygon initialization and cleanup
DECLARE_DYNAMIC_2D_ARRAY_FUNCS(POLYGON_2D, POINT_2D, FLOAT, polygon_2d, v,
			       n_sides, SET_NEXT_NULL);
DECLARE_DYNAMIC_2D_ARRAY_FUNCS(POLYGON_3D, POINT_3D, FLOAT, polygon_3d, v,
			       n_sides, SET_NEXT_NULL);

// rudimentary vectors of variable size
void init_vec(VECTOR * v)
{
    *v = 0;
}

void clear_vec(VECTOR * v)
{
    safe_free(*v);
    init_vec(v);
}

void setup_vec(VECTOR * v, SIZE n)
{
    clear_vec(v);
    *v = safe_malloc(n * sizeof(FLOAT));
}

void init_and_setup_vec(VECTOR * v, SIZE n)
{
    *v = safe_malloc(n * sizeof(FLOAT));
}

void zero_vec(VECTOR r, SIZE n)
{
    INDEX i;

    for (i = 0; i < n; i++)
	r[i] = 0;
}

void copy_vec(VECTOR r, VECTOR v, SIZE n)
{
    INDEX i;

    for (i = 0; i < n; i++)
	r[i] = v[i];
}

FLOAT length_vec_2d(VECTOR_2D v)
{
    return sqrt(dot_2d(v, v));
}

FLOAT length_vec_3d(VECTOR_3D v)
{
    return sqrt(dot_3d(v, v));
}

FLOAT dist_2d(POINT_2D p1, POINT_2D p2)
{
    VECTOR_2D dif;
    sub_pts_2d(dif, p1, p2);
    return length_vec_2d(dif);
}

FLOAT dist_3d(POINT_3D p1, POINT_3D p2)
{
    VECTOR_3D dif;
    sub_pts_3d(dif, p1, p2);
    return length_vec_3d(dif);
}

FLOAT length_vec_2d_sqr(VECTOR_2D v)
{
    return dot_2d(v, v);
}

FLOAT length_vec_3d_sqr(VECTOR_3D v)
{
    return dot_3d(v, v);
}

FLOAT dist_2d_sqr(POINT_2D p1, POINT_2D p2)
{
    VECTOR_2D dif;
    sub_pts_2d(dif, p1, p2);
    return length_vec_2d_sqr(dif);
}

FLOAT dist_3d_sqr(POINT_3D p1, POINT_3D p2)
{
    VECTOR_3D dif;
    sub_pts_3d(dif, p1, p2);
    return length_vec_3d_sqr(dif);
}

void zero_vec_2d(VECTOR_2D v)
{
    v[X] = v[Y] = 0;
}

void zero_vec_3d(VECTOR_3D v)
{
    v[X] = v[Y] = v[Z] = 0;
}

void negate_vec_2d(VECTOR_2D r, VECTOR_2D v)
{
    r[X] = -v[X];
    r[Y] = -v[Y];
}

void negate_vec_3d(VECTOR_3D r, VECTOR_3D v)
{
    r[X] = -v[X];
    r[Y] = -v[Y];
    r[Z] = -v[Z];
}

void copy_vec_2d(VECTOR_2D r, VECTOR_2D s)
{
    r[X] = s[X];
    r[Y] = s[Y];
}

void copy_vec_3d(VECTOR_3D r, VECTOR_3D s)
{
    r[X] = s[X];
    r[Y] = s[Y];
    r[Z] = s[Z];
}

void scale_vec_2d(VECTOR_2D r, VECTOR_2D v, FLOAT s)
{
    r[X] = v[X] * s;
    r[Y] = v[Y] * s;
}

void scale_vec_3d(VECTOR_3D r, VECTOR_3D v, FLOAT s)
{
    r[X] = v[X] * s;
    r[Y] = v[Y] * s;
    r[Z] = v[Z] * s;
}

int find_unit_vec_2d(VECTOR_2D r, VECTOR_2D v)
{
    FLOAT len = length_vec_2d(v);
    if (len <= FLT_EPSILON) {
	r[X] = 1;
	r[Y] = 0;
	return 0;
    } else {
	scale_vec_2d(r, v, 1 / len);
	return 1;
    }
}

int find_unit_vec_3d(VECTOR_3D r, VECTOR_3D v)
{
    FLOAT len = length_vec_3d(v);
    if (len == FLT_EPSILON) {
	r[X] = 1;
	r[Y] = r[Z] = 0;
	return 0;
    } else {
	scale_vec_3d(r, v, 1 / len);
	return 1;
    }
}

void add_vecs_2d(VECTOR_2D r, VECTOR_2D a, VECTOR_2D b)
{
    r[X] = a[X] + b[X];
    r[Y] = a[Y] + b[Y];
}

void add_vecs_3d(VECTOR_3D r, VECTOR_3D a, VECTOR_3D b)
{
    r[X] = a[X] + b[X];
    r[Y] = a[Y] + b[Y];
    r[Z] = a[Z] + b[Z];
}

void sub_vecs_2d(VECTOR_2D r, VECTOR_2D a, VECTOR_2D b)
{
    r[X] = a[X] - b[X];
    r[Y] = a[Y] - b[Y];
}

void sub_vecs_3d(VECTOR_3D r, VECTOR_3D a, VECTOR_3D b)
{
    r[X] = a[X] - b[X];
    r[Y] = a[Y] - b[Y];
    r[Z] = a[Z] - b[Z];
}

void add_vec_to_pt_2d(POINT_2D r, POINT_2D pt, VECTOR_2D v)
{
    r[X] = pt[X] + v[X];
    r[Y] = pt[Y] + v[Y];
}

void add_vec_to_pt_3d(POINT_3D r, POINT_3D pt, VECTOR_3D v)
{
    r[X] = pt[X] + v[X];
    r[Y] = pt[Y] + v[Y];
    r[Z] = pt[Z] + v[Z];
}

void add_scaled_vec_to_pt_2d(POINT_2D r, POINT_2D pt, VECTOR_2D v, FLOAT s)
{
    r[X] = pt[X] + v[X] * s;
    r[Y] = pt[Y] + v[Y] * s;
}

void add_scaled_vec_to_pt_3d(POINT_3D r, POINT_3D pt, VECTOR_3D v, FLOAT s)
{
    r[X] = pt[X] + v[X] * s;
    r[Y] = pt[Y] + v[Y] * s;
    r[Z] = pt[Z] + v[Z] * s;
}

void sub_pts_2d(VECTOR_2D r, POINT_2D a, POINT_2D b)
{
    r[X] = a[X] - b[X];
    r[Y] = a[Y] - b[Y];
}

void sub_pts_3d(VECTOR_3D r, POINT_3D a, POINT_3D b)
{
    r[X] = a[X] - b[X];
    r[Y] = a[Y] - b[Y];
    r[Z] = a[Z] - b[Z];
}

void fold_min_pt_2d(POINT_2D min, POINT_2D new_pt)
{
    int i;

    for (i = 0; i < 2; i++)
	if (new_pt[i] < min[i])
	    min[i] = new_pt[i];
}

void fold_min_pt_3d(POINT_3D min, POINT_3D new_pt)
{
    int i;

    for (i = 0; i < 3; i++)
	if (new_pt[i] < min[i])
	    min[i] = new_pt[i];
}

void fold_max_pt_2d(POINT_2D max, POINT_3D new_pt)
{
    int i;

    for (i = 0; i < 2; i++)
	if (new_pt[i] > max[i])
	    max[i] = new_pt[i];
}

void fold_max_pt_3d(POINT_3D max, POINT_3D new_pt)
{
    int i;

    for (i = 0; i < 3; i++)
	if (new_pt[i] > max[i])
	    max[i] = new_pt[i];
}

FLOAT dot_2d(VECTOR_2D a, VECTOR_2D b)
{
    return a[X] * b[X] + a[Y] * b[Y];
}

FLOAT dot_3d(VECTOR_3D a, VECTOR_3D b)
{
    return a[X] * b[X] + a[Y] * b[Y] + a[Z] * b[Z];
}

void cross(VECTOR_3D r, VECTOR_3D a, VECTOR_3D b)
{
    r[X] = a[Y] * b[Z] - a[Z] * b[Y];
    r[Y] = a[Z] * b[X] - a[X] * b[Z];
    r[Z] = a[X] * b[Y] - a[Y] * b[X];
}

void lerp_2d(POINT_2D r, FLOAT t, POINT_2D p1, POINT_2D p2)
{
    r[0] = p1[0] + t * (p2[0] - p1[0]);
    r[1] = p1[1] + t * (p2[1] - p1[1]);
}

void lerp_3d(POINT_3D r, FLOAT t, POINT_3D p1, POINT_3D p2)
{
    r[0] = p1[0] + t * (p2[0] - p1[0]);
    r[1] = p1[1] + t * (p2[1] - p1[1]);
    r[2] = p1[2] + t * (p2[2] - p1[2]);
}

int
line_intersect_2d(POINT_2D a, POINT_2D b, POINT_2D c, POINT_2D d,
		  FLOAT eps, FLOAT * t_ab, FLOAT * t_cd)
{
    FLOAT dx_ab, dy_ab, dx_dc, dy_dc, det, dx_ac, dy_ac;

    dx_ab = b[X] - a[X];
    dy_ab = b[Y] - a[Y];
    dx_dc = c[X] - d[X];
    dy_dc = c[Y] - d[Y];
    det = dx_ab * dy_dc - dx_dc * dy_ab;
    if (-eps < det && det < eps)
	return 1;
    dx_ac = c[X] - a[X];
    dy_ac = c[Y] - a[Y];
    *t_ab = (dx_ac * dy_dc - dx_dc * dy_ac) / det;
    *t_cd = (dx_ab * dy_ac - dx_ac * dy_ab) / det;
    return 0;
}

void find_polygon_plane(PLANE * plane, POLYGON_3D * polygon)
{
    int i, j;
    VECTOR_3D sum, dif;

    zero_vec_3d(plane->p);
    zero_vec_3d(plane->n);
    for (i = 0, j = polygon->n_sides - 1; i < polygon->n_sides; j = i++) {
	add_vecs_3d(plane->p, plane->p, polygon->v[i]);
	add_vecs_3d(sum, polygon->v[j], polygon->v[i]);
	sub_vecs_3d(dif, polygon->v[j], polygon->v[i]);
	plane->n[X] += dif[Y] * sum[Z];
	plane->n[Y] += dif[Z] * sum[X];
	plane->n[Z] += dif[X] * sum[Y];
    }
    scale_vec_3d(plane->p, plane->p, 1.0 / polygon->n_sides);
    find_unit_vec_3d(plane->n, plane->n);
    plane->c = -dot_3d(plane->p, plane->n);
}

int pt_side_of_plane(PLANE * plane, POINT_3D p)
{
    FLOAT d = dot_3d(p, plane->n) + plane->c;
    return d < -PLANE_HALF_THICKNESS ? S_IN :
	d > PLANE_HALF_THICKNESS ? S_OUT :
	d < 0 ? S_IN_ON : d > 0 ? S_OUT_ON : S_ON;
}

int polygon_side_of_plane(POLYGON_3D * polygon, PLANE * plane)
{
    int i, j, i_side, j_side, n_in, n_out;

    // initialize with last point in polygon
    // scan for OUT-IN or IN-OUT pair
    j = polygon->n_sides - 1;
    j_side = pt_side_of_plane(plane, polygon->v[j]);
    n_in = n_out = 0;
    for (i = 0; i < polygon->n_sides; i++) {

	// advance to next vertex
	i_side = pt_side_of_plane(plane, polygon->v[i]);

	if ((i_side | j_side) == (S_IN | S_OUT))
	    // found a straddling pair
	    return S_SPLIT;

	if (i_side & (S_IN | S_OUT))
	    // found an IN or an OUT; remember it
	    j_side = i_side;

	// keep counts for polygons entirely inside the thick plane
	if (i_side == S_OUT_ON)
	    n_out++;
	if (i_side == S_IN_ON)
	    n_in++;
    }
    return
	j_side & (S_IN | S_OUT) ? j_side :
	(n_out > n_in) ? S_OUT : (n_in > n_out) ? S_IN : S_ON;
}

#if TREAT_POLYLINE_POINTS_ON_PLANE_AS_IN_OR_OUT

// this will work only with BSPs, not with depth sort
// it causes polylines that end  on a plane to be split into a line and a point
int polyline_side_of_plane(POLYLINE_3D * polyline, PLANE * plane)
{
    int i, j, i_side, j_side, n_in, n_out;
    //  predicate for "if more than one bit set..."
    //                       0  1  2  3  4  5  6  7
    static int is_split_p[] = { 0, 0, 0, 1, 0, 1, 1, 1 };

    // initialize with first point in polyline
    // scan for OUT-IN or IN-OUT pair
    j = 0;
    i_side = pt_side_of_plane(plane, polyline->v[j]);
    n_in = n_out = 0;
    for (i = 1; i < polyline->n_vertices; i++) {
	// advance to next vertex, remembering side of last
	j_side = i_side;
	i_side = pt_side_of_plane(plane, polyline->v[i]);

	if (is_split_p[(i_side | j_side) & 7])
	    return S_SPLIT;

	// keep counts for polylines entirely inside the thick plane
	if (i_side == S_OUT_ON)
	    n_out++;
	if (i_side == S_IN_ON)
	    n_in++;
    }
    return
	i_side & (S_IN | S_OUT) ? i_side :
	(n_out > n_in) ? S_OUT : (n_in > n_out) ? S_IN : S_ON;
}

#else

int polyline_side_of_plane(POLYLINE_3D * polyline, PLANE * plane)
{
    int i, j, i_side, j_side, n_in, n_out;

    // initialize with last point in polygon
    // scan for OUT-IN or IN-OUT pair
    j = polyline->n_vertices - 1;
    j_side = pt_side_of_plane(plane, polyline->v[j]);
    n_in = n_out = 0;
    for (i = 0; i < polyline->n_vertices; i++) {

	// advance to next vertex
	i_side = pt_side_of_plane(plane, polyline->v[i]);

	if ((i_side | j_side) == (S_IN | S_OUT))
	    // found a straddling pair
	    return S_SPLIT;

	if (i_side & (S_IN | S_OUT))
	    // found an IN or an OUT; remember it
	    j_side = i_side;

	// keep counts for polylines entirely inside the thick plane
	if (i_side == S_OUT_ON)
	    n_out++;
	if (i_side == S_IN_ON)
	    n_in++;
    }
    return
	j_side & (S_IN | S_OUT) ? j_side :
	(n_out > n_in) ? S_OUT : (n_in > n_out) ? S_IN : S_ON;
}

#endif

void init_box_2d(BOX_2D * b)
{
    b->min[X] = b->min[Y] = FLOAT_MAX;
    b->max[X] = b->max[Y] = -FLOAT_MAX;
}

void init_box_3d(BOX_3D * b)
{
    b->min[X] = b->min[Y] = b->min[Z] = FLOAT_MAX;
    b->max[X] = b->max[Y] = b->max[Z] = -FLOAT_MAX;
}

void fold_min_max_pt_2d(BOX_2D * b, POINT_2D p)
{
    fold_min_pt_2d(b->min, p);
    fold_max_pt_2d(b->max, p);
}

void fold_min_max_pt_3d(BOX_3D * b, POINT_3D p)
{
    fold_min_pt_3d(b->min, p);
    fold_max_pt_3d(b->max, p);
}

void fold_min_max_polygon_2d(BOX_2D * b, POLYGON_2D * polygon)
{
    int i;

    for (i = 0; i < polygon->n_sides; i++)
	fold_min_max_pt_2d(b, polygon->v[i]);
}

void fold_min_max_polygon_3d(BOX_3D * b, POLYGON_3D * polygon)
{
    int i;

    for (i = 0; i < polygon->n_sides; i++)
	fold_min_max_pt_3d(b, polygon->v[i]);
}

void fold_min_max_polyline_2d(BOX_2D * b, POLYLINE_2D * polyline)
{
    int i;

    for (i = 0; i < polyline->n_vertices; i++)
	fold_min_max_pt_2d(b, polyline->v[i]);
}

void fold_min_max_polyline_3d(BOX_3D * b, POLYLINE_3D * polyline)
{
    int i;

    for (i = 0; i < polyline->n_vertices; i++)
	fold_min_max_pt_3d(b, polyline->v[i]);
}

void copy_box_2d(BOX_2D * r, BOX_2D * s)
{
    *r = *s;
}

void copy_box_3d(BOX_3D * r, BOX_3D * s)
{
    *r = *s;
}

int boxes_2d_intersect_p(BOX_2D * a, BOX_2D * b)
{
    if (a->max[X] < b->min[X])	// a left of b
	return 0;
    if (a->min[X] > b->max[X])	// a right of b
	return 0;
    if (a->max[Y] < b->min[Y])	// a below  b
	return 0;
    if (a->min[Y] > b->max[Y])	// a above b
	return 0;
    return 1;
}

int boxes_3d_intersect_p(BOX_2D * a, BOX_2D * b)
{
    if (a->max[X] < b->min[X])	// a left of b
	return 0;
    if (a->min[X] > b->max[X])	// a right of b
	return 0;
    if (a->max[Y] < b->min[Y])	// a below  b
	return 0;
    if (a->min[Y] > b->max[Y])	// a above b
	return 0;
    if (a->max[Z] < b->min[Z])	// a behind  b
	return 0;
    if (a->min[Z] > b->max[Z])	// a in front of b
	return 0;
    return 1;
}

void copy_transform(TRANSFORM r, TRANSFORM s)
{
    int i;

    for (i = 0; i < 16; i++)
	r[i] = s[i];
}

#define R(I,J) r[IT(I,J)]

void set_ident(TRANSFORM r)
{
    R(1, 1) = 1;		// hard code for speed
    R(2, 1) = 0;
    R(3, 1) = 0;
    R(4, 1) = 0;

    R(1, 2) = 0;
    R(2, 2) = 1;
    R(3, 2) = 0;
    R(4, 2) = 0;

    R(1, 3) = 0;
    R(2, 3) = 0;
    R(3, 3) = 1;
    R(4, 3) = 0;

    R(1, 4) = 0;
    R(2, 4) = 0;
    R(3, 4) = 0;
    R(4, 4) = 1;
}

void set_scale(TRANSFORM r, FLOAT sx, FLOAT sy, FLOAT sz)
{
    set_ident(r);
    R(1, 1) = sx;
    R(2, 2) = sy;
    R(3, 3) = sz;
}

void set_translation(TRANSFORM r, FLOAT dx, FLOAT dy, FLOAT dz)
{
    set_ident(r);
    R(1, 4) = dx;
    R(2, 4) = dy;
    R(3, 4) = dz;
}

#define SQR(A) ((A) * (A))

void set_angle_axis_rot(TRANSFORM r, FLOAT theta, VECTOR_3D u)
{
    FLOAT c = cos(theta);
    FLOAT s = sin(theta);
    FLOAT d = 1 - c;

    R(1, 1) = d * (SQR(u[X]) - 1) + 1;
    R(1, 2) = d * u[X] * u[Y] - u[Z] * s;
    R(1, 3) = d * u[X] * u[Z] + u[Y] * s;

    R(2, 1) = d * u[X] * u[Y] + u[Z] * s;
    R(2, 2) = d * (SQR(u[Y]) - 1) + 1;
    R(2, 3) = d * u[Y] * u[Z] - u[X] * s;

    R(3, 1) = d * u[X] * u[Z] - u[Y] * s;
    R(3, 2) = d * u[Y] * u[Z] + u[X] * s;
    R(3, 3) = d * (SQR(u[Z]) - 1) + 1;

    R(1, 4) = R(4, 1) = R(2, 4) = R(4, 2) = R(3, 4) = R(4, 3) = 0;
    R(4, 4) = 1;
}

void
set_angle_axis_rot_about_point(TRANSFORM r, FLOAT theta, POINT_3D p,
			       VECTOR_3D u)
{
    VECTOR_3D u_unit;
    TRANSFORM tmp;

    if (u) {
	find_unit_vec_3d(u_unit, u);
    } else {
	u_unit[X] = u_unit[Y] = 0;
	u_unit[Z] = 1;
    }
    set_angle_axis_rot(r, theta, u_unit);
    if (p) {
	set_translation(tmp, -p[X], -p[Y], -p[Z]);
	compose(r, r, tmp);
	set_translation(tmp, p[X], p[Y], p[Z]);
	compose(r, tmp, r);
    }
}

void set_perspective_projection(TRANSFORM r, FLOAT p)
{
    set_scale(r, p, p, p);
    R(4, 4) = 0;
    R(4, 3) = -1;
}

void set_perspective_transform(TRANSFORM r, FLOAT p)
{
    set_scale(r, p, p, 1);
    R(3, 4) = 1;
    R(4, 3) = -1;
    R(4, 4) = 0;
}

void set_parallel_projection(TRANSFORM r)
{
    set_scale(r, 1, 1, 0);
}

void
set_view_transform(TRANSFORM r, POINT_3D eye, VECTOR_3D vd, VECTOR_3D up)
{
    static VECTOR_3D default_up = { 0, 1, 0 };

    VECTOR_3D unit_vd, unit_up, h, v;
    TRANSFORM tmp;

    if (vd) {
	find_unit_vec_3d(unit_vd, vd);
    } else {
	negate_vec_3d(unit_vd, eye);	// assumes point and vector are compatible
	find_unit_vec_3d(unit_vd, unit_vd);
    }

    if (up)
	find_unit_vec_3d(unit_up, up);
    else
	copy_vec_3d(unit_up, default_up);

    cross(h, unit_vd, unit_up);
    cross(v, h, unit_vd);

    R(1, 1) = h[X];
    R(1, 2) = h[Y];
    R(1, 3) = h[Z];
    R(1, 4) = 0;
    R(2, 1) = v[X];
    R(2, 2) = v[Y];
    R(2, 3) = v[Z];
    R(2, 4) = 0;
    R(3, 1) = -unit_vd[X];
    R(3, 2) = -unit_vd[Y];
    R(3, 3) = -unit_vd[Z];
    R(3, 4) = 0;
    R(4, 1) = 0;
    R(4, 2) = 0;
    R(4, 3) = 0;
    R(4, 4) = 1;

    if (eye) {
	set_translation(tmp, -eye[X], -eye[Y], -eye[Z]);
	compose(r, r, tmp);
    }
}

void
set_view_transform_with_look_at(TRANSFORM r, POINT_3D eye,
				POINT_3D look_at, VECTOR_3D up)
{
    VECTOR_3D vd;
    sub_vecs_3d(vd, look_at, eye);
    set_view_transform(r, eye, vd, up);
}

#define M(I,J) m[IT(I,J)]

// invert a transform using the method of cofactors
// this code was generated by the Perl program geninv.pl
void invert(TRANSFORM r, FLOAT * det_rtn, TRANSFORM m, FLOAT min_det)
{
    int i;
    FLOAT det;
    FLOAT t001, t002, t003, t004, t005, t006, t007, t008,
	t009, t010, t011, t012, t013, t014, t015, t016,
	t017, t018, t019, t020, t021, t022, t023, t024,
	t025, t026, t027, t028, t029, t030, t031, t032,
	t033, t034, t035, t036, t037, t038, t039, t040,
	t057, t058, t061, t062, t065, t066, t072, t073,
	t076, t077, t085, t086, t097, t098, t101, t102,
	t105, t106, t112, t113, t116, t117, t125, t126;
    t001 = M(3, 3) * M(4, 4);
    t002 = M(3, 4) * M(4, 3);
    t003 = t001 - t002;
    t004 = M(2, 2) * t003;
    t005 = M(3, 2) * M(4, 4);
    t006 = M(3, 4) * M(4, 2);
    t007 = t006 - t005;
    t008 = M(2, 3) * t007;
    t009 = M(3, 2) * M(4, 3);
    t010 = M(3, 3) * M(4, 2);
    t011 = t009 - t010;
    t012 = M(2, 4) * t011;
    t013 = t004 + t008 + t012;
    R(1, 1) = t013;
    t014 = t002 - t001;
    t015 = M(2, 1) * t014;
    t016 = M(3, 1) * M(4, 4);
    t017 = M(3, 4) * M(4, 1);
    t018 = t016 - t017;
    t019 = M(2, 3) * t018;
    t020 = M(3, 1) * M(4, 3);
    t021 = M(3, 3) * M(4, 1);
    t022 = t021 - t020;
    t023 = M(2, 4) * t022;
    t024 = t015 + t019 + t023;
    R(2, 1) = t024;
    t025 = t005 - t006;
    t026 = M(2, 1) * t025;
    t027 = t017 - t016;
    t028 = M(2, 2) * t027;
    t029 = M(3, 1) * M(4, 2);
    t030 = M(3, 2) * M(4, 1);
    t031 = t029 - t030;
    t032 = M(2, 4) * t031;
    t033 = t026 + t028 + t032;
    R(3, 1) = t033;
    t034 = t010 - t009;
    t035 = M(2, 1) * t034;
    t036 = t020 - t021;
    t037 = M(2, 2) * t036;
    t038 = t030 - t029;
    t039 = M(2, 3) * t038;
    t040 = t035 + t037 + t039;
    R(4, 1) = t040;
    det =
	(M(1, 1) * t013) + (M(1, 2) * t024) + (M(1, 3) * t033) +
	(M(1, 4) * t040);
    R(1, 2) = (M(1, 2) * t014) + (M(1, 3) * t025) + (M(1, 4) * t034);
    R(2, 2) = (M(1, 1) * t003) + (M(1, 3) * t027) + (M(1, 4) * t036);
    R(3, 2) = (M(1, 1) * t007) + (M(1, 2) * t018) + (M(1, 4) * t038);
    R(4, 2) = (M(1, 1) * t011) + (M(1, 2) * t022) + (M(1, 3) * t031);
    t057 = M(2, 3) * M(4, 4);
    t058 = M(2, 4) * M(4, 3);
    t061 = M(2, 2) * M(4, 4);
    t062 = M(2, 4) * M(4, 2);
    t065 = M(2, 2) * M(4, 3);
    t066 = M(2, 3) * M(4, 2);
    R(1, 3) =
	((t057 - t058) * M(1, 2)) + ((t062 - t061) * M(1,
						       3)) + ((t065 -
							       t066) * M(1,
									 4));
    t072 = M(2, 1) * M(4, 4);
    t073 = M(2, 4) * M(4, 1);
    t076 = M(2, 1) * M(4, 3);
    t077 = M(2, 3) * M(4, 1);
    R(2, 3) =
	((t058 - t057) * M(1, 1)) + ((t072 - t073) * M(1,
						       3)) + ((t077 -
							       t076) * M(1,
									 4));
    t085 = M(2, 1) * M(4, 2);
    t086 = M(2, 2) * M(4, 1);
    R(3, 3) =
	((t061 - t062) * M(1, 1)) + ((t073 - t072) * M(1,
						       2)) + ((t085 -
							       t086) * M(1,
									 4));
    R(4, 3) =
	((t066 - t065) * M(1, 1)) + ((t076 - t077) * M(1,
						       2)) + ((t086 -
							       t085) * M(1,
									 3));
    t097 = M(2, 3) * M(3, 4);
    t098 = M(2, 4) * M(3, 3);
    t101 = M(2, 2) * M(3, 4);
    t102 = M(2, 4) * M(3, 2);
    t105 = M(2, 2) * M(3, 3);
    t106 = M(2, 3) * M(3, 2);
    R(1, 4) =
	((t098 - t097) * M(1, 2)) + ((t101 - t102) * M(1,
						       3)) + ((t106 -
							       t105) * M(1,
									 4));
    t112 = M(2, 1) * M(3, 4);
    t113 = M(2, 4) * M(3, 1);
    t116 = M(2, 1) * M(3, 3);
    t117 = M(2, 3) * M(3, 1);
    R(2, 4) =
	((t097 - t098) * M(1, 1)) + ((t113 - t112) * M(1,
						       3)) + ((t116 -
							       t117) * M(1,
									 4));
    t125 = M(2, 1) * M(3, 2);
    t126 = M(2, 2) * M(3, 1);
    R(3, 4) =
	((t102 - t101) * M(1, 1)) + ((t112 - t113) * M(1,
						       2)) + ((t126 -
							       t125) * M(1,
									 4));
    R(4, 4) =
	((t105 - t106) * M(1, 1)) + ((t117 - t116) * M(1,
						       2)) + ((t125 -
							       t126) * M(1,
									 3));
    if (-min_det <= det && det <= min_det) {
	*det_rtn = 0;
    } else {
	*det_rtn = det;
	for (i = 0; i < 16; i++)
	    r[i] *= 1 / det;
    }
}

#define A(I,J) a[IT(I,J)]
#define B(I,J) b[IT(I,J)]
void compose_unsafe(TRANSFORM r, TRANSFORM a, TRANSFORM b)
{
    int i, j;
    FLOAT *p = r;

    for (j = 1; j <= 4; j++)
	for (i = 1; i <= 4; i++)
	    *p++ =
		A(i, 1) * B(1, j) + A(i, 2) * B(2, j) + A(i, 3) * B(3,
								    j) +
		A(i, 4) * B(4, j);
}

void compose(TRANSFORM r, TRANSFORM a, TRANSFORM b)
{
    TRANSFORM t;
    compose_unsafe(t, a, b);
    copy_transform(r, t);
}

void transform_pt_3d(POINT_3D r, TRANSFORM m, POINT_3D p)
{
    POINT_3D t;
    FLOAT wi;

    wi = 1 / (M(4, 1) * p[X] + M(4, 2) * p[Y] + M(4, 3) * p[Z] + M(4, 4));
    t[X] =
	(M(1, 1) * p[X] + M(1, 2) * p[Y] + M(1, 3) * p[Z] + M(1, 4)) * wi;
    t[Y] =
	(M(2, 1) * p[X] + M(2, 2) * p[Y] + M(2, 3) * p[Z] + M(2, 4)) * wi;
    t[Z] =
	(M(3, 1) * p[X] + M(3, 2) * p[Y] + M(3, 3) * p[Z] + M(3, 4)) * wi;
    copy_pt_3d(r, t);
}

void transform_vec_3d(VECTOR_3D r, TRANSFORM m, VECTOR_3D v)
{
    VECTOR_3D t;

    t[X] = M(1, 1) * v[X] + M(1, 2) * v[Y] + M(1, 3) * v[Z];
    t[Y] = M(2, 1) * v[X] + M(2, 2) * v[Y] + M(2, 3) * v[Z];
    t[Z] = M(3, 1) * v[X] + M(3, 2) * v[Y] + M(3, 3) * v[Z];
    copy_vec_3d(r, t);
}

void set_ident_quat(QUATERNION q)
{
    q[W] = 1;
    q[X] = q[Y] = q[Z] = 0;
}

void set_angle_axis_quat(QUATERNION q, FLOAT theta, VECTOR_3D axis)
{
    VECTOR_3D v;

    find_unit_vec_3d(v, axis);
    scale_vec_3d(&q[X], v, sin(theta));
    q[W] = cos(theta);
}

void mult_quat(QUATERNION r, QUATERNION a, QUATERNION b)
{
    r[W] = a[W] * b[W] - a[X] * b[X] - a[Y] * b[Y] - a[Z] * b[Z];
    r[X] = a[W] * b[X] + a[X] * b[W] + a[Y] * b[Z] - a[Z] * b[Y];
    r[Y] = a[W] * b[Y] - a[X] * b[Z] + a[Y] * b[W] + a[Z] * b[X];
    r[Z] = a[W] * b[Z] + a[X] * b[Y] - a[Y] * b[X] + a[Z] * b[W];
}

#define R(I,J) r[IT(I,J)]
#define SQR(A) ((A) * (A))

void find_rot_from_quat(TRANSFORM r, QUATERNION q)
{
    FLOAT len2 = SQR(q[W]) + SQR(q[X]) + SQR(q[Y]) + SQR(q[Z]);
    FLOAT s = len2 > 0 ? 2 / len2 : 0;

    R(1, 1) = 1 - s * (SQR(q[Y]) + SQR(q[Z]));
    R(1, 2) = s * (q[X] * q[Y] - q[W] * q[Z]);
    R(1, 3) = s * (q[X] * q[Z] + q[W] * q[Y]);

    R(2, 1) = s * (q[X] * q[Y] + q[W] * q[Z]);
    R(2, 2) = 1 - s * (SQR(q[X]) + SQR(q[Z]));
    R(2, 3) = s * (q[Y] * q[Z] - q[W] * q[X]);

    R(3, 1) = s * (q[X] * q[Z] - q[W] * q[Y]);
    R(3, 2) = s * (q[Y] * q[Z] + q[W] * q[X]);
    R(3, 3) = 1 - s * (SQR(q[X]) + SQR(q[Y]));

    R(1, 4) = R(4, 1) = R(2, 4) = R(4, 2) = R(3, 4) = R(4, 3) = 0;
    R(4, 4) = 1;
}

void find_quat_from_rot(QUATERNION q, TRANSFORM r)
{
    if (R(1, 1) + R(2, 2) + R(3, 3) >= 0) {	// w first
	FLOAT w2 = sqrt(R(1, 1) + R(2, 2) + R(3, 3) + 1);
	q[W] = 0.5 * w2;	// 1st
	q[X] = (0.5 / w2) * (R(3, 2) - R(2, 3));	// (f)
	q[Y] = (0.5 / w2) * (R(1, 3) - R(3, 1));	// (d)
	q[Z] = (0.5 / w2) * (R(2, 1) - R(1, 2));	// (b)
	return;
    }
    // x, y, or z first
    if (R(1, 1) > R(2, 2))
	if (R(1, 1) > R(3, 3))
	    goto x_first;
	else
	    goto z_first;
    else			// R(2,2) >= R(1,1)
    if (R(2, 2) > R(3, 3))
	goto y_first;
    else
	goto z_first;

  x_first:{
	FLOAT x2 = sqrt(R(1, 1) - R(2, 2) - R(3, 3) + 1);
	q[W] = (0.5 / x2) * (R(3, 2) - R(2, 3));	// (f)
	q[X] = 0.5 * x2;	// 1st
	q[Y] = (0.5 / x2) * (R(2, 1) + R(1, 2));	// (a)
	q[Z] = (0.5 / x2) * (R(1, 3) + R(3, 1));	// (c)
	return;
    }

  y_first:{
	FLOAT y2 = sqrt(-R(1, 1) + R(2, 2) - R(3, 3) + 1);
	q[W] = (0.5 / y2) * (R(1, 3) - R(3, 1));	// (d)
	q[X] = (0.5 / y2) * (R(2, 1) + R(1, 2));	// (a)
	q[Y] = 0.5 * y2;	// 1st
	q[Z] = (0.5 / y2) * (R(3, 2) + R(2, 3));	// (e)
	return;
    }

  z_first:{
	FLOAT z2 = sqrt(-R(1, 1) - R(2, 2) + R(3, 3) + 1);
	q[W] = (0.5 / z2) * (R(2, 1) - R(1, 2));	// (b)
	q[X] = (0.5 / z2) * (R(1, 3) + R(3, 1));	// (c)
	q[Y] = (0.5 / z2) * (R(3, 2) + R(2, 3));	// (e)
	q[Z] = 0.5 * z2;	// 1st
	return;
    }
}

#undef R

void
make_cso_polygon_2d(POLYGON_2D * r, POLYGON_2D * a, POINT_2D p,
		    POLYGON_2D * b)
{
    int j, ia, ja, ib, jb, ir, nb;
    FLOAT x, y, dx_a, dy_a, dx_b, dy_b;

    setup_polygon_2d(r, a->n_sides + b->n_sides);
    r->n_sides = a->n_sides + b->n_sides;

    ja = 0;
    x = a->v[ja][X];
    for (j = 1; j < a->n_sides; j++)
	if (a->v[j][X] < x) {
	    x = a->v[j][X];
	    ja = j;
	}

    jb = 0;
    x = b->v[0][X];
    for (j = 1; j < b->n_sides; j++)
	if (b->v[j][X] > x) {
	    x = b->v[j][X];
	    jb = j;
	}
    // this point is certain to be an extreme point of the cso
    x = b->v[jb][X] + (p[X] - a->v[ja][X]);
    y = b->v[jb][Y] + (p[Y] - a->v[ja][Y]);

    ia = (ja + 1) % a->n_sides;
    dx_a = a->v[ja][X] - a->v[ia][X];
    dy_a = a->v[ja][Y] - a->v[ia][Y];
    ib = (jb + 1) % b->n_sides;
    dx_b = b->v[ib][X] - b->v[jb][X];
    dy_b = b->v[ib][Y] - b->v[jb][Y];
    nb = b->n_sides;
    ir = 0;
    for (;;) {

	// record obstacle polygon point and quit if done
	r->v[ir][X] = x;
	r->v[ir][Y] = y;
	if (++ir == r->n_sides)
	    break;

	// merge next edge of lowest theta. */
	if (nb == 0 || dx_a * dy_b - dy_a * dx_b > 0) {
	    x += dx_a;
	    y += dy_a;
	    ja = ia;
	    ia = (ja + 1) % a->n_sides;
	    dx_a = a->v[ja][X] - a->v[ia][X];
	    dy_a = a->v[ja][Y] - a->v[ia][Y];
	} else {
	    x += dx_b;
	    y += dy_b;
	    jb = ib;
	    ib = (jb + 1) % b->n_sides;
	    dx_b = b->v[ib][X] - b->v[jb][X];
	    dy_b = b->v[ib][Y] - b->v[jb][Y];
	    nb--;
	}
    }
}

int point_near_convex_polygon_2d_p(POINT_2D p, POLYGON_2D * a, FLOAT eps)
{
    int i, j;
    VECTOR_2D vji_perp, vjp;

    // if the point is more than eps right of any edge, we're outside
    for (i = 0, j = a->n_sides - 1; i < a->n_sides; j = i++) {
	vji_perp[X] = a->v[j][Y] - a->v[i][Y];
	vji_perp[Y] = a->v[i][X] - a->v[j][X];
	find_unit_vec_2d(vji_perp, vji_perp);
	sub_pts_2d(vjp, p, a->v[j]);
	if (dot_2d(vjp, vji_perp) <= eps)
	    return 0;
    }
    // else we're inside!
    return 1;
}

int point_inside_convex_polygon_2d_p(POINT_2D p, POLYGON_2D * a)
{
    int i, j;

    // if the point is right of any edge, we're outside
    for (i = 0, j = a->n_sides - 1; i < a->n_sides; j = i++)
	if ((p[X] - a->v[j][X]) * (a->v[i][Y] - a->v[j][Y]) -
	    (p[Y] - a->v[j][Y]) * (a->v[i][X] - a->v[j][X]) >= 0)
	    return 0;

    // else we're inside!
    return 1;
}

// The Franklin code...
int point_inside_polygon_2d_p(POINT_2D p, POLYGON_2D * a)
{
    int i, j, r = 0;
    for (i = 0, j = a->n_sides - 1; i < a->n_sides; j = i++) {
	if (((a->v[i][Y] <= p[Y] && p[Y] < a->v[j][Y]) ||
	     (a->v[j][Y] <= p[Y] && p[Y] < a->v[i][Y])) &&
	    (p[X] < (a->v[j][X] - a->v[i][X]) * (p[Y] - a->v[i][Y]) /
	     (a->v[j][Y] - a->v[i][Y]) + a->v[i][X]))
	    r ^= 1;
    }
    return r;
}

#ifdef TEST_INVERT

void print_transform(TRANSFORM m)
{
    int i, j;
    printf("[\n");
    for (i = 1; i <= 4; i++) {
	printf("[");
	for (j = 1; j <= 4; j++) {
	    printf(" %8.3g", m[IT(i, j)]);
	}
	printf("]\n");
    }
    printf("]\n");
}

int main(void)
{
    TRANSFORM m =
	{ 1, 0, 1, 1, 2, 4, 0, 19, 3, 5, 6, 57, 14, -3, 34, 1 }, r;
    FLOAT det;
    VECTOR_3D axis = { 1, 2, 3 };
    POINT_3D pt = { -10, 2, 41 };

    // set_angle_axis_rot_about_point(m, 30, pt, axis);
    print_transform(m);
    invert(r, &det, m, 1e-4);
    printf("det=%.3g\n", det);
    print_transform(r);
    invert(m, &det, r, 1e-4);
    printf("det=%.3g\n", det);
    print_transform(m);
}

#endif

#ifdef TEST_DYNARRAY_H

// we need a dynamic arrao of these things
typedef struct foo_t {
    char *name;
    int count;
} FOO;

typedef struct foo_array_t {
    DYNAMIC_ARRAY_FIELDS(FOO, val, n_vals);
} FOO_ARRAY;

// do the prototypes for the constructor, destructor, and accessor functions
DECLARE_DYNAMIC_ARRAY_PROTOS(FOO_ARRAY, FOO, foo_list, val, n_vals)
// ---- in foo.c ----
// create the bodies for the constructor, destructor, and accessor functions
    DECLARE_DYNAMIC_ARRAY_FUNCS(FOO_ARRAY, FOO, foo_list, val, n_vals)
// use all the new stuff!
void do_stuff_with_foos(void)
{
    int i;
    char buf[100];
    FOO_ARRAY list[1];		// or FOO_ARRAY list; but then we're forever &'ing
    FOO_ARRAY copy[1];

    init_foo_list(list);	// do this JUST ONCE right after declaration
    init_foo_list(copy);	// (not necessary for static/global decls)

    setup_foo_list(list, 10);	// allow for 10 elements

    // read some data and push it on the list tail
    while (scanf("%d %s", &i, buf) == 2) {
	// get pointer to new (empty) element at the end of array
	FOO *p = pushed_foo_list_val(list);
	// fill in field values
	p->name = strdup(buf);
	p->count = i;
    }

    // shows unsafe access to elements
    printf("forward listing:\n");
    for (i = 0; i < list->n_vals; i++)
	printf("name=%s count=%d (%d)\n", list->val[i].name,	// fast unsafe access
	       foo_list_val_ptr(list, i)->count,	// slower safe pointer access
	       foo_list_val(list, i).count);	// copying access

    copy_foo_list_filled(copy, list);	// copies only filled elements

    // print in reverse order by popping from tail
    printf("backward listing:\n");
    while (copy->n_vals > 0) {
	FOO *p = popped_foo_list_val(copy);
	printf("name=%s count=%d\n", p->name, p->count);
    }

    // clear out all the allocated storage for the ilst
    clear_foo_list(list);
    clear_foo_list(copy);
}

#endif
