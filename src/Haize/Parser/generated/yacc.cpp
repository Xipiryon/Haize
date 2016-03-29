/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */

/* Line 371 of yacc.c  */
#line 68 "yacc.cpp"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "yacc.hpp".  */
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
/* Line 387 of yacc.c  */
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



/* Line 387 of yacc.c  */
#line 117 "yacc.cpp"

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
/* Line 387 of yacc.c  */
#line 13 "yacc.y"

	m::String* string;
	int integer;
	float floating;
	bool boolean;
	hz::parser::ASTNode* node;


/* Line 387 of yacc.c  */
#line 223 "yacc.cpp"
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

/* Copy the second part of user declarations.  */
/* Line 390 of yacc.c  */
#line 42 "yacc.y"

#pragma warning(disable: 4065) // switch statement contains 'default' but no 'case' labels

#include <Muon/Core/Define.hpp>
#include "flex.l.hpp"

extern void yyerror(YYLTYPE*, yyscan_t, struct hz::parser::ASTNode*, struct hz::Error*, const char*);
#if defined(MUON_PLATFORM_WINDOWS)
#	define AST_NODE(...)		MUON_NEW(hz::parser::ASTNode, __VA_ARGS__)
#else
#	define AST_NODE(args...)	MUON_NEW(hz::parser::ASTNode, ##args)
#endif
#define AST_NODE_N(id)	MUON_NEW(hz::parser::ASTNode, id, MUON_STR(id))
#define EXTRACT_STR(str, out) do {out = (m::String)*str; MUON_DELETE(str); } while(false);

