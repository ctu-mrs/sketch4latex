/* bsp.c
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
#include "bsp.h"
#include "geomio.h"

DECLARE_DYNAMIC_ARRAY_FUNCS(BSP_POLYGON_ATTR, BSP_VERTEX_ATTR,
			    polygon_attr, elt, n_elts, NO_OTHER_INIT);
// ---- polylines --------------------------------------------------------------

static BSP_POLYLINE_NODE *new_bsp_polyline_node(void *attr)
{
    BSP_POLYLINE_NODE *n = safe_malloc(sizeof *n);
    n->tag = BSP_POLYLINE;
    n->attr = attr;
    n->prev = n->next = n->mark = n->in = n->on = n->out = NULL;
    n->first_p = n->last_p = 0;
    init_box_3d(n->extent);
    init_polyline_3d(n->polyline);
    return n;
}

static void delete_bsp_polyline_node(BSP_POLYLINE_NODE * node)
{
    clear_polyline_3d(node->polyline);
    safe_free(node);
}

static void set_bsp_polyline_extent(BSP_POLYLINE_NODE * node)
{
    // set extent
    init_box_3d(node->extent);
    fold_min_max_polyline_3d(node->extent, node->polyline);
}

static void
init_bsp_polyline(BSP_POLYLINE_NODE * node, POLYLINE_3D * polyline)
{
    // initial polyline contains first and last points; split ones don't
    node->first_p = node->last_p = 1;
    copy_polyline_3d(node->polyline, polyline);
    set_bsp_polyline_extent(node);
}

static void
split_polyline_with_plane(BSP_POLYLINE_NODE * node,
			  PLANE * plane,
			  BSP_POLYLINE_NODE ** in_nodes,
			  BSP_POLYLINE_NODE ** on_nodes,
			  BSP_POLYLINE_NODE ** out_nodes)
{
    int i, j, i_side, current_side;
    BSP_POLYLINE_NODE *current, **list;
    VECTOR_3D v, dp;
    POINT_3D isect;
    FLOAT t;

    // initialize all the output lists to empty
    *in_nodes = *on_nodes = *out_nodes = NULL;

    // initialize the current polyline that's being "built", copying attributes
    // of the original
    current = new_bsp_polyline_node(node->attr);

    // copy source polygon's first_ attribute
    current->first_p = node->first_p;

    // j is the trail index as we step through vertices with head i
    j = 0;

    // copy first vertex of polyline onto current output polyline
    copy_pt_3d(pushed_polyline_3d_v(current->polyline),
	       node->polyline->v[j]);

    // find side of cutting plane that first vertex is on.
    current_side =
	(S_IN | S_ON | S_OUT) & pt_side_of_plane(plane,
						 node->polyline->v[j]);

    // loop through all directed segments j->i
    for (i = 1; i < node->polyline->n_vertices; j = i++) {
	i_side =
	    (S_IN | S_ON | S_OUT) & pt_side_of_plane(plane,
						     node->polyline->v[i]);

#define HASH(Current, I) ((Current << 3) | I)

	switch (HASH(current_side, i_side)) {

	case HASH(S_OUT, S_IN):
	case HASH(S_IN, S_OUT):

	    // vertices straddle the plane; compute the intersection
	    sub_pts_3d(v, node->polyline->v[i], node->polyline->v[j]);	// direction vector
	    sub_pts_3d(dp, plane->p, node->polyline->v[j]);	// P - L
	    t = dot_3d(dp, plane->n) / dot_3d(v, plane->n);	// parameter of intersect
	    add_scaled_vec_to_pt_3d(isect, node->polyline->v[j], v, t);	// intersect

	    // finish current polyline and add to current list
	    copy_pt_3d(pushed_polyline_3d_v(current->polyline), isect);
	    set_bsp_polyline_extent(current);
	    list = current_side == S_IN ? in_nodes : out_nodes;
	    current->in = (BSP_NODE *) * list;
	    *list = current;

	    // start a new one by adding the isect and new point
	    current = new_bsp_polyline_node(node->attr);
	    copy_pt_3d(pushed_polyline_3d_v(current->polyline), isect);
	    copy_pt_3d(pushed_polyline_3d_v(current->polyline),
		       node->polyline->v[i]);
	    current_side = i_side;
	    break;

	case HASH(S_OUT, S_ON):
	case HASH(S_IN, S_ON):

	    // line that was away from the plane joins it;
	    // finish current polyline and add to current list
	    copy_pt_3d(pushed_polyline_3d_v(current->polyline),
		       node->polyline->v[i]);
	    set_bsp_polyline_extent(current);
	    list = current_side == S_IN ? in_nodes : out_nodes;
	    current->in = (BSP_NODE *) * list;
	    *list = current;

	    // start a new one if there are still vertices left to process
	    if (i < node->polyline->n_vertices - 1) {
		current = new_bsp_polyline_node(node->attr);
		copy_pt_3d(pushed_polyline_3d_v(current->polyline),
			   node->polyline->v[i]);
		current_side = S_ON;
	    } else {
		// copy last_p attribute from source
		current->last_p = node->last_p;
		current = NULL;
		current_side = 0;
	    }
	    break;

	case HASH(S_ON, S_OUT):
	case HASH(S_ON, S_IN):

	    // line that was on the plane springs away from it;
	    // if there is more than one point in the current polyline,
	    // complete it and start a new one
	    if (current->polyline->n_vertices > 1) {
		FLOAT *last_vertex = current->polyline->v[current->polyline->n_vertices - 1];	//remember last vertex
		set_bsp_polyline_extent(current);
		current->in = (BSP_NODE *) * on_nodes;
		*on_nodes = current;
		current = new_bsp_polyline_node(node->attr);
		copy_pt_3d(pushed_polyline_3d_v(current->polyline),
			   last_vertex);
	    }
	    // add the new vertex to the current polyline
	    copy_pt_3d(pushed_polyline_3d_v(current->polyline),
		       node->polyline->v[i]);
	    current_side = i_side;	// now either in or out
	    break;

	default:

	    // nothing has changed, so just add the new point
	    // to the ccurrent polygon
	    copy_pt_3d(pushed_polyline_3d_v(current->polyline),
		       node->polyline->v[i]);
	    break;
	}
    }
    // finish the final polyline
    if (current) {
	// copy last_p attribute from source
	current->last_p = node->last_p;

	set_bsp_polyline_extent(current);
	list = (current_side & S_IN) ? in_nodes :
	    (current_side & S_OUT) ? out_nodes : on_nodes;
	current->in = (BSP_NODE *) * list;
	*list = current;
    }
}

static void insert_polyline(BSP_TREE * bsp, BSP_POLYLINE_NODE * node)
{
    if (*bsp == NULL) {
	*bsp = (BSP_NODE *) node;
    } else if ((*bsp)->tag == BSP_POLYLINE) {
	BSP_POLYLINE_NODE *bsp_node = (BSP_POLYLINE_NODE *) * bsp;
#ifdef EXPERIMENTAL_OPTIMIZATION
	node->in = bsp_node;
	*bsp = (BSP_NODE *) node;
#else
	insert_polyline((BSP_TREE *) & bsp_node->in, node);
#endif
    } else {			// BSP_POLYGON
	BSP_POLYGON_NODE *bsp_node = (BSP_POLYGON_NODE *) * bsp;
	int side = polyline_side_of_plane(node->polyline, bsp_node->plane);
	if (side == S_IN)
	    insert_polyline(&bsp_node->in, node);
	else if (side == S_ON)
	    insert_polyline(&bsp_node->on, node);
	else if (side == S_OUT)
	    insert_polyline(&bsp_node->out, node);
	else {			// S_SPLIT
	    BSP_POLYLINE_NODE *in_polylines, *on_polylines, *out_polylines,
		*p, *p_next;
	    split_polyline_with_plane(node, bsp_node->plane, &in_polylines,
				      &on_polylines, &out_polylines);
	    // remove polylines from lists and add to respective bsp subtrees recursively
	    for (p = in_polylines; p; p = p_next) {
		p_next = (BSP_POLYLINE_NODE *) p->in;
		p->in = NULL;
		insert_polyline(&bsp_node->in, p);
	    }
	    for (p = on_polylines; p; p = p_next) {
		p_next = (BSP_POLYLINE_NODE *) p->in;
		p->in = NULL;
		insert_polyline(&bsp_node->on, p);
	    }
	    for (p = out_polylines; p; p = p_next) {
		p_next = (BSP_POLYLINE_NODE *) p->in;
		p->in = NULL;
		insert_polyline(&bsp_node->out, p);
	    }
	    delete_bsp_polyline_node(node);
	}
    }
}

void
add_polyline_to_bsp(BSP_TREE * bsp, POLYLINE_3D * polyline, void *attr)
{
    BSP_POLYLINE_NODE *node = new_bsp_polyline_node(attr);
    init_bsp_polyline(node, polyline);
    insert_polyline(bsp, node);
}

// ---- polygons ---------------------------------------------------------------

static BSP_POLYGON_NODE *new_bsp_polygon_node(void *attr)
{
    BSP_POLYGON_NODE *n = safe_malloc(sizeof *n);
    n->tag = BSP_POLYGON;
    n->attr = attr;
    n->prev = n->next = n->mark = n->in = n->on = n->out = NULL;
    init_box_3d(n->extent);
    init_polygon_3d(n->polygon);
    init_polygon_attr(n->polygon_attr);
    return n;
}

static void set_bsp_polygon_extent(BSP_POLYGON_NODE * node)
{
    // set extent
    init_box_3d(node->extent);
    fold_min_max_polygon_3d(node->extent, node->polygon);
}

static void
init_bsp_polygon_node(BSP_POLYGON_NODE * node, POLYGON_3D * polygon)
{
    int i;

    // fill in the polygon verticies
    copy_polygon_3d(node->polygon, polygon);

    // fill in the plane
    find_polygon_plane(node->plane, polygon);

    // mark all edges as border edges
    setup_polygon_attr(node->polygon_attr, polygon->n_sides);
    for (i = 0; i < polygon->n_sides; i++)
	node->polygon_attr->elt[i].border_p = 1;
    node->polygon_attr->n_elts = polygon->n_sides;

    // fill in the extent
    set_bsp_polygon_extent(node);
}

static void delete_bsp_polygon_node(BSP_POLYGON_NODE * node)
{
    clear_polygon_3d(node->polygon);
    clear_polygon_attr(node->polygon_attr);
    safe_free(node);
}

// decide whether a j->i edge of the the new polygon that has 
// been split from parent is part of the border.
static int
is_new_border_p(BSP_VERTEX_ATTR * i_attr,
		BSP_VERTEX_ATTR * j_attr,
		BSP_POLYGON_ATTR * parent_attr, int parent_n_sides)
{
    int parent_is_border_p;

    if (parent_attr->n_elts != parent_n_sides)
	die(no_line, "failed assumption on attribute size");
    parent_is_border_p = parent_attr->elt[j_attr->parent_vtx].border_p;
    if (!parent_is_border_p)
	return 0;

    if (i_attr->cut_p) {
	if (j_attr->cut_p) {
	    return 0;
	} else {
	    return i_attr->parent_vtx == j_attr->parent_vtx;
	}
    }
    return (i_attr->parent_vtx - j_attr->parent_vtx +
	    parent_n_sides) % parent_n_sides == 1;
}

static void
decide_boundaries(BSP_POLYGON_NODE * new_node, BSP_POLYGON_NODE * node)
{
    int i, j, last_border_p;

    i = 0;
    j = new_node->polygon->n_sides - 1;
    last_border_p =
	is_new_border_p(&new_node->polygon_attr->elt[i],
			&new_node->polygon_attr->elt[j],
			node->polygon_attr, node->polygon->n_sides);
    for (j = i++; i < new_node->polygon->n_sides; j = i++) {
	new_node->polygon_attr->elt[j].border_p =
	    is_new_border_p(&new_node->polygon_attr->elt[i],
			    &new_node->polygon_attr->elt[j],
			    node->polygon_attr, node->polygon->n_sides);
    }
    new_node->polygon_attr->elt[j].border_p = last_border_p;
}

static void
push_polygon_attr(BSP_POLYGON_NODE * node, int parent_vtx, int cut_p)
{
    BSP_VERTEX_ATTR *vertex_attr =
	pushed_polygon_attr_elt(node->polygon_attr);
    vertex_attr->border_p = 0;
    vertex_attr->parent_vtx = parent_vtx;
    vertex_attr->cut_p = cut_p;
}

static void
split_polygon_with_plane(BSP_POLYGON_NODE * node,
			 PLANE * plane,
			 BSP_POLYGON_NODE * in_node,
			 BSP_POLYGON_NODE * out_node)
{
    int i, j, i_side, j_side;
    VECTOR_3D v, dp;
    POINT_3D isect;
    FLOAT t;

    // reset fill pointers
    in_node->polygon->n_sides = out_node->polygon->n_sides = 0;
    in_node->polygon_attr->n_elts = out_node->polygon_attr->n_elts = 0;

    // process all pairs of vertices
    j = node->polygon->n_sides - 1;
    i_side = pt_side_of_plane(plane, node->polygon->v[j]);
    for (i = 0; i < node->polygon->n_sides; j = i++) {
	j_side = i_side;
	i_side = pt_side_of_plane(plane, node->polygon->v[i]);

	if ((i_side | j_side) == (S_IN | S_OUT)) {

	    // the two most recent points straddle the the plane
	    // compute the intersection
	    sub_pts_3d(v, node->polygon->v[i], node->polygon->v[j]);	// direction vector
	    sub_pts_3d(dp, plane->p, node->polygon->v[j]);	// P - L
	    t = dot_3d(dp, plane->n) / dot_3d(v, plane->n);	// parameter of intersect
	    add_scaled_vec_to_pt_3d(isect, node->polygon->v[j], v, t);	// intersect

	    // put intersect in both polygons
	    copy_pt_3d(pushed_polygon_3d_v(in_node->polygon), isect);
	    copy_pt_3d(pushed_polygon_3d_v(out_node->polygon), isect);

	    if (i_side == S_IN) {
		// edge traversed from outside to in
		push_polygon_attr(out_node, j, 1);
		push_polygon_attr(in_node, j, 1);
		copy_pt_3d(pushed_polygon_3d_v(in_node->polygon),
			   node->polygon->v[i]);
		push_polygon_attr(in_node, i, 0);
	    } else {
		// edge traversed from inside to out
		push_polygon_attr(out_node, j, 1);
		push_polygon_attr(in_node, j, 1);
		copy_pt_3d(pushed_polygon_3d_v(out_node->polygon),
			   node->polygon->v[i]);
		push_polygon_attr(out_node, i, 0);;
	    }
	} else if (i_side & S_ON) {

	    // the current vertex is on the plane
	    // put it in both polygons
	    copy_pt_3d(pushed_polygon_3d_v(in_node->polygon),
		       node->polygon->v[i]);
	    copy_pt_3d(pushed_polygon_3d_v(out_node->polygon),
		       node->polygon->v[i]);
	    push_polygon_attr(in_node, i, 0);
	    push_polygon_attr(out_node, i, 0);
	} else {

	    // the new vertex is not straddling nor on the plane
	    // so we output it to the correct polygon
	    if (i_side == S_IN) {
		copy_pt_3d(pushed_polygon_3d_v(in_node->polygon),
			   node->polygon->v[i]);
		push_polygon_attr(in_node, i, 0);
	    } else {
		copy_pt_3d(pushed_polygon_3d_v(out_node->polygon),
			   node->polygon->v[i]);
		push_polygon_attr(out_node, i, 0);
	    }
	}
    }
    // fill in the planes
    find_polygon_plane(in_node->plane, in_node->polygon);
    find_polygon_plane(out_node->plane, out_node->polygon);

    // set up extents
    set_bsp_polygon_extent(in_node);
    set_bsp_polygon_extent(out_node);

    // make a pass around the in and out polygons to fill in boundardy_p
    decide_boundaries(in_node, node);
    decide_boundaries(out_node, node);
}

static void insert_polygon(BSP_TREE * bsp, BSP_POLYGON_NODE * node)
{
    if (*bsp == NULL)
	*bsp = (BSP_NODE *) node;
    else {
	BSP_POLYGON_NODE *bsp_node = (BSP_POLYGON_NODE *) * bsp;
	int side = polygon_side_of_plane(node->polygon, bsp_node->plane);
	if (side & (S_IN | S_ON))
	    insert_polygon(&bsp_node->in, node);
	else if (side == S_OUT)
	    insert_polygon(&bsp_node->out, node);
	else {
	    BSP_POLYGON_NODE *in_node = new_bsp_polygon_node(node->attr);
	    BSP_POLYGON_NODE *out_node = new_bsp_polygon_node(node->attr);
	    split_polygon_with_plane(node, bsp_node->plane, in_node,
				     out_node);
	    insert_polygon(&bsp_node->in, in_node);
	    insert_polygon(&bsp_node->out, out_node);
	    delete_bsp_polygon_node(node);
	}
    }
}

void add_polygon_to_bsp(BSP_TREE * bsp, POLYGON_3D * polygon, void *attr)
{
    BSP_POLYGON_NODE *node = new_bsp_polygon_node(attr);
    init_bsp_polygon_node(node, polygon);
    insert_polygon(bsp, node);
}

static struct {
    BSP_NODE_FUNC func;
    void *env;
} traverse_closure[1];

static void walk_bsp(BSP_NODE * bsp)
{
    if (bsp == NULL)
	return;
    if (bsp->tag == BSP_POLYGON) {
	BSP_POLYGON_NODE *node = (BSP_POLYGON_NODE *) bsp;
	if (node->plane->n[Z] < 0) {
	    walk_bsp(node->out);
	    traverse_closure->func(bsp, traverse_closure->env);
	    walk_bsp(node->on);
	    walk_bsp(node->in);
	} else {
	    walk_bsp(node->in);
	    traverse_closure->func(bsp, traverse_closure->env);
	    walk_bsp(node->on);
	    walk_bsp(node->out);
	}
    } else {			// BSP_POLYLINE
	BSP_POLYLINE_NODE *node = (BSP_POLYLINE_NODE *) bsp;
	traverse_closure->func(bsp, traverse_closure->env);
	walk_bsp((BSP_NODE *) node->in);
    }
}

void traverse_bsp(BSP_NODE * bsp, BSP_NODE_FUNC func, void *env)
{
    traverse_closure->func = func;
    traverse_closure->env = env;
    walk_bsp(bsp);
}

void traverse_depth_sort(BSP_NODE * bsp, BSP_NODE_FUNC func, void *env)
{
    traverse_closure->func = func;
    traverse_closure->env = env;
    for (; bsp; bsp = bsp->next)
	walk_bsp(bsp);
}

static void indent(FILE * f, int n)
{
    while (n-- > 0)
	fprintf(f, " ");
}

static void print_bsp_internal(FILE * f, BSP_NODE * bsp, int n)
{
    if (bsp == NULL)
	return;

    indent(f, 2 * n);
    if (bsp->tag == BSP_POLYGON) {

	BSP_POLYGON_NODE *node = (BSP_POLYGON_NODE *) bsp;
	fprintf(f, "BSPpolygon\n");

	indent(f, 2 * n + 1);
	print_plane(f, node->plane);

	indent(f, 2 * n + 1);
	print_polygon_3d(f, node->polygon);

	indent(f, 2 * n + 1);
	fprintf(f, "in\n");
	print_bsp_internal(f, node->in, n + 1);

	indent(f, 2 * n + 1);
	fprintf(f, "on\n");
	print_bsp_internal(f, node->on, n + 1);

	indent(f, 2 * n + 1);
	fprintf(f, "out\n");
	print_bsp_internal(f, node->out, n + 1);
    } else {			// BSP_POLYLINE
	BSP_POLYLINE_NODE *node = (BSP_POLYLINE_NODE *) bsp;
	fprintf(f, "BSPpolyline ");
	print_polyline_3d(f, node->polyline);
	print_bsp_internal(f, (BSP_NODE *) node->in, n);
    }
}

void print_bsp(FILE * f, BSP_NODE * bsp)
{
    print_bsp_internal(f, bsp, 0);
}

void add_polygon_to_sort(BSP_TREE * bsp, POLYGON_3D * polygon, void *attr)
{
    BSP_POLYGON_NODE *node = new_bsp_polygon_node(attr);
    init_bsp_polygon_node(node, polygon);
    node->next = *bsp;
    *bsp = (BSP_NODE *) node;
}

void
add_polyline_to_sort(BSP_TREE * bsp, POLYLINE_3D * polyline, void *attr)
{
    BSP_POLYLINE_NODE *node = new_bsp_polyline_node(attr);
    init_bsp_polyline(node, polyline);
    node->next = *bsp;
    *bsp = (BSP_NODE *) node;
}

// quicksort of linked list
#define FAR_DEPTH(Node) (-(Node)->extent->min[Z])
#define NEAR_DEPTH(Node) (-(Node)->extent->max[Z])

// leq provides ascending sort, so reverse to get max depth at head of list
#define LEQ(A,B) (FAR_DEPTH(A) >= FAR_DEPTH(B))

static void qs(BSP_NODE * hd, BSP_NODE * tl, BSP_NODE ** rtn)
{
    int nlo, nhi;
    BSP_NODE *lo, *hi, *q, *p;

    /* Invariant:  Return head sorted with `tl' appended. */
    while (hd != NULL) {

	nlo = nhi = 0;
	lo = hi = NULL;
	q = hd;
	p = hd->next;

	/* Reverse ascending prefix onto hi.  This gives
	   O(n) behavior on sorted and reverse-sorted inputs. */
	while (p != NULL && LEQ(p, hd)) {
	    hd->next = hi;
	    hi = hd;
	    ++nhi;
	    hd = p;
	    p = p->next;
	}

	/* If entire list was ascending, we're done. */
	if (p == NULL) {
	    *rtn = hd;
	    hd->next = hi;
	    q->next = tl;
	    return;
	}

	/* Partition and count sizes. */
	while (p != NULL) {
	    q = p->next;
	    if (LEQ(p, hd)) {
		p->next = lo;
		lo = p;
		++nlo;
	    } else {
		p->next = hi;
		hi = p;
		++nhi;
	    }
	    p = q;
	}

	/* Recur to establish invariant for sublists of hd, 
	   choosing shortest list first to limit stack. */
	if (nlo < nhi) {
	    qs(lo, hd, rtn);
	    rtn = &hd->next;
	    hd = hi;		/* Eliminated tail-recursive call. */
	} else {
	    qs(hi, tl, &hd->next);
	    tl = hd;
	    hd = lo;		/* Eliminated tail-recursive call. */
	}
    }
    /* Base case of recurrence. Invariant is easy here. */
    *rtn = tl;
}

