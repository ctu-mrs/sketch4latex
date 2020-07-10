/* emit.c
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

#include <math.h>
#include "emit.h"
#include "scene.h"
#include "version.h"

// ---- emit output -----------------------------------------------------------

char standard_us_doc_template_file_name_flag[] =
    "<standard us doc template file name flag>";
char standard_euro_doc_template_file_name_flag[] =
    "<standard euro doc template file name flag>";

// concise floating point output
// idea of %g but with fixed rather than relative precision
// removing excessive 0's often reduces output file size dramatically
// and also eases reading
char *flt_str_fmt(char *fmt, char *buf, double f)
{
    size_t i;

    sprintf(buf, fmt, f);

    // trim off useless zeros and decimals
    for (i = strlen(buf); i > 0 && buf[i - 1] == '0'; i--)
	/* skip */ ;
    if (i > 0 && buf[i - 1] == '.')
	i--;
    buf[i] = '\0';

    // remove leading zeros before decimal
    if (buf[0] == '0' && buf[1] == '.')
	for (i = 0; (buf[i] = buf[i + 1]) != '\0'; i++)
	    /* skip */ ;
    else if (buf[0] == '-' && buf[1] == '0' && buf[2] == '.')
	for (i = 1; (buf[i] = buf[i + 1]) != '\0'; i++)
	    /* skip */ ;

    // fix -0
    if (strcmp(buf, "-0") == 0)
	strcpy(buf, "0");

    return buf;
}

char *flt_str(char *buf, double f)
{
    return flt_str_fmt("%.3f", buf, f);
}

// TikZ only does integer angles
char *fmt_angle_tikz(char *buf, double theta, SRC_LINE line)
{
    int i_theta = (int) ((theta >= 0) ? (theta + 0.5) : (theta - 0.5));
    double err = theta - i_theta;
    if (fabs(err) >= 0.1)
	warn(line, "TikZ angle rounding error is %.2 degrees", err);
    return flt_str_fmt("%1.f", buf, theta);
}

char *fmt_angle_pst(char *buf, double theta, SRC_LINE line)
{
    return flt_str(buf, theta);
}

typedef char *(*FMT_ANGLE_FUNC) (char *buf, double theta, SRC_LINE line);

FMT_ANGLE_FUNC fmt_angle_tbl[] = {
    fmt_angle_pst,
    fmt_angle_tikz,
    fmt_angle_pst,
    fmt_angle_tikz,
};

#define SA_SCALAR_BIT (1 << SA_SCALAR)
#define SA_POINT_BIT  (1 << SA_POINT)
#define SA_VECTOR_BIT (1 << SA_VECTOR)

SPECIAL_ARG *range_check(SPECIAL_OBJECT * special, int *p, int types,
			 SRC_LINE line)
{
    int i = *p;
    if (1 <= i && i <= special->args->n_args) {
	SPECIAL_ARG *arg = &special->args->arg[i - 1];
	if (arg->tag == SA_SCALAR && (types & SA_SCALAR_BIT) == 0)
	    err(line, "reference to special arg %d cannot accept a scalar",
		i);
	if (arg->tag == SA_POINT && (types & SA_POINT_BIT) == 0)
	    err(line, "reference to special arg %d cannot accept a point",
		i);
	if (arg->tag == SA_VECTOR && (types & SA_VECTOR_BIT) == 0)
	    err(line, "reference to special arg %d cannot accept a vector",
		i);
	return arg;
    }
    err(line, "reference to special arg %d: out of range [1-%d]",
	i, special->args->n_args);
    *p = i = (i < 1) ? 1 : special->args->n_args;
    return &special->args->arg[i - 1];
}