/* Line 390 of yacc.c  */
#line 279 "yacc.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   193

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  83
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  25
/* YYNRULES -- Number of rules.  */
#define YYNRULES  94
/* YYNRULES -- Number of states.  */
#define YYNSTATES  132

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   337

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    13,    16,    17,    20,
      23,    26,    29,    31,    33,    35,    37,    39,    41,    43,
      46,    49,    53,    56,    62,    70,    71,    73,    76,    81,
      82,    84,    86,    88,    94,    95,    97,    99,   104,   105,
     108,   111,   115,   116,   119,   123,   125,   127,   129,   131,
     133,   135,   137,   139,   141,   143,   145,   147,   150,   155,
     161,   166,   169,   171,   175,   177,   179,   182,   184,   186,
     188,   190,   192,   194,   196,   198,   200,   202,   204,   206,
     208,   210,   212,   214,   216,   218,   221,   224,   227,   230,
     232,   236,   240,   242,   244
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      84,     0,    -1,    -1,    84,    90,    -1,    84,    91,    -1,
      84,    95,    -1,    84,    99,    -1,    -1,    85,    90,    -1,
      85,    91,    -1,    85,    95,    -1,    85,    99,    -1,    25,
      -1,    23,    -1,    24,    -1,    20,    -1,    22,    -1,    26,
      -1,    19,    -1,    88,    19,    -1,    19,    82,    -1,    88,
      19,    82,    -1,    19,    19,    -1,    42,    19,    12,    85,
      13,    -1,    89,    10,    92,    11,    12,   100,    13,    -1,
      -1,    93,    -1,    94,    89,    -1,    93,    16,    94,    89,
      -1,    -1,    27,    -1,    28,    -1,    29,    -1,    43,    19,
      12,    96,    13,    -1,    -1,    89,    -1,    91,    -1,    19,
      10,    98,    11,    -1,    -1,   107,    16,    -1,    98,   107,
      -1,    41,    89,    17,    -1,    -1,   104,    17,    -1,   100,
     104,    17,    -1,    55,    -1,    54,    -1,    53,    -1,    52,
      -1,    51,    -1,    50,    -1,    49,    -1,    48,    -1,    47,
      -1,    46,    -1,    45,    -1,    44,    -1,    30,    89,    -1,
      30,    89,    55,   106,    -1,    30,    89,    10,   106,    11,
      -1,    31,    10,    19,    11,    -1,    31,    19,    -1,   107,
      -1,    87,   101,   106,    -1,   102,    -1,   103,    -1,    40,
     106,    -1,    70,    -1,    69,    -1,    73,    -1,    72,    -1,
      71,    -1,    62,    -1,    61,    -1,    66,    -1,    64,    -1,
      65,    -1,    63,    -1,    77,    -1,    58,    -1,    59,    -1,
      60,    -1,    68,    -1,    67,    -1,    76,    -1,    69,   106,
      -1,    70,   106,    -1,    79,   106,    -1,    78,   106,    -1,
     107,    -1,    10,   106,    11,    -1,   106,   105,   106,    -1,
      87,    -1,    86,    -1,    97,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   143,   143,   144,   145,   146,   147,   151,   152,   153,
     154,   155,   159,   160,   161,   162,   167,   172,   180,   181,
     197,   204,   216,   236,   251,   260,   261,   264,   270,   277,
     278,   279,   280,   284,   293,   294,   295,   299,   307,   308,
     309,   336,   340,   341,   342,   346,   347,   348,   349,   350,
     351,   352,   353,   354,   355,   356,   357,   361,   362,   363,
     367,   368,   372,   373,   374,   375,   376,   382,   383,   384,
     385,   386,   388,   389,   390,   391,   392,   393,   394,   396,
     397,   398,   399,   400,   401,   407,   408,   409,   410,   411,
     412,   413,   422,   423,   424
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NT_ROOT", "NT_CHUNK", "NT_NAMESPACE",
  "NT_FUNCTION", "NT_ARGUMENTS", "NT_CLASS", "NT_STMT", "S_LPARENT",
  "S_RPARENT", "S_LBRACE", "S_RBRACE", "S_LBRACKET", "S_RBRACKET",
  "S_COMMA", "S_SEPARATOR", "S_NEWLINE", "V_IDENTIFIER", "V_STRING",
  "V_FLOATING", "V_NUMBER", "V_TRUE", "V_FALSE", "V_NIL", "V_INTEGER",
  "K_IN", "K_OUT", "K_INOUT", "K_NEW", "K_DELETE", "K_IF", "K_THEN",
  "K_ELSE", "K_FOR", "K_WHILE", "K_SWITCH", "K_CONTINUE", "K_BREAK",
  "K_RETURN", "K_GLOBAL", "K_NAMESPACE", "K_CLASS", "MATH_ASN_BITWISE_RSH",
  "MATH_ASN_BITWISE_LSH", "MATH_ASN_BITWISE_NOT", "MATH_ASN_BITWISE_XOR",
  "MATH_ASN_BITWISE_AND", "MATH_ASN_BITWISE_OR", "MATH_ASN_MOD",
  "MATH_ASN_DIV", "MATH_ASN_MUL", "MATH_ASN_SUB", "MATH_ASN_ADD",
  "MATH_ASN", "LOGIC_OR", "LOGIC_AND", "BITWISE_OR", "BITWISE_XOR",
  "BITWISE_AND", "LOGIC_NEQ", "LOGIC_EQ", "LOGIC_GET", "LOGIC_LET",
  "LOGIC_GT", "LOGIC_LT", "BITWISE_RSH", "BITWISE_LSH", "MATH_SUB",
  "MATH_ADD", "MATH_MOD", "MATH_DIV", "MATH_MUL", "UNARY_PLUS",
  "UNARY_MINUS", "BITWISE_NOT", "LOGIC_NOT", "MATH_PREFDEC",
  "MATH_PREFINC", "MATH_POSTDEC", "MATH_POSTINC", "S_ACCESSOR", "$accept",
  "program", "chunk", "constant", "variable", "variable_accessor",
  "var_type", "namespace_decl", "func_decl", "args_list_decl", "args_decl",
  "arg_prefix", "class_decl", "class_body", "func_call", "args_call",
  "global_decl", "stmt_list", "assign_op", "new_variable",
  "delete_variable", "stmt", "binary_op", "expr", "lvalue", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    83,    84,    84,    84,    84,    84,    85,    85,    85,
      85,    85,    86,    86,    86,    86,    86,    86,    87,    87,
      88,    88,    89,    90,    91,    92,    92,    93,    93,    94,
      94,    94,    94,    95,    96,    96,    96,    97,    98,    98,
      98,    99,   100,   100,   100,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   102,   102,   102,
     103,   103,   104,   104,   104,   104,   104,   105,   105,   105,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   105,   105,   106,   106,   106,   106,   106,
     106,   106,   107,   107,   107
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     0,     2,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     3,     2,     5,     7,     0,     1,     2,     4,     0,
       1,     1,     1,     5,     0,     1,     1,     4,     0,     2,
       2,     3,     0,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     4,     5,
       4,     2,     1,     3,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     2,     2,     1,
       3,     3,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     3,     4,
       5,     6,    22,     0,     0,     0,    25,    41,     7,    34,
      30,    31,    32,     0,    26,     0,     0,    35,    36,     0,
       0,    29,    27,    23,     8,     9,    10,    11,    33,    42,
       0,    18,    15,    16,    13,    14,    12,    17,     0,     0,
       0,    93,    92,     0,    94,     0,    64,    65,     0,    62,
      28,    38,    20,    57,     0,    61,     0,     0,     0,     0,
       0,    92,    66,    89,    56,    55,    54,    53,    52,    51,
      50,    49,    48,    47,    46,    45,     0,    19,    24,     0,
      43,     0,     0,     0,     0,     0,     0,    85,    86,    88,
      87,    79,    80,    81,    73,    72,    77,    75,    76,    74,
      83,    82,    68,    67,    71,    70,    69,    84,    78,     0,
      63,    21,    44,    37,    40,    39,     0,    58,    60,    90,
      91,    59
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,    26,    51,    71,    53,     7,     8,     9,    23,
      24,    25,    10,    29,    54,    91,    11,    55,    86,    56,
      57,    58,   119,    72,    73
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -65
static const yytype_int16 yypact[] =
{
     -65,     8,   -65,     4,    17,    19,    26,    43,   -65,   -65,
     -65,   -65,   -65,    39,    45,    46,    13,   -65,   -65,    17,
     -65,   -65,   -65,    48,    47,    17,    24,    43,   -65,    55,
      59,    -8,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   109,
      17,   -10,   -65,   -65,   -65,   -65,   -65,   -65,    17,    25,
      -9,   -65,   138,    70,   -65,    96,   -65,   -65,    73,   -65,
     -65,   122,   -65,    -3,    91,   -65,    -9,    -9,    -9,    -9,
      -9,   -65,    92,   -65,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,   -65,   -65,   -65,   -65,    -9,    31,   -65,    97,
     -65,   155,   101,    -9,    -9,   112,    15,   -29,   -29,   -29,
     -29,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,    -9,
      92,   -65,   -65,   -65,   -65,   -65,    35,    92,   -65,   -65,
      92,   -65
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -65,   -65,   -65,   -65,   -30,   -65,    14,    98,     5,   -65,
     -65,    94,   104,   -65,   -65,   -65,   111,   -65,   -65,   -65,
     -65,    83,   -65,   -64,   -27
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -30
static const yytype_int16 yytable[] =
{
      61,    66,    96,    97,    98,    99,   100,    93,     2,    52,
      41,    42,    59,    43,    44,    45,    46,    47,    13,    20,
      21,    22,   120,    12,    28,    52,   129,     3,    59,   126,
     127,    35,   -29,    27,    92,    64,     3,    33,    14,    32,
      20,    21,    22,     3,    65,    15,   131,   117,   118,     4,
       5,     6,    94,    16,    60,   130,    17,    18,    19,    30,
      67,    68,    63,    31,   124,     4,     5,     6,    38,    69,
      70,    39,    62,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,    87,
      90,   117,   118,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,    88,
      95,   117,   118,   121,   122,    41,    42,   125,    43,    44,
      45,    46,    47,   128,    34,    40,    48,    49,    41,    42,
      36,    43,    44,    45,    46,    47,    50,    37,    89,    48,
      49,    41,    42,     0,    43,    44,    45,    46,    47,    50,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   123,     0,   117,   118,
       0,     0,     0,     0,    41,    42,     0,    43,    44,    45,
      46,    47,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-65)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int8 yycheck[] =
{
      10,    10,    66,    67,    68,    69,    70,    10,     0,    39,
      19,    20,    39,    22,    23,    24,    25,    26,     4,    27,
      28,    29,    86,    19,    19,    55,    11,    19,    55,    93,
      94,    26,    19,    19,    61,    10,    19,    13,    19,    25,
      27,    28,    29,    19,    19,    19,    11,    76,    77,    41,
      42,    43,    55,    10,    40,   119,    17,    12,    12,    11,
      69,    70,    48,    16,    91,    41,    42,    43,    13,    78,
      79,    12,    82,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    19,
      17,    76,    77,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    13,
      19,    76,    77,    82,    17,    19,    20,    16,    22,    23,
      24,    25,    26,    11,    26,    31,    30,    31,    19,    20,
      26,    22,    23,    24,    25,    26,    40,    26,    55,    30,
      31,    19,    20,    -1,    22,    23,    24,    25,    26,    40,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    11,    -1,    76,    77,
      -1,    -1,    -1,    -1,    19,    20,    -1,    22,    23,    24,
      25,    26,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    84,     0,    19,    41,    42,    43,    89,    90,    91,
      95,    99,    19,    89,    19,    19,    10,    17,    12,    12,
      27,    28,    29,    92,    93,    94,    85,    89,    91,    96,
      11,    16,    89,    13,    90,    91,    95,    99,    13,    12,
      94,    19,    20,    22,    23,    24,    25,    26,    30,    31,
      40,    86,    87,    88,    97,   100,   102,   103,   104,   107,
      89,    10,    82,    89,    10,    19,    10,    69,    70,    78,
      79,    87,   106,   107,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,   101,    19,    13,   104,
      17,    98,   107,    10,    55,    19,   106,   106,   106,   106,
     106,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    76,    77,   105,
     106,    82,    17,    11,   107,    16,   106,   106,    11,    11,
     106,    11
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
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (&yylloc, scanner, m_node, m_error, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (YYID (N))                                                     \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (YYID (0))
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5))
#  define __attribute__(Spec) /* empty */
# endif
#endif

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

__attribute__((__unused__))
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
#else
static unsigned
yy_location_print_ (yyo, yylocp)
    FILE *yyo;
    YYLTYPE const * const yylocp;
#endif
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += fprintf (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += fprintf (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += fprintf (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += fprintf (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += fprintf (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc, scanner)
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
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location, scanner, m_node, m_error); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void* scanner, hz::parser::ASTNode* m_node, hz::Error* m_error)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, scanner, m_node, m_error)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    void* scanner;
    hz::parser::ASTNode* m_node;
    hz::Error* m_error;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (scanner);
  YYUSE (m_node);
  YYUSE (m_error);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
        break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void* scanner, hz::parser::ASTNode* m_node, hz::Error* m_error)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, scanner, m_node, m_error)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    void* scanner;
    hz::parser::ASTNode* m_node;
    hz::Error* m_error;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, scanner, m_node, m_error);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, void* scanner, hz::parser::ASTNode* m_node, hz::Error* m_error)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, scanner, m_node, m_error)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    void* scanner;
    hz::parser::ASTNode* m_node;
    hz::Error* m_error;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , scanner, m_node, m_error);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, scanner, m_node, m_error); \
} while (YYID (0))

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
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
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
      YYSIZE_T yyn = 0;
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, void* scanner, hz::parser::ASTNode* m_node, hz::Error* m_error)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, scanner, m_node, m_error)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    void* scanner;
    hz::parser::ASTNode* m_node;
    hz::Error* m_error;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (scanner);
  YYUSE (m_node);
  YYUSE (m_error);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}