static int xy_intersect_p(BOX_3D * a, BOX_3D * b)
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

#define SHORTEST_ALLOWABLE_SIDE 1e-4

static void
make_polygon_projection(POLYGON_2D * projection,
			BSP_POLYGON_NODE * polygon_node)
{
    int i, j;

    clear_polygon_2d(projection);
    if (polygon_node->plane->n[Z] >= 0) {
	for (i = 0, j = polygon_node->polygon->n_sides - 1;
	     i < polygon_node->polygon->n_sides; j = i++) {
	    if (dist_2d
		(polygon_node->polygon->v[i],
		 polygon_node->polygon->v[j]) > SHORTEST_ALLOWABLE_SIDE)
		copy_pt_2d(pushed_polygon_2d_v(projection),
			   polygon_node->polygon->v[i]);
	}
    } else {
	for (i = polygon_node->polygon->n_sides - 1, j = 0; i >= 0;
	     j = i--) {
	    if (dist_2d
		(polygon_node->polygon->v[i],
		 polygon_node->polygon->v[j]) > SHORTEST_ALLOWABLE_SIDE)
		copy_pt_2d(pushed_polygon_2d_v(projection),
			   polygon_node->polygon->v[i]);
	}
    }
}

#define OVERLAP_EPS 1e-3

