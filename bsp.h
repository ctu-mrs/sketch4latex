/* bsp.h
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

#ifndef __BSP_H
#define __BSP_H

#include "geometry.h"

typedef enum bsp_node_type_t {
    BSP_POLYGON,
    BSP_POLYLINE,
} BSP_NODE_TYPE;

#define BASE_BSP_NODE_FIELDS  \
  BSP_NODE_TYPE tag; \
  struct bsp_node_t *prev, *next, *mark, *in, *on, *out; \
  void *attr; \
  BOX_3D extent[1]

typedef struct bsp_node_t {
    BASE_BSP_NODE_FIELDS;
} BSP_NODE, *BSP_TREE;

typedef struct bsp_vertex_attr_t {
    int border_p;
    int parent_vtx;
    int cut_p;
} BSP_VERTEX_ATTR;

typedef struct bsp_polygon_attr_t {
    DYNAMIC_ARRAY_FIELDS(BSP_VERTEX_ATTR, elt, n_elts);
} BSP_POLYGON_ATTR;

DECLARE_DYNAMIC_ARRAY_PROTOS(BSP_POLYGON_ATTR, BSP_VERTEX_ATTR,
			     polygon_attr, elt, n_elts);
typedef struct bsp_polygon_node_t {
    BASE_BSP_NODE_FIELDS;
    PLANE plane[1];
    POLYGON_3D polygon[1];
    BSP_POLYGON_ATTR polygon_attr[1];	// attributes of polygon vertices
} BSP_POLYGON_NODE;

typedef struct bsp_polyline_node_t {
    BASE_BSP_NODE_FIELDS;
    POLYLINE_3D polyline[1];
    int first_p, last_p;
} BSP_POLYLINE_NODE;

void add_polygon_to_bsp(BSP_TREE * bsp, POLYGON_3D * polygon, void *attr);

void add_polyline_to_bsp(BSP_TREE * bsp, POLYLINE_3D * polylines,
			 void *attr);

typedef void (*BSP_NODE_FUNC) (BSP_NODE * node, void *env);

void traverse_bsp(BSP_NODE * bsp, BSP_NODE_FUNC func, void *env);

void traverse_depth_sort(BSP_NODE * bsp, BSP_NODE_FUNC func, void *env);

void print_bsp(FILE * f, BSP_NODE * bsp);

void add_polygon_to_sort(BSP_TREE * bsp, POLYGON_3D * polygon, void *attr);

void add_polyline_to_sort(BSP_TREE * bsp, POLYLINE_3D * polyline,
			  void *attr);

void sort_by_depth(BSP_TREE * bsp);

#endif