static int
process_special_arg(FILE * f, SPECIAL_OBJECT * special, int p,
		    SRC_LINE line)
{
    char tic, buf1[16], buf2[16], buf3[16];
    int i1, i2, len;
    FLOAT *v1, *v2;
    SPECIAL_ARG *arg1, *arg2;

    // try #, two-arg cases, then one with tic, then one, then assume zero
    if (special->code[p] == '#') {
	if (f)
	    fprintf(f, "#");
	len = 1;
    } else if (sscanf(&special->code[p], "%d-%d%n", &i1, &i2, &len) >= 2 ||
	       sscanf(&special->code[p], "{%d-%d}%n", &i1, &i2,
		      &len) >= 2) {
	arg1 = range_check(special, &i1, SA_POINT_BIT, line);
	arg2 = range_check(special, &i2, SA_POINT_BIT, line);
	v1 = arg1->val.pt;
	v2 = arg2->val.pt;
	if (f)
	    fprintf(f, "{%s}",
		    (*fmt_angle_tbl[global_env->output_language])
		    (buf1, 180 / PI * atan2(v2[Y] - v1[Y], v2[X] - v1[X]),
		     line));
    } else if (sscanf(&special->code[p], "%d'%c%n", &i1, &tic, &len) >= 2) {
	arg1 =
	    range_check(special, &i1, (SA_POINT_BIT | SA_VECTOR_BIT),
			line);
	v1 = (arg1->tag == SA_POINT) ? arg1->val.pt : arg1->val.v;
	switch (tic) {
	case 'x':
	case 'y':
	case 'z':
	    if (f)
		fprintf(f, "%s", flt_str(buf1, v1[tic - 'x']));
	    break;
	case '2':
	    if (f)
		fprintf(f, "(%s,%s)",
			flt_str(buf1, v1[X]), flt_str(buf2, v1[Y]));
	    break;
	case '3':
	    if (f)
		fprintf(f, "(%s,%s,%s)",
			flt_str(buf1, v1[X]),
			flt_str(buf2, v1[Y]), flt_str(buf3, v1[Z]));
	    break;
	default:
	    if (f)
		fprintf(f, "(%s,%s)'%c",
			flt_str(buf1, v1[X]), flt_str(buf2, v1[Y]), tic);
	    else
		warn(line,
		     "bad tick option character in #%d'%c (try #{%d}'%c)",
		     i1, tic, i1, tic);
	    break;
	}
    } else if (sscanf(&special->code[p], "%d%n", &i1, &len) >= 1 ||
	       sscanf(&special->code[p], "{%d}%n", &i1, &len) >= 1) {
	// simple single arg can be scalar or point
	arg1 =
	    range_check(special, &i1,
			(SA_SCALAR_BIT | SA_POINT_BIT | SA_VECTOR_BIT),
			line);
	switch (arg1->tag) {
	case SA_SCALAR:
	    if (f)
		fprintf(f, "%s", flt_str(buf1, arg1->val.flt));
	    break;
	case SA_POINT:
	case SA_VECTOR:
	    v1 = (arg1->tag == SA_POINT) ? arg1->val.pt : arg1->val.v;
	    if (f)
		fprintf(f, "(%s,%s)",
			flt_str(buf1, v1[X]), flt_str(buf2, v1[Y]));
	    break;
	default:
	    die(line, "unknown special arg from flatten");
	    break;
	}
    } else {
	len = 0;
	if (f)
	    fprintf(f, "#");
	else
	    warn(line, "use of '#' not as special arg (try ##)");
    }
    return p + len;
}

void process_special(FILE * f, SPECIAL_OBJECT * special, SRC_LINE line)
{
    int p, q;

    // print stretches of code between args and call the
    // arg processor to take care of them
    p = q = 0;
    while (special->code[p] != '\0') {
	if (special->code[p] == '#') {
	    if (f)
		fprintf(f, "%.*s", p - q, &special->code[q]);
	    p = q = process_special_arg(f, special, p + 1, line);
	} else {
	    p++;
	}
    }
    if (f)
	fprintf(f, "%.*s", p - q, &special->code[q]);
}

static void emit_points_pst(FILE * f, POINT_LIST_3D * pts)
{
    int i;
    char buf1[16], buf2[16];

    for (i = 0; i < pts->n_pts; i++)
	fprintf(f, "(%s,%s)",
		flt_str(buf1, pts->v[i][X]), flt_str(buf2, pts->v[i][Y]));
}

