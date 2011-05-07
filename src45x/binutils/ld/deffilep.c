/* A Bison parser, made by GNU Bison 1.875c.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

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
     NAME = 258,
     LIBRARY = 259,
     DESCRIPTION = 260,
     STACKSIZE_K = 261,
     HEAPSIZE = 262,
     CODE = 263,
     DATAU = 264,
     DATAL = 265,
     SECTIONS = 266,
     EXPORTS = 267,
     IMPORTS = 268,
     VERSIONK = 269,
     BASE = 270,
     CONSTANTU = 271,
     CONSTANTL = 272,
     PRIVATEU = 273,
     PRIVATEL = 274,
     ALIGNCOMM = 275,
     READ = 276,
     WRITE = 277,
     EXECUTE = 278,
     SHARED = 279,
     NONAMEU = 280,
     NONAMEL = 281,
     DIRECTIVE = 282,
     EQUAL = 283,
     ID = 284,
     DIGITS = 285
   };
#endif
#define NAME 258
#define LIBRARY 259
#define DESCRIPTION 260
#define STACKSIZE_K 261
#define HEAPSIZE 262
#define CODE 263
#define DATAU 264
#define DATAL 265
#define SECTIONS 266
#define EXPORTS 267
#define IMPORTS 268
#define VERSIONK 269
#define BASE 270
#define CONSTANTU 271
#define CONSTANTL 272
#define PRIVATEU 273
#define PRIVATEL 274
#define ALIGNCOMM 275
#define READ 276
#define WRITE 277
#define EXECUTE 278
#define SHARED 279
#define NONAMEU 280
#define NONAMEL 281
#define DIRECTIVE 282
#define EQUAL 283
#define ID 284
#define DIGITS 285




/* Copy the first part of user declarations.  */
#line 1 "deffilep.y"
 /* deffilep.y - parser for .def files */

/*   Copyright 1995, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2005, 2006,
     2007, 2009 Free Software Foundation, Inc.

     This file is part of GNU Binutils.

     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
     MA 02110-1301, USA.  */

#include "sysdep.h"
#include "libiberty.h"
#include "safe-ctype.h"
#include "bfd.h"
#include "ld.h"
#include "ldmisc.h"
#include "deffile.h"

#define TRACE 0

#define ROUND_UP(a, b) (((a)+((b)-1))&~((b)-1))

/* Remap normal yacc parser interface names (yyparse, yylex, yyerror, etc),
   as well as gratuitiously global symbol names, so we can have multiple
   yacc generated parsers in ld.  Note that these are only the variables
   produced by yacc.  If other parser generators (bison, byacc, etc) produce
   additional global names that conflict at link time, then those parser
   generators need to be fixed instead of adding those names to this list.  */

#define	yymaxdepth def_maxdepth
#define	yyparse	def_parse
#define	yylex	def_lex
#define	yyerror	def_error
#define	yylval	def_lval
#define	yychar	def_char
#define	yydebug	def_debug
#define	yypact	def_pact	
#define	yyr1	def_r1			
#define	yyr2	def_r2			
#define	yydef	def_def		
#define	yychk	def_chk		
#define	yypgo	def_pgo		
#define	yyact	def_act		
#define	yyexca	def_exca
#define yyerrflag def_errflag
#define yynerrs	def_nerrs
#define	yyps	def_ps
#define	yypv	def_pv
#define	yys	def_s
#define	yy_yys	def_yys
#define	yystate	def_state
#define	yytmp	def_tmp
#define	yyv	def_v
#define	yy_yyv	def_yyv
#define	yyval	def_val
#define	yylloc	def_lloc
#define yyreds	def_reds		/* With YYDEBUG defined.  */
#define yytoks	def_toks		/* With YYDEBUG defined.  */
#define yylhs	def_yylhs
#define yylen	def_yylen
#define yydefred def_yydefred
#define yydgoto	def_yydgoto
#define yysindex def_yysindex
#define yyrindex def_yyrindex
#define yygindex def_yygindex
#define yytable	 def_yytable
#define yycheck	 def_yycheck

static void def_description (const char *);
static void def_exports (const char *, const char *, int, int, const char *);
static void def_heapsize (int, int);
static void def_import (const char *, const char *, const char *, const char *,
			int, const char *);
static void def_image_name (const char *, int, int);
static void def_section (const char *, int);
static void def_section_alt (const char *, const char *);
static void def_stacksize (int, int);
static void def_version (int, int);
static void def_directive (char *);
static void def_aligncomm (char *str, int align);
static int def_parse (void);
static int def_error (const char *);
static int def_lex (void);

