/* global.c
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

#include "global.h"

GLOBAL_ENV __the_global_env[1];

// must match GEOL_lang definitions in global.h
char *output_language_str[] = {
    "PSTricks,LaTeX",
    "PGF/TikZ,LaTeX",
    "PSTricks,ConTeXt",
    "PGF/TikZ,ConTeXt",
};

void
init_global_env(GLOBAL_ENV * env,
		PST_VERSION * pst_version, TIKZ_VERSION * tikz_version)
{
    env->set_p_mask = 0;
    set_ident(env->camera);
    copy_pt_3d(env->bb1, origin_3d);
    copy_pt_3d(env->bb2, origin_3d);
    init_opts(env->opts);
    env->baseline = 0;
    env->output_language = GEOL_PSTRICKS | GEOL_LATEX;
    *env->pst_version = *pst_version;
    *env->tikz_version = *tikz_version;
}

void set_global_baseline(GLOBAL_ENV * env, FLOAT baseline, SRC_LINE line)
{
    if (baseline == NO_BASELINE)
	return;
    if (global_env_is_set_p(env, GE_BASELINE)) {
	err(line, "baseline is already set");
	return;
    }
    env->set_p_mask |= bit(GE_BASELINE);
    env->baseline = baseline;
}

void
set_global_env_extent(GLOBAL_ENV * env, POINT_3D p1, POINT_3D p2,
		      SRC_LINE line)
{
    if (global_env_is_set_p(env, GE_EXTENT)) {
	err(line, "bounding box is already set");
	return;
    }
    env->set_p_mask |= bit(GE_EXTENT);
    copy_pt_3d(env->bb1, p1);
    copy_pt_3d(env->bb2, p2);
}

int get_transformed_global_env_extent(BOX_3D * extent, GLOBAL_ENV * env)
{
    POINT_3D t;
    int i;

    if (!global_env_is_set_p(env, GE_EXTENT))
	return 0;

    init_box_3d(extent);
    if (global_env_is_set_p(env, GE_CAMERA)) {
	for (i = 0; i < 8; i++) {
	    t[X] = (i & 1) ? env->bb1[X] : env->bb2[X];
	    t[Y] = (i & 2) ? env->bb1[Y] : env->bb2[Y];
	    t[Z] = (i & 4) ? env->bb1[Z] : env->bb2[Z];
	    transform_pt_3d(t, env->camera, t);
	    fold_min_max_pt_3d(extent, t);
	}
    } else {
	fold_min_max_pt_3d(extent, env->bb1);
	fold_min_max_pt_3d(extent, env->bb2);
    }
    return 1;
}

void set_global_env_opts(GLOBAL_ENV * env, char *opt_str, SRC_LINE line)
{
    if (global_env_is_set_p(env, GE_OPTS)) {
	err(line, "global options are already set");
	return;
    }
    env->set_p_mask |= bit(GE_OPTS);
    setup_opts(env->opts, opt_str, no_line);
}

void set_global_env_frame(GLOBAL_ENV * env, char *opt_str, SRC_LINE line)
{
    if (global_env_is_set_p(env, GE_FRAME)) {
	err(line, "frame is already set");
	return;
    }
    env->set_p_mask |= bit(GE_FRAME);
    safe_free(env->frame_opts);
    env->frame_opts = opt_str ? safe_strdup(opt_str) : NULL;
}

void
set_global_env_camera(GLOBAL_ENV * env, TRANSFORM camera, SRC_LINE line)
{
    if (global_env_is_set_p(env, GE_CAMERA)) {
	err(line, "camera is already set");
	return;
    }
    env->set_p_mask |= bit(GE_CAMERA);
    copy_transform(env->camera, camera);
}

void
set_global_output_language(GLOBAL_ENV * env, int output_language,
			   SRC_LINE line)
{
    if (global_env_is_set_p(env, GE_OUTPUT_LANGUAGE)) {
	err(line, "output language is already set");
	return;
    }
    env->set_p_mask |= bit(GE_OUTPUT_LANGUAGE);
    env->output_language = output_language;
}

int get_global_output_language(int *output_language, GLOBAL_ENV * env)
{
    *output_language = env->output_language;
    return global_env_is_set_p(env, GE_OUTPUT_LANGUAGE);
}

int cmp_with_global_pst_version(GLOBAL_ENV * env,
				char *version_str, SRC_LINE line)
{
    PST_VERSION v[1];

    parse_pst_version(v, version_str, line);
    return pst_version_cmp(env->pst_version, v);
}

int cmp_with_global_tikz_version(GLOBAL_ENV * env,
				 char *version_str, SRC_LINE line)
{
    PST_VERSION v[1];

    parse_tikz_version(v, version_str, line);
    return tikz_version_cmp(env->tikz_version, v);
}
