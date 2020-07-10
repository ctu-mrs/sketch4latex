/* cmdline.h
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

#ifndef __CMDLINE_H
#define __CMDLINE_H

#include "langver.h"
#include "symbol.h"

typedef struct cmd_opt_env_t {
    int i;			// index of opt currently being processed (init zero)
    int argc;			// cmd line param count
    char **argv;		// cmd line params
    SYMBOL_TABLE *sym_tab;	// symbol table
    int bsp_only_p;		// bsp only (default off)
    char *doc_template_file_name;	// document template file name (default NULL)
    char *out_file_name;	// output file name (default NULL for stdout)
    int skip_input_p;		// true for options where processing input makes no sense
    int n_files;		// number of input file names
    PST_VERSION pst_version[1];	// version of pstricks to generate for output
    TIKZ_VERSION tikz_version[1];	// version of tikz to generate for output
} CMD_LINE_OPT_ENV;

void usage(int exit_code);

// process argv[1..argc-1] to fill in env and prepare it for wrapping
void process_global_options(CMD_LINE_OPT_ENV * env, int argc, char **argv,
			    SYMBOL_TABLE * sym_tab);

// advance the environment initialized above until the next filename has been found
// return a pointer to the name or NULL if there is none
char *advance_to_next_file_name(CMD_LINE_OPT_ENV * env);

#endif
