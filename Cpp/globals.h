/************************************************************************/
/* File: globals.h                                                      */
/* Globals type and vars for C- compiler                                */
/* must come before other include files                                 */
/************************************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 6

typedef enum
   /* book-keeping tokens */
{    ENDFILE,ERROR,
	/* reserved words */
	IF,ELSE,INT,RETURN,VOID,WHILE,
	/* multi character tokens */
	ID,NUM,
	/* special symbols */
	/* + - * / < <= > >= == != = ; , ( ) [ ] { } */
	PLUS,MINUS,TIMES,OVER,LT,LTEQ,RT,RTEQ,EQ,NEQ,ASSIGN,SEMT,COMMA,
	LPAREN,RPAREN,LSQUARE,RSQUARE,LBRACE,RBRACE,LNOTE,RNOTE
}TokenType;

extern FILE* source;  /* source code text file */
extern FILE* listing; /* listing output text file */
extern FILE* code;    /* code text file for C- simulator */

extern int lineno; /* source line number for listing */

/************************************************************************/
/***************      Syntax tree for parsing               *************/
/************************************************************************/


typedef enum{StmtK,ExpK,DeclK} NodeKind;     /* C-的三種基本的結構類型: 語句、表達式和聲明 */
typedef enum{IfK,WhileK,Returnk,Expressionk,Compoundk} StmtKind;  /* 語句類型: 選擇語句、循環語句、返回語句、表達式語句、複合語句 */
typedef enum{Operatek,Constk,Identifyk,ParamK,IndexK,ActualPK} Expkind;  /* 表達式類型: 算數表達式、常量表達式、標識符表達式,實際上就是指單獨表達式類型 */
typedef enum{Functionk,Variablek} Declkind;   /* 聲明類型: 函數聲明、變量聲明 */
/* ExpType is used for type checking */
typedef enum{Void,Integer,Boolean} ExpType;

#define MAXCHILDREN 3     /* 最大孩子數 */
#define MAXTOKENTEMP 20   /* 前看時暫存的token最大數,即是數組的長度 */
/* C-語法樹結點聲明 */
typedef struct treeNode
{
	struct treeNode * child[MAXCHILDREN];  /* 一棵語法樹的孩子結點 */
	struct treeNode * sibling;     /* 一棵語法樹的兄弟結點 */
	int lineno;     /* 用以記錄、輸出源代碼行數 */
	NodeKind nodekind;   /* 此行的結構類型屬性 */
	union {StmtKind stmt; Expkind exp; Declkind decl;} kind; /* 三種結構類型的聚合 */
	union {
		TokenType Operater;      /* 涉及到Tonken類型時的屬性 */
		int val;           /* 變量值屬性 */
		char * name;       /* 標識符存儲屬性 */
	} attribute;   /* 結點屬性 */
	ExpType type;  /* for type checking of exp */
} TreeNode;




/************************************************************************/
/***************          Flags for tracing                 *************/
/************************************************************************/
/* EchoSource = TRUE causes the source program to
 * be echoed to the listing file with line numbers 
 * during parsing
 */

extern int EchoSource;

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */

extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */

extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */

extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
 * to the C- code file as code is generated
 */

extern int TraceCode;

/* Error = TRUE prevents further passes if an error occurs */

extern int Error;

#endif