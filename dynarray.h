/* dynarray.h
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

#ifndef __DYNARRAY
#define __DYNARRAY

/* 
  dyanamic arrays in C (through preprocessor abuse)

  parameters:

  ELEMENT_TYPE - type of elements of dynamic array to be declared
  NAME - base name used in constructor, destructor, and accessor functions
  ELTS - field name of C array of elements inside the dynamic array struct
  N_ELTS - field name for fill pointer, current number of valid elements

  structure:

  a dynamic array is a struct with the following fields:

    current_size - the number of array elements currently allocated to the array

    N_ELTS - a "fill pointer" that tracks the number of elements that have been
      pushed onto the array so far; push()ing grows the array automatically

    ELTS - a pointer to ELEMENT_TYPE with specified name; these are the 
      array elements

  an example

    // ---- in foo.h ----

  // we need a dynamic array of these things
    typedef struct foo_t {
      char *name;
      int count;
    } FOO;

    // create the typedef for the type FOO_ARRAY
    TYPEDEF_DYNAMIC_ARRAY(FOO_ARRAY, FOO, foo_list, val, n_vals) // no semicolons!

    // do the prototypes for the constructor, destructor, and accessor functions
    DECLARE_DYNAMIC_ARRAY_PROTOS(FOO_ARRAY, FOO, foo_list, val, n_vals)

    // ---- in foo.c ----

  // create the bodies for the constructor, destructor, and accessor functions
    DECLARE_DYNAMIC_ARRAY_FUNCS(FOO_ARRAY, FOO, foo_list, val, n_vals)

    // use all the new stuff!
    void do_stuff_with_foos(void)
    {
      int i;
      char buf[100];
      FOO_ARRAY list[1];  // or FOO_ARRAY list; but then we're forever &'ing
      FOO_ARRAY copy[1];
  
      init_foo_list(list);  // do this JUST ONCE right after declaration
      init_foo_list(copy);   // (not necessary for static/global decls)

      setup_foo_list(list, 10);  // allow for 10 elements

      // read some data and push it on the list tail
      while (scanf("%d %s", &i, buf) == 2) {
        // get pointer to new (empty) element at the end of array
        FOO *p = pushed_foo_list_val(list);
        // fill in field values
        p->name = strdup(buf);
        p->count = i;
      }

      // shows unsafe access to elements
      printf("forward listing:\n");
      for (i = 0; i < list->n_val; i++)
        printf("name=%s count=%d (%d)\n", 
          list->val[i].name,                  // fast unsafe access
          foo_list_val_ptr(list, i)->count,   // slower safe pointer access
          foo_list_val(list, i).count);       // copying access

      copy_foo_list_filled(copy, list);       // copies only filled elements

      // print in reverse order by popping from tail
      printf("backward listing:\n");
      while (copy->n_val > 0) {
        FOO *p = popped_foo_list_val(copy);
        printf("name=%s count=%d\n", p->name, p->count);
      }

      // clear out all the allocated storage for the ilst
      clear_foo_list(list);
      clear_foo_list(copy);
    }

  notes on the example:

  * NAME (foo_list) must be unique in the namespace to avoid collisions

  * ELTS need not be unique

  * the declaration FOO_ARRAY list[1]; is an idiom that avoids lots of &'s
    in the rest of the code;  feel free to use FOO_ARRAY list; if you like &'s

  * init_foo_list() is not needed on static or global declarations because
    it merely sets things to zero

  * the call pushed_foo_list_val() grows the list automatically to accomodate
    more than 10 elements; arrays grow (never shrink) until they are clear()ed;
    the fill pointer is foo_list->n_val

  * safe copying access is good for reading small elements; pointer access is
    for writing elements and for reading within large struct elements

  * copy_foo_list_filled() copies only n_val elements after ensuring there is
    enough space in the destination; copy_foo_list() does the same thing
    for all current_size elements; it ignores the fill pointer except to copy 
    its value
  
  macros:

  TYPEDEF_DYNAMIC_ARRAY(ELEMENT_TYPE, NAME, ELTS) - declare a typedef
  for a new dyamic array type with the given attributes

*/

