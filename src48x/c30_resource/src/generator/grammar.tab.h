/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INT_LIT = 258,
     VERSION_LIT = 259,
     STR_LIT = 260,
     CHAR_LIT = 261,
     T_TOOL_CHAIN = 262,
     T_TOOL_VERSION = 263,
     T_RESOURCE_VERSION = 264,
     T_FIELD_COUNT = 265,
     T_FIELD_SIZES = 266,
     T_RESOURCE_INFO_START = 267,
     LSHIFT = 268,
     RSHIFT = 269
   };
#endif
/* Tokens.  */
#define INT_LIT 258
#define VERSION_LIT 259
#define STR_LIT 260
#define CHAR_LIT 261
#define T_TOOL_CHAIN 262
#define T_TOOL_VERSION 263
#define T_RESOURCE_VERSION 264
#define T_FIELD_COUNT 265
#define T_FIELD_SIZES 266
#define T_RESOURCE_INFO_START 267
#define LSHIFT 268
#define RSHIFT 269




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 25 "grammar.y"
{
  unsigned int   ival;
  char *sval;
  unsigned char  cval;
  struct resource_version version;
  struct resource_data *data;
}
/* Line 1529 of yacc.c.  */
#line 85 "grammar.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