static void emit_dots_pst(FILE * f, OBJECT * obj)
{
    DOTS_OBJECT *dots = (DOTS_OBJECT *) obj;
    fprintf(f, "\\psdots");
    emit_opts(f, dots->opts, global_env->output_language);
    emit_points_pst(f, dots->pts);
    fprintf(f, "\n");
}

static void emit_line_pst(FILE * f, OBJECT * obj)
{
    LINE_OBJECT *line = (LINE_OBJECT *) obj;
    fprintf(f, "\\psline");
    emit_opts(f, line->opts, global_env->output_language);
    emit_points_pst(f, line->pts);
    fprintf(f, "\n");
}

static void emit_curve_pst(FILE * f, OBJECT * obj)
{
    CURVE_OBJECT *curve = (CURVE_OBJECT *) obj;
    fprintf(f, "\\pscurve");
    emit_opts(f, curve->opts, global_env->output_language);
    emit_points_pst(f, curve->pts);
    fprintf(f, "\n");
}

static void emit_polygon_pst(FILE * f, OBJECT * obj)
{
    POLYGON_OBJECT *poly = (POLYGON_OBJECT *) obj;
    fprintf(f, "\\pspolygon");
    emit_opts(f, poly->opts, global_env->output_language);
    emit_points_pst(f, poly->pts);
    fprintf(f, "\n");
}

static void emit_special_pst(FILE * f, OBJECT * obj)
{
    process_special(f, (SPECIAL_OBJECT *) obj, no_line);
}

typedef void (*EMIT_FUNC) (FILE * f, OBJECT *);

static EMIT_FUNC emit_tbl_pst[] = {
    NULL,			// O_BASE
    NULL,			// O_TAG_DEF
    NULL,			// O_OPTS_DEF
    NULL,			// O_SCALAR_DEF
    NULL,			// O_POINT_DEF
    NULL,			// O_VECTOR_DEF
    NULL,			// O_TRANSFORM_DEF
    emit_dots_pst,
    emit_line_pst,
    emit_curve_pst,
    emit_polygon_pst,
    emit_special_pst,
    NULL,			// O_SWEEP (flattened)
    NULL,			// O_REPEAT (flattened)
    NULL,			// O_COMPOUND (flattened)
};

static void
emit_points_tkz(FILE * f, POINT_LIST_3D * pts, char *twixt, char *final)
{
    int i;
    char buf1[16], buf2[16];

    for (i = 0; i < pts->n_pts; i++)
	fprintf(f, "(%s,%s)%s",
		flt_str(buf1, pts->v[i][X]),
		flt_str(buf2, pts->v[i][Y]),
		(i == pts->n_pts - 1) ? final : twixt);
}

static void emit_dots_tkz(FILE * f, OBJECT * obj)
{
    static char *skip[] = { "dotsize", NULL };
    char *dotsize, *cmd;

    DOTS_OBJECT *dots = (DOTS_OBJECT *) obj;

    // An ugly hack because TikZ uses special syntax for circles...
    dotsize = opt_val(dots->opts, "dotsize");
    if (dotsize == NULL)
	dotsize = "2pt";
    cmd = safe_malloc(strlen(dotsize) + 100);
    sprintf(cmd, " circle (%s)", dotsize);

    fprintf(f, "\\filldraw");
    emit_opts_with_exceptions(f, dots->opts, skip,
			      global_env->output_language);
    emit_points_tkz(f, dots->pts, cmd, cmd);
    fprintf(f, ";\n");

    safe_free(cmd);
}

static void emit_line_tkz(FILE * f, OBJECT * obj)
{
    LINE_OBJECT *line = (LINE_OBJECT *) obj;
    fprintf(f, "\\draw");
    emit_opts(f, line->opts, global_env->output_language);
    emit_points_tkz(f, line->pts, "--", "");
    fprintf(f, ";\n");
}

