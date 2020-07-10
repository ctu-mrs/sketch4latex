/* main.c
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
#include "main.h"
#include "cmdline.h"
#include "parse.h"
#include "emit.h"

static CMD_LINE_OPT_ENV wrap_env[1];

int main(int argc, char *argv[])
{
    OBJECT *scene, *hsr_scene;
    int ret;
    FILE *out_file;
    SYMBOL_TABLE *sym_tab;

    // create the outer symbol table
    sym_tab = new_scope(NULL);

    // make first pass through options to process those that are position-independent
    // save the rest in the wrap environment for later processing
    process_global_options(wrap_env, argc, argv, sym_tab);

    // die if there were errors parsing options
    if (trouble_p())
	report_errors();

    // if options show no interest in input
    if (wrap_env->skip_input_p) {
	if (wrap_env->n_files > 0)
	    warn(no_line, "input ignored");
	return 0;
    }

    if (wrap_env->out_file_name) {
	out_file = fopen(wrap_env->out_file_name, "w");
	if (!out_file)
	    err(no_line, "can't open '%s' for output",
		wrap_env->out_file_name);
    } else {
	out_file = stdout;
    }

    // die if there were errors preparing output stream
    if (trouble_p())
	report_errors();

    // set up the global environment for the parser
    init_global_env(global_env, wrap_env->pst_version,
		    wrap_env->tikz_version);

    // process first set of tag defs and set up first input file
    ret = yywrap();

    // quits if there were file opening errors in wrap
    if (ret == 0) {
	if (trouble_p())
	    report_errors();
    } else {
	set_lexer_file("<stdin>", stdin);
    }

    if (parse(sym_tab) != 0 && !trouble_p())
	// emit an error to ensure report_errors halts 
	err(line, "parse error");

    // quits if there is trouble
    report_errors();

    // flatten the object hierarchy into a scene
    scene = flat_scene(parsed_objects(), global_env);

    // painter's algorithm for HSR / HLR
    if (wrap_env->bsp_only_p) {
	hsr_scene = hsr_scene_with_bsp(scene);
    } else {
	hsr_scene = hsr_scene_with_depth_sort(scene);
    }

    // emit PStricks or TikZ
    emit(out_file, hsr_scene, global_env,
	 wrap_env->doc_template_file_name);

    return 0;
}

int yywrap(void)
{
    char *file_name = advance_to_next_file_name(wrap_env);
    if (file_name) {
	FILE *f = fopen(file_name, "r");
	if (f) {
	    set_lexer_file(file_name, f);
	    return 0;
	} else {
	    err(no_line, "can't open file '%s' for input", file_name);
	}
    }
    return 1;
}