#include <string.h>
#include "error.h"

#define DYNAMIC_ARRAY_FIELDS(ELEMENT_TYPE, ELTS, N_ELTS)  \
  int current_size, N_ELTS;  \
  ELEMENT_TYPE *ELTS

#define DECLARE_DYNAMIC_ARRAY_PROTOS(ARRAY_TYPE, ELEMENT_TYPE, NAME, ELTS, N_ELTS)  \
void init_##NAME(ARRAY_TYPE *a);  \
ARRAY_TYPE *new_##NAME(int size);  \
void delete_##NAME(ARRAY_TYPE *a); \
void setup_##NAME(ARRAY_TYPE *a, int size);  \
void extend_##NAME(ARRAY_TYPE *a, int new_size);  \
ELEMENT_TYPE *pushed_##NAME##_##ELTS(ARRAY_TYPE *a);  \
ELEMENT_TYPE *popped_##NAME##_##ELTS(ARRAY_TYPE *a);  \
void copy_##NAME(ARRAY_TYPE *dst, ARRAY_TYPE *src);  \
void copy_filled_##NAME(ARRAY_TYPE *dst, ARRAY_TYPE *src);  \
void reverse_copy_##NAME(ARRAY_TYPE *dst, ARRAY_TYPE *src);  \
void clear_##NAME(ARRAY_TYPE *a);  \
ELEMENT_TYPE *NAME##_##ELTS##_ptr(ARRAY_TYPE *a, int i);  \
ELEMENT_TYPE NAME##_##ELTS(ARRAY_TYPE *a, int i)

// use this for OTHER_INIT parameter when there is none
#define NO_OTHER_INIT /**/
#define DECLARE_DYNAMIC_ARRAY_FUNCS(ARRAY_TYPE, ELEMENT_TYPE, NAME, ELTS, N_ELTS, OTHER_INIT)  \
  \
/* initialize raw array record */  \
void init_##NAME(ARRAY_TYPE *a)  \
{  \
  a->current_size = a->N_ELTS = 0;  \
  a->ELTS = 0;  \
  OTHER_INIT  \
}  \
  \
/* allocate an array dynamically and initialize it */  \
ARRAY_TYPE *new_##NAME(int size)  \
{  \
  ARRAY_TYPE *a = safe_malloc(sizeof(ARRAY_TYPE));  \
  init_##NAME(a);  \
  setup_##NAME(a, size);  \
  return a;  \
}  \
 \
void delete_##NAME(ARRAY_TYPE *a) \
{ \
  if (!a) return; \
  clear_##NAME(a); \
  safe_free(a); \
} \
/* set up (or increase size of existing) initialized array with given size */ \
void setup_##NAME(ARRAY_TYPE *a, int size) \
{ \
  if (size > a->current_size) { \
    a->ELTS = safe_realloc(a->ELTS, size * sizeof(ELEMENT_TYPE)); \
    a->current_size = size; \
  } \
} \
 \
void extend_##NAME(ARRAY_TYPE *a, int new_size) \
{ \
  int actual_new_size = a->current_size; \
  if (actual_new_size <= 0) actual_new_size = 1; \
  while (actual_new_size < new_size) \
    actual_new_size *= 2; \
  setup_##NAME(a, actual_new_size); \
} \
ELEMENT_TYPE *pushed_##NAME##_##ELTS(ARRAY_TYPE *a) \
{ \
  extend_##NAME(a, a->N_ELTS + 1); \
  return &a->ELTS[a->N_ELTS++]; \
} \
 \
