#ifndef YY_CppParserCoreBase_h_included
#define YY_CppParserCoreBase_h_included
/* before anything */
#ifdef c_plusplus
#ifndef __cplusplus
#define __cplusplus
#endif
#endif
#ifdef __cplusplus
#ifndef YY_USE_CLASS
#define YY_USE_CLASS
#endif
#else
#endif
#include <stdio.h>
#define YY_CppParserCoreBase_LSP_NEEDED 
#define YY_CppParserCoreBase_ERROR_BODY  = 0
#define YY_CppParserCoreBase_LEX_BODY  = 0

#include <iostream>
#include <fstream>

typedef union {
	int i_type;
	char c_type;
} yy_CppParserCoreBase_stype;
#define YY_CppParserCoreBase_STYPE yy_CppParserCoreBase_stype
 /* %{ and %header{ and %union, during decl */
#ifndef YY_CppParserCoreBase_COMPATIBILITY
#ifndef YY_USE_CLASS
#define  YY_CppParserCoreBase_COMPATIBILITY 1
#else
#define  YY_CppParserCoreBase_COMPATIBILITY 0
#endif
#endif

#if YY_CppParserCoreBase_COMPATIBILITY != 0
/* backward compatibility */
#ifdef YYLTYPE
#ifndef YY_CppParserCoreBase_LTYPE
#define YY_CppParserCoreBase_LTYPE YYLTYPE
/* WARNING obsolete !!! user defined YYLTYPE not reported into generated header */
/* use %define LTYPE */
#endif
#endif
#ifdef YYSTYPE
#ifndef YY_CppParserCoreBase_STYPE 
#define YY_CppParserCoreBase_STYPE YYSTYPE
/* WARNING obsolete !!! user defined YYSTYPE not reported into generated header */
/* use %define STYPE */
#endif
#endif
#ifdef YYDEBUG
#ifndef YY_CppParserCoreBase_DEBUG
#define  YY_CppParserCoreBase_DEBUG YYDEBUG
/* WARNING obsolete !!! user defined YYDEBUG not reported into generated header */
/* use %define DEBUG */
#endif
#endif
#ifdef YY_CppParserCoreBase_STYPE
#ifndef yystype
#define yystype YY_CppParserCoreBase_STYPE
#endif
#endif
/* use goto to be compatible */
#ifndef YY_CppParserCoreBase_USE_GOTO
#define YY_CppParserCoreBase_USE_GOTO 1
#endif
#endif

/* use no goto to be clean in C++ */
#ifndef YY_CppParserCoreBase_USE_GOTO
#define YY_CppParserCoreBase_USE_GOTO 0
#endif

#ifndef YY_CppParserCoreBase_PURE
/* YY_CppParserCoreBase_PURE */
#endif
/* prefix */
#ifndef YY_CppParserCoreBase_DEBUG
/* YY_CppParserCoreBase_DEBUG */
#endif
#ifndef YY_CppParserCoreBase_LSP_NEEDED
 /* YY_CppParserCoreBase_LSP_NEEDED*/
#endif
/* DEFAULT LTYPE*/
#ifdef YY_CppParserCoreBase_LSP_NEEDED
#ifndef YY_CppParserCoreBase_LTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YY_CppParserCoreBase_LTYPE yyltype
#endif
#endif
/* DEFAULT STYPE*/
#ifndef YY_CppParserCoreBase_STYPE
#define YY_CppParserCoreBase_STYPE int
#endif
/* DEFAULT MISCELANEOUS */
#ifndef YY_CppParserCoreBase_PARSE
#define YY_CppParserCoreBase_PARSE yyparse
#endif
#ifndef YY_CppParserCoreBase_LEX
#define YY_CppParserCoreBase_LEX yylex
#endif
#ifndef YY_CppParserCoreBase_LVAL
#define YY_CppParserCoreBase_LVAL yylval
#endif
#ifndef YY_CppParserCoreBase_LLOC
#define YY_CppParserCoreBase_LLOC yylloc
#endif
#ifndef YY_CppParserCoreBase_CHAR
#define YY_CppParserCoreBase_CHAR yychar
#endif
#ifndef YY_CppParserCoreBase_NERRS
#define YY_CppParserCoreBase_NERRS yynerrs
#endif
#ifndef YY_CppParserCoreBase_DEBUG_FLAG
#define YY_CppParserCoreBase_DEBUG_FLAG yydebug
#endif
#ifndef YY_CppParserCoreBase_ERROR
#define YY_CppParserCoreBase_ERROR yyerror
#endif

#ifndef YY_CppParserCoreBase_PARSE_PARAM
#ifndef __STDC__
#ifndef __cplusplus
#ifndef YY_USE_CLASS
#define YY_CppParserCoreBase_PARSE_PARAM
#ifndef YY_CppParserCoreBase_PARSE_PARAM_DEF
#define YY_CppParserCoreBase_PARSE_PARAM_DEF
#endif
#endif
#endif
#endif
#ifndef YY_CppParserCoreBase_PARSE_PARAM
#define YY_CppParserCoreBase_PARSE_PARAM void
#endif
#endif

/* TOKEN C */
#ifndef YY_USE_CLASS