int projections_overlap_p(BSP_POLYGON_NODE * p, BSP_POLYGON_NODE * q)
{
    int r;
    POLYGON_2D p_projection[1], q_projection[1], cso[1];

    init_polygon_2d(p_projection);
    init_polygon_2d(q_projection);
    init_polygon_2d(cso);

    make_polygon_projection(p_projection, p);
    make_polygon_projection(q_projection, q);
    if (p_projection->n_sides < 2 && q_projection->n_sides < 2) {
	r = 0;
    } else if (p_projection->n_sides < 2) {
	r = point_near_convex_polygon_2d_p(p->polygon->v[0], q_projection,
					   OVERLAP_EPS);
    } else if (q_projection->n_sides < 2) {
	r = point_near_convex_polygon_2d_p(q->polygon->v[0], p_projection,
					   OVERLAP_EPS);
    } else {
	make_cso_polygon_2d(cso, p_projection, origin_2d, q_projection);
	r = point_near_convex_polygon_2d_p(origin_2d, cso, OVERLAP_EPS);
    }

    clear_polygon_2d(p_projection);
    clear_polygon_2d(q_projection);
    clear_polygon_2d(cso);
    return r;
}

#define OVERLAP_TOLERANCE 1e-3

int
polyline_projection_overlaps_polygon(BSP_POLYLINE_NODE * polyline_node,
				     BSP_POLYGON_NODE * polygon_node)
{
    int i, r;
    POLYGON_2D polygon_projection[1], line_seg_projection[1], cso[1];

    init_polygon_2d(polygon_projection);
    init_polygon_2d(line_seg_projection);
    init_polygon_2d(cso);

    make_polygon_projection(polygon_projection, polygon_node);
    if (polygon_projection->n_sides < 2) {
	// a point can't obscure a line
	r = 0;
    } else if (polyline_node->polyline->n_vertices == 1) {
	// polyline is single vertex; just check to see if it lies in projection
	r = point_near_convex_polygon_2d_p(polyline_node->polyline->v[0],
					   polygon_projection,
					   OVERLAP_TOLERANCE);
    } else {

	// use a two-sided polygon to model each edge;
	setup_polygon_2d(line_seg_projection, 2);
	line_seg_projection->n_sides = 2;
	r = 0;
	for (i = 0; i < polyline_node->polyline->n_vertices - 1; i++) {
	    copy_pt_2d(line_seg_projection->v[0],
		       polyline_node->polyline->v[i]);
	    copy_pt_2d(line_seg_projection->v[1],
		       polyline_node->polyline->v[i + 1]);
	    make_cso_polygon_2d(cso, line_seg_projection, origin_2d,
				polygon_projection);
	    r |= point_near_convex_polygon_2d_p(origin_2d, cso,
						OVERLAP_TOLERANCE);
	    if (r)
		break;
	}
    }
    clear_polygon_2d(polygon_projection);
    clear_polygon_2d(line_seg_projection);
    clear_polygon_2d(cso);
    return r;
}