static void emit_curve_tkz(FILE * f, OBJECT * obj)
{
    CURVE_OBJECT *curve = (CURVE_OBJECT *) obj;
    fprintf(f, "\\curve");
    emit_opts(f, curve->opts, global_env->output_language);
    emit_points_tkz(f, curve->pts, "--", "");
    fprintf(f, ";\n");
}

static void emit_polygon_tkz(FILE * f, OBJECT * obj)
{
    POLYGON_OBJECT *poly = (POLYGON_OBJECT *) obj;
    fprintf(f, "\\filldraw");
    emit_opts(f, poly->opts, global_env->output_language);
    emit_points_tkz(f, poly->pts, "--", "--cycle");
    fprintf(f, ";\n");
}

static void emit_special_tkz(FILE * f, OBJECT * obj)
{
    process_special(f, (SPECIAL_OBJECT *) obj, no_line);
}

static EMIT_FUNC emit_tbl_tkz[] = {
    NULL,			// O_BASE
    NULL,			// O_TAG_DEF
    NULL,			// O_OPTS_DEF
    NULL,			// O_SCALAR_DEF
    NULL,			// O_POINT_DEF
    NULL,			// O_VECTOR_DEF
    NULL,			// O_TRANSFORM_DEF
    emit_dots_tkz,
    emit_line_tkz,
    emit_curve_tkz,
    emit_polygon_tkz,
    emit_special_tkz,
    NULL,			// O_SWEEP (flattened)
    NULL,			// O_REPEAT (flattened)
    NULL,			// O_COMPOUND (flattened)
};

static EMIT_FUNC *emit_tbl_tbl[] = {
    emit_tbl_pst,
    emit_tbl_tkz,
    emit_tbl_pst,
    emit_tbl_tkz,
};

#define DOC_TEMPLATE_ESCAPE_STRING "%%SKETCH_OUTPUT%%"
#define DOC_TEMPLATE_ESCAPE_STRING_LEN (sizeof(DOC_TEMPLATE_ESCAPE_STRING) - 1)

char standard_us_doc_template_tikz_latex[] =
    "\\documentclass[letterpaper,12pt]{article}\n"
    "\\usepackage[x11names,rgb]{xcolor}\n"
    "\\usepackage{tikz}\n"
    "\\usetikzlibrary{snakes}\n"
    "\\usetikzlibrary{arrows}\n"
    "\\usetikzlibrary{shapes}\n"
    "\\usetikzlibrary{backgrounds}\n"
    "\\usepackage{amsmath}\n"
    "\\oddsidemargin 0in\n"
    "\\evensidemargin 0in\n"
    "\\topmargin 0in\n"
    "\\headheight 0in\n"
    "\\headsep 0in\n"
    "\\textheight 9in\n"
    "\\textwidth 6.5in\n"
    "\\begin{document}\n"
    "\\pagestyle{empty}\n"
    "\\vspace*{\\fill}\n"
    "\\begin{center}\n"
    DOC_TEMPLATE_ESCAPE_STRING "\n"
    "\\end{center}\n" "\\vspace*{\\fill}\n" "\\end{document}\n";

char standard_euro_doc_template_tikz_latex[] =
    "\\documentclass[a4paper,12pt]{article}\n"
    "\\usepackage[x11names,rgb]{xcolor}\n"
    "\\usepackage{tikz}\n"
    "\\usetikzlibrary{snakes}\n"
    "\\usetikzlibrary{arrows}\n"
    "\\usetikzlibrary{shapes}\n"
    "\\usetikzlibrary{backgrounds}\n"
    "\\usepackage{amsmath}\n"
    "\\oddsidemargin -10mm\n"
    "\\evensidemargin -10mm\n"
    "\\topmargin 5mm\n"
    "\\headheight 0cm\n"
    "\\headsep 0cm\n"
    "\\textheight 247mm\n"
    "\\textwidth 160mm\n"
    "\\begin{document}\n"
    "\\pagestyle{empty}\n"
    "\\vspace*{\\fill}\n"
    "\\begin{center}\n"
    DOC_TEMPLATE_ESCAPE_STRING "\n"
    "\\end{center}\n" "\\vspace*{\\fill}\n" "\\end{document}\n";