#ifndef YY_CppParserCoreBase_PURE
extern YY_CppParserCoreBase_STYPE YY_CppParserCoreBase_LVAL;
#endif

#define	TT_CppOP_Plus	258
#define	TT_CppOP_Minul	259
#define	TT_CppOP_Asterisk	260
#define	TT_CppOP_Slash	261
#define	UNKNOWN	262
#define	PLUS	263
#define	MINUS	264
#define	EQUALS	265
#define	NUMBER	266

 /* #defines token */
/* after #define tokens, before const tokens S5*/
#else
#ifndef YY_CppParserCoreBase_CLASS
#define YY_CppParserCoreBase_CLASS CppParserCoreBase
#endif

#ifndef YY_CppParserCoreBase_INHERIT
#define YY_CppParserCoreBase_INHERIT
#endif
#ifndef YY_CppParserCoreBase_MEMBERS
#define YY_CppParserCoreBase_MEMBERS 
#endif
#ifndef YY_CppParserCoreBase_LEX_BODY
#define YY_CppParserCoreBase_LEX_BODY  
#endif
#ifndef YY_CppParserCoreBase_ERROR_BODY
#define YY_CppParserCoreBase_ERROR_BODY  
#endif
#ifndef YY_CppParserCoreBase_CONSTRUCTOR_PARAM
#define YY_CppParserCoreBase_CONSTRUCTOR_PARAM
#endif
/* choose between enum and const */
#ifndef YY_CppParserCoreBase_USE_CONST_TOKEN
#define YY_CppParserCoreBase_USE_CONST_TOKEN 0
/* yes enum is more compatible with flex,  */
/* so by default we use it */ 
#endif
#if YY_CppParserCoreBase_USE_CONST_TOKEN != 0
#ifndef YY_CppParserCoreBase_ENUM_TOKEN
#define YY_CppParserCoreBase_ENUM_TOKEN yy_CppParserCoreBase_enum_token
#endif
#endif

class YY_CppParserCoreBase_CLASS YY_CppParserCoreBase_INHERIT
{
public: 
#if YY_CppParserCoreBase_USE_CONST_TOKEN != 0
/* static const int token ... */
static const int TT_CppOP_Plus;
static const int TT_CppOP_Minul;
static const int TT_CppOP_Asterisk;
static const int TT_CppOP_Slash;
static const int UNKNOWN;
static const int PLUS;
static const int MINUS;
static const int EQUALS;
static const int NUMBER;

 /* decl const */
#else
enum YY_CppParserCoreBase_ENUM_TOKEN { YY_CppParserCoreBase_NULL_TOKEN=0
	,TT_CppOP_Plus=258
	,TT_CppOP_Minul=259
	,TT_CppOP_Asterisk=260
	,TT_CppOP_Slash=261
	,UNKNOWN=262
	,PLUS=263
	,MINUS=264
	,EQUALS=265
	,NUMBER=266

 /* enum token */
     }; /* end of enum declaration */
#endif
public:
 int YY_CppParserCoreBase_PARSE(YY_CppParserCoreBase_PARSE_PARAM);
 virtual void YY_CppParserCoreBase_ERROR(char *msg) YY_CppParserCoreBase_ERROR_BODY;
#ifdef YY_CppParserCoreBase_PURE
#ifdef YY_CppParserCoreBase_LSP_NEEDED
 virtual int  YY_CppParserCoreBase_LEX(YY_CppParserCoreBase_STYPE *YY_CppParserCoreBase_LVAL,YY_CppParserCoreBase_LTYPE *YY_CppParserCoreBase_LLOC) YY_CppParserCoreBase_LEX_BODY;
#else
 virtual int  YY_CppParserCoreBase_LEX(YY_CppParserCoreBase_STYPE *YY_CppParserCoreBase_LVAL) YY_CppParserCoreBase_LEX_BODY;
#endif
#else
 virtual int YY_CppParserCoreBase_LEX() YY_CppParserCoreBase_LEX_BODY;
 YY_CppParserCoreBase_STYPE YY_CppParserCoreBase_LVAL;
#ifdef YY_CppParserCoreBase_LSP_NEEDED
 YY_CppParserCoreBase_LTYPE YY_CppParserCoreBase_LLOC;
#endif
 int YY_CppParserCoreBase_NERRS;
 int YY_CppParserCoreBase_CHAR;
#endif
#if YY_CppParserCoreBase_DEBUG != 0
public:
 int YY_CppParserCoreBase_DEBUG_FLAG;	/*  nonzero means print parse trace	*/
#endif
public:
 YY_CppParserCoreBase_CLASS(YY_CppParserCoreBase_CONSTRUCTOR_PARAM);
public:
 YY_CppParserCoreBase_MEMBERS 
};
/* other declare folow */
#endif


#if YY_CppParserCoreBase_COMPATIBILITY != 0
/* backward compatibility */
#ifndef YYSTYPE
#define YYSTYPE YY_CppParserCoreBase_STYPE
#endif

#ifndef YYLTYPE
#define YYLTYPE YY_CppParserCoreBase_LTYPE
#endif
#ifndef YYDEBUG
#ifdef YY_CppParserCoreBase_DEBUG 
#define YYDEBUG YY_CppParserCoreBase_DEBUG
#endif
#endif

#endif
/* END */
#endif