static void debug_print(BSP_NODE * p)
{
    fprintf(stderr, "\nlist:\n");
    while (p) {
	fprintf(stderr, "  %p:%sprev=%p near=%.4g far=%.4g\n",
		p,
		p->mark ? "*" : " ", p->prev, NEAR_DEPTH(p), FAR_DEPTH(p));
	p = p->next;
    }
}

typedef struct make_list_of_bsp_env_t {
    BSP_NODE *head, *tail;
    int n;
} MAKE_LIST_OF_BSP_ENV;

static void make_list_of_bsp(BSP_NODE * bsp, void *v_env)
{
    MAKE_LIST_OF_BSP_ENV *env = (MAKE_LIST_OF_BSP_ENV *) v_env;
    if (env->tail) {
	env->tail->next = bsp;
	bsp->prev = env->tail;
	bsp->next = NULL;
	env->tail = bsp;
    } else {
	env->head = env->tail = bsp;
    }
    ++env->n;
}

// check invariants in the depth sort list
static void check_consistency(BSP_TREE hd)
{
    int n_marks, n_other;
    BSP_NODE *p, *q;

    n_marks = 0;
    for (q = NULL, p = hd; p && p->mark; q = p, p = p->next) {
	n_marks++;
	if (p->prev != q) {
	    debug_print(hd);
	    die(no_line, "broken prev pointer @ %d (%p != %p)", n_marks,
		p->prev, q);
	}
	if (p->extent->min[X] == 0 && p->extent->max[X] == 0 &&
	    p->extent->min[Y] == 0 && p->extent->max[Y] == 0 &&
	    p->extent->min[Z] == 0 && p->extent->max[Z] == 0)
	    die(no_line, "unset extent");
    }

    n_other = 0;
    for (; p; q = p, p = p->next) {
	n_other++;
	if (p->mark)
	    die(no_line, "unexpected mark");
	if (p->prev != q) {
	    debug_print(hd);
	    die(no_line, "broken prev pointer @ %d (%p != %p)",
		n_marks + n_other, p->prev, q);
	}
	if (p->extent->min[X] > p->extent->max[X])
	    die(no_line, "unset extent");
	if (q && !q->mark && FAR_DEPTH(p) > FAR_DEPTH(q)) {
	    debug_print(hd);
	    die(no_line, "far depth out of order @ %d", n_marks + n_other);
	}
    }
}

