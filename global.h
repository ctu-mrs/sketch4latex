/* global.h
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

#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "opts.h"
#include "error.h"
#include "geometry.h"
#include "langver.h"

#define GE_EXTENT           0
#define GE_BASELINE         1
#define GE_OPTS             2
#define GE_FRAME            3
#define GE_SPLIT            4
#define GE_CAMERA           8
#define GE_OUTPUT_LANGUAGE  9

// these must match table in opt.c and global.c
#define GEOL_GRAPHICS_BIT   1
#define GEOL_TEX_MACROS_BIT 2
#define GEOL_PSTRICKS       (GEOL_GRAPHICS_BIT*0)
#define GEOL_TIKZ           (GEOL_GRAPHICS_BIT*1)
#define GEOL_LATEX          (GEOL_TEX_MACROS_BIT*0)
#define GEOL_CONTEXT        (GEOL_TEX_MACROS_BIT*1)

// this must match string table in global.c
extern char *output_language_str[];

typedef struct global_env_t {
    unsigned set_p_mask;
    OPTS opts[1];
    POINT_3D bb1, bb2;		// bounding box 
    char *frame_opts;		// null if frame has none
    FLOAT baseline;
    int split_p;
    TRANSFORM camera;
    int output_language;
    PST_VERSION pst_version[1];
    TIKZ_VERSION tikz_version[1];
} GLOBAL_ENV;

#define global_env_is_set_p(Env, F) ((Env)->set_p_mask & bit(F))

void init_global_env(GLOBAL_ENV * env, PST_VERSION * pst_version,
		     TIKZ_VERSION * tikz_version);
void set_global_baseline(GLOBAL_ENV * env, FLOAT baseline, SRC_LINE line);
#define NO_BASELINE FLT_MAX
void set_global_env_extent(GLOBAL_ENV * env, POINT_3D p1, POINT_3D p2,
			   SRC_LINE line);
int get_transformed_global_env_extent(BOX_3D * extent, GLOBAL_ENV * env);
void set_global_env_opts(GLOBAL_ENV * env, char *opt_str, SRC_LINE line);
void set_global_env_frame(GLOBAL_ENV * env, char *opt_str, SRC_LINE line);
void set_global_env_camera(GLOBAL_ENV * env, TRANSFORM camera,
			   SRC_LINE line);
void set_global_output_language(GLOBAL_ENV * env, int output_language,
				SRC_LINE line);
int get_global_output_language(int *output_language, GLOBAL_ENV * env);
int cmp_with_global_pst_version(GLOBAL_ENV * env,
				char *version_str, SRC_LINE line);
int cmp_with_global_tikz_version(GLOBAL_ENV * env,
				 char *version_str, SRC_LINE line);

extern GLOBAL_ENV __the_global_env[1];

#define global_env  __the_global_env

#endif