static int lex_forced_token = 0;
static const char *lex_parse_string = 0;
static const char *lex_parse_string_end = 0;



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 103 "deffilep.y"
typedef union YYSTYPE {
  char *id;
  int number;
  char *digits;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 244 "deffilep.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 256 "deffilep.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
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
	  register YYSIZE_T yyi;		\
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
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  44
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   125

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  35
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  25
/* YYNRULES -- Number of rules. */
#define YYNRULES  73
/* YYNRULES -- Number of states. */
#define YYNSTATES  117

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   285

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    32,     2,    31,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    33,     2,     2,    34,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     6,     8,    12,    16,    19,    23,    27,
      30,    33,    36,    39,    42,    45,    50,    53,    58,    59,
      61,    64,    72,    76,    77,    79,    81,    83,    85,    87,
      89,    91,    93,    96,    98,   107,   116,   123,   130,   137,
     142,   145,   147,   150,   153,   157,   159,   161,   162,   165,
     166,   168,   170,   172,   174,   176,   180,   181,   184,   185,
     188,   189,   192,   193,   197,   198,   200,   204,   206,   211,
     213,   214,   216,   217
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      36,     0,    -1,    36,    37,    -1,    37,    -1,     3,    50,
      54,    -1,     4,    50,    54,    -1,     5,    29,    -1,     6,
      59,    48,    -1,     7,    59,    48,    -1,     8,    46,    -1,
       9,    46,    -1,    11,    44,    -1,    12,    38,    -1,    13,
      42,    -1,    14,    59,    -1,    14,    59,    31,    59,    -1,
      27,    29,    -1,    20,    56,    32,    59,    -1,    -1,    39,
      -1,    38,    39,    -1,    55,    53,    52,    47,    40,    47,
      51,    -1,    41,    47,    40,    -1,    -1,    25,    -1,    26,
      -1,    16,    -1,    17,    -1,     9,    -1,    10,    -1,    18,
      -1,    19,    -1,    42,    43,    -1,    43,    -1,    29,    33,
      29,    31,    29,    31,    29,    51,    -1,    29,    33,    29,
      31,    29,    31,    59,    51,    -1,    29,    33,    29,    31,
      29,    51,    -1,    29,    33,    29,    31,    59,    51,    -1,
      29,    31,    29,    31,    29,    51,    -1,    29,    31,    29,
      51,    -1,    44,    45,    -1,    45,    -1,    29,    46,    -1,
      29,    29,    -1,    46,    47,    49,    -1,    49,    -1,    32,
      -1,    -1,    32,    59,    -1,    -1,    21,    -1,    22,    -1,
      23,    -1,    24,    -1,    29,    -1,    29,    31,    29,    -1,
      -1,    28,    29,    -1,    -1,    34,    59,    -1,    -1,    33,
      55,    -1,    -1,    15,    33,    59,    -1,    -1,    29,    -1,
      55,    31,    29,    -1,    29,    -1,    56,    31,    57,    58,
      -1,    30,    -1,    -1,    29,    -1,    -1,    30,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   124,   124,   125,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   146,   148,
     149,   156,   163,   164,   167,   168,   169,   170,   171,   172,
     173,   174,   177,   178,   182,   184,   186,   188,   190,   192,
     197,   198,   202,   203,   207,   208,   212,   213,   215,   216,
     220,   221,   222,   223,   226,   227,   233,   236,   237,   241,
     242,   246,   247,   250,   251,   254,   255,   263,   264,   272,
     273,   276,   277,   280
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NAME", "LIBRARY", "DESCRIPTION",
  "STACKSIZE_K", "HEAPSIZE", "CODE", "DATAU", "DATAL", "SECTIONS",
  "EXPORTS", "IMPORTS", "VERSIONK", "BASE", "CONSTANTU", "CONSTANTL",
  "PRIVATEU", "PRIVATEL", "ALIGNCOMM", "READ", "WRITE", "EXECUTE",
  "SHARED", "NONAMEU", "NONAMEL", "DIRECTIVE", "EQUAL", "ID", "DIGITS",
  "'.'", "','", "'='", "'@'", "$accept", "start", "command", "explist",
  "expline", "exp_opt_list", "exp_opt", "implist", "impline", "seclist",
  "secline", "attr_list", "opt_comma", "opt_number", "attr", "opt_name",
  "opt_equalequal_name", "opt_ordinal", "opt_equal_name", "opt_base",
  "dot_name", "anylang_id", "opt_digits", "opt_id", "NUMBER", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,    46,    44,    61,    64
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    35,    36,    36,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    38,    38,
      38,    39,    40,    40,    41,    41,    41,    41,    41,    41,
      41,    41,    42,    42,    43,    43,    43,    43,    43,    43,
      44,    44,    45,    45,    46,    46,    47,    47,    48,    48,
      49,    49,    49,    49,    50,    50,    50,    51,    51,    52,
      52,    53,    53,    54,    54,    55,    55,    56,    56,    57,
      57,    58,    58,    59
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     1,     3,     3,     2,     3,     3,     2,
       2,     2,     2,     2,     2,     4,     2,     4,     0,     1,
       2,     7,     3,     0,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     8,     8,     6,     6,     6,     4,
       2,     1,     2,     2,     3,     1,     1,     0,     2,     0,
       1,     1,     1,     1,     1,     3,     0,     2,     0,     2,
       0,     2,     0,     3,     0,     1,     3,     1,     4,     1,
       0,     1,     0,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,    56,    56,     0,     0,     0,     0,     0,     0,    18,
       0,     0,     0,     0,     0,     3,    54,    64,    64,     6,
      73,    49,    49,    50,    51,    52,    53,     9,    45,    10,
       0,    11,    41,    65,    12,    19,    62,     0,    13,    33,
      14,    67,     0,    16,     1,     2,     0,     0,     4,     5,
       0,     7,     8,    46,     0,    43,    42,    40,    20,     0,
       0,    60,     0,     0,    32,     0,    70,     0,    55,     0,
      48,    44,    66,    61,     0,    47,    58,     0,    15,    69,
      72,    17,    63,    59,    23,     0,     0,    39,     0,    71,
      68,    28,    29,    26,    27,    30,    31,    24,    25,    47,
      47,    57,    58,    58,    58,    58,    23,    38,     0,    36,
      37,    21,    22,    58,    58,    34,    35
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,    14,    15,    34,    35,    99,   100,    38,    39,    31,
      32,    27,    54,    51,    28,    17,    87,    75,    61,    48,
      36,    42,    80,    90,    21
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -75
static const yysigned_char yypact[] =
{
      29,   -15,   -15,   -10,     0,     0,    -1,    -1,    10,    17,
      21,     0,    28,    32,     4,   -75,    62,    65,    65,   -75,
     -75,    60,    60,   -75,   -75,   -75,   -75,    31,   -75,    31,
      55,    10,   -75,   -75,    17,   -75,    -4,    54,    21,   -75,
      63,   -75,   -29,   -75,   -75,   -75,    57,    64,   -75,   -75,
       0,   -75,   -75,   -75,    -1,   -75,    31,   -75,   -75,    66,
      17,    67,    69,    70,   -75,     0,    72,     0,   -75,     0,
     -75,   -75,   -75,    73,     0,    68,    16,    74,   -75,   -75,
      77,   -75,   -75,   -75,    49,    78,    79,   -75,    59,   -75,
     -75,   -75,   -75,   -75,   -75,   -75,   -75,   -75,   -75,    68,
      68,   -75,    81,    20,    81,    81,    49,   -75,    61,   -75,
     -75,   -75,   -75,    81,    81,   -75,   -75
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -75,   -75,    82,   -75,    76,     5,   -75,   -75,    75,   -75,
      83,    -2,   -74,    90,    71,   113,   -32,   -75,   -75,    98,
      58,   -75,   -75,   -75,    -5
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -48
static const yysigned_char yytable[] =
{
      22,    84,    66,    67,    44,    29,    40,     1,     2,     3,
       4,     5,     6,     7,    16,     8,     9,    10,    11,    19,
      23,    24,    25,    26,    12,   105,   106,    59,    56,    60,
      20,    13,     1,     2,     3,     4,     5,     6,     7,    30,
       8,     9,    10,    11,    85,    70,    33,    86,    85,    12,
      37,   108,   -47,   -47,   -47,   -47,    13,    41,    91,    92,
      78,    43,    81,    53,    82,    93,    94,    95,    96,    83,
     107,   109,   110,   111,    97,    98,    23,    24,    25,    26,
      47,   115,   116,   104,    55,    62,    68,    63,   103,    20,
     113,    20,    50,    46,    65,    72,    45,    69,    76,    77,
      53,    74,    79,   114,    59,    88,    89,   101,   102,    85,
      58,   112,    52,    64,    57,    18,    49,     0,    73,     0,
       0,     0,     0,     0,     0,    71
};

static const yysigned_char yycheck[] =
{
       5,    75,    31,    32,     0,     7,    11,     3,     4,     5,
       6,     7,     8,     9,    29,    11,    12,    13,    14,    29,
      21,    22,    23,    24,    20,    99,   100,    31,    30,    33,
      30,    27,     3,     4,     5,     6,     7,     8,     9,    29,
      11,    12,    13,    14,    28,    50,    29,    31,    28,    20,
      29,    31,    21,    22,    23,    24,    27,    29,     9,    10,
      65,    29,    67,    32,    69,    16,    17,    18,    19,    74,
     102,   103,   104,   105,    25,    26,    21,    22,    23,    24,
      15,   113,   114,    88,    29,    31,    29,    33,    29,    30,
      29,    30,    32,    31,    31,    29,    14,    33,    29,    29,
      32,    34,    30,   108,    31,    31,    29,    29,    29,    28,
      34,   106,    22,    38,    31,     2,    18,    -1,    60,    -1,
      -1,    -1,    -1,    -1,    -1,    54
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    11,    12,
      13,    14,    20,    27,    36,    37,    29,    50,    50,    29,
      30,    59,    59,    21,    22,    23,    24,    46,    49,    46,
      29,    44,    45,    29,    38,    39,    55,    29,    42,    43,
      59,    29,    56,    29,     0,    37,    31,    15,    54,    54,
      32,    48,    48,    32,    47,    29,    46,    45,    39,    31,
      33,    53,    31,    33,    43,    31,    31,    32,    29,    33,
      59,    49,    29,    55,    34,    52,    29,    29,    59,    30,
      57,    59,    59,    59,    47,    28,    31,    51,    31,    29,
      58,     9,    10,    16,    17,    18,    19,    25,    26,    40,
      41,    29,    29,    29,    59,    47,    47,    51,    31,    51,
      51,    51,    40,    29,    59,    51,    51
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

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
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
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

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
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
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
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
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
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
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

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
  register const char *yys = yystr;

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
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



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
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

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
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

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



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
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

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



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
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
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
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
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

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

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
        case 4:
#line 129 "deffilep.y"
    { def_image_name (yyvsp[-1].id, yyvsp[0].number, 0); }
    break;

  case 5:
#line 130 "deffilep.y"
    { def_image_name (yyvsp[-1].id, yyvsp[0].number, 1); }
    break;

  case 6:
#line 131 "deffilep.y"
    { def_description (yyvsp[0].id);}
    break;

  case 7:
#line 132 "deffilep.y"
    { def_stacksize (yyvsp[-1].number, yyvsp[0].number);}
    break;

  case 8:
#line 133 "deffilep.y"
    { def_heapsize (yyvsp[-1].number, yyvsp[0].number);}
    break;

  case 9:
#line 134 "deffilep.y"
    { def_section ("CODE", yyvsp[0].number);}
    break;

  case 10:
#line 135 "deffilep.y"
    { def_section ("DATA", yyvsp[0].number);}
    break;

  case 14:
#line 139 "deffilep.y"
    { def_version (yyvsp[0].number, 0);}
    break;

  case 15:
#line 140 "deffilep.y"
    { def_version (yyvsp[-2].number, yyvsp[0].number);}
    break;

  case 16:
#line 141 "deffilep.y"
    { def_directive (yyvsp[0].id);}
    break;

  case 17:
#line 142 "deffilep.y"
    { def_aligncomm (yyvsp[-2].id, yyvsp[0].number);}
    break;

  case 21:
#line 157 "deffilep.y"
    { def_exports (yyvsp[-6].id, yyvsp[-5].id, yyvsp[-4].number, yyvsp[-2].number, yyvsp[0].id); }
    break;

  case 22:
#line 163 "deffilep.y"
    { yyval.number = yyvsp[-2].number | yyvsp[0].number; }
    break;

  case 23:
#line 164 "deffilep.y"
    { yyval.number = 0; }
    break;

  case 24:
#line 167 "deffilep.y"
    { yyval.number = 1; }
    break;

  case 25:
#line 168 "deffilep.y"
    { yyval.number = 1; }
    break;

  case 26:
#line 169 "deffilep.y"
    { yyval.number = 2; }
    break;

  case 27:
#line 170 "deffilep.y"
    { yyval.number = 2; }
    break;

  case 28:
#line 171 "deffilep.y"
    { yyval.number = 4; }
    break;

  case 29:
#line 172 "deffilep.y"
    { yyval.number = 4; }
    break;

  case 30:
#line 173 "deffilep.y"
    { yyval.number = 8; }
    break;

  case 31:
#line 174 "deffilep.y"
    { yyval.number = 8; }
    break;

  case 34:
#line 183 "deffilep.y"
    { def_import (yyvsp[-7].id, yyvsp[-5].id, yyvsp[-3].id, yyvsp[-1].id, -1, yyvsp[0].id); }
    break;

  case 35:
#line 185 "deffilep.y"
    { def_import (yyvsp[-7].id, yyvsp[-5].id, yyvsp[-3].id,  0, yyvsp[-1].number, yyvsp[0].id); }
    break;

  case 36:
#line 187 "deffilep.y"
    { def_import (yyvsp[-5].id, yyvsp[-3].id,  0, yyvsp[-1].id, -1, yyvsp[0].id); }
    break;

  case 37:
#line 189 "deffilep.y"
    { def_import (yyvsp[-5].id, yyvsp[-3].id,  0,  0, yyvsp[-1].number, yyvsp[0].id); }
    break;

  case 38:
#line 191 "deffilep.y"
    { def_import( 0, yyvsp[-5].id, yyvsp[-3].id, yyvsp[-1].id, -1, yyvsp[0].id); }
    break;

  case 39:
#line 193 "deffilep.y"
    { def_import ( 0, yyvsp[-3].id,  0, yyvsp[-1].id, -1, yyvsp[0].id); }
    break;

  case 42:
#line 202 "deffilep.y"
    { def_section (yyvsp[-1].id, yyvsp[0].number);}
    break;

  case 43:
#line 203 "deffilep.y"
    { def_section_alt (yyvsp[-1].id, yyvsp[0].id);}
    break;

  case 44:
#line 207 "deffilep.y"
    { yyval.number = yyvsp[-2].number | yyvsp[0].number; }
    break;

  case 45:
#line 208 "deffilep.y"
    { yyval.number = yyvsp[0].number; }
    break;

  case 48:
#line 215 "deffilep.y"
    { yyval.number=yyvsp[0].number;}
    break;

  case 49:
#line 216 "deffilep.y"
    { yyval.number=-1;}
    break;

  case 50:
#line 220 "deffilep.y"
    { yyval.number = 1;}
    break;

  case 51:
#line 221 "deffilep.y"
    { yyval.number = 2;}
    break;

  case 52:
#line 222 "deffilep.y"
    { yyval.number=4;}
    break;

  case 53:
#line 223 "deffilep.y"
    { yyval.number=8;}
    break;

  case 54:
#line 226 "deffilep.y"
    { yyval.id = yyvsp[0].id; }
    break;

  case 55:
#line 228 "deffilep.y"
    { 
	    char *name = xmalloc (strlen (yyvsp[-2].id) + 1 + strlen (yyvsp[0].id) + 1);
	    sprintf (name, "%s.%s", yyvsp[-2].id, yyvsp[0].id);
	    yyval.id = name;
	  }
    break;

  case 56:
#line 233 "deffilep.y"
    { yyval.id = ""; }
    break;

  case 57:
#line 236 "deffilep.y"
    { yyval.id = yyvsp[0].id; }
    break;

  case 58:
#line 237 "deffilep.y"
    { yyval.id = 0; }
    break;

  case 59:
#line 241 "deffilep.y"
    { yyval.number = yyvsp[0].number;}
    break;

  case 60:
#line 242 "deffilep.y"
    { yyval.number = -1;}
    break;

  case 61:
#line 246 "deffilep.y"
    { yyval.id = yyvsp[0].id; }
    break;

  case 62:
#line 247 "deffilep.y"
    { yyval.id =  0; }
    break;

  case 63:
#line 250 "deffilep.y"
    { yyval.number = yyvsp[0].number;}
    break;

  case 64:
#line 251 "deffilep.y"
    { yyval.number = -1;}
    break;

  case 65:
#line 254 "deffilep.y"
    { yyval.id = yyvsp[0].id; }
    break;

  case 66:
#line 256 "deffilep.y"
    { 
	    char *name = xmalloc (strlen (yyvsp[-2].id) + 1 + strlen (yyvsp[0].id) + 1);
	    sprintf (name, "%s.%s", yyvsp[-2].id, yyvsp[0].id);
	    yyval.id = name;
	  }
    break;

  case 67:
#line 263 "deffilep.y"
    { yyval.id = yyvsp[0].id; }
    break;

  case 68:
#line 265 "deffilep.y"
    {
	    char *id = xmalloc (strlen (yyvsp[-3].id) + 1 + strlen (yyvsp[-1].digits) + strlen (yyvsp[0].id) + 1);
	    sprintf (id, "%s.%s%s", yyvsp[-3].id, yyvsp[-1].digits, yyvsp[0].id);
	    yyval.id = id;
	  }
    break;

  case 69:
#line 272 "deffilep.y"
    { yyval.digits = yyvsp[0].digits; }
    break;

  case 70:
#line 273 "deffilep.y"
    { yyval.digits = ""; }
    break;

  case 71:
#line 276 "deffilep.y"
    { yyval.id = yyvsp[0].id; }
    break;

  case 72:
#line 277 "deffilep.y"
    { yyval.id = ""; }
    break;

  case 73:
#line 280 "deffilep.y"
    { yyval.number = strtoul (yyvsp[0].digits, 0, 0); }
    break;


    }

/* Line 1000 of yacc.c.  */
#line 1550 "deffilep.c"

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
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
	  yychar = YYEMPTY;

	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 282 "deffilep.y"


/*****************************************************************************
 API
 *****************************************************************************/

static FILE *the_file;
static const char *def_filename;
static int linenumber;
static def_file *def;
static int saw_newline;

struct directive
  {
    struct directive *next;
    char *name;
    int len;
  };

static struct directive *directives = 0;

def_file *
def_file_empty (void)
{
  def_file *rv = xmalloc (sizeof (def_file));
  memset (rv, 0, sizeof (def_file));
  rv->is_dll = -1;
  rv->base_address = (bfd_vma) -1;
  rv->stack_reserve = rv->stack_commit = -1;
  rv->heap_reserve = rv->heap_commit = -1;
  rv->version_major = rv->version_minor = -1;
  return rv;
}

def_file *
def_file_parse (const char *filename, def_file *add_to)
{
  struct directive *d;

  the_file = fopen (filename, "r");
  def_filename = filename;
  linenumber = 1;
  if (!the_file)
    {
      perror (filename);
      return 0;
    }
  if (add_to)
    {
      def = add_to;
    }
  else
    {
      def = def_file_empty ();
    }

  saw_newline = 1;
  if (def_parse ())
    {
      def_file_free (def);
      fclose (the_file);
      return 0;
    }

  fclose (the_file);

  for (d = directives; d; d = d->next)
    {
#if TRACE
      printf ("Adding directive %08x `%s'\n", d->name, d->name);
#endif
      def_file_add_directive (def, d->name, d->len);
    }

  return def;
}

void
def_file_free (def_file *fdef)
{
  int i;

  if (!fdef)
    return;
  if (fdef->name)
    free (fdef->name);
  if (fdef->description)
    free (fdef->description);

  if (fdef->section_defs)
    {
      for (i = 0; i < fdef->num_section_defs; i++)
	{
	  if (fdef->section_defs[i].name)
	    free (fdef->section_defs[i].name);
	  if (fdef->section_defs[i].class)
	    free (fdef->section_defs[i].class);
	}
      free (fdef->section_defs);
    }

  if (fdef->exports)
    {
      for (i = 0; i < fdef->num_exports; i++)
	{
	  if (fdef->exports[i].internal_name
	      && fdef->exports[i].internal_name != fdef->exports[i].name)
	    free (fdef->exports[i].internal_name);
	  if (fdef->exports[i].name)
	    free (fdef->exports[i].name);
	  if (fdef->exports[i].its_name)
	    free (fdef->exports[i].its_name);
	}
      free (fdef->exports);
    }

  if (fdef->imports)
    {
      for (i = 0; i < fdef->num_imports; i++)
	{
	  if (fdef->imports[i].internal_name
	      && fdef->imports[i].internal_name != fdef->imports[i].name)
	    free (fdef->imports[i].internal_name);
	  if (fdef->imports[i].name)
	    free (fdef->imports[i].name);
	  if (fdef->imports[i].its_name)
	    free (fdef->imports[i].its_name);
	}
      free (fdef->imports);
    }

  while (fdef->modules)
    {
      def_file_module *m = fdef->modules;

      fdef->modules = fdef->modules->next;
      free (m);
    }

  while (fdef->aligncomms)
    {
      def_file_aligncomm *c = fdef->aligncomms;

      fdef->aligncomms = fdef->aligncomms->next;
      free (c->symbol_name);
      free (c);
    }

  free (fdef);
}

#ifdef DEF_FILE_PRINT
void
def_file_print (FILE *file, def_file *fdef)
{
  int i;

  fprintf (file, ">>>> def_file at 0x%08x\n", fdef);
  if (fdef->name)
    fprintf (file, "  name: %s\n", fdef->name ? fdef->name : "(unspecified)");
  if (fdef->is_dll != -1)
    fprintf (file, "  is dll: %s\n", fdef->is_dll ? "yes" : "no");
  if (fdef->base_address != (bfd_vma) -1)
    fprintf (file, "  base address: 0x%08x\n", fdef->base_address);
  if (fdef->description)
    fprintf (file, "  description: `%s'\n", fdef->description);
  if (fdef->stack_reserve != -1)
    fprintf (file, "  stack reserve: 0x%08x\n", fdef->stack_reserve);
  if (fdef->stack_commit != -1)
    fprintf (file, "  stack commit: 0x%08x\n", fdef->stack_commit);
  if (fdef->heap_reserve != -1)
    fprintf (file, "  heap reserve: 0x%08x\n", fdef->heap_reserve);
  if (fdef->heap_commit != -1)
    fprintf (file, "  heap commit: 0x%08x\n", fdef->heap_commit);

  if (fdef->num_section_defs > 0)
    {
      fprintf (file, "  section defs:\n");

      for (i = 0; i < fdef->num_section_defs; i++)
	{
	  fprintf (file, "    name: `%s', class: `%s', flags:",
		   fdef->section_defs[i].name, fdef->section_defs[i].class);
	  if (fdef->section_defs[i].flag_read)
	    fprintf (file, " R");
	  if (fdef->section_defs[i].flag_write)
	    fprintf (file, " W");
	  if (fdef->section_defs[i].flag_execute)
	    fprintf (file, " X");
	  if (fdef->section_defs[i].flag_shared)
	    fprintf (file, " S");
	  fprintf (file, "\n");
	}
    }

  if (fdef->num_exports > 0)
    {
      fprintf (file, "  exports:\n");

      for (i = 0; i < fdef->num_exports; i++)
	{
	  fprintf (file, "    name: `%s', int: `%s', ordinal: %d, flags:",
		   fdef->exports[i].name, fdef->exports[i].internal_name,
		   fdef->exports[i].ordinal);
	  if (fdef->exports[i].flag_private)
	    fprintf (file, " P");
	  if (fdef->exports[i].flag_constant)
	    fprintf (file, " C");
	  if (fdef->exports[i].flag_noname)
	    fprintf (file, " N");
	  if (fdef->exports[i].flag_data)
	    fprintf (file, " D");
	  fprintf (file, "\n");
	}
    }

  if (fdef->num_imports > 0)
    {
      fprintf (file, "  imports:\n");

      for (i = 0; i < fdef->num_imports; i++)
	{
	  fprintf (file, "    int: %s, from: `%s', name: `%s', ordinal: %d\n",
		   fdef->imports[i].internal_name,
		   fdef->imports[i].module,
		   fdef->imports[i].name,
		   fdef->imports[i].ordinal);
	}
    }

  if (fdef->version_major != -1)
    fprintf (file, "  version: %d.%d\n", fdef->version_major, fdef->version_minor);

  fprintf (file, "<<<< def_file at 0x%08x\n", fdef);
}
#endif

def_file_export *
def_file_add_export (def_file *fdef,
		     const char *external_name,
		     const char *internal_name,
		     int ordinal,
		     const char *its_name)
{
  def_file_export *e;
  int max_exports = ROUND_UP(fdef->num_exports, 32);

  if (fdef->num_exports >= max_exports)
    {
      max_exports = ROUND_UP(fdef->num_exports + 1, 32);
      if (fdef->exports)
	fdef->exports = xrealloc (fdef->exports,
				 max_exports * sizeof (def_file_export));
      else
	fdef->exports = xmalloc (max_exports * sizeof (def_file_export));
    }
  e = fdef->exports + fdef->num_exports;
  memset (e, 0, sizeof (def_file_export));
  if (internal_name && !external_name)
    external_name = internal_name;
  if (external_name && !internal_name)
    internal_name = external_name;
  e->name = xstrdup (external_name);
  e->internal_name = xstrdup (internal_name);
  e->its_name = (its_name ? xstrdup (its_name) : NULL);
  e->ordinal = ordinal;
  fdef->num_exports++;
  return e;
}

def_file_module *
def_get_module (def_file *fdef, const char *name)
{
  def_file_module *s;

  for (s = fdef->modules; s; s = s->next)
    if (strcmp (s->name, name) == 0)
      return s;

  return NULL;
}

static def_file_module *
def_stash_module (def_file *fdef, const char *name)
{
  def_file_module *s;

  if ((s = def_get_module (fdef, name)) != NULL)
      return s;
  s = xmalloc (sizeof (def_file_module) + strlen (name));
  s->next = fdef->modules;
  fdef->modules = s;
  s->user_data = 0;
  strcpy (s->name, name);
  return s;
}

def_file_import *
def_file_add_import (def_file *fdef,
		     const char *name,
		     const char *module,
		     int ordinal,
		     const char *internal_name,
		     const char *its_name)
{
  def_file_import *i;
  int max_imports = ROUND_UP (fdef->num_imports, 16);

  if (fdef->num_imports >= max_imports)
    {
      max_imports = ROUND_UP (fdef->num_imports+1, 16);

      if (fdef->imports)
	fdef->imports = xrealloc (fdef->imports,
				 max_imports * sizeof (def_file_import));
      else
	fdef->imports = xmalloc (max_imports * sizeof (def_file_import));
    }
  i = fdef->imports + fdef->num_imports;
  memset (i, 0, sizeof (def_file_import));
  if (name)
    i->name = xstrdup (name);
  if (module)
    i->module = def_stash_module (fdef, module);
  i->ordinal = ordinal;
  if (internal_name)
    i->internal_name = xstrdup (internal_name);
  else
    i->internal_name = i->name;
  i->its_name = (its_name ? xstrdup (its_name) : NULL);
  fdef->num_imports++;

  return i;
}

struct
{
  char *param;
  int token;
}
diropts[] =
{
  { "-heap", HEAPSIZE },
  { "-stack", STACKSIZE_K },
  { "-attr", SECTIONS },
  { "-export", EXPORTS },
  { "-aligncomm", ALIGNCOMM },
  { 0, 0 }
};

void
def_file_add_directive (def_file *my_def, const char *param, int len)
{
  def_file *save_def = def;
  const char *pend = param + len;
  char * tend = (char *) param;
  int i;

  def = my_def;

  while (param < pend)
    {
      while (param < pend
	     && (ISSPACE (*param) || *param == '\n' || *param == 0))
	param++;

      if (param == pend)
	break;

      /* Scan forward until we encounter any of:
          - the end of the buffer
	  - the start of a new option
	  - a newline seperating options
          - a NUL seperating options.  */
      for (tend = (char *) (param + 1);
	   (tend < pend
	    && !(ISSPACE (tend[-1]) && *tend == '-')
	    && *tend != '\n' && *tend != 0);
	   tend++)
	;

      for (i = 0; diropts[i].param; i++)
	{
	  len = strlen (diropts[i].param);

	  if (tend - param >= len
	      && strncmp (param, diropts[i].param, len) == 0
	      && (param[len] == ':' || param[len] == ' '))
	    {
	      lex_parse_string_end = tend;
	      lex_parse_string = param + len + 1;
	      lex_forced_token = diropts[i].token;
	      saw_newline = 0;
	      if (def_parse ())
		continue;
	      break;
	    }
	}

      if (!diropts[i].param)
	{
	  char saved;

	  saved = * tend;
	  * tend = 0;
	  /* xgettext:c-format */
	  einfo (_("Warning: .drectve `%s' unrecognized\n"), param);
	  * tend = saved;
	}

      lex_parse_string = 0;
      param = tend;
    }

  def = save_def;
}

/* Parser Callbacks.  */

static void
def_image_name (const char *name, int base, int is_dll)
{
  /* If a LIBRARY or NAME statement is specified without a name, there is nothing
     to do here.  We retain the output filename specified on command line.  */
  if (*name)
    {
      const char* image_name = lbasename (name);

      if (image_name != name)
	einfo ("%s:%d: Warning: path components stripped from %s, '%s'\n",
	       def_filename, linenumber, is_dll ? "LIBRARY" : "NAME",
	       name);
      if (def->name)
	free (def->name);
      /* Append the default suffix, if none specified.  */ 
      if (strchr (image_name, '.') == 0)
	{
	  const char * suffix = is_dll ? ".dll" : ".exe";

	  def->name = xmalloc (strlen (image_name) + strlen (suffix) + 1);
	  sprintf (def->name, "%s%s", image_name, suffix);
        }
      else
	def->name = xstrdup (image_name);
    }

  /* Honor a BASE address statement, even if LIBRARY string is empty.  */
  def->base_address = base;
  def->is_dll = is_dll;
}

static void
def_description (const char *text)
{
  int len = def->description ? strlen (def->description) : 0;

  len += strlen (text) + 1;
  if (def->description)
    {
      def->description = xrealloc (def->description, len);
      strcat (def->description, text);
    }
  else
    {
      def->description = xmalloc (len);
      strcpy (def->description, text);
    }
}

static void
def_stacksize (int reserve, int commit)
{
  def->stack_reserve = reserve;
  def->stack_commit = commit;
}

static void
def_heapsize (int reserve, int commit)
{
  def->heap_reserve = reserve;
  def->heap_commit = commit;
}

static void
def_section (const char *name, int attr)
{
  def_file_section *s;
  int max_sections = ROUND_UP (def->num_section_defs, 4);

  if (def->num_section_defs >= max_sections)
    {
      max_sections = ROUND_UP (def->num_section_defs+1, 4);

      if (def->section_defs)
	def->section_defs = xrealloc (def->section_defs,
				      max_sections * sizeof (def_file_import));
      else
	def->section_defs = xmalloc (max_sections * sizeof (def_file_import));
    }
  s = def->section_defs + def->num_section_defs;
  memset (s, 0, sizeof (def_file_section));
  s->name = xstrdup (name);
  if (attr & 1)
    s->flag_read = 1;
  if (attr & 2)
    s->flag_write = 1;
  if (attr & 4)
    s->flag_execute = 1;
  if (attr & 8)
    s->flag_shared = 1;

  def->num_section_defs++;
}

static void
def_section_alt (const char *name, const char *attr)
{
  int aval = 0;

  for (; *attr; attr++)
    {
      switch (*attr)
	{
	case 'R':
	case 'r':
	  aval |= 1;
	  break;
	case 'W':
	case 'w':
	  aval |= 2;
	  break;
	case 'X':
	case 'x':
	  aval |= 4;
	  break;
	case 'S':
	case 's':
	  aval |= 8;
	  break;
	}
    }
  def_section (name, aval);
}

static void
def_exports (const char *external_name,
	     const char *internal_name,
	     int ordinal,
	     int flags,
	     const char *its_name)
{
  def_file_export *dfe;

  if (!internal_name && external_name)
    internal_name = external_name;
#if TRACE
  printf ("def_exports, ext=%s int=%s\n", external_name, internal_name);
#endif

  dfe = def_file_add_export (def, external_name, internal_name, ordinal,
  							 its_name);
  if (flags & 1)
    dfe->flag_noname = 1;
  if (flags & 2)
    dfe->flag_constant = 1;
  if (flags & 4)
    dfe->flag_data = 1;
  if (flags & 8)
    dfe->flag_private = 1;
}

static void
def_import (const char *internal_name,
	    const char *module,
	    const char *dllext,
	    const char *name,
	    int ordinal,
	    const char *its_name)
{
  char *buf = 0;
  const char *ext = dllext ? dllext : "dll";    
   
  buf = xmalloc (strlen (module) + strlen (ext) + 2);
  sprintf (buf, "%s.%s", module, ext);
  module = buf;

  def_file_add_import (def, name, module, ordinal, internal_name, its_name);
  if (buf)
    free (buf);
}

static void
def_version (int major, int minor)
{
  def->version_major = major;
  def->version_minor = minor;
}

static void
def_directive (char *str)
{
  struct directive *d = xmalloc (sizeof (struct directive));

  d->next = directives;
  directives = d;
  d->name = xstrdup (str);
  d->len = strlen (str);
}

static void
def_aligncomm (char *str, int align)
{
  def_file_aligncomm *c = xmalloc (sizeof (def_file_aligncomm));

  c->symbol_name = xstrdup (str);
  c->alignment = (unsigned int) align;

  c->next = def->aligncomms;
  def->aligncomms = c;
}

static int
def_error (const char *err)
{
  einfo ("%P: %s:%d: %s\n",
	 def_filename ? def_filename : "<unknown-file>", linenumber, err);
  return 0;
}


/* Lexical Scanner.  */

#undef TRACE
#define TRACE 0

/* Never freed, but always reused as needed, so no real leak.  */
static char *buffer = 0;
static int buflen = 0;
static int bufptr = 0;

static void
put_buf (char c)
{
  if (bufptr == buflen)
    {
      buflen += 50;		/* overly reasonable, eh?  */
      if (buffer)
	buffer = xrealloc (buffer, buflen + 1);
      else
	buffer = xmalloc (buflen + 1);
    }
  buffer[bufptr++] = c;
  buffer[bufptr] = 0;		/* not optimal, but very convenient.  */
}

static struct
{
  char *name;
  int token;
}
tokens[] =
{
  { "BASE", BASE },
  { "CODE", CODE },
  { "CONSTANT", CONSTANTU },
  { "constant", CONSTANTL },
  { "DATA", DATAU },
  { "data", DATAL },
  { "DESCRIPTION", DESCRIPTION },
  { "DIRECTIVE", DIRECTIVE },
  { "EXECUTE", EXECUTE },
  { "EXPORTS", EXPORTS },
  { "HEAPSIZE", HEAPSIZE },
  { "IMPORTS", IMPORTS },
  { "LIBRARY", LIBRARY },
  { "NAME", NAME },
  { "NONAME", NONAMEU },
  { "noname", NONAMEL },
  { "PRIVATE", PRIVATEU },
  { "private", PRIVATEL },
  { "READ", READ },
  { "SECTIONS", SECTIONS },
  { "SEGMENTS", SECTIONS },
  { "SHARED", SHARED },
  { "STACKSIZE", STACKSIZE_K },
  { "VERSION", VERSIONK },
  { "WRITE", WRITE },
  { 0, 0 }
};

static int
def_getc (void)
{
  int rv;

  if (lex_parse_string)
    {
      if (lex_parse_string >= lex_parse_string_end)
	rv = EOF;
      else
	rv = *lex_parse_string++;
    }
  else
    {
      rv = fgetc (the_file);
    }
  if (rv == '\n')
    saw_newline = 1;
  return rv;
}

static int
def_ungetc (int c)
{
  if (lex_parse_string)
    {
      lex_parse_string--;
      return c;
    }
  else
    return ungetc (c, the_file);
}

static int
def_lex (void)
{
  int c, i, q;

  if (lex_forced_token)
    {
      i = lex_forced_token;
      lex_forced_token = 0;
#if TRACE
      printf ("lex: forcing token %d\n", i);
#endif
      return i;
    }

  c = def_getc ();

  /* Trim leading whitespace.  */
  while (c != EOF && (c == ' ' || c == '\t') && saw_newline)
    c = def_getc ();

  if (c == EOF)
    {
#if TRACE
      printf ("lex: EOF\n");
#endif
      return 0;
    }

  if (saw_newline && c == ';')
    {
      do
	{
	  c = def_getc ();
	}
      while (c != EOF && c != '\n');
      if (c == '\n')
	return def_lex ();
      return 0;
    }

  /* Must be something else.  */
  saw_newline = 0;

  if (ISDIGIT (c))
    {
      bufptr = 0;
      while (c != EOF && (ISXDIGIT (c) || (c == 'x')))
	{
	  put_buf (c);
	  c = def_getc ();
	}
      if (c != EOF)
	def_ungetc (c);
      yylval.digits = xstrdup (buffer);
#if TRACE
      printf ("lex: `%s' returns DIGITS\n", buffer);
#endif
      return DIGITS;
    }

  if (ISALPHA (c) || strchr ("$:-_?@", c))
    {
      bufptr = 0;
      q = c;
      put_buf (c);
      c = def_getc ();

      if (q == '@')
	{
          if (ISBLANK (c) ) /* '@' followed by whitespace.  */
	    return (q);
          else if (ISDIGIT (c)) /* '@' followed by digit.  */
            {
	      def_ungetc (c);
              return (q);
	    }
#if TRACE
	  printf ("lex: @ returns itself\n");
#endif
	}

      while (c != EOF && (ISALNUM (c) || strchr ("$:-_?/@<>", c)))
	{
	  put_buf (c);
	  c = def_getc ();
	}
      if (c != EOF)
	def_ungetc (c);
      if (ISALPHA (q)) /* Check for tokens.  */
	{
          for (i = 0; tokens[i].name; i++)
	    if (strcmp (tokens[i].name, buffer) == 0)
	      {
#if TRACE
	        printf ("lex: `%s' is a string token\n", buffer);
#endif
	        return tokens[i].token;
	      }
	}
#if TRACE
      printf ("lex: `%s' returns ID\n", buffer);
#endif
      yylval.id = xstrdup (buffer);
      return ID;
    }

  if (c == '\'' || c == '"')
    {
      q = c;
      c = def_getc ();
      bufptr = 0;

      while (c != EOF && c != q)
	{
	  put_buf (c);
	  c = def_getc ();
	}
      yylval.id = xstrdup (buffer);
#if TRACE
      printf ("lex: `%s' returns ID\n", buffer);
#endif
      return ID;
    }

  if ( c == '=')
    {
      c = def_getc ();
      if (c == '=')
        {
#if TRACE
          printf ("lex: `==' returns EQUAL\n");
#endif
		  return EQUAL;
        }
      def_ungetc (c);
#if TRACE
      printf ("lex: `=' returns itself\n");
#endif
      return '=';
    }
  if (c == '.' || c == ',')
    {
#if TRACE
      printf ("lex: `%c' returns itself\n", c);
#endif
      return c;
    }

  if (c == '\n')
    {
      linenumber++;
      saw_newline = 1;
    }

  /*printf ("lex: 0x%02x ignored\n", c); */
  return def_lex ();
}

