/* langver.c
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

#include <string.h>
#include "langver.h"

int is_digit(int ch)
{
    return '0' <= ch && ch <= '9';
}

int lower(int ch)
{
    return ('A' <= ch && ch <= 'Z') ? ch + ('a' - 'A') : ch;
}

int is_alpha(int ch)
{
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
}

int parse_pst_version(PST_VERSION * v, char *str, SRC_LINE line)
{
#define M (sizeof v->key / 2)
    int i = 0, iv = 0, i_minor = -1;

    memset(v->key, '0', sizeof v->key);
    memset(v->str, '\0', sizeof v->str);

    if (strlen(str) > sizeof v->str - 1) {
	err(line, "PSTricks version string too long");
	return 1;
    }
//A :
    if (is_digit(str[i])) {
	v->str[iv++] = str[i++];
	goto B;
    } else {
	err(line, "bad character '%c' in PSTricks version", str[i]);
	return 2;
    }
  B:
    if (is_digit(str[i])) {
	v->str[iv++] = str[i++];
	goto B;
    } else if (str[i] == '.') {
	memcpy(&v->key[M - i], v->str, i);	// save major in key
	v->str[iv++] = str[i++];
	i_minor = iv;		// remember where minor version starts
	goto C;
    } else {
	err(line, "expected dot in PSTricks version");
	return 3;
    }
  C:
    if (is_digit(str[i])) {
	v->str[iv++] = str[i++];
	goto D;
    } else {
	err(line, "expected digit after dot in PSTricks version");
	return 4;
    }
  D:
    if (is_digit(str[i])) {
	v->str[iv++] = str[i++];
	goto D;
    } else if (is_alpha(str[i])) {
	v->str[iv++] = lower(str[i++]);
	goto F;
    } else if (str[i] == '\0') {
	memcpy(&v->key[M], &v->str[i_minor], i - i_minor);	// save minor in key
	return 0;		/* accept */
    } else {
	err(line,
	    "expected digit or subversion letter in PSTricks version");
	return 5;
    }
  F:
    if (str[i] == '\0') {
	memcpy(&v->key[M], &v->str[i_minor], i - i_minor);
	return 0;		/* accept */
    } else {
	err(line, "expected end of PSTricks version, found '%c'", str[i]);
	return 6;
    }
    return -1;
}

int pst_version_cmp(PST_VERSION * a, PST_VERSION * b)
{
    return strncmp(a->key, b->key, sizeof a->key);
}

int parse_tikz_version(TIKZ_VERSION * v, char *str, SRC_LINE line)
{
    return parse_pst_version(v, str, line);
}

int tikz_version_cmp(TIKZ_VERSION * a, TIKZ_VERSION * b)
{
    return pst_version_cmp(a, b);
}
