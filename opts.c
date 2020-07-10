/* opts.c
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

#include "opts.h"
#include "geometry.h"

DECLARE_DYNAMIC_ARRAY_FUNCS(OPT_LIST, OPT, opt_list, elt, n_elts,
			    NO_OTHER_INIT);

// ---- useful string stuff ----------------------------------------------------
// slice a string using Perl/Python position indexing conventions
// (position == dst_size is always at the end of the string)

static int clipat0(int x) { return x < 0 ? 0 : x; }

char *str_slice(char *dst, int dst_size, char *src, int beg, int end)
{
    int len;

    if (dst_size > 0) {
	len = strlen(src);
	if (beg < 0)
	    beg = clipat0(len + beg);
	else if (beg > len)
	    beg = len;
	if (end < 0)
	    end = clipat0(len + end);
	else if (end > len)
	    end = len;
	len = end - beg;
	if (len <= 0) {
	    dst[0] = '\0';
	} else {
	    if (len >= dst_size)
		len = dst_size - 1;
	    memcpy(dst, &src[beg], len);
	    dst[len] = '\0';
	}
    }
    return dst;
}

// a modified version of C library strtok
// uses state variable p, which should be
// initially set to zero.
char *istrtok(int *p, char *s, char sep)
{
    int i, r;

    // advance r to next non-space character
    for (r = *p; s[r] == ' ' || s[r] == '\t'; r++)
	/* skip */ ;

    // if we're at terminating null, return null
    if (s[r] == '\0') {
	*p = r;
	return NULL;
    }
    // look for a separator character
    for (i = r; s[i] != '\0'; i++) {
	if (s[i] == sep) {
	    // found one; set to null char,
	    // advance state variable, and
	    // return pointer to first char
	    s[i] = '\0';
	    *p = i + 1;
	    return &s[r];
	}
    }
    // did not find a terminator, so this
    // is the last token; return it
    *p = i;
    return &s[r];
}

int str_last_occurance(char *src, char *set)
{
    int i;

    for (i = 0; src[i]; i++)
	/* skip */ ;
    for (--i; i >= 0 && !strchr(set, src[i]); --i)
	/* skip */ ;
    return i;
}

// ---- options ----------------------------------------------------------------

void init_opts(OPTS * opts)
{
    init_opt_list(opts->list);
}

OPTS *raw_opts(void)
{
    OPTS *r = safe_malloc(sizeof *r);
    init_opts(r);
    return r;
}

void setup_opts(OPTS * opts, char *opts_str, SRC_LINE line)
{
    int p_pair, p_side;
    char *pair, *key, *val, *buf;
    OPT *opt;

    clear_opts(opts);
    buf = safe_strdup(opts_str);
    p_pair = 0;
    while ((pair = istrtok(&p_pair, buf, ',')) != NULL) {
	p_side = 0;
	key = istrtok(&p_side, pair, '=');
	if (key == NULL) {
	    err(line, "null keyword in option");
	    key = "";
	}
	val = istrtok(&p_side, pair, ',');
	if (val == NULL) {
	    err(line, "null value in option");
	    val = "";
	}
	opt = pushed_opt_list_elt(opts->list);
	opt->key = safe_strdup(key);
	opt->val = safe_strdup(val);
    }
    safe_free(buf);
}

OPTS *new_opts(char *opts_str, SRC_LINE line)
{
    OPTS *r = raw_opts();
    setup_opts(r, opts_str, line);
    return r;
}

void clear_opts(OPTS * opts)
{
    int i;

    for (i = 0; i < opts->list->n_elts; ++i) {
	safe_free(opts->list->elt[i].key);
	safe_free(opts->list->elt[i].val);
    }
    clear_opt_list(opts->list);
}

char *opt_val(OPTS * opts, char *opt)
{
    int i;

    if (!opts)
	return 0;

    for (i = 0; i < opts->list->n_elts; i++)
	if (strcmp(opts->list->elt[i].key, opt) == 0)
	    return opts->list->elt[i].val;
    return NULL;
}

int bool_opt_p(OPTS * opts, char *opt, int default_p)
{
    char *r = opt_val(opts, opt);
    if (!r)
	return default_p;
    return strcmp(r, "false") != 0;	// all not false is true
}

typedef struct opt_desc_t {
    char *opt;
    int type;
} OPT_DESC;