void insert_by_depth(BSP_TREE * hd, BSP_NODE * node)
{
    BSP_NODE *p, *q;

    // place p after insert point and q before
    for (q = NULL, p = *hd;
	 p && (p->mark || FAR_DEPTH(p) > FAR_DEPTH(node));
	 q = p, p = p->next)
	/* skip */ ;

    // insert
    node->prev = q;
    node->next = p;
    if (q)
	q->next = node;
    else
	*hd = node;
    if (p)
	p->prev = node;
}

// this is taken almost directly from Newell's 1972 paper except that 
// a BSP is used to resolve intersections and cyclic overlaps and it
// incorporates polyline objects
void sort_by_depth(BSP_TREE * bsp)
{
    int side,
	n_probes, n_swaps, n_nodes,
	n_bsps, n_in, n_out, n_ppos, n_plos, n_bsp_in_nodes,
	n_bsp_out_nodes;
    BSP_NODE *p, *p_next, *q, *prev, *t, *t_next, *r;
    BSP_POLYGON_NODE *polygon_node;
    BSP_POLYLINE_NODE *polyline_node;
    PLANE *plane;
    BSP_TREE sub_bsp;
    MAKE_LIST_OF_BSP_ENV env[1];

    // quicksort on deepest vertex, back-to-front
    qs(*bsp, NULL, &p);

    // hook up prev pointers and make sure marks are clear
    n_nodes = 0;
    for (prev = NULL, q = p; q; prev = q, q = q->next) {
	q->prev = prev;
	q->mark = NULL;
	++n_nodes;
    }

    // keep some stats just for fun
    n_probes = n_swaps = n_bsps = n_bsp_in_nodes = n_bsp_out_nodes =
	n_ppos = n_plos = 0;

    // debug_print(p); 

    // this is now output list
    r = NULL;

    // goto here whenever the current check fails 
    // for "p", the hopeful deepest polygon 
  restart_overlap_check:

    while (p) {

	if (n_nodes < 1000)
	    check_consistency(p);

	// check overlapping objects for necessary swaps.
	for (q = p->next;
	     q && (q->mark || FAR_DEPTH(q) > NEAR_DEPTH(p)); q = q->next) {

	    ++n_probes;

	    // rectangular x-y extents don't overlap, so a moo point (utterly meaningless)
	    if (!xy_intersect_p(p->extent, q->extent))
		continue;

	    // two polylines don't matter
	    // DEBUG: it really does if they're different colors
	    if (p->tag == BSP_POLYLINE && q->tag == BSP_POLYLINE)
		continue;

	    // two polygons
	    if (p->tag == BSP_POLYGON && q->tag == BSP_POLYGON) {

		// p is contained wholly in the back halfspace of q
		polygon_node = (BSP_POLYGON_NODE *) p;
		plane = ((BSP_POLYGON_NODE *) q)->plane;
		side = polygon_side_of_plane(polygon_node->polygon, plane);
		if (side == S_ON ||
		    (plane->n[Z] >= 0 && side == S_IN) ||
		    (plane->n[Z] <= 0 && side == S_OUT))
		    continue;

		// q is contained wholly in the front halfspace of p
		polygon_node = (BSP_POLYGON_NODE *) q;
		plane = ((BSP_POLYGON_NODE *) p)->plane;
		side = polygon_side_of_plane(polygon_node->polygon, plane);
		if (side == S_ON ||
		    (plane->n[Z] >= 0 && side == S_OUT) ||
		    (plane->n[Z] <= 0 && side == S_IN))
		    continue;

		// projections do not overlap
		++n_ppos;
		if (!projections_overlap_p
		    ((BSP_POLYGON_NODE *) p, (BSP_POLYGON_NODE *) q))
		    continue;
	    }

	    if (p->tag == BSP_POLYLINE) {	// and q is a polygon
		polyline_node = (BSP_POLYLINE_NODE *) p;
		plane = ((BSP_POLYGON_NODE *) q)->plane;
		side =
		    polyline_side_of_plane(polyline_node->polyline, plane);

		// line is contained wholly in the back halfspace of plane
		// lines lying on plane should be swapped so plane is drawn first
		if ((plane->n[Z] >= 0 && side == S_IN) ||
		    (plane->n[Z] <= 0 && side == S_OUT))
		    continue;

		// projections do not overlap
		++n_plos;
		if (!polyline_projection_overlaps_polygon
		    (polyline_node, (BSP_POLYGON_NODE *) q))
		    continue;
	    }

	    if (q->tag == BSP_POLYLINE) {	// and p is a polygon
		polyline_node = (BSP_POLYLINE_NODE *) q;
		plane = ((BSP_POLYGON_NODE *) p)->plane;
		side =
		    polyline_side_of_plane(polyline_node->polyline, plane);

		// line is on or contained wholly in the front halfspace of the plane
		// a line lying on the plane can stay where it is
		if (side == S_ON ||
		    (plane->n[Z] >= 0 && side == S_OUT) ||
		    (plane->n[Z] <= 0 && side == S_IN))
		    continue;

		// projections do not overlap
		++n_plos;
		if (!polyline_projection_overlaps_polygon
		    (polyline_node, (BSP_POLYGON_NODE *) p))
		    continue;
	    }

	    if (q->mark) {

		// we've discovered an intersection or cyclic overlap; break it by 
		// putting all the marked nodes in a bsp, then pulling them
		// out and inserting them back on the list; remember our bsps
		// need all polygons inserted before all polylines

		++n_bsps;
		sub_bsp = NULL;
		n_in = 0;
		t = NULL;	// use t to hold polylines temporarily
		while (p && p->mark) {
		    p_next = p->next;

		    if (p->tag == BSP_POLYGON) {
			p->next = p->prev = NULL;
			insert_polygon(&sub_bsp, (BSP_POLYGON_NODE *) p);
		    } else {	// save polyline temporarily
			p->next = t;
			t = p;
		    }
		    ++n_in;
		    p = p_next;
		    if (p)
			p->prev = NULL;
		}
		// insert the polylines now that all polygons are complete
		while (t) {
		    t_next = t->next;
		    t->next = t->prev = NULL;
		    insert_polyline(&sub_bsp, (BSP_POLYLINE_NODE *) t);
		    t = t_next;
		}

		// now traverse the bsp to get the objects back out, including split ones
		env->n = 0;
		env->head = env->tail = NULL;
		traverse_bsp(sub_bsp, make_list_of_bsp, env);
		n_out = env->n;

		// splitting should always increase the number of primitives, but
		// polygons very close in depth can cause split to fail; just shovel
		// the result polygons to the output with a warning.
		if (n_out <= n_in) {
		    warn(no_line, "split failed in=%d, out=%d", n_in,
			 n_out);
		    remark(no_line,
			   "if hidden surfaces are wrong, try -b option");
		    for (t = env->head; t; t = t_next) {
			t_next = t->next;
			t->next = r;
			t->in = t->out = t->on = t->prev = t->mark = NULL;
			r = t;
		    }
		    goto restart_overlap_check;
		}
		// re-insert in the sorted list
		for (t = env->head; t; t = t_next) {
		    t_next = t->next;
		    t->in = t->out = t->on = t->prev = t->next = t->mark =
			NULL;
		    insert_by_depth(&p, t);
		}

		n_bsp_in_nodes += n_in;
		n_bsp_out_nodes += n_out;

		goto restart_overlap_check;
	    } else {

		// no overlap, so pull q forward to head of list

		// unlink q
		if (q->next)
		    q->next->prev = q->prev;
		q->prev->next = q->next;

		// mark
		q->mark = p;

		// push 
		q->prev = NULL;

		q->next = p;
		p->prev = q;
		p = q;

		++n_swaps;

		goto restart_overlap_check;
	    }
	}

	// overlap check saw no problems; pop head onto return list
	p_next = p->next;
	if (p_next)
	    p_next->prev = NULL;

	// push on output
	p->next = r;
	p->prev = NULL;
	r = p;

	// move to next item
	p = p_next;
    }
    // pop from q and push onto q until q is empty
    q = r;
    r = NULL;
    while (q) {
	t = q;
	q = q->next;		// pop
	t->next = r;
	r = t;			// push
    }

    {
	int n_probes_possible = n_nodes + n_bsp_out_nodes - n_bsp_in_nodes;

	remark(no_line,
	       "node=%d probe=%.1lf swap=%d split=%d (in=%d out=%d) ols=%d/%d",
	       n_nodes,
	       (n_probes_possible >
		0) ? (double) n_probes / n_probes_possible : 0.0, n_swaps,
	       n_bsps, n_bsp_in_nodes, n_bsp_out_nodes, n_ppos, n_plos);
    }

    *bsp = r;
}