char standard_us_doc_template_pst_latex[] =
    "\\documentclass[letterpaper,12pt]{article}\n"
    "\\usepackage{amsmath}\n"
    "\\usepackage{pstricks}\n"
    "\\usepackage{pstricks-add}\n"
    "\\oddsidemargin 0in\n"
    "\\evensidemargin 0in\n"
    "\\topmargin 0in\n"
    "\\headheight 0in\n"
    "\\headsep 0in\n"
    "\\textheight 9in\n"
    "\\textwidth 6.5in\n"
    "\\begin{document}\n"
    "\\pagestyle{empty}\n"
    "\\vspace*{\\fill}\n"
    "\\begin{center}\n"
    DOC_TEMPLATE_ESCAPE_STRING "\n"
    "\\end{center}\n" "\\vspace*{\\fill}\n" "\\end{document}\n";

char standard_euro_doc_template_pst_latex[] =
    "\\documentclass[a4paper,12pt]{article}\n"
    "\\usepackage{amsmath}\n"
    "\\usepackage{pstricks}\n"
    "\\usepackage{pstricks-add}\n"
    "\\oddsidemargin -10mm\n"
    "\\evensidemargin -10mm\n"
    "\\topmargin 5mm\n"
    "\\headheight 0cm\n"
    "\\headsep 0cm\n"
    "\\textheight 247mm\n"
    "\\textwidth 160mm\n"
    "\\begin{document}\n"
    "\\pagestyle{empty}\n"
    "\\vspace*{\\fill}\n"
    "\\begin{center}\n"
    DOC_TEMPLATE_ESCAPE_STRING "\n"
    "\\end{center}\n" "\\vspace*{\\fill}\n" "\\end{document}\n";

/* ---------------------------------------------------------------------- */

char standard_us_doc_template_tikz_context[] =
    "\\usemodule[tikz] \\usetikzlibrary[snakes,arrows,shapes,backgrounds]\n"
    "\\setuppapersize[letter][letter]\n"
    "\\setuplayout[topspace=0in,backspace=0in,header=0in,footer=0in,height=middle,width=middle]\n"
    "\\setuppagenumbering[state=stop] % no page numbers\n"
    "\\starttext\n"
    "\\startalignment[middle]\n"
    DOC_TEMPLATE_ESCAPE_STRING "\n" "\\stopalignment\n" "\\stoptext\n";

char standard_euro_doc_template_tikz_context[] =
    "\\usemodule[tikz] \\usetikzlibrary[snakes,arrows,shapes,backgrounds]\n"
    "\\setuppapersize[a4][a4]\n"
    "\\setuplayout[topspace=0cm,backspace=0cm,header=0cm,footer=0cm,height=middle,width=middle]\n"
    "\\setuppagenumbering[state=stop] % no page numbers\n"
    "\\starttext\n"
    "\\startalignment[middle]\n"
    DOC_TEMPLATE_ESCAPE_STRING "\n" "\\stopalignment\n" "\\stoptext\n";

char standard_us_doc_template_pst_context[] =
    "PSTricks does not work with ConTeXt as of 1 Feb 2008.\n";

char standard_euro_doc_template_pst_context[] =
    "PSTricks does not work with ConTeXt as of 1 Feb 2008.\n";

char *standard_us_doc_template[] = {
    standard_us_doc_template_pst_latex,
    standard_us_doc_template_tikz_latex,
    standard_us_doc_template_pst_context,
    standard_us_doc_template_tikz_context,
};

char *standard_euro_doc_template[] = {
    standard_euro_doc_template_pst_latex,
    standard_euro_doc_template_tikz_latex,
    standard_euro_doc_template_pst_context,
    standard_euro_doc_template_tikz_context,
};