typedef struct opt_desc_tbl_t {
    OPT_DESC *key_desc;
    int n_key_desc;
    OPT_DESC *val_desc;
    int n_val_desc;
} OPT_DESC_TBL;

static OPT_DESC key_tbl_pst[] = {
    {"arrows", OPT_LINE},
    {"cull", OPT_INTERNAL},
    {"dash", OPT_LINE},
    {"dotsep", OPT_LINE},
    {"fillcolor", OPT_POLYGON | OPT_FILL_COLOR},
    {"fillstyle", OPT_POLYGON | OPT_FILL_STYLE},
    {"lay", OPT_INTERNAL},
    {"linecolor", OPT_LINE},
    {"linestyle", OPT_LINE | OPT_LINE_STYLE},
    {"linewidth", OPT_LINE},
    {"opacity", OPT_POLYGON},
    {"showpoints", OPT_LINE | OPT_POLYGON},
    {"split", OPT_INTERNAL},
    {"strokeopacity", OPT_LINE},
    {"transpalpha", OPT_POLYGON}
};

OPT_DESC_TBL opt_desc_tbl_pst[1] = { {
				      key_tbl_pst, ARRAY_SIZE(key_tbl_pst),
				      NULL, 0}
};

static OPT_DESC opt_key_tbl_tikz[] = {
    {"arrows", OPT_LINE},
    {"cap", OPT_LINE},
    {"color", OPT_LINE | OPT_POLYGON | OPT_FILL_COLOR},
    {"cull", OPT_INTERNAL},
    {"dash pattern", OPT_LINE},
    {"dash phase", OPT_LINE},
    {"double distance", OPT_LINE},
    {"double", OPT_LINE | OPT_LINE_STYLE},
    {"draw", OPT_LINE | OPT_LINE_STYLE},
    {"draw opacity", OPT_LINE},
    {"fill", OPT_POLYGON | OPT_FILL_COLOR},
    {"fill opacity", OPT_POLYGON},
    {"fill style", OPT_POLYGON | OPT_FILL_COLOR | OPT_EMIT_VAL},
    {"join", OPT_LINE},
    {"lay", OPT_INTERNAL},
    {"line style", OPT_LINE | OPT_EMIT_VAL},
    {"line width", OPT_LINE},
    {"miter limit", OPT_LINE},
    {"pattern", OPT_POLYGON | OPT_FILL_COLOR},
    {"pattern color", OPT_POLYGON},
    {"split", OPT_INTERNAL},
    {"style", OPT_TYPE_IN_VAL | OPT_EMIT_VAL},
};

static OPT_DESC opt_val_tbl_tikz[] = {
    {"dashed", OPT_LINE},
    {"densely dashed", OPT_LINE},
    {"densely dotted", OPT_LINE},
    {"dotted", OPT_LINE},
    {"double", OPT_LINE},
    {"loosely dashed", OPT_LINE},
    {"loosely dotted", OPT_LINE},
    {"nearly opaque", OPT_POLYGON},
    {"nearly transparent", OPT_POLYGON},
    {"semithick", OPT_LINE},
    {"semitransparent", OPT_POLYGON},
    {"solid", OPT_LINE},
    {"thick", OPT_LINE},
    {"thin", OPT_LINE},
    {"transparent", OPT_POLYGON},
    {"ultra nearly transparent", OPT_POLYGON},
    {"ultra thick", OPT_LINE},
    {"ultra thin", OPT_LINE},
    {"very nearly transparent", OPT_POLYGON},
    {"very thick", OPT_LINE},
    {"very thin", OPT_LINE},
};

OPT_DESC_TBL opt_desc_tbl_tikz[1] = { {
				       opt_key_tbl_tikz,
				       ARRAY_SIZE(opt_key_tbl_tikz),
				       opt_val_tbl_tikz,
				       ARRAY_SIZE(opt_val_tbl_tikz),
				       }
};

int opt_index(char *opt, OPT_DESC * desc, int n_desc)
{
    int hi, lo, mid, cmp_val;

    hi = n_desc - 1;
    lo = 0;
    while (hi >= lo) {
	mid = (hi + lo) / 2;
	cmp_val = strcmp(opt, desc[mid].opt);
	if (cmp_val < 0)
	    hi = mid - 1;
	else if (cmp_val > 0)
	    lo = mid + 1;
	else
	    return mid;
    }
    return -1;
}