ELEMENT_TYPE *popped_##NAME##_##ELTS(ARRAY_TYPE *a) \
{ \
  if (a->N_ELTS <= 0) \
    die(no_line, "popped_" #NAME "_" #ELTS ": no elements to pop"); \
  return &a->ELTS[--a->N_ELTS]; \
} \
 \
void copy_##NAME(ARRAY_TYPE *dst, ARRAY_TYPE *src) \
{ \
  extend_##NAME(dst, src->current_size); \
  dst->N_ELTS = src->N_ELTS; \
  memcpy(dst->ELTS, src->ELTS, src->current_size * sizeof(ELEMENT_TYPE)); \
} \
 \
void copy_##NAME##_filled(ARRAY_TYPE *dst, ARRAY_TYPE *src) \
{ \
  extend_##NAME(dst, src->N_ELTS); \
  dst->N_ELTS = src->N_ELTS; \
  memcpy(dst->ELTS, src->ELTS, src->N_ELTS * sizeof(ELEMENT_TYPE)); \
} \
 \
void reverse_copy_##NAME(ARRAY_TYPE *dst, ARRAY_TYPE *src) \
{ \
  int i, j; \
  extend_##NAME(dst, src->N_ELTS); \
  dst->N_ELTS = src->N_ELTS; \
  for (i = 0, j = dst->N_ELTS - 1; j >= 0; i++, j--) \
    dst->ELTS[j] = src->ELTS[i]; \
} \
 \
void clear_##NAME(ARRAY_TYPE *a) \
{ \
  safe_free(a->ELTS); \
  init_##NAME(a); \
} \
 \
ELEMENT_TYPE *NAME##_##ELTS##_ptr(ARRAY_TYPE *a, int i) \
{ \
  if (i < 0 || i >= a->N_ELTS) \
    die(no_line, #NAME "_elt_ptr: " #ELEMENT_TYPE "_ARRAY reference [%d] out of bounds", i); \
  return &a->ELTS[i]; \
} \
 \
ELEMENT_TYPE NAME##_##ELTS(ARRAY_TYPE *a, int i) \
{ \
  if (i < 0 || i >= a->N_ELTS) \
    die(no_line, #NAME "_elt: " #ELEMENT_TYPE "_ARRAY reference [%d] out of bounds", i); \
  return a->ELTS[i]; \
} \
void init_##NAME(ARRAY_TYPE *a)
// ---- dyanmic arrays of elements that are static one-dimensional arrays ------
// this is meant to be identical to the above except to compensate for C's strange
// quirks regarding arrays of arrays
#define DYNAMIC_2D_ARRAY_FIELDS(ELEMENT_TYPE, ELTS, N_ELTS) \
  int current_size, N_ELTS; \
  ELEMENT_TYPE *ELTS
#define DECLARE_DYNAMIC_2D_ARRAY_PROTOS(ARRAY_TYPE, ELEMENT_TYPE, SUB_ELEMENT_TYPE, NAME, ELTS, N_ELTS) \
void init_##NAME(ARRAY_TYPE *a); \
ARRAY_TYPE *new_##NAME(int size); \
void delete_##NAME(ARRAY_TYPE *a); \
void setup_##NAME(ARRAY_TYPE *a, int size); \
void extend_##NAME(ARRAY_TYPE *a, int new_size); \
SUB_ELEMENT_TYPE *pushed_##NAME##_##ELTS(ARRAY_TYPE *a); \
SUB_ELEMENT_TYPE *popped_##NAME##_##ELTS(ARRAY_TYPE *a); \
void copy_##NAME(ARRAY_TYPE *dst, ARRAY_TYPE *src); \
void copy_##NAME##_filled(ARRAY_TYPE *dst, ARRAY_TYPE *src); \
void reverse_copy_##NAME(ARRAY_TYPE *dst, ARRAY_TYPE *src); \
void clear_##NAME(ARRAY_TYPE *a); \
SUB_ELEMENT_TYPE *NAME##_##ELTS(ARRAY_TYPE *a, int i); \
SUB_ELEMENT_TYPE NAME##_##ELTS##_elt(ARRAY_TYPE *a, int i, int j)
#define DECLARE_DYNAMIC_2D_ARRAY_FUNCS(ARRAY_TYPE, ELEMENT_TYPE, SUB_ELEMENT_TYPE, NAME, ELTS, N_ELTS, OTHER_INIT) \
 \
/* initialize raw array record */ \
void init_##NAME(ARRAY_TYPE *a) \
{ \
  a->current_size = a->N_ELTS = 0; \
  a->ELTS = 0; \
  OTHER_INIT \
} \
 \
/* allocate an array dynamically and initialize it */ \
ARRAY_TYPE *new_##NAME(int size) \
{ \
  ARRAY_TYPE *a = safe_malloc(sizeof(ARRAY_TYPE)); \
  init_##NAME(a); \
  setup_##NAME(a, size); \
  return a; \
} \
 \
void delete_##NAME(ARRAY_TYPE *a) \
{ \
  if (!a) return; \
  clear_##NAME(a); \
  safe_free(a); \
} \
 \
/* set up (or increase size of existing) initialized array with given size */ \
void setup_##NAME(ARRAY_TYPE *a, int size) \
{ \
  if (size > a->current_size) { \
    a->ELTS = safe_realloc(a->ELTS, size * sizeof(ELEMENT_TYPE)); \
    a->current_size = size; \
  } \
} \
 \
void extend_##NAME(ARRAY_TYPE *a, int new_size) \
{ \
  int actual_new_size = a->current_size; \
  if (actual_new_size <= 0) actual_new_size = 1; \
  while (actual_new_size < new_size) \
    actual_new_size *= 2; \
  setup_##NAME(a, actual_new_size); \
} \
 \
SUB_ELEMENT_TYPE *pushed_##NAME##_##ELTS(ARRAY_TYPE *a) \
{ \
  extend_##NAME(a, a->N_ELTS + 1); \
  return a->ELTS[a->N_ELTS++]; \
} \
 \
SUB_ELEMENT_TYPE *popped_##NAME##_##ELTS(ARRAY_TYPE *a) \
{ \
  if (a->N_ELTS <= 0) \
    die(no_line, "popped_" #NAME "_" #ELTS ": no elements to pop"); \
  return a->ELTS[--a->N_ELTS]; \
} \
 \
void copy_##NAME(ARRAY_TYPE *dst, ARRAY_TYPE *src) \
{ \
  extend_##NAME(dst, src->current_size); \
  dst->N_ELTS = src->N_ELTS; \
  memcpy(dst->ELTS, src->ELTS, src->current_size * sizeof(ELEMENT_TYPE)); \
} \
 \
void copy_filled_##NAME(ARRAY_TYPE *dst, ARRAY_TYPE *src) \
{ \
  extend_##NAME(dst, src->N_ELTS); \
  dst->N_ELTS = src->N_ELTS; \
  memcpy(dst->ELTS, src->ELTS, src->N_ELTS * sizeof(ELEMENT_TYPE)); \
} \
 \
void reverse_copy_##NAME(ARRAY_TYPE *dst, ARRAY_TYPE *src) \
{ \
  int i, j; \
  extend_##NAME(dst, src->N_ELTS); \
  dst->N_ELTS = src->N_ELTS; \
  for (i = 0, j = dst->N_ELTS - 1; j >= 0; i++, j--) \
    memcpy(dst->ELTS[j], src->ELTS[i], sizeof dst->ELTS[0]); \
} \
 \
void clear_##NAME(ARRAY_TYPE *a) \
{ \
  safe_free(a->ELTS); \
  init_##NAME(a); \
} \
 \
SUB_ELEMENT_TYPE *NAME##_##ELTS(ARRAY_TYPE *a, int i) \
{ \
  if (i < 0 || i > a->N_ELTS) \
    die(no_line, #NAME "_elt: " #ELEMENT_TYPE "_ARRAY reference out of bounds"); \
  return a->ELTS[i]; \
} \
 \
SUB_ELEMENT_TYPE NAME##_##ELTS##_elt(ARRAY_TYPE *a, int i, int j) \
{ \
  if (i < 0 || i >= a->N_ELTS || j < 0 || j >= sizeof(ELEMENT_TYPE) / sizeof(SUB_ELEMENT_TYPE)) \
    die(no_line, #NAME "_subelt: " #ELEMENT_TYPE "_ARRAY reference [%d][%d] out of bounds", i, j); \
  return a->ELTS[i][j]; \
} \
void init_##NAME(ARRAY_TYPE *a)
#endif
