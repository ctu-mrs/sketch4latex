/* geomio.h
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

#ifndef __GEOMIO_H
#define __GEOMIO_H

#include <stdio.h>
#include "geometry.h"

void print_pt_2d(FILE * f, POINT_2D pt);
void print_pt_3d(FILE * f, POINT_3D pt);
void print_polyline_3d(FILE * f, POLYLINE_3D * polyline);
void print_polygon_2d(FILE * f, POLYGON_2D * polygon);
void print_polygon_3d(FILE * f, POLYGON_3D * polygon);
void print_plane(FILE * f, PLANE * plane);


#endif
