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


typedef enum{StmtK,ExpK,DeclK} NodeKind;     /* C-�����N�����ĽY�����: �Z�䡢���_ʽ���� */
typedef enum{IfK,WhileK,Returnk,Expressionk,Compoundk} StmtKind;  /* �Z�����: �x���Z�䡢ѭ�h�Z�䡢�����Z�䡢���_ʽ�Z�䡢�}���Z�� */
typedef enum{Operatek,Constk,Identifyk,ParamK,IndexK,ActualPK} Expkind;  /* ���_ʽ���: �㔵���_ʽ���������_ʽ�����R�����_ʽ,���H�Ͼ���ָ�Ϊ����_ʽ��� */
typedef enum{Functionk,Variablek} Declkind;   /* �����: ��������׃���� */
/* ExpType is used for type checking */
typedef enum{Void,Integer,Boolean} ExpType;

#define MAXCHILDREN 3     /* ����Ӕ� */
#define MAXTOKENTEMP 20   /* ǰ���r�����token���,���ǔ��M���L�� */
/* C-�Z����Y�c�� */
typedef struct treeNode
{
	struct treeNode * child[MAXCHILDREN];  /* һ���Z����ĺ��ӽY�c */
	struct treeNode * sibling;     /* һ���Z������ֵܽY�c */
	int lineno;     /* ����ӛ䛡�ݔ��Դ���a�Д� */
	NodeKind nodekind;   /* ���еĽY����͌��� */
	union {StmtKind stmt; Expkind exp; Declkind decl;} kind; /* ���N�Y����͵ľۺ� */
	union {
		TokenType Operater;      /* �漰��Tonken��͕r�Č��� */
		int val;           /* ׃��ֵ���� */
		char * name;       /* ���R���惦���� */
	} attribute;   /* �Y�c���� */
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