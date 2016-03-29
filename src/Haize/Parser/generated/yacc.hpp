/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_YY_YACC_HPP_INCLUDED
# define YY_YY_YACC_HPP_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
/* Line 2058 of yacc.c  */
#line 25 "yacc.y"

#include <Muon/String.hpp>
#include "Haize/Error.hpp"
#include "Haize/Parser/ASTNode.hpp"

namespace generated
{
	enum ParamPrefix
	{
		IN 	= 0x1,
		OUT	= 0x2,
	};
}



/* Line 2058 of yacc.c  */
#line 63 "yacc.hpp"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NT_ROOT = 258,
     NT_CHUNK = 259,
     NT_NAMESPACE = 260,
     NT_FUNCTION = 261,
     NT_ARGUMENTS = 262,
     NT_CLASS = 263,
     NT_STMT = 264,
     S_LPARENT = 265,
     S_RPARENT = 266,
     S_LBRACE = 267,
     S_RBRACE = 268,
     S_LBRACKET = 269,
     S_RBRACKET = 270,
     S_COMMA = 271,
     S_SEPARATOR = 272,
     S_NEWLINE = 273,
     V_IDENTIFIER = 274,
     V_STRING = 275,
     V_FLOATING = 276,
     V_NUMBER = 277,
     V_TRUE = 278,
     V_FALSE = 279,
     V_NIL = 280,
     V_INTEGER = 281,
     K_IN = 282,
     K_OUT = 283,
     K_INOUT = 284,
     K_NEW = 285,
     K_DELETE = 286,
     K_IF = 287,
     K_THEN = 288,
     K_ELSE = 289,
     K_FOR = 290,
     K_WHILE = 291,
     K_SWITCH = 292,
     K_CONTINUE = 293,
     K_BREAK = 294,
     K_RETURN = 295,
     K_GLOBAL = 296,
     K_NAMESPACE = 297,
     K_CLASS = 298,
     MATH_ASN_BITWISE_RSH = 299,
     MATH_ASN_BITWISE_LSH = 300,
     MATH_ASN_BITWISE_NOT = 301,
     MATH_ASN_BITWISE_XOR = 302,
     MATH_ASN_BITWISE_AND = 303,
     MATH_ASN_BITWISE_OR = 304,
     MATH_ASN_MOD = 305,
     MATH_ASN_DIV = 306,
     MATH_ASN_MUL = 307,
     MATH_ASN_SUB = 308,
     MATH_ASN_ADD = 309,
     MATH_ASN = 310,
     LOGIC_OR = 311,
     LOGIC_AND = 312,
     BITWISE_OR = 313,
     BITWISE_XOR = 314,
     BITWISE_AND = 315,
     LOGIC_NEQ = 316,
     LOGIC_EQ = 317,
     LOGIC_GET = 318,
     LOGIC_LET = 319,
     LOGIC_GT = 320,
     LOGIC_LT = 321,
     BITWISE_RSH = 322,
     BITWISE_LSH = 323,
     MATH_SUB = 324,
     MATH_ADD = 325,
     MATH_MOD = 326,
     MATH_DIV = 327,
     MATH_MUL = 328,
     UNARY_PLUS = 329,
     UNARY_MINUS = 330,
     BITWISE_NOT = 331,
     LOGIC_NOT = 332,
     MATH_PREFDEC = 333,
     MATH_PREFINC = 334,
     MATH_POSTDEC = 335,
     MATH_POSTINC = 336,
     S_ACCESSOR = 337
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 13 "yacc.y"

	m::String* string;
	int integer;
	float floating;
	bool boolean;
	hz::parser::ASTNode* node;


/* Line 2058 of yacc.c  */
#line 169 "yacc.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void* scanner, hz::parser::ASTNode* m_node, hz::Error* m_error);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_YACC_HPP_INCLUDED  */