/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void* scanner, hz::parser::ASTNode* m_node, hz::Error* m_error)
#else
int
yyparse (scanner, m_node, m_error)
    void* scanner;
    hz::parser::ASTNode* m_node;
    hz::Error* m_error;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;


#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
static YYSTYPE yyval_default;
# define YY_INITIAL_VALUE(Value) = Value
#endif
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc = yyloc_default;


    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
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
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
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
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
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

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
/* Line 1792 of yacc.c  */
#line 143 "yacc.y"
    { }
    break;

  case 3:
/* Line 1792 of yacc.c  */
#line 144 "yacc.y"
    { m_node->addChild((yyvsp[(2) - (2)].node)); }
    break;

  case 4:
/* Line 1792 of yacc.c  */
#line 145 "yacc.y"
    { m_node->addChild((yyvsp[(2) - (2)].node)); }
    break;

  case 5:
/* Line 1792 of yacc.c  */
#line 146 "yacc.y"
    { m_node->addChild((yyvsp[(2) - (2)].node)); }
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 147 "yacc.y"
    { m_node->addChild((yyvsp[(2) - (2)].node)); }
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 151 "yacc.y"
    { (yyval.node) = AST_NODE_N(NT_CHUNK); }
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 152 "yacc.y"
    { (yyvsp[(1) - (2)].node)->addChild((yyvsp[(2) - (2)].node)); (yyval.node) = (yyvsp[(1) - (2)].node); }
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 153 "yacc.y"
    { (yyvsp[(1) - (2)].node)->addChild((yyvsp[(2) - (2)].node)); (yyval.node) = (yyvsp[(1) - (2)].node); }
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 154 "yacc.y"
    { (yyvsp[(1) - (2)].node)->addChild((yyvsp[(2) - (2)].node)); (yyval.node) = (yyvsp[(1) - (2)].node); }
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 155 "yacc.y"
    { (yyvsp[(1) - (2)].node)->addChild((yyvsp[(2) - (2)].node)); (yyval.node) = (yyvsp[(1) - (2)].node); }
    break;

  case 12:
/* Line 1792 of yacc.c  */
#line 159 "yacc.y"
    { (yyval.node) = AST_NODE_N(V_NIL); }
    break;

  case 13:
/* Line 1792 of yacc.c  */
#line 160 "yacc.y"
    { (yyval.node) = AST_NODE_N(V_TRUE); }
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 161 "yacc.y"
    { (yyval.node) = AST_NODE_N(V_FALSE); }
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 163 "yacc.y"
    {
			(yyval.node) = AST_NODE_N(V_STRING);
			EXTRACT_STR((yyvsp[(1) - (1)].string), (yyval.node)->value);
		}
    break;

  case 16:
/* Line 1792 of yacc.c  */
#line 168 "yacc.y"
    {
			(yyval.node) = AST_NODE_N(V_NUMBER);
			(yyval.node)->value = (yyvsp[(1) - (1)].floating);
		}
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 173 "yacc.y"
    {
			(yyval.node) = AST_NODE_N(V_INTEGER);
			(yyval.node)->value = (yyvsp[(1) - (1)].integer);
		}
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 180 "yacc.y"
    { (yyval.node) = AST_NODE(V_IDENTIFIER); EXTRACT_STR((yyvsp[(1) - (1)].string), (yyval.node)->name); }
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 182 "yacc.y"
    { 
			hz::parser::ASTNode* nIdent = AST_NODE(V_IDENTIFIER); 
			EXTRACT_STR((yyvsp[(2) - (2)].string), nIdent->name); 
			(yyvsp[(1) - (2)].node)->addChild(nIdent);
			// Variable is the highest "accessor" level
			hz::parser::ASTNode* accessor = (yyvsp[(1) - (2)].node);
			while(accessor->parent && accessor->parent->type == S_ACCESSOR)
			{
				accessor = accessor->parent;
			}
			(yyval.node) = accessor;
		}
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 198 "yacc.y"
    {
			(yyval.node) = AST_NODE_N(S_ACCESSOR);
			hz::parser::ASTNode* nIdent = AST_NODE(V_IDENTIFIER); 
			EXTRACT_STR((yyvsp[(1) - (2)].string), nIdent->name); 
			(yyval.node)->addChild(nIdent);
		}
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 205 "yacc.y"
    {
			hz::parser::ASTNode* nAccess = AST_NODE_N(S_ACCESSOR); 
			hz::parser::ASTNode* nIdent = AST_NODE(V_IDENTIFIER); 
			EXTRACT_STR((yyvsp[(2) - (3)].string), nIdent->name); 
			(yyvsp[(1) - (3)].node)->addChild(nAccess);
			nAccess->addChild(nIdent);
			(yyval.node) = nAccess;
		}
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 217 "yacc.y"
    {
			m::String var = (*(yyvsp[(2) - (2)].string));
			MUON_DELETE((yyvsp[(2) - (2)].string));
			// Node name is the variable name
			// Token value is the return type
			hz::parser::ASTNode* nodeType = AST_NODE(V_IDENTIFIER, var);
			EXTRACT_STR((yyvsp[(1) - (2)].string), nodeType->value);

			(yyval.node) = nodeType;
		}
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 237 "yacc.y"
    {
			m::String name = (*(yyvsp[(2) - (5)].string));
			MUON_DELETE((yyvsp[(2) - (5)].string));
			(yyval.node) = AST_NODE(NT_NAMESPACE, name);
			// Extract all child of chunk (as there is a NT_CHUNK token)
			for(m::u32 i = 0; i < (yyvsp[(4) - (5)].node)->children->size(); ++i)
			{
				(yyval.node)->addChild((yyvsp[(4) - (5)].node)->children->at(i));
			}
			MUON_DELETE((yyvsp[(4) - (5)].node));
		}
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 252 "yacc.y"
    {
			(yyval.node) = AST_NODE_N(NT_FUNCTION);
			(yyval.node)->addChild((yyvsp[(1) - (7)].node));
			(yyval.node)->addChild((yyvsp[(3) - (7)].node));
			(yyval.node)->addChild((yyvsp[(6) - (7)].node));
		}
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 260 "yacc.y"
    { (yyval.node) = AST_NODE_N(NT_ARGUMENTS); }
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 261 "yacc.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); /* Node have been created by args_decl */}
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 265 "yacc.y"
    {
			generated::ParamPrefix prefix = (generated::ParamPrefix)(yyvsp[(1) - (2)].integer);
			(yyval.node) = AST_NODE_N(NT_ARGUMENTS);
			(yyval.node)->addChild((yyvsp[(2) - (2)].node));
		}
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 271 "yacc.y"
    {
			generated::ParamPrefix prefix = (generated::ParamPrefix)(yyvsp[(3) - (4)].integer);
			(yyvsp[(1) - (4)].node)->addChild((yyvsp[(4) - (4)].node));
		}
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 277 "yacc.y"
    { (yyval.integer) = generated::ParamPrefix::IN; }
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 278 "yacc.y"
    { (yyval.integer) = generated::ParamPrefix::IN; }
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 279 "yacc.y"
    { (yyval.integer) = generated::ParamPrefix::OUT; }
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 280 "yacc.y"
    { (yyval.integer) = (generated::ParamPrefix::IN | generated::ParamPrefix::OUT); }
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 285 "yacc.y"
    {
			m::String name = (*(yyvsp[(2) - (5)].string));
			MUON_DELETE((yyvsp[(2) - (5)].string));
			(yyval.node) = AST_NODE(NT_CLASS, name);
		}
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 293 "yacc.y"
    { printf("class::E\n");}
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 294 "yacc.y"
    { printf("class::var_type\n"); }
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 295 "yacc.y"
    { printf("class::func_decl\n"); (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 300 "yacc.y"
    {
			(yyval.node) = AST_NODE_N(NT_FUNCTION);
			EXTRACT_STR((yyvsp[(1) - (4)].string), (yyval.node)->name);
			(yyval.node)->addChild((yyvsp[(3) - (4)].node));
		}
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 307 "yacc.y"
    { (yyval.node) = AST_NODE_N(NT_ARGUMENTS); }
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 308 "yacc.y"
    { (yyval.node) = AST_NODE_N(NT_ARGUMENTS); (yyval.node)->addChild((yyvsp[(1) - (2)].node)); }
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 309 "yacc.y"
    { (yyval.node) = (yyvsp[(1) - (2)].node); (yyval.node)->addChild((yyvsp[(2) - (2)].node)); }
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 336 "yacc.y"
    { (yyval.node) = AST_NODE_N(K_GLOBAL); (yyval.node)->addChild((yyvsp[(2) - (3)].node)); }
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 340 "yacc.y"
    { (yyval.node) = AST_NODE_N(NT_STMT); }
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 341 "yacc.y"
    { (yyval.node) = AST_NODE_N(NT_STMT); (yyval.node)->addChild((yyvsp[(1) - (2)].node)); }
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 342 "yacc.y"
    { (yyval.node) = (yyvsp[(1) - (3)].node); (yyval.node)->addChild((yyvsp[(2) - (3)].node)); }
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 346 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_ASN); }
    break;

  case 46:
/* Line 1792 of yacc.c  */
#line 347 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_ASN_ADD); }
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 348 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_ASN_SUB); }
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 349 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_ASN_MUL); }
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 350 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_ASN_DIV); }
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 351 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_ASN_MOD); }
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 352 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_ASN_BITWISE_OR); }
    break;

  case 52:
/* Line 1792 of yacc.c  */
#line 353 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_ASN_BITWISE_AND); }
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 354 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_ASN_BITWISE_XOR); }
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 355 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_ASN_BITWISE_NOT); }
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 356 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_ASN_BITWISE_LSH); }
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 357 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_ASN_BITWISE_RSH); }
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 361 "yacc.y"
    { (yyval.node) = AST_NODE_N(K_NEW); (yyval.node)->addChild((yyvsp[(2) - (2)].node)); }
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 362 "yacc.y"
    { (yyval.node) = AST_NODE_N(K_NEW); (yyval.node)->addChild((yyvsp[(2) - (4)].node)); (yyval.node)->addChild((yyvsp[(4) - (4)].node)); }
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 363 "yacc.y"
    { (yyval.node) = AST_NODE_N(K_NEW); (yyval.node)->addChild((yyvsp[(2) - (5)].node)); (yyval.node)->addChild((yyvsp[(4) - (5)].node)); }
    break;

  case 60:
/* Line 1792 of yacc.c  */
#line 367 "yacc.y"
    { (yyval.node) = AST_NODE_N(K_DELETE); EXTRACT_STR((yyvsp[(3) - (4)].string), (yyval.node)->value); }
    break;

  case 61:
/* Line 1792 of yacc.c  */
#line 368 "yacc.y"
    { (yyval.node) = AST_NODE_N(K_DELETE); EXTRACT_STR((yyvsp[(2) - (2)].string), (yyval.node)->value); }
    break;

  case 62:
/* Line 1792 of yacc.c  */
#line 372 "yacc.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 373 "yacc.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); (yyval.node)->addChild((yyvsp[(1) - (3)].node)); (yyval.node)->addChild((yyvsp[(3) - (3)].node)); }
    break;

  case 64:
/* Line 1792 of yacc.c  */
#line 374 "yacc.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 375 "yacc.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 66:
/* Line 1792 of yacc.c  */
#line 376 "yacc.y"
    { (yyval.node) = AST_NODE_N(K_RETURN); (yyval.node)->addChild((yyvsp[(2) - (2)].node)); }
    break;

  case 67:
/* Line 1792 of yacc.c  */
#line 382 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_ADD); }
    break;

  case 68:
/* Line 1792 of yacc.c  */
#line 383 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_SUB); }
    break;

  case 69:
/* Line 1792 of yacc.c  */
#line 384 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_MUL); }
    break;

  case 70:
/* Line 1792 of yacc.c  */
#line 385 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_DIV); }
    break;

  case 71:
/* Line 1792 of yacc.c  */
#line 386 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_MOD); }
    break;

  case 72:
/* Line 1792 of yacc.c  */
#line 388 "yacc.y"
    { (yyval.node) = AST_NODE_N(LOGIC_EQ); }
    break;

  case 73:
/* Line 1792 of yacc.c  */
#line 389 "yacc.y"
    { (yyval.node) = AST_NODE_N(LOGIC_NEQ); }
    break;

  case 74:
/* Line 1792 of yacc.c  */
#line 390 "yacc.y"
    { (yyval.node) = AST_NODE_N(LOGIC_LT); }
    break;

  case 75:
/* Line 1792 of yacc.c  */
#line 391 "yacc.y"
    { (yyval.node) = AST_NODE_N(LOGIC_LET); }
    break;

  case 76:
/* Line 1792 of yacc.c  */
#line 392 "yacc.y"
    { (yyval.node) = AST_NODE_N(LOGIC_GT); }
    break;

  case 77:
/* Line 1792 of yacc.c  */
#line 393 "yacc.y"
    { (yyval.node) = AST_NODE_N(LOGIC_GET); }
    break;

  case 78:
/* Line 1792 of yacc.c  */
#line 394 "yacc.y"
    { (yyval.node) = AST_NODE_N(LOGIC_NOT); }
    break;

  case 79:
/* Line 1792 of yacc.c  */
#line 396 "yacc.y"
    { (yyval.node) = AST_NODE_N(BITWISE_OR); }
    break;

  case 80:
/* Line 1792 of yacc.c  */
#line 397 "yacc.y"
    { (yyval.node) = AST_NODE_N(BITWISE_XOR); }
    break;

  case 81:
/* Line 1792 of yacc.c  */
#line 398 "yacc.y"
    { (yyval.node) = AST_NODE_N(BITWISE_AND); }
    break;

  case 82:
/* Line 1792 of yacc.c  */
#line 399 "yacc.y"
    { (yyval.node) = AST_NODE_N(BITWISE_LSH); }
    break;

  case 83:
/* Line 1792 of yacc.c  */
#line 400 "yacc.y"
    { (yyval.node) = AST_NODE_N(BITWISE_RSH); }
    break;

  case 84:
/* Line 1792 of yacc.c  */
#line 401 "yacc.y"
    { (yyval.node) = AST_NODE_N(BITWISE_NOT); }
    break;

  case 85:
/* Line 1792 of yacc.c  */
#line 407 "yacc.y"
    { (yyval.node) = AST_NODE_N(UNARY_MINUS); (yyval.node)->addChild((yyvsp[(2) - (2)].node)); }
    break;

  case 86:
/* Line 1792 of yacc.c  */
#line 408 "yacc.y"
    { (yyval.node) = (yyvsp[(2) - (2)].node); }
    break;

  case 87:
/* Line 1792 of yacc.c  */
#line 409 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_PREFINC); (yyval.node)->addChild((yyvsp[(2) - (2)].node)); }
    break;

  case 88:
/* Line 1792 of yacc.c  */
#line 410 "yacc.y"
    { (yyval.node) = AST_NODE_N(MATH_PREFDEC); (yyval.node)->addChild((yyvsp[(2) - (2)].node)); }
    break;

  case 89:
/* Line 1792 of yacc.c  */
#line 411 "yacc.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 90:
/* Line 1792 of yacc.c  */
#line 412 "yacc.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); }
    break;

  case 91:
/* Line 1792 of yacc.c  */
#line 414 "yacc.y"
    {
			(yyval.node) = (yyvsp[(2) - (3)].node);
			(yyval.node)->addChild((yyvsp[(1) - (3)].node));
			(yyval.node)->addChild((yyvsp[(3) - (3)].node));
		}
    break;

  case 92:
/* Line 1792 of yacc.c  */
#line 422 "yacc.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 93:
/* Line 1792 of yacc.c  */
#line 423 "yacc.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 94:
/* Line 1792 of yacc.c  */
#line 424 "yacc.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;


/* Line 1792 of yacc.c  */
#line 2415 "yacc.cpp"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

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
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, scanner, m_node, m_error, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, scanner, m_node, m_error, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc, scanner, m_node, m_error);
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
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
      if (!yypact_value_is_default (yyn))
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

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, scanner, m_node, m_error);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, scanner, m_node, m_error, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, scanner, m_node, m_error);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, scanner, m_node, m_error);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