char *read_file_as_string(FILE * f)
{
    size_t len = 0;
    int buf_size = 1024;
    char *buf = safe_malloc(buf_size + 1);
    for (;;) {
	len += fread(buf + len, 1, buf_size - len, f);
	if (feof(f) || ferror(f)) {
	    buf[len] = '\0';
	    return buf;
	}
	buf_size *= 2;
	buf = safe_realloc(buf, buf_size + 1);
    }
}

char *doc_template_from_file(char *file_name, int output_language)
{
    FILE *f;
    char *r;

    if (file_name == NULL)
	return NULL;
    if (file_name == standard_us_doc_template_file_name_flag)
	return safe_strdup(standard_us_doc_template[output_language]);
    if (file_name == standard_euro_doc_template_file_name_flag)
	return safe_strdup(standard_euro_doc_template[output_language]);

    f = fopen(file_name, "r");
    if (!f) {
	err(no_line, "can't open document template '%s%' for input\n",
	    file_name);
	return safe_strdup(standard_us_doc_template_pst_latex);
    }
    r = read_file_as_string(f);
    fclose(f);
    return r;
}

void emit_preamble_pst_latex(FILE * f, BOX_3D * ext, GLOBAL_ENV * env)
{
    char buf1[16], buf2[16], buf3[16], buf4[16];

    if (global_env_is_set_p(env, GE_OPTS)) {
	fprintf(f, "\\psset{");
	emit_opts_raw(f, env->opts, global_env->output_language);
	fprintf(f, "}\n");
    }

    if (global_env_is_set_p(env, GE_FRAME)) {
	if (env->frame_opts)
	    fprintf(f, "\\psframebox[%s]{", env->frame_opts);
	else
	    fprintf(f, "\\psframebox[framesep=0pt]{");
    }

    fprintf(f, "\\begin{pspicture%s}",
	    global_env_is_set_p(env, GE_EXTENT) ? "*" : "");

    if (global_env_is_set_p(env, GE_BASELINE))
	fprintf(f, "[%s]", flt_str(buf1, env->baseline));

    fprintf(f,
	    "(%s,%s)(%s,%s)\n",
	    flt_str(buf1, ext->min[X]),
	    flt_str(buf2, ext->min[Y]),
	    flt_str(buf3, ext->max[X]), flt_str(buf4, ext->max[Y]));

    if (cmp_with_global_pst_version
	(env, STRINGIFY(PST_LINEJOIN_VERSION), no_line) < 0) {
	// old way to set linejoin
	fprintf(f, "\\pstVerb{1 setlinejoin}\n");
    } else {
	fprintf(f,
		"%% If your PSTricks is earlier than Version "
		STRINGIFY(PST_LINEJOIN_VERSION) ", it will fail here.\n"
		"%% Use sketch -V option for backward compatibility.\n"
		"\\psset{linejoin=1}\n");
    }
}

void emit_preamble_tkz_latex(FILE * f, BOX_3D * ext, GLOBAL_ENV * env)
{
    char buf1[16], buf2[16], buf3[16], buf4[16];

    if (global_env_is_set_p(env, GE_FRAME)) {
	if (env->frame_opts)
	    warn(no_line, "frame options [%s] ignored (TikZ)",
		 env->frame_opts);
	else {
	    fprintf(f, "{\\fboxsep=0pt\\fbox{");
	    warn(no_line,
		 "remove frame around TikZ/PGF pictures for debugging");
	}
    }

    if (cmp_with_global_tikz_version
	(env, STRINGIFY(TIKZ_DECORATIONS_VERSION), no_line) >= 0) {
	remark(no_line,
	       "TikZ warning about deprecated decorations library is okay to ignore");
    }

    fprintf(f, "\\begin{tikzpicture}[line join=round");
    if (global_env_is_set_p(env, GE_OPTS)) {
	fprintf(f, ",");
	emit_opts_raw(f, env->opts, global_env->output_language);
    }
    if (global_env_is_set_p(env, GE_BASELINE)) {
	fprintf(f, ",");
	fprintf(f, "baseline=%s", flt_str(buf1, env->baseline));
    }
    fprintf(f, "]\n");
    if (global_env_is_set_p(env, GE_EXTENT)) {
	flt_str(buf1, ext->min[X]);
	flt_str(buf2, ext->min[Y]);
	flt_str(buf3, ext->max[X]);
	flt_str(buf4, ext->max[Y]);
	fprintf(f,
		"\\useasboundingbox(%s,%s) rectangle (%s,%s);\n"
		"\\clip(%s,%s) rectangle (%s,%s);\n",
		buf1, buf2, buf3, buf4, buf1, buf2, buf3, buf4);
    }
}

