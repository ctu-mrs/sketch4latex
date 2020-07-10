/* geomio.c
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

#include "geomio.h"

void print_pt_2d(FILE * f, POINT_2D pt)
{
    fprintf(f, "(%.3f,%.3f)", pt[X], pt[Y]);
}

void print_pt_3d(FILE * f, POINT_3D pt)
{
    fprintf(f, "(%.3f,%.3f,%.3f)", pt[X], pt[Y], pt[Z]);
}

void print_polyline_3d(FILE * f, POLYLINE_3D * polyline)
{
    int i;

    fprintf(f, "\\line");
    for (i = 0; i < polyline->n_vertices; i++)
	print_pt_3d(f, polyline->v[i]);
    fprintf(f, "\n");
}

void print_polygon_2d(FILE * f, POLYGON_2D * polygon)
{
    int i;

    fprintf(f, "\\polygon");
    for (i = 0; i < polygon->n_sides; i++)
	print_pt_2d(f, polygon->v[i]);
    fprintf(f, "\n");
}

void print_polygon_3d(FILE * f, POLYGON_3D * polygon)
{
    int i;

    fprintf(f, "\\polygon");
    for (i = 0; i < polygon->n_sides; i++)
	print_pt_3d(f, polygon->v[i]);
    fprintf(f, "\n");
}

void print_plane(FILE * f, PLANE * plane)
{
    fprintf(f, "\\plane[n=%.3f %.3f %.3f,p=%.3f %.3f %.3f,c=%.3f]\n",
	    plane->n[X], plane->n[Y], plane->n[Z],
	    plane->p[X], plane->p[Y], plane->p[Z], plane->c);
}
