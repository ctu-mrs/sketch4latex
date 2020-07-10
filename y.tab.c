/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ID = 258,
     PAREN_ID = 259,
     BRACKET_ID = 260,
     DBL_BRACKET_ID = 261,
     CURLY_ID = 262,
     ANGLE_ID = 263,
     NUM = 264,
     OPTS_STR = 265,
     SPECIAL = 266,
     TICK = 267,
     THEN = 268,
     DEF = 269,
     EMPTY_ANGLE = 270,
     DOTS = 271,
     LINE = 272,
     CURVE = 273,
     POLYGON = 274,
     REPEAT = 275,
     SWEEP = 276,
     PUT = 277,
     TRANSLATE = 278,
     ROTATE = 279,
     SCALE = 280,
     PROJECT = 281,
     PERSPECTIVE = 282,
     VIEW = 283,
     SQRT = 284,
     SIN = 285,
     ASIN = 286,
     COS = 287,
     ACOS = 288,
     ATAN2 = 289,
     UNIT = 290,
     INVERSE = 291,
     GLOBAL = 292,
     SET = 293,
     PICTUREBOX = 294,
     FRAME = 295,
     CAMERA = 296,
     LANGUAGE = 297,
     PSTRICKS = 298,
     TIKZ = 299,
     LaTeX = 300,
     ConTeXt = 301,
     NEG = 302
   };
#endif
/* Tokens.  */
#define ID 258
#define PAREN_ID 259
#define BRACKET_ID 260
#define DBL_BRACKET_ID 261
#define CURLY_ID 262
#define ANGLE_ID 263
#define NUM 264
#define OPTS_STR 265
#define SPECIAL 266
#define TICK 267
#define THEN 268
#define DEF 269
#define EMPTY_ANGLE 270
#define DOTS 271
#define LINE 272
#define CURVE 273
#define POLYGON 274
#define REPEAT 275
#define SWEEP 276
#define PUT 277
#define TRANSLATE 278
#define ROTATE 279
#define SCALE 280
#define PROJECT 281
#define PERSPECTIVE 282
#define VIEW 283
#define SQRT 284
#define SIN 285
#define ASIN 286
#define COS 287
#define ACOS 288
#define ATAN2 289
#define UNIT 290
#define INVERSE 291
#define GLOBAL 292
#define SET 293
#define PICTUREBOX 294
#define FRAME 295
#define CAMERA 296
#define LANGUAGE 297
#define PSTRICKS 298
#define TIKZ 299
#define LaTeX 300
#define ConTeXt 301
#define NEG 302




/* Copy the first part of user declarations.  */
#line 21 "sketch.y"


#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
#  include <malloc.h>
#  if !defined(alloca)
#    define alloca _alloca
#  endif
#  ifdef _MSC_VER
     // turn of warning about unused goto label in bison skeleton
#    pragma warning(disable:4102)
#  endif
#  define YYSTACK_USE_ALLOCA 1
#endif

#include "parse.h"
#include "expr.h"
#include "global.h"

int yylex(void);

void yyerror (char *s)  /* Called by yyparse on error */
{
  extern SRC_LINE line;
  err(line, "%s", s);
}

static SYMBOL_TABLE *sym_tab;