// -----------------------------------------------------------------

void emit_preamble_pst_context(FILE * f, BOX_3D * ext, GLOBAL_ENV * env)
{
    char buf1[16], buf2[16], buf3[16], buf4[16];

    if (global_env_is_set_p(env, GE_OPTS)) {
	fprintf(f, "\\psset{");
	emit_opts_raw(f, env->opts, global_env->output_language);
	fprintf(f, "}\n");
    }

    fprintf(f,
	    "%% ConTeXt does not yet support PSTricks.\n"
	    "%% This is a guess at what the syntax might be.\n");

    if (global_env_is_set_p(env, GE_FRAME)) {
	if (env->frame_opts)
	    fprintf(f, "\\psframebox[%s]{", env->frame_opts);
	else
	    fprintf(f, "\\psframebox[framesep=0pt]{");
    }

    fprintf(f, "\\startpspicture%s",
	    global_env_is_set_p(env, GE_EXTENT) ? "*" : "");

    if (global_env_is_set_p(env, GE_BASELINE))
	fprintf(f, "[%s]", flt_str(buf1, env->baseline));

    fprintf(f,
	    "(%s,%s)(%s,%s)\n",
	    flt_str(buf1, ext->min[X]),
	    flt_str(buf2, ext->min[Y]),
	    flt_str(buf3, ext->max[X]), flt_str(buf4, ext->max[Y]));

    fprintf(f, "\\pstVerb{1 setlinejoin}\n");
}

void emit_preamble_tkz_context(FILE * f, BOX_3D * ext, GLOBAL_ENV * env)
{
    char buf1[16], buf2[16], buf3[16], buf4[16];

    if (global_env_is_set_p(env, GE_FRAME)) {
	if (env->frame_opts)
	    warn(no_line, "frame options [%s] ignored (TikZ)",
		 env->frame_opts);
	else {
	    fprintf(f, "{\\fboxsep=0pt\\fbox{");
	    warn(no_line,
		 "remove frame around TikZ/PGF pictures for debugging");
	}
    }

    fprintf(f, "\\starttikzpicture[join=round");
    if (global_env_is_set_p(env, GE_OPTS)) {
	fprintf(f, ",");
	emit_opts_raw(f, env->opts, global_env->output_language);
    }
    if (global_env_is_set_p(env, GE_BASELINE)) {
	fprintf(f, ",");
	fprintf(f, "baseline=%s", flt_str(buf1, env->baseline));
    }
    fprintf(f, "]\n");
    if (global_env_is_set_p(env, GE_EXTENT)) {
	flt_str(buf1, ext->min[X]);
	flt_str(buf2, ext->min[Y]);
	flt_str(buf3, ext->max[X]);
	flt_str(buf4, ext->max[Y]);
	fprintf(f,
		"\\useasboundingbox(%s,%s) rectangle (%s,%s);\n"
		"\\clip(%s,%s) rectangle (%s,%s);\n",
		buf1, buf2, buf3, buf4, buf1, buf2, buf3, buf4);
    }
}

typedef void (*EMIT_PREAMBLE_FUNC) (FILE * f, BOX_3D * ext,
				    GLOBAL_ENV * env);

EMIT_PREAMBLE_FUNC emit_preamble_tbl[] = {
    emit_preamble_pst_latex,
    emit_preamble_tkz_latex,
    emit_preamble_pst_context,
    emit_preamble_tkz_context,
};