static OPT_DESC_TBL *lang_to_opt_desc_tbl[] = {
    opt_desc_tbl_pst,
    opt_desc_tbl_tikz,
    opt_desc_tbl_pst,
    opt_desc_tbl_tikz,
};

int simple_opt_type(OPT * opt, int default_type, int lang)
{
    OPT_DESC_TBL *desc;
    int i;

    if (lang < 0)
	return default_type;
    desc = lang_to_opt_desc_tbl[lang];
    i = opt_index(opt->key, desc->key_desc, desc->n_key_desc);
    return (i < 0) ? default_type : desc->key_desc[i].type;
}

int opt_type(OPT * opt, int default_type, int lang)
{
    OPT_DESC_TBL *desc;
    int i, type;

    type = simple_opt_type(opt, default_type, lang);
    if (type & OPT_TYPE_IN_VAL) {
	desc = lang_to_opt_desc_tbl[lang];
	i = opt_index(opt->val, desc->val_desc, desc->n_val_desc);
	if (i < 0)
	    return default_type;
	type = desc->val_desc[i].type;
    }
    return type;
}

typedef struct opts_desc_t {
    OPT *opts;
    int n_opts;
} OPTS_DESC;

OPT no_edges_opts_pst[] = {
    {"linestyle", "none"}
};

OPT no_edges_opts_tikz[] = {
    {"draw", "none"}
};

OPTS_DESC no_edges_opts_desc_tbl[] = {
    {no_edges_opts_pst, ARRAY_SIZE(no_edges_opts_pst)},
    {no_edges_opts_tikz, ARRAY_SIZE(no_edges_opts_tikz)},
    {no_edges_opts_pst, ARRAY_SIZE(no_edges_opts_pst)},
    {no_edges_opts_tikz, ARRAY_SIZE(no_edges_opts_tikz)},
};

static int any_opt_p(OPTS * opts, int type, int lang)
{
    int i;

    if (!opts)
	return 0;
    for (i = 0; i < opts->list->n_elts; i++)
	if (type & opt_type(&opts->list->elt[i], OPT_NONE, lang))
	    return 1;
    return 0;
}

static void add_default_opt(OPTS ** opts_ptr, OPT * default_opt, int lang)
{
    OPT *opt;
    OPTS *opts;
    int default_type;

    opts = *opts_ptr;
    default_type = opt_type(default_opt, OPT_NONE, lang) & OPT_DEFAULTS;
    if (any_opt_p(opts, default_type, lang))
	return;
    if (!opts)
	opts = raw_opts();
    opt = pushed_opt_list_elt(opts->list);
    opt->key = safe_strdup(default_opt->key);
    opt->val = safe_strdup(default_opt->val);
    *opts_ptr = opts;
}

static void
add_default_opts(OPTS ** opts_ptr, OPTS_DESC * opts_desc, int lang)
{
    int i;
    for (i = 0; i < opts_desc->n_opts; i++)
	add_default_opt(opts_ptr, &opts_desc->opts[i], lang);
}

void add_no_edges_default_opt(OPTS ** opts_ptr, int lang)
{
    add_default_opts(opts_ptr, &no_edges_opts_desc_tbl[lang], lang);
}

OPT solid_white_opts_pst[] = {
    {"fillstyle", "solid"},
    {"fillcolor", "white"}
};

OPT solid_white_opts_tikz[] = {
    {"fill", "white"}
};

OPTS_DESC solid_white_opts_desc_tbl[] = {
    {solid_white_opts_pst, ARRAY_SIZE(solid_white_opts_pst)},
    {solid_white_opts_tikz, ARRAY_SIZE(solid_white_opts_tikz)},
    {solid_white_opts_pst, ARRAY_SIZE(solid_white_opts_pst)},
    {solid_white_opts_tikz, ARRAY_SIZE(solid_white_opts_tikz)},
};

void add_solid_white_default_opt(OPTS ** opts_ptr, int lang)
{
    add_default_opts(opts_ptr, &solid_white_opts_desc_tbl[lang], lang);
}

void
check_opts(OPTS * opts,
	   int allowed, char *allowed_msg, int lang, SRC_LINE line)
{
    int i, type;

    if (!opts)
	return;

    for (i = 0; i < opts->list->n_elts; i++) {
	type = opt_type(&opts->list->elt[i], OPT_NONE, lang);
	if ((type & allowed) == 0)
	    warn(line, allowed_msg, opts->list->elt[i].key,
		 opts->list->elt[i].val);
    }
}