// exported parse tree and global environment
static OBJECT *objects;



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 57 "sketch.y"
typedef union YYSTYPE {
  char *str;
  FLOAT flt;
  POINT_3D pt;
  VECTOR_3D vec;
  TRANSFORM xf;
  EXPR_VAL exv;
  SYMBOL_NAME name;
  OBJECT *obj;
  OPTS *opts;
  int bool;
  int index;
} YYSTYPE;
/* Line 196 of yacc.c.  */
#line 229 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 219 of yacc.c.  */
#line 241 "y.tab.c"

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T) && (defined (__STDC__) || defined (__cplusplus))
# include <stddef.h> /* INFRINGES ON USER NAME SPACE */
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if defined (__STDC__) || defined (__cplusplus)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     define YYINCLUDED_STDLIB_H
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2005 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM ((YYSIZE_T) -1)
#  endif
#  ifdef __cplusplus
extern "C" {
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if (! defined (malloc) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if (! defined (free) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifdef __cplusplus
}
#  endif
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  33
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   648

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  62
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  40
/* YYNRULES -- Number of rules. */
#define YYNRULES  121
/* YYNRULES -- Number of states. */
#define YYNSTATES  269

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   302

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      60,    61,    49,    48,    58,    47,    51,    50,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    56,     2,    57,    53,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    54,    59,    55,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    52
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     6,    11,    12,    15,    17,    20,    25,
      30,    33,    35,    38,    41,    43,    46,    48,    50,    53,
      54,    56,    58,    62,    63,    65,    68,    70,    72,    74,
      78,    82,    86,    91,    95,    97,    99,   101,   105,   109,
     113,   117,   127,   137,   145,   151,   154,   156,   157,   158,
     164,   166,   167,   171,   173,   175,   177,   181,   182,   184,
     187,   189,   191,   194,   195,   197,   199,   201,   203,   205,
     207,   211,   213,   215,   217,   219,   221,   225,   229,   233,
     237,   241,   245,   249,   252,   256,   260,   264,   268,   272,
     276,   280,   284,   290,   293,   295,   297,   299,   307,   313,
     315,   317,   319,   321,   329,   335,   337,   339,   378,   382,
     388,   396,   400,   404,   407,   411,   415,   423,   429,   433,
     437,   439
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      63,     0,    -1,    72,    64,    -1,    37,    54,    65,    55,
      -1,    -1,    65,    66,    -1,    66,    -1,    38,    10,    -1,
      39,    56,    93,    57,    -1,    39,    71,    94,    94,    -1,
      41,   101,    -1,    40,    -1,    40,    10,    -1,    42,    67,
      -1,    75,    -1,    68,    69,    -1,    43,    -1,    44,    -1,
      58,    70,    -1,    -1,    45,    -1,    46,    -1,    56,    93,
      57,    -1,    -1,    73,    -1,    73,    74,    -1,    74,    -1,
      75,    -1,    78,    -1,    14,     3,    77,    -1,    76,    15,
      77,    -1,    14,     3,    15,    -1,    14,     3,     8,    77,
      -1,    76,     8,    77,    -1,    91,    -1,    78,    -1,    10,
      -1,    16,    83,    84,    -1,    17,    83,    84,    -1,    18,
      83,    84,    -1,    19,    83,    84,    -1,    21,    83,    54,
      93,    81,    58,    89,    55,    94,    -1,    21,    83,    54,
      93,    81,    58,    89,    55,    78,    -1,    20,    54,    93,
      58,    89,    55,    78,    -1,    22,    54,   101,    55,    78,
      -1,    11,    86,    -1,     7,    -1,    -1,    -1,    54,    79,
      72,    80,    55,    -1,    15,    -1,    -1,    82,    58,     3,
      -1,     3,    -1,    10,    -1,     5,    -1,    56,    82,    57,
      -1,    -1,    85,    -1,    85,    94,    -1,    94,    -1,    87,
      -1,    87,    88,    -1,    -1,    92,    -1,    94,    -1,    97,
      -1,    10,    -1,     5,    -1,    90,    -1,    90,    58,   101,
      -1,   101,    -1,    92,    -1,    94,    -1,    96,    -1,   100,
      -1,    91,    48,    91,    -1,    91,    47,    91,    -1,    91,
      49,    91,    -1,    91,    50,    91,    -1,    91,    51,    91,
      -1,    91,    13,    91,    -1,    59,    91,    59,    -1,    47,
      91,    -1,    91,    53,    91,    -1,    60,    91,    61,    -1,
      35,    91,    61,    -1,    29,    91,    61,    -1,    30,    91,
      61,    -1,    32,    91,    61,    -1,    31,    91,    61,    -1,
      33,    91,    61,    -1,    34,    91,    58,    91,    61,    -1,
      91,    12,    -1,     9,    -1,     3,    -1,    91,    -1,    60,
      93,    58,    93,    58,    93,    61,    -1,    60,    93,    58,
      93,    61,    -1,     4,    -1,    91,    -1,    97,    -1,    98,
      -1,    56,    93,    58,    93,    58,    93,    57,    -1,    56,
      93,    58,    93,    57,    -1,     5,    -1,    91,    -1,    56,
      56,    93,    58,    93,    58,    93,    58,    93,    57,    56,
      93,    58,    93,    58,    93,    58,    93,    57,    56,    93,
      58,    93,    58,    93,    58,    93,    57,    56,    93,    58,
      93,    58,    93,    58,    93,    57,    57,    -1,    24,    93,
      61,    -1,    24,    93,    58,    91,    61,    -1,    24,    93,
      58,    95,    58,    99,    61,    -1,    23,    99,    61,    -1,
      25,    91,    61,    -1,    26,    61,    -1,    26,    93,    61,
      -1,    27,    93,    61,    -1,    28,    95,    58,    91,    58,
      99,    61,    -1,    28,    95,    58,    91,    61,    -1,    28,
      95,    61,    -1,    36,   101,    61,    -1,     6,    -1,    91,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   105,   105,   108,   109,   112,   113,   116,   120,   124,
     129,   133,   137,   141,   145,   148,   151,   152,   155,   156,
     159,   160,   163,   164,   167,   170,   171,   174,   175,   179,
     180,   181,   184,   185,   188,   189,   190,   193,   194,   195,
     196,   197,   201,   205,   209,   210,   211,   212,   213,   212,
     222,   223,   226,   230,   237,   238,   239,   240,   243,   246,
     247,   250,   253,   254,   257,   258,   259,   260,   261,   264,
     267,   268,   271,   272,   273,   274,   275,   276,   277,   278,
     279,   280,   281,   282,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   295,   296,   299,   302,   306,   310,
     313,   316,   317,   320,   324,   330,   333,   336,   348,   352,
     362,   366,   370,   387,   388,   389,   390,   400,   410,   415,
     416,   419
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "PAREN_ID", "BRACKET_ID",
  "DBL_BRACKET_ID", "CURLY_ID", "ANGLE_ID", "NUM", "OPTS_STR", "SPECIAL",
  "TICK", "THEN", "DEF", "EMPTY_ANGLE", "DOTS", "LINE", "CURVE", "POLYGON",
  "REPEAT", "SWEEP", "PUT", "TRANSLATE", "ROTATE", "SCALE", "PROJECT",
  "PERSPECTIVE", "VIEW", "SQRT", "SIN", "ASIN", "COS", "ACOS", "ATAN2",
  "UNIT", "INVERSE", "GLOBAL", "SET", "PICTUREBOX", "FRAME", "CAMERA",
  "LANGUAGE", "PSTRICKS", "TIKZ", "LaTeX", "ConTeXt", "'-'", "'+'", "'*'",
  "'/'", "'.'", "NEG", "'^'", "'{'", "'}'", "'['", "']'", "','", "'|'",
  "'('", "')'", "$accept", "input", "global_decl_block", "global_decls",
  "global_decl", "output_language", "graphics_language",
  "comma_macro_package", "macro_package", "opt_baseline", "defs_and_decls",
  "rev_defs_and_decls", "def_or_decl", "def", "tagged_defs", "defable",
  "decl", "@1", "@2", "opt_star", "option_id_list", "options", "points",
  "rev_points", "special_args", "rev_special_args", "special_arg",
  "transforms", "rev_transforms", "expr", "scalar", "scalar_expr", "point",
  "point_expr", "vector", "vector_literal", "vector_id", "vector_expr",
  "transform", "transform_expr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,    45,    43,    42,
      47,    46,   302,    94,   123,   125,    91,    93,    44,   124,
      40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    62,    63,    64,    64,    65,    65,    66,    66,    66,
      66,    66,    66,    66,    66,    67,    68,    68,    69,    69,
      70,    70,    71,    71,    72,    73,    73,    74,    74,    75,
      75,    75,    76,    76,    77,    77,    77,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    79,    80,    78,
      81,    81,    82,    82,    83,    83,    83,    83,    84,    85,
      85,    86,    87,    87,    88,    88,    88,    88,    88,    89,
      90,    90,    91,    91,    91,    91,    91,    91,    91,    91,
      91,    91,    91,    91,    91,    91,    91,    91,    91,    91,
      91,    91,    91,    91,    92,    92,    93,    94,    94,    94,
      95,    96,    96,    97,    97,    98,    99,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   101
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     4,     0,     2,     1,     2,     4,     4,
       2,     1,     2,     2,     1,     2,     1,     1,     2,     0,
       1,     1,     3,     0,     1,     2,     1,     1,     1,     3,
       3,     3,     4,     3,     1,     1,     1,     3,     3,     3,
       3,     9,     9,     7,     5,     2,     1,     0,     0,     5,
       1,     0,     3,     1,     1,     1,     3,     0,     1,     2,
       1,     1,     2,     0,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     1,     1,     1,     3,     3,     3,     3,
       3,     3,     3,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     5,     2,     1,     1,     1,     7,     5,     1,
       1,     1,     1,     7,     5,     1,     1,    38,     3,     5,
       7,     3,     3,     2,     3,     3,     7,     5,     3,     3,
       1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,    46,    63,     0,    57,    57,    57,    57,     0,    57,
       0,    47,     0,     4,    24,    26,    27,     0,    28,    45,
      61,     0,    55,    54,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     1,     0,     2,    25,     0,     0,    95,
      99,    68,    94,    67,     0,     0,    62,    64,    65,    66,
     105,   120,     0,    36,    31,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,    35,    34,    72,    73,    74,   101,
     102,    75,    53,     0,    37,    58,    60,    38,    39,    40,
      96,     0,     0,   121,     0,    48,     0,    33,    30,     0,
       0,    32,   106,     0,     0,     0,   113,     0,     0,   100,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    83,
       0,     0,    96,    93,     0,     0,     0,     0,     0,     0,
       0,    56,     0,    59,     0,    51,     0,     0,     0,    23,
      11,     0,     0,     0,     6,    14,     0,     0,   111,     0,
     108,   112,   114,   115,     0,   118,    87,    88,    90,    89,
      91,     0,    86,   119,     0,    82,    85,    81,    77,    76,
      78,    79,    80,    84,    52,     0,    69,    71,    50,     0,
      44,    49,     7,     0,     0,    12,    10,    16,    17,    13,
      19,     3,     5,     0,     0,   100,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    15,   104,     0,     0,
      98,   109,     0,     0,   117,    92,     0,    43,    70,     0,
       8,     9,    20,    21,    18,     0,     0,     0,     0,     0,
       0,   103,    97,   110,   116,     0,    42,    41,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   107
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,    12,    35,   143,   144,   189,   190,   206,   224,   184,
      13,    14,    15,    16,    17,    73,    74,    32,   137,   179,
      83,    25,    84,    85,    19,    20,    46,   175,   176,    90,
      76,    99,    77,   110,    78,    79,    80,   103,    81,   177
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -101
static const short int yypact[] =
{
      78,  -101,  -101,    21,    -3,    -3,    -3,    -3,   -35,    -3,
     -28,  -101,    31,    11,    78,  -101,  -101,     5,  -101,  -101,
      55,   291,  -101,  -101,    53,     2,     2,     2,     2,   116,
      29,   116,    78,  -101,    36,  -101,  -101,   367,   367,  -101,
    -101,  -101,  -101,  -101,   116,   116,  -101,  -101,  -101,  -101,
    -101,  -101,   367,  -101,  -101,   116,   116,   116,    46,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     407,   116,   116,  -101,  -101,   595,  -101,  -101,  -101,  -101,
    -101,  -101,  -101,   -23,  -101,     2,  -101,  -101,  -101,  -101,
     595,    43,   116,   595,    61,  -101,    -2,  -101,  -101,    52,
      72,  -101,   595,    70,   -16,   447,  -101,    73,    74,   595,
     -14,   456,   463,   479,   506,   521,   111,   530,    75,    -7,
     407,   316,   537,  -101,   116,   116,   116,   116,   116,   116,
     116,  -101,   130,  -101,   116,   123,   232,    98,   145,   100,
     155,   116,    23,   143,  -101,  -101,   116,   116,  -101,   116,
    -101,  -101,  -101,  -101,   116,  -101,  -101,  -101,  -101,  -101,
    -101,   116,  -101,  -101,   108,  -101,  -101,   595,   293,   293,
      -7,    -7,    -7,    -7,  -101,   113,   115,  -101,  -101,   119,
    -101,  -101,  -101,   116,     2,  -101,  -101,  -101,  -101,  -101,
     120,  -101,  -101,    51,    -4,   552,   128,   432,   580,   116,
     232,   116,   116,   114,     2,    58,  -101,  -101,   116,   116,
    -101,  -101,   116,   116,  -101,  -101,    69,  -101,  -101,   133,
      24,  -101,  -101,  -101,  -101,   132,   131,   134,   135,   116,
     186,  -101,  -101,  -101,  -101,    71,  -101,  -101,   116,   137,
     154,   116,   141,   116,   153,   116,   157,   116,   160,   163,
     116,   162,   116,   164,   116,   166,   116,   169,   173,   116,
     175,   116,   177,   116,   179,   116,   184,   185,  -101
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -101,  -101,  -101,  -101,    48,  -101,  -101,  -101,  -101,  -101,
     215,  -101,   241,   -82,  -101,   -20,     1,  -101,  -101,  -101,
    -101,    16,    60,  -101,  -101,  -101,  -101,    54,  -101,   207,
     237,   -29,   -17,   109,  -101,   240,  -101,  -100,  -101,   -27
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -23
static const short int yytable[] =
{
      91,    18,    22,    48,    94,   123,    40,    23,    86,    86,
      86,    86,     3,    37,   145,    18,   100,    97,    98,    29,
      38,    26,    27,    28,    21,    30,    31,   104,   -22,   107,
     108,    33,   101,    18,   131,   132,   138,   139,   140,   141,
     142,   118,   149,   100,   154,   150,   130,   155,    34,    39,
      40,    50,    51,    24,   209,    42,    82,   210,    39,    40,
      41,   145,    45,   135,    42,    43,   187,   188,   133,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    92,   -22,     1,    87,    88,    89,     2,
      96,   164,     3,    69,     4,     5,     6,     7,     8,     9,
      10,   134,    70,   222,   223,    71,    72,   106,   207,   208,
     146,    44,   227,   228,   186,    45,   136,   193,   194,    39,
      40,    50,    51,   123,   124,    42,   207,   229,   231,   238,
     147,   148,    11,   174,   152,   153,   163,   180,   178,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,   181,   203,   182,   183,     3,   125,   126,
     127,   128,   129,    69,   130,   185,   199,   204,   200,   161,
     216,   220,    70,   201,   218,    71,    72,   202,   205,   225,
     226,   138,   139,   140,   141,   142,   212,   221,   230,   231,
      40,   192,   232,     1,   240,   233,   234,     2,   191,   243,
     235,   217,     4,     5,     6,     7,     8,     9,    10,   239,
     241,   245,   242,   237,   244,   247,   246,   249,   248,   250,
     252,   251,   254,   253,   256,   255,   258,   257,    75,   259,
     260,   236,   262,   261,   264,   263,   266,   265,    93,     1,
      11,   267,   268,     2,    75,    75,    45,    95,     4,     5,
       6,     7,     8,     9,    10,    36,   219,    47,   196,    75,
      49,     0,   102,     0,   105,     0,     0,   109,   111,   112,
     113,   114,   115,   116,   117,    93,   119,     0,   121,   122,
       0,     0,     0,     0,     0,     0,    11,     0,     0,     0,
       0,     0,     0,     0,    39,    40,    50,    51,     1,    52,
      42,    53,     2,     0,     0,   123,    54,     4,     5,     6,
       7,     8,     9,    10,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,   123,   124,
       0,   167,   168,   169,   170,   171,   172,   173,    69,     0,
       0,    93,   127,   128,   129,    11,   130,    70,    93,     0,
      71,    72,     0,     0,     0,     0,   195,     0,     0,     0,
       0,   197,     0,   125,   126,   127,   128,   129,   198,   130,
      39,    40,    50,    51,     1,   165,    42,    53,     2,     0,
       0,     0,     0,     4,     5,     6,     7,     8,     9,    10,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,     0,     0,     0,     0,    93,    93,
      39,    40,    50,    51,    69,     0,    42,     0,     0,   102,
     102,    11,     0,    70,     0,     0,    71,    72,     0,     0,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,   123,   124,     0,     0,     0,     0,
       0,     0,     0,     0,    69,     0,     0,     0,     0,   123,
     124,     0,     0,   120,     0,     0,    71,    72,   123,   124,
       0,     0,     0,     0,     0,   123,   124,     0,     0,   125,
     126,   127,   128,   129,     0,   130,     0,     0,     0,     0,
     213,   123,   124,   214,   125,   126,   127,   128,   129,     0,
     130,     0,     0,   125,   126,   127,   128,   129,   151,   130,
     125,   126,   127,   128,   129,     0,   130,   156,   123,   124,
       0,     0,     0,     0,   157,     0,   125,   126,   127,   128,
     129,     0,   130,   123,   124,     0,     0,     0,     0,     0,
     158,     0,   123,   124,     0,     0,     0,     0,     0,   123,
     124,     0,     0,   125,   126,   127,   128,   129,     0,   130,
       0,     0,     0,     0,   123,   124,     0,   159,   125,   126,
     127,   128,   129,     0,   130,     0,     0,   125,   126,   127,
     128,   129,   160,   130,   125,   126,   127,   128,   129,     0,
     130,   162,   123,   124,     0,     0,     0,     0,   166,   125,
     126,   127,   128,   129,     0,   130,     0,   123,   124,     0,
       0,     0,     0,   211,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   125,   126,   127,
     128,   129,     0,   130,     0,     0,     0,     0,     0,     0,
       0,   215,   125,   126,   127,   128,   129,     0,   130
};

static const short int yycheck[] =
{
      29,     0,     5,    20,    31,    12,     4,    10,    25,    26,
      27,    28,    14,     8,    96,    14,    45,    37,    38,    54,
      15,     5,     6,     7,     3,     9,    54,    56,     4,    58,
      59,     0,    52,    32,    57,    58,    38,    39,    40,    41,
      42,    68,    58,    72,    58,    61,    53,    61,    37,     3,
       4,     5,     6,    56,    58,     9,     3,    61,     3,     4,
       5,   143,    60,    92,     9,    10,    43,    44,    85,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    54,    60,     7,    26,    27,    28,    11,
      54,   120,    14,    47,    16,    17,    18,    19,    20,    21,
      22,    58,    56,    45,    46,    59,    60,    61,    57,    58,
      58,    56,   212,   213,   141,    60,    55,   146,   147,     3,
       4,     5,     6,    12,    13,     9,    57,    58,    57,    58,
      58,    61,    54,     3,    61,    61,    61,   136,    15,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    55,   183,    10,    56,    14,    47,    48,
      49,    50,    51,    47,    53,    10,    58,   184,    55,    58,
     199,    57,    56,    58,   201,    59,    60,    58,    58,   208,
     209,    38,    39,    40,    41,    42,    58,   204,    55,    57,
       4,   143,    61,     7,    57,    61,    61,    11,    55,    58,
     229,   200,    16,    17,    18,    19,    20,    21,    22,   238,
      56,    58,   241,   230,   243,    58,   245,    57,   247,    56,
      58,   250,    58,   252,    58,   254,    57,   256,    21,    56,
     259,   230,   261,    58,   263,    58,   265,    58,    31,     7,
      54,    57,    57,    11,    37,    38,    60,    32,    16,    17,
      18,    19,    20,    21,    22,    14,   202,    20,   149,    52,
      20,    -1,    55,    -1,    57,    -1,    -1,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    -1,    71,    72,
      -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    -1,    -1,    12,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    12,    13,
      -1,   124,   125,   126,   127,   128,   129,   130,    47,    -1,
      -1,   134,    49,    50,    51,    54,    53,    56,   141,    -1,
      59,    60,    -1,    -1,    -1,    -1,   149,    -1,    -1,    -1,
      -1,   154,    -1,    47,    48,    49,    50,    51,   161,    53,
       3,     4,     5,     6,     7,    59,     9,    10,    11,    -1,
      -1,    -1,    -1,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    -1,    -1,    -1,    -1,   201,   202,
       3,     4,     5,     6,    47,    -1,     9,    -1,    -1,   212,
     213,    54,    -1,    56,    -1,    -1,    59,    60,    -1,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    12,    13,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    12,
      13,    -1,    -1,    56,    -1,    -1,    59,    60,    12,    13,
      -1,    -1,    -1,    -1,    -1,    12,    13,    -1,    -1,    47,
      48,    49,    50,    51,    -1,    53,    -1,    -1,    -1,    -1,
      58,    12,    13,    61,    47,    48,    49,    50,    51,    -1,
      53,    -1,    -1,    47,    48,    49,    50,    51,    61,    53,
      47,    48,    49,    50,    51,    -1,    53,    61,    12,    13,
      -1,    -1,    -1,    -1,    61,    -1,    47,    48,    49,    50,
      51,    -1,    53,    12,    13,    -1,    -1,    -1,    -1,    -1,
      61,    -1,    12,    13,    -1,    -1,    -1,    -1,    -1,    12,
      13,    -1,    -1,    47,    48,    49,    50,    51,    -1,    53,
      -1,    -1,    -1,    -1,    12,    13,    -1,    61,    47,    48,
      49,    50,    51,    -1,    53,    -1,    -1,    47,    48,    49,
      50,    51,    61,    53,    47,    48,    49,    50,    51,    -1,
      53,    61,    12,    13,    -1,    -1,    -1,    -1,    61,    47,
      48,    49,    50,    51,    -1,    53,    -1,    12,    13,    -1,
      -1,    -1,    -1,    61,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    49,
      50,    51,    -1,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    47,    48,    49,    50,    51,    -1,    53
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     7,    11,    14,    16,    17,    18,    19,    20,    21,
      22,    54,    63,    72,    73,    74,    75,    76,    78,    86,
      87,     3,     5,    10,    56,    83,    83,    83,    83,    54,
      83,    54,    79,     0,    37,    64,    74,     8,    15,     3,
       4,     5,     9,    10,    56,    60,    88,    92,    94,    97,
       5,     6,     8,    10,    15,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    47,
      56,    59,    60,    77,    78,    91,    92,    94,    96,    97,
      98,   100,     3,    82,    84,    85,    94,    84,    84,    84,
      91,    93,    54,    91,   101,    72,    54,    77,    77,    93,
      93,    77,    91,    99,    93,    91,    61,    93,    93,    91,
      95,    91,    91,    91,    91,    91,    91,    91,   101,    91,
      56,    91,    91,    12,    13,    47,    48,    49,    50,    51,
      53,    57,    58,    94,    58,    93,    55,    80,    38,    39,
      40,    41,    42,    65,    66,    75,    58,    58,    61,    58,
      61,    61,    61,    61,    58,    61,    61,    61,    61,    61,
      61,    58,    61,    61,    93,    59,    61,    91,    91,    91,
      91,    91,    91,    91,     3,    89,    90,   101,    15,    81,
      78,    55,    10,    56,    71,    10,   101,    43,    44,    67,
      68,    55,    66,    93,    93,    91,    95,    91,    91,    58,
      55,    58,    58,    93,    94,    58,    69,    57,    58,    58,
      61,    61,    58,    58,    61,    61,    93,    78,   101,    89,
      57,    94,    45,    46,    70,    93,    93,    99,    99,    58,
      55,    57,    61,    61,    61,    93,    78,    94,    58,    93,
      57,    56,    93,    58,    93,    58,    93,    58,    93,    57,
      56,    93,    58,    93,    58,    93,    58,    93,    57,    56,
      93,    58,    93,    58,    93,    58,    93,    57,    57
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr,					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname[yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      size_t yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()
    ;
#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 105 "sketch.y"
    { objects = (yyvsp[-1].obj); }
    break;

  case 7:
#line 117 "sketch.y"
    { 
                            set_global_env_opts(global_env, (yyvsp[0].str), line);
                          }
    break;

  case 8:
#line 121 "sketch.y"
    { 
                            set_global_baseline(global_env, (yyvsp[-1].flt), line);
                          }
    break;

  case 9:
#line 125 "sketch.y"
    { 
                            set_global_baseline(global_env, (yyvsp[-2].flt), line);
                            set_global_env_extent(global_env, (yyvsp[-1].pt), (yyvsp[0].pt), line);
                          }
    break;

  case 10:
#line 130 "sketch.y"
    {
                            set_global_env_camera(global_env, (yyvsp[0].xf), line);
                          }
    break;

  case 11:
#line 134 "sketch.y"
    { 
                            set_global_env_frame(global_env, NULL, line);
                          }
    break;

  case 12:
#line 138 "sketch.y"
    { 
                            set_global_env_frame(global_env, (yyvsp[0].str), line);
                          }
    break;

  case 13:
#line 142 "sketch.y"
    {
                            set_global_output_language(global_env, (yyvsp[0].index), line);
		          }
    break;

  case 15:
#line 148 "sketch.y"
    { (yyval.index) = (yyvsp[-1].index) | (yyvsp[0].index); }
    break;

  case 16:
#line 151 "sketch.y"
    { (yyval.index) = GEOL_PSTRICKS; }
    break;

  case 17:
#line 152 "sketch.y"
    { (yyval.index) = GEOL_TIKZ; }
    break;

  case 18:
#line 155 "sketch.y"
    { (yyval.index) = (yyvsp[0].index); }
    break;

  case 19:
#line 156 "sketch.y"
    { (yyval.index) = GEOL_LATEX; }
    break;

  case 20:
#line 159 "sketch.y"
    { (yyval.index) = GEOL_LATEX; }
    break;

  case 21:
#line 160 "sketch.y"
    { (yyval.index) = GEOL_CONTEXT; }
    break;

  case 22:
#line 163 "sketch.y"
    { (yyval.flt) = (yyvsp[-1].flt); }
    break;

  case 23:
#line 164 "sketch.y"
    { (yyval.flt) = NO_BASELINE; }
    break;

  case 24:
#line 167 "sketch.y"
    { (yyval.obj) = sibling_reverse((yyvsp[0].obj)); }
    break;

  case 25:
#line 170 "sketch.y"
    { (yyval.obj) = cat_objects(sibling_reverse((yyvsp[0].obj)), (yyvsp[-1].obj)); }
    break;

  case 26:
#line 171 "sketch.y"
    { (yyval.obj) = sibling_reverse((yyvsp[0].obj)); }
    break;

  case 27:
#line 174 "sketch.y"
    { (yyval.obj) = NULL; }
    break;

  case 28:
#line 175 "sketch.y"
    { (yyval.obj) = (yyvsp[0].obj); }
    break;

  case 29:
#line 179 "sketch.y"
    { new_symbol(sym_tab, (yyvsp[-1].name), 0, (yyvsp[0].obj), line); }
    break;

  case 30:
#line 180 "sketch.y"
    { new_symbol(sym_tab, (yyvsp[-2].name), 0, (yyvsp[0].obj), line); }
    break;

  case 31:
#line 181 "sketch.y"
    { new_symbol(sym_tab, (yyvsp[-1].name), 0, new_tag_def(), line); }
    break;

  case 32:
#line 184 "sketch.y"
    { strcpy((yyval.name), new_symbol(sym_tab, (yyvsp[-2].name), (yyvsp[-1].name), (yyvsp[0].obj), line) ? "" : (yyvsp[-2].name)); }
    break;

  case 33:
#line 185 "sketch.y"
    { strcpy((yyval.name), new_symbol(sym_tab, (yyvsp[-2].name), (yyvsp[-1].name), (yyvsp[0].obj), line) ? "" : (yyvsp[-2].name)); }
    break;

  case 34:
#line 188 "sketch.y"
    { (yyval.obj) = object_from_expr(&(yyvsp[0].exv)); }
    break;

  case 35:
#line 189 "sketch.y"
    { (yyval.obj) = (yyvsp[0].obj); }
    break;

  case 36:
#line 190 "sketch.y"
    { (yyval.obj) = new_opts_def((yyvsp[0].str), line); }
    break;

  case 37:
#line 193 "sketch.y"
    { (yyval.obj) = new_dots((yyvsp[-1].opts), (yyvsp[0].obj)); }
    break;

  case 38:
#line 194 "sketch.y"
    { (yyval.obj) = new_line((yyvsp[-1].opts), (yyvsp[0].obj)); }
    break;

  case 39:
#line 195 "sketch.y"
    { (yyval.obj) = new_curve((yyvsp[-1].opts), (yyvsp[0].obj)); }
    break;

  case 40:
#line 196 "sketch.y"
    { (yyval.obj) = new_polygon((yyvsp[-1].opts), (yyvsp[0].obj)); }
    break;

  case 41:
#line 198 "sketch.y"
    { 
                              (yyval.obj) = new_sweep((yyvsp[-7].opts), (yyvsp[-5].flt), (yyvsp[-4].bool), (yyvsp[-2].obj), new_point_def((yyvsp[0].pt)));
                          }
    break;

  case 42:
#line 202 "sketch.y"
    {
                              (yyval.obj) = new_sweep((yyvsp[-7].opts), (yyvsp[-5].flt), (yyvsp[-4].bool), (yyvsp[-2].obj), (yyvsp[0].obj));
                          }
    break;

  case 43:
#line 206 "sketch.y"
    {
                              (yyval.obj) = new_repeat((yyvsp[-4].flt), (yyvsp[-2].obj), (yyvsp[0].obj));
                          }
    break;

  case 44:
#line 209 "sketch.y"
    { (yyval.obj) = new_compound((yyvsp[-2].xf), (yyvsp[0].obj)); }
    break;

  case 45:
#line 210 "sketch.y"
    { (yyval.obj) = new_special((yyvsp[-1].str), (yyvsp[0].obj), line); }
    break;

  case 46:
#line 211 "sketch.y"
    { look_up_drawable(sym_tab, &(yyval.obj), line, (yyvsp[0].name)); }
    break;

  case 47:
#line 212 "sketch.y"
    { sym_tab = new_scope(sym_tab); }
    break;

  case 48:
#line 213 "sketch.y"
    { sym_tab = old_scope(sym_tab); }
    break;

  case 49:
#line 215 "sketch.y"
    {
                            if ((yyvsp[-2].obj) == NULL)
                              err(line, "no drawables in compound declaration");
                            (yyval.obj) = (yyvsp[-2].obj);
                          }
    break;

  case 50:
#line 222 "sketch.y"
    { (yyval.bool) = 1; }
    break;

  case 51:
#line 223 "sketch.y"
    { (yyval.bool) = 0; }
    break;

  case 52:
#line 227 "sketch.y"
    { 
                            (yyval.opts) = look_up_and_append_to_opts(sym_tab, &(yyvsp[-2].opts), line, (yyvsp[0].name));
                          }
    break;

  case 53:
#line 231 "sketch.y"
    { 
                            (yyval.opts) = NULL;
                            (yyval.opts) = look_up_and_append_to_opts(sym_tab, &(yyval.opts), line, (yyvsp[0].name));
			  }
    break;

  case 54:
#line 237 "sketch.y"
    { (yyval.opts) = new_opts((yyvsp[0].str), line); }
    break;

  case 55:
#line 238 "sketch.y"
    { look_up_opts(sym_tab, &(yyval.opts), line, (yyvsp[0].name)); }
    break;

  case 56:
#line 239 "sketch.y"
    { (yyval.opts) = (yyvsp[-1].opts); }
    break;

  case 57:
#line 240 "sketch.y"
    { (yyval.opts) = NULL; }
    break;

  case 58:
#line 243 "sketch.y"
    { (yyval.obj) = sibling_reverse((yyvsp[0].obj)); }
    break;

  case 59:
#line 246 "sketch.y"
    { (yyval.obj) = cat_objects(new_point_def((yyvsp[0].pt)), (yyvsp[-1].obj)); }
    break;

  case 60:
#line 247 "sketch.y"
    { (yyval.obj) = new_point_def((yyvsp[0].pt)); }
    break;

  case 61:
#line 250 "sketch.y"
    { (yyval.obj) = sibling_reverse((yyvsp[0].obj)); }
    break;

  case 62:
#line 253 "sketch.y"
    { (yyval.obj) = cat_objects((yyvsp[0].obj), (yyvsp[-1].obj)); }
    break;

  case 63:
#line 254 "sketch.y"
    { (yyval.obj) = NULL; }
    break;

  case 64:
#line 257 "sketch.y"
    { (yyval.obj) = new_scalar_def((yyvsp[0].flt)); }
    break;

  case 65:
#line 258 "sketch.y"
    { (yyval.obj) = new_point_def((yyvsp[0].pt)); }
    break;

  case 66:
#line 259 "sketch.y"
    { (yyval.obj) = new_vector_def((yyvsp[0].vec)); }
    break;

  case 67:
#line 260 "sketch.y"
    { (yyval.obj) = new_opts_def((yyvsp[0].str), line); }
    break;

  case 68:
#line 261 "sketch.y"
    { look_up_vector_or_opts(sym_tab, &(yyval.obj), line, (yyvsp[0].name)); }
    break;

  case 69:
#line 264 "sketch.y"
    { (yyval.obj) = sibling_reverse((yyvsp[0].obj)); }
    break;

  case 70:
#line 267 "sketch.y"
    { (yyval.obj) = cat_objects(new_transform_def((yyvsp[0].xf)), (yyvsp[-2].obj)); }
    break;

  case 71:
#line 268 "sketch.y"
    { (yyval.obj) = new_transform_def((yyvsp[0].xf)); }
    break;

  case 72:
#line 271 "sketch.y"
    { set_float(&(yyval.exv), (yyvsp[0].flt)); }
    break;

  case 73:
#line 272 "sketch.y"
    { set_point(&(yyval.exv), (yyvsp[0].pt)); }
    break;

  case 74:
#line 273 "sketch.y"
    { set_vector(&(yyval.exv), (yyvsp[0].vec)); }
    break;

  case 75:
#line 274 "sketch.y"
    { set_transform(&(yyval.exv), (yyvsp[0].xf)); }
    break;

  case 76:
#line 275 "sketch.y"
    { do_add(&(yyval.exv), &(yyvsp[-2].exv), &(yyvsp[0].exv), line); }
    break;

  case 77:
#line 276 "sketch.y"
    { do_sub(&(yyval.exv), &(yyvsp[-2].exv), &(yyvsp[0].exv), line); }
    break;

  case 78:
#line 277 "sketch.y"
    { do_mul(&(yyval.exv), &(yyvsp[-2].exv), &(yyvsp[0].exv), line); }
    break;

  case 79:
#line 278 "sketch.y"
    { do_dvd(&(yyval.exv), &(yyvsp[-2].exv), &(yyvsp[0].exv), line); }
    break;

  case 80:
#line 279 "sketch.y"
    { do_dot(&(yyval.exv), &(yyvsp[-2].exv), &(yyvsp[0].exv), line); }
    break;

  case 81:
#line 280 "sketch.y"
    { do_thn(&(yyval.exv), &(yyvsp[-2].exv), &(yyvsp[0].exv), line); }
    break;

  case 82:
#line 281 "sketch.y"
    { do_mag(&(yyval.exv), &(yyvsp[-1].exv), line); }
    break;

  case 83:
#line 282 "sketch.y"
    { do_neg(&(yyval.exv), &(yyvsp[0].exv), line); }
    break;

  case 84:
#line 283 "sketch.y"
    { do_pwr(&(yyval.exv), &(yyvsp[-2].exv), &(yyvsp[0].exv), line); }
    break;

  case 85:
#line 284 "sketch.y"
    { (yyval.exv) = (yyvsp[-1].exv); }
    break;

  case 86:
#line 285 "sketch.y"
    { do_unit(&(yyval.exv), &(yyvsp[-1].exv), line); }
    break;

  case 87:
#line 286 "sketch.y"
    { do_sqrt(&(yyval.exv), &(yyvsp[-1].exv), line); }
    break;

  case 88:
#line 287 "sketch.y"
    { do_sin(&(yyval.exv), &(yyvsp[-1].exv), line); }
    break;

  case 89:
#line 288 "sketch.y"
    { do_cos(&(yyval.exv), &(yyvsp[-1].exv), line); }
    break;

  case 90:
#line 289 "sketch.y"
    { do_asin(&(yyval.exv), &(yyvsp[-1].exv), line); }
    break;

  case 91:
#line 290 "sketch.y"
    { do_acos(&(yyval.exv), &(yyvsp[-1].exv), line); }
    break;

  case 92:
#line 291 "sketch.y"
    { do_atan2(&(yyval.exv), &(yyvsp[-3].exv), &(yyvsp[-1].exv), line); }
    break;

  case 93:
#line 292 "sketch.y"
    { do_index(&(yyval.exv), &(yyvsp[-1].exv), (yyvsp[0].index), line); }
    break;

  case 94:
#line 295 "sketch.y"
    { (yyval.flt) = (yyvsp[0].flt); }
    break;

  case 95:
#line 296 "sketch.y"
    { look_up_scalar(sym_tab, &(yyval.flt), line, (yyvsp[0].name)); }
    break;

  case 96:
#line 299 "sketch.y"
    { coerce_to_float(&(yyvsp[0].exv), &(yyval.flt), line); }
    break;

  case 97:
#line 303 "sketch.y"
    { 
                            (yyval.pt)[X] = (yyvsp[-5].flt); (yyval.pt)[Y] = (yyvsp[-3].flt); (yyval.pt)[Z] = (yyvsp[-1].flt);
                          }
    break;

  case 98:
#line 307 "sketch.y"
    { 
                            (yyval.pt)[X] = (yyvsp[-3].flt); (yyval.pt)[Y] = (yyvsp[-1].flt); (yyval.pt)[Z] = 0;
                          }
    break;

  case 99:
#line 310 "sketch.y"
    { look_up_point(sym_tab, (yyval.pt), line, (yyvsp[0].name)); }
    break;

  case 100:
#line 313 "sketch.y"
    { coerce_to_point(&(yyvsp[0].exv), (yyval.pt), line); }
    break;

  case 101:
#line 316 "sketch.y"
    { copy_vec_3d((yyval.vec), (yyvsp[0].vec)); }
    break;

  case 102:
#line 317 "sketch.y"
    { copy_vec_3d((yyval.vec), (yyvsp[0].vec)); }
    break;

  case 103:
#line 321 "sketch.y"
    { 
                            (yyval.vec)[X] = (yyvsp[-5].flt); (yyval.vec)[Y] = (yyvsp[-3].flt); (yyval.vec)[Z] = (yyvsp[-1].flt);
                          }
    break;

  case 104:
#line 325 "sketch.y"
    { 
                            (yyval.vec)[X] = (yyvsp[-3].flt); (yyval.vec)[Y] = (yyvsp[-1].flt); (yyval.vec)[Z] = 0;
                          }
    break;

  case 105:
#line 330 "sketch.y"
    { look_up_vector(sym_tab, (yyval.vec), line, (yyvsp[0].name)); }
    break;

  case 106:
#line 333 "sketch.y"
    { coerce_to_vector(&(yyvsp[0].exv), (yyval.vec), line); }
    break;

  case 107:
#line 342 "sketch.y"
    { // transform is column major while elements are row major
                            (yyval.xf)[0] = (yyvsp[-35].flt);  (yyval.xf)[4] = (yyvsp[-33].flt);  (yyval.xf)[8]  = (yyvsp[-31].flt);  (yyval.xf)[12] = (yyvsp[-29].flt);
                            (yyval.xf)[1] = (yyvsp[-26].flt); (yyval.xf)[5] = (yyvsp[-24].flt); (yyval.xf)[9]  = (yyvsp[-22].flt); (yyval.xf)[13] = (yyvsp[-20].flt);
                            (yyval.xf)[2] = (yyvsp[-17].flt); (yyval.xf)[6] = (yyvsp[-15].flt); (yyval.xf)[10] = (yyvsp[-13].flt); (yyval.xf)[14] = (yyvsp[-11].flt);
                            (yyval.xf)[3] = (yyvsp[-8].flt); (yyval.xf)[7] = (yyvsp[-6].flt); (yyval.xf)[11] = (yyvsp[-4].flt); (yyval.xf)[15] = (yyvsp[-2].flt);
                          }
    break;

  case 108:
#line 349 "sketch.y"
    {
                            set_angle_axis_rot_about_point((yyval.xf), (yyvsp[-1].flt) * (PI/180), 0, 0);
                          }
    break;

  case 109:
#line 353 "sketch.y"
    {
                            if (EXPR_TYPE_IS(&(yyvsp[-1].exv), E_POINT))
                              set_angle_axis_rot_about_point((yyval.xf), (yyvsp[-3].flt) * (PI/180), (yyvsp[-1].exv).val.pt, 0);
                            else if (EXPR_TYPE_IS(&(yyvsp[-1].exv), E_VECTOR))
                              set_angle_axis_rot_about_point((yyval.xf), (yyvsp[-3].flt) * (PI/180), 0, (yyvsp[-1].exv).val.vec);
                            else
                              err(line, "expected point or vector rotation parameter, and it's a %s",
                                expr_val_type_str[(yyvsp[-1].exv).tag]);
                          }
    break;

  case 110:
#line 363 "sketch.y"
    {
                            set_angle_axis_rot_about_point((yyval.xf), (yyvsp[-5].flt) * (PI/180), (yyvsp[-3].pt), (yyvsp[-1].vec));
                          }
    break;

  case 111:
#line 367 "sketch.y"
    {
                            set_translation((yyval.xf), (yyvsp[-1].vec)[X], (yyvsp[-1].vec)[Y], (yyvsp[-1].vec)[Z]);
                          }
    break;

  case 112:
#line 371 "sketch.y"
    {
                            if ((yyvsp[-1].exv).tag == E_FLOAT) {
                              FLOAT s = (yyvsp[-1].exv).val.flt;
                              set_scale((yyval.xf), s, s, s);
                            }
                            else if ((yyvsp[-1].exv).tag == E_VECTOR) {
                              VECTOR v = (yyvsp[-1].exv).val.vec;
                              set_scale((yyval.xf), v[X], v[Y], v[Z]);
                            }
                            else {
                              err(line, 
                                "expected scalar or vector scale parameter, and it's a %s",
                                expr_val_type_str[(yyvsp[-1].exv).tag]);
                              set_ident((yyval.xf));
                            }
                          }
    break;

  case 113:
#line 387 "sketch.y"
    { set_parallel_projection((yyval.xf)); }
    break;

  case 114:
#line 388 "sketch.y"
    { set_perspective_projection((yyval.xf), (yyvsp[-1].flt)); }
    break;

  case 115:
#line 389 "sketch.y"
    { set_perspective_transform((yyval.xf), (yyvsp[-1].flt)); }
    break;

  case 116:
#line 391 "sketch.y"
    {
                            if ((yyvsp[-3].exv).tag == E_VECTOR)
                              set_view_transform((yyval.xf), (yyvsp[-5].pt), (yyvsp[-3].exv).val.vec, (yyvsp[-1].vec));
                            else if ((yyvsp[-3].exv).tag == E_POINT)
                              set_view_transform_with_look_at((yyval.xf), (yyvsp[-5].pt), (yyvsp[-3].exv).val.pt, (yyvsp[-1].vec));
                            else
                              err(line, "expected point or vector view parameter, and it's a %s",
                                expr_val_type_str[(yyvsp[-3].exv).tag]);
                          }
    break;

  case 117:
#line 401 "sketch.y"
    {
                            if ((yyvsp[-1].exv).tag == E_VECTOR)
                              set_view_transform((yyval.xf), (yyvsp[-3].pt), (yyvsp[-1].exv).val.vec, NULL);
                            else if ((yyvsp[-1].exv).tag == E_POINT)
                              set_view_transform_with_look_at((yyval.xf), (yyvsp[-3].pt), (yyvsp[-1].exv).val.pt, NULL);
                            else
                              err(line, "expected point or vector view parameter, and it's a %s",
                                expr_val_type_str[(yyvsp[-1].exv).tag]);
                          }
    break;

  case 118:
#line 411 "sketch.y"
    {
                            set_view_transform((yyval.xf), (yyvsp[-1].pt), NULL, NULL);
                          }
    break;

  case 119:
#line 415 "sketch.y"
    { do_inverse((yyval.xf), (yyvsp[-1].xf), line); }
    break;

  case 120:
#line 416 "sketch.y"
    { look_up_transform(sym_tab, (yyval.xf), line, (yyvsp[0].name)); }
    break;

  case 121:
#line 419 "sketch.y"
    { coerce_to_transform(&(yyvsp[0].exv), (yyval.xf), line); }
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */
#line 2209 "y.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  int yytype = YYTRANSLATE (yychar);
	  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
	  YYSIZE_T yysize = yysize0;
	  YYSIZE_T yysize1;
	  int yysize_overflow = 0;
	  char *yymsg = 0;
#	  define YYERROR_VERBOSE_ARGS_MAXIMUM 5
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;

#if 0
	  /* This is so xgettext sees the translatable formats that are
	     constructed on the fly.  */
	  YY_("syntax error, unexpected %s");
	  YY_("syntax error, unexpected %s, expecting %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
#endif
	  char *yyfmt;
	  char const *yyf;
	  static char const yyunexpected[] = "syntax error, unexpected %s";
	  static char const yyexpecting[] = ", expecting %s";
	  static char const yyor[] = " or %s";
	  char yyformat[sizeof yyunexpected
			+ sizeof yyexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
			   * (sizeof yyor - 1))];
	  char const *yyprefix = yyexpecting;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 1;

	  yyarg[0] = yytname[yytype];
	  yyfmt = yystpcpy (yyformat, yyunexpected);

	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
		  {
		    yycount = 1;
		    yysize = yysize0;
		    yyformat[sizeof yyunexpected - 1] = '\0';
		    break;
		  }
		yyarg[yycount++] = yytname[yyx];
		yysize1 = yysize + yytnamerr (0, yytname[yyx]);
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + yystrlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow && yysize <= YYSTACK_ALLOC_MAXIMUM)
	    yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg)
	    {
	      /* Avoid sprintf, as that infringes on the user's name space.
		 Don't have undefined behavior even if the translation
		 produced a string with the wrong number of "%s"s.  */
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
		{
		  if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		    {
		      yyp += yytnamerr (yyp, yyarg[yyi++]);
		      yyf += 2;
		    }
		  else
		    {
		      yyp++;
		      yyf++;
		    }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    {
	      yyerror (YY_("syntax error"));
	      goto yyexhaustedlab;
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (YY_("syntax error"));
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (0)
     goto yyerrorlab;

yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping", yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK;
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 422 "sketch.y"


int parse(SYMBOL_TABLE *st)
{
  int ret;

  objects = NULL;
  sym_tab = st;
  ret = yyparse();

  // should set sym_tab back to NULL
  sym_tab = old_scope(sym_tab);
  if (sym_tab)
    die(no_line, "zombie symbol table");

  return ret;
}

OBJECT *parsed_objects(void)
{
	return objects;
}