void emit_postamble_pst_latex(FILE * f, GLOBAL_ENV * env)
{
    fprintf(f, "\\end{pspicture%s}",
	    global_env_is_set_p(env, GE_EXTENT) ? "*" : "");
    if (global_env_is_set_p(env, GE_FRAME))
	fprintf(f, "}");
}

void emit_postamble_tkz_latex(FILE * f, GLOBAL_ENV * env)
{
    fprintf(f, "\\end{tikzpicture}");
    if (global_env_is_set_p(env, GE_FRAME))
	fprintf(f, "}}");
}

void emit_postamble_pst_context(FILE * f, GLOBAL_ENV * env)
{
    fprintf(f, "\\stoppspicture%s}",
	    global_env_is_set_p(env, GE_EXTENT) ? "*" : "");
    if (global_env_is_set_p(env, GE_FRAME))
	fprintf(f, "}");
}

void emit_postamble_tkz_context(FILE * f, GLOBAL_ENV * env)
{
    fprintf(f, "\\stoptikzpicture");
    if (global_env_is_set_p(env, GE_FRAME))
	fprintf(f, "}}");
}

typedef void (*EMIT_POSTAMBLE_FUNC) (FILE * f, GLOBAL_ENV * env);

EMIT_POSTAMBLE_FUNC emit_postamble_tbl[] = {
    emit_postamble_pst_latex,
    emit_postamble_tkz_latex,
    emit_postamble_pst_context,
    emit_postamble_tkz_context,
};

void
emit(FILE * f, OBJECT * obj, GLOBAL_ENV * env,
     char *doc_template_file_name)
{
    BOX_3D ext[1];
    int n_obj;
    OBJECT *p;
    char buf1[16], buf2[16], buf3[16], buf4[16];
    char *escape, *doc_template;

    doc_template =
	doc_template_from_file(doc_template_file_name,
			       env->output_language);

    get_extent(obj, ext, &n_obj);
    if (n_obj == 0)
	err(no_line, "no objects to write");
    else {

	remark(no_line, "scene bb=(%s,%s)(%s,%s)",
	       flt_str(buf1, ext->min[X]),
	       flt_str(buf2, ext->min[Y]),
	       flt_str(buf3, ext->max[X]), flt_str(buf4, ext->max[Y]));

	if (get_transformed_global_env_extent(ext, env)) {
	    remark(no_line, "actual bb=(%s,%s)(%s,%s)",
		   flt_str(buf1, ext->min[X]),
		   flt_str(buf2, ext->min[Y]),
		   flt_str(buf3, ext->max[X]), flt_str(buf4, ext->max[Y]));
	}

	remark(no_line, "writing %d objects", n_obj);

	fprintf(f,
		"%% Sketch output, version " VER_STRING "\n"
		"%% Output language: %s\n",
		output_language_str[env->output_language]);
	escape = NULL;
	if (doc_template) {
	    escape = strstr(doc_template, DOC_TEMPLATE_ESCAPE_STRING);
	    if (escape)
		fprintf(f, "%.*s", (int) (escape - doc_template),
			doc_template);
	    else
		warn(no_line,
		     "document template with no escape '%s' has been ignored",
		     DOC_TEMPLATE_ESCAPE_STRING);
	}

	(*emit_preamble_tbl[env->output_language]) (f, ext, env);

	for (p = obj; p; p = p->sibling) {
	    if (emit_tbl_tbl[global_env->output_language][p->tag] == NULL)
		die(no_line, "emit: bad tag %d", p->tag);
	    if (xy_overlap_p(p, ext))
		(*emit_tbl_tbl[global_env->output_language][p->tag]) (f,
								      p);
	}

	(*emit_postamble_tbl[env->output_language]) (f, env);

	if (escape) {
	    escape += DOC_TEMPLATE_ESCAPE_STRING_LEN;
	    fprintf(f, "%s", escape);
	    if (strstr(escape, DOC_TEMPLATE_ESCAPE_STRING))
		warn(no_line,
		     "more than one escape in document template; all but first ignored");
	}
	fprintf(f, "%% End sketch output\n");
    }
}