// selective copy for splitting option lists by type
OPTS *copy_opts(OPTS * opts, int type_mask, int lang)
{
    int i;
    OPTS *r;
    OPT *opt;

    if (!opts)
	return NULL;

    r = raw_opts();
    for (i = 0; i < opts->list->n_elts; i++)
	if (type_mask & opt_type(&opts->list->elt[i], OPT_NONE, lang)) {
	    opt = pushed_opt_list_elt(r->list);
	    opt->key = safe_strdup(opts->list->elt[i].key);
	    opt->val = safe_strdup(opts->list->elt[i].val);
	}
    return r;
}

OPTS *cat_opts(OPTS * dst, OPTS * src)
{
    int i;
    OPT *opt;

    for (i = 0; i < src->list->n_elts; i++) {
	opt = pushed_opt_list_elt(dst->list);
	opt->key = safe_strdup(src->list->elt[i].key);
	opt->val = safe_strdup(src->list->elt[i].val);
    }
    return dst;
}

// selective copy for splitting out line options and modifying arrows
OPTS *copy_line_opts(OPTS * opts, int first_p, int last_p, int lang)
{
    int i;
    OPTS *r;
    char buf[100];

    if (!opts)
	return NULL;

    // no modifications necessary if line contains first and last points
    if (first_p && last_p)
	return opts;

    // make a clean copy and modify the arrows
    r = copy_opts(opts, OPT_LINE, lang);

    for (i = 0; i < r->list->n_elts; i++) {
	if (strcmp("arrows", r->list->elt[i].key) == 0) {
	    char *val = r->list->elt[i].val;
	    char *dash = strchr(val, '-');
	    if (!dash) {
		warn(no_line,
		     "could not find '-' while splitting arrows option");
		continue;
	    }
	    if (first_p) {
		str_slice(buf, sizeof buf, val, 0, dash - val + 1);
	    } else if (last_p) {
		str_slice(buf, sizeof buf, val, dash - val, SLICE_TO_END);
	    } else {
		// could just delete option entirely, but this is good for debugging
		str_slice(buf, sizeof buf, val, dash - val,
			  dash - val + 1);
	    }
	    r->list->elt[i].val = safe_strdup(buf);
	}
    }
    return r;
}

static int member_p(char *str, char **str_list)
{
    if (str_list == NULL)
	return 0;

    while (*str_list) {
	if (strcmp(str, *str_list) == 0)
	    return 1;
	++str_list;
    }
    return 0;
}

static void
emit_opts_internal(FILE * f, OPTS * opts, char **exceptions,
		   int brackets_p, int lang)
{
    int i, n, type;

    // do nothing if no options
    if (!opts || !opts->list || opts->list->n_elts == 0)
	return;

    // do nothing if no non-excepted options
    for (n = i = 0; i < opts->list->n_elts; i++) {
	if (!member_p(opts->list->elt[i].key, exceptions))
	    ++n;
    }
    if (n == 0)
	return;

    if (brackets_p)
	fputc('[', f);

    for (n = i = 0; i < opts->list->n_elts; i++) {

	if (member_p(opts->list->elt[i].key, exceptions))
	    continue;

	type = simple_opt_type(&opts->list->elt[i], OPT_NONE, lang);
	if ((type & OPT_INTERNAL) == 0) {
	    if (n > 0)
		fprintf(f, ",");
	    if (type & OPT_EMIT_VAL)
		fprintf(f, "%s", opts->list->elt[i].val);
	    else
		fprintf(f, "%s=%s", opts->list->elt[i].key,
			opts->list->elt[i].val);
	    ++n;
	}
    }
    if (brackets_p)
	fputc(']', f);
}

void emit_opts_raw(FILE * f, OPTS * opts, int lang)
{
    emit_opts_internal(f, opts, NULL, 0, lang);
}

void emit_opts(FILE * f, OPTS * opts, int lang)
{
    emit_opts_internal(f, opts, NULL, 1, lang);
}

void
emit_opts_with_exceptions(FILE * f, OPTS * opts, char **exceptions,
			  int lang)
{
    emit_opts_internal(f, opts, exceptions, 1, lang);
}
