/* cmdline.c
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

#include <stdlib.h>
#include "cmdline.h"
#include "emit.h"
#include "memutil.h"
#include "parse.h"
#include "symbol.h"
#include "version.h"

void usage(int exit_code)
{
    fprintf(stderr, "\n"
	    "sketch [options] file1 [-D tag][-U tag] file2...\n"
	    "-h                 Print this message to standard error\n"
	    "-v                 Print version info to standard output\n"
	    "-V x.y             Tell Sketch your PSTricks or TikZ version is x.y\n"
	    "-d                 Debug parser (for developers)\n"
	    "-b                 Use BSP rather than Painters algorithm for HSR\n"
	    "-T[u|e][p[P|T]]    Produce a complete LaTeX document\n"
	    "  u                U.S. paper size (8.5 x 11 inches) (default)\n"
	    "  e                European A4 paper size (297 x 210 mm)\n"
	    "  p                Print document template to stdout\n"
	    "    P              Print PSTricks version of doc template (default)\n"
	    "    T              Print TikZ version of doc template\n"
	    "      L            Print LaTeX version of template (default)\n"
	    "      C            Print ConTeXt version of template\n"
	    "-t templatefile    Load user document template\n"
	    "                     (any text file with escape %%SKETCH_OUTPUT%%)\n"
	    "-o outfile         Put output in outfile (default is stdout)\n"
	    "-D tag             Define given tag\n"
	    "-U tag             Undefine given tag\n");
    exit(exit_code);
}

static int is_doc_template_file_name(char *s)
{
    return s[0] != '-' || s[1] == '\0';
}

// process argv[1..argc-1] to fill in env and prepare it for wrapping
void
process_global_options(CMD_LINE_OPT_ENV * env, int argc, char **argv,
		       SYMBOL_TABLE * sym_tab)
{
    int i, j;
    unsigned lang, set;

    // i = 0
    // argc = 0;
    // argv = NULL;
    // sym_tab = NULL;
    // bsp_only_p = false
    // doc_template_file_name = NULL
    // out_file_name = NULL
    // skip_input_p = false
    // n_files = 0
    SET_STRUCT_ZERO(env);
    env->sym_tab = sym_tab;
    parse_pst_version(env->pst_version, STRINGIFY(ASSUMED_PST_VERSION),
		      no_line);
    parse_tikz_version(env->tikz_version, STRINGIFY(ASSUMED_TIKZ_VERSION),
		       no_line);

    // we'll copy args that need to be processed in order with filenames here.
    // the convention will be that args with paramaters like -Dfoo will be 
    // separated into -D and foo, so we could end up doubling the number of args.
    env->argv = safe_malloc(2 * (argc - 1) * sizeof(char *));

    for (i = 1; i < argc; i++) {
	if (argv[i][0] == '-') {
	    switch (argv[i][1]) {
	    case 'b':
		env->bsp_only_p = 1;
		break;
	    case 'd':
		yydebug = 1;
		break;
	    case 'h':
		usage(0);
		break;
	    case 'D':
	    case 'U':
		env->argv[env->argc++] = argv[i];
		if (argv[i][2])
		    env->argv[env->argc++] = &argv[i][2];
		else if (i + 1 < argc)
		    env->argv[env->argc++] = argv[++i];
		else
		    err(no_line, "missing tag after %s", argv[i]);
		break;
	    case 'o':
		if (env->out_file_name)
		    err(no_line, "only one use of -o is allowed");
		if (argv[i][2])
		    env->out_file_name = &argv[i][2];
		else {
		    if (i + 1 < argc)
			env->out_file_name = &argv[++i][0];
		    else
			err(no_line, "missing file name after -o");
		}
		break;
	    case 't':
		if (argv[i][2])
		    env->doc_template_file_name = &argv[i][2];
		else if (i + 1 < argc
			 && is_doc_template_file_name(argv[i + 1]))
		    env->doc_template_file_name = argv[++i];
		else
		    err(no_line,
			"missing document template file name after -t");
		break;
	    case 'T':
		j = 2;
		if (argv[i][j] == 'e') {
		    env->doc_template_file_name =
			standard_euro_doc_template_file_name_flag;
		    ++j;
		} else if (argv[i][j] == 'u') {
		    env->doc_template_file_name =
			standard_us_doc_template_file_name_flag;
		    ++j;
		} else {
		    env->doc_template_file_name =
			standard_us_doc_template_file_name_flag;
		}
		if (argv[i][j] == 'p') {
		    lang = set = 0;
		    for (++j; argv[i][j]; ++j) {
			switch (argv[i][j]) {
			case 'P':
			    if (set & GEOL_GRAPHICS_BIT)
				err(no_line,
				    "-tP with two graphics package specs");
			    lang |= GEOL_PSTRICKS;
			    set |= GEOL_GRAPHICS_BIT;
			    break;
			case 'T':
			    if (set & GEOL_GRAPHICS_BIT)
				err(no_line,
				    "-tP with two graphics package specs");
			    lang |= GEOL_TIKZ;
			    set |= GEOL_GRAPHICS_BIT;
			    break;
			case 'L':
			    if (set & GEOL_TEX_MACROS_BIT)
				err(no_line,
				    "-tP with two macro package specs");
			    lang |= GEOL_LATEX;
			    set |= GEOL_TEX_MACROS_BIT;
			    break;
			case 'C':
			    if (set & GEOL_TEX_MACROS_BIT)
				err(no_line,
				    "-tP with two macro package specs");
			    lang |= GEOL_CONTEXT;
			    set |= GEOL_TEX_MACROS_BIT;
			    break;
			default:
			    err(no_line,
				"unrecognized language spec after -Tp, '%c'",
				argv[i][j]);
			}
		    }
		    printf("%% %s document template:\n%s",
			   output_language_str[lang],
			   doc_template_from_file(env->
						  doc_template_file_name,
						  lang));
		    env->skip_input_p = 1;
		} else if (argv[i][j] != '\0') {
		    err(no_line, "unrecognized modifier of option '-T%c'",
			argv[i][j]);
		}
		break;
	    case 'V':
		if (argv[i][2] != '\0') {
		    parse_pst_version(env->pst_version, &argv[i][2],
				      no_line);
		    parse_tikz_version(env->tikz_version, &argv[i][2],
				       no_line);
		} else {
		    if (i + 1 < argc) {
			++i;
			parse_pst_version(env->pst_version, &argv[i][0],
					  no_line);
			parse_tikz_version(env->pst_version, &argv[i][0],
					   no_line);
		    } else {
			err(no_line,
			    "missing PSTricks or TikZ version after -V");
		    }
		}
		break;
	    case 'v':
		fprintf(stdout, "%% sketch version %s\n", VER_STRING);
		fprintf(stdout, "%% assumes PSTricks version %s\n",
			env->pst_version->str);
		fprintf(stdout, "%% assumes TikZ version %s\n",
			env->tikz_version->str);
		fprintf(stdout, "%% use -V to select earlier versions\n");
		env->skip_input_p = 1;
		break;
	    default:
		err(no_line, "unrecognized option '%s'", argv[i]);
		break;
	    }
	} else {
	    // no leading -
	    ++env->n_files;
	    env->argv[env->argc++] = argv[i];
	}
    }
    env->argv = safe_realloc(env->argv, env->argc * sizeof(char *));
}

// advance the environment initialized above until the next filename
// has been found, processing non-global arguments
char *advance_to_next_file_name(CMD_LINE_OPT_ENV * env)
{
    for (; env->i < env->argc; env->i++) {
	if (env->argv[env->i][0] == '-') {
	    switch (env->argv[env->i][1]) {
	    case 'D':
		++env->i;
		(void) new_symbol(env->sym_tab, env->argv[env->i], NULL,
				  new_tag_def(), no_line);
		break;
	    case 'U':
		// this will produce an error message if tag doesn't exist
		++env->i;
		if (tag_exists_p(env->sym_tab, env->argv[env->i]))
		    remove_symbol(env->sym_tab, env->argv[env->i],
				  no_line);
		break;
	    default:
		die(no_line,
		    "advance_to_next_file_name: unexpected option %c",
		    env->argv[env->i][1]);
		break;
	    }
	} else {
	    return env->argv[env->i++];
	}
    }
    return NULL;
}
