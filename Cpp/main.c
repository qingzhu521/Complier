#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE

/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE

/* set NO_CODE to TRUE to get a compiler that does not
 *  generate code
 */
#define NO_CODE FALSE

#if NO_PARSE
#include "scan.h"
#endif

/* allocate global variables,��ʼ��ȫ��׃�� */
int lineno = 0;
FILE* source;
FILE* listing;
FILE* code;

/* allocate and set tracing flags */
int EchoSource = TRUE;
int TraceScan = TRUE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = TRUE;

int Error = FALSE;

int main(int argc, char* argv[])
{
	TreeNode * syntaxTree;   /* Դ���a�Z����ָ� */
	char pgm[120];   /* source code file name */
	if (argc != 2)   /* main�����ĵ�һ���΅�argc��ʾmain�������Ѕ�������,���а���main�����Լ� */
	{
		fprintf(stderr, "usage: %s <filename>\n",argv[0]);  /* �xȡ����ʧ��,�˳����� */
		exit(1);          /* ֱ���˳����� */
	}
//	strcpy(pgm,"H:\\compiler\\Debug\\sort.txt");   /* ��main�����ڶ���������������ļ����}�u�opgm */
	strcpy(pgm,argv[1]);   /* ��main�����ڶ���������������ļ����}�u�opgm */
	if (strchr(pgm, '.') == NULL)   /* ���ļ������ҵ��ַ�.�״γ��F��λ��,�Ҳ����t����NULL */
	{
		strcat(pgm,".txt");     /* ���ļ������txt��Y */
	}
	if ((source = fopen(pgm,"r")) == NULL)    /* �xȡC-�����ļ�ʧ�� */
	{
		fprintf(stderr,"File %s not found\n",pgm);
		exit(1);
	}
	listing = stdout;      /* send listing to screen,ϵ�y�ṩ���ļ�����ݔ����ʽ */
    fprintf(listing, "\nC- COMPILATION: %s\n",pgm);
//#if NO_PARSE
//    while(getToken() != ENDFILE);
	syntaxTree = parse();             /* �{��parse()���� */
	if (syntaxTree)
	{/* ���Դ���a�Z����ָᘲ���� */
		fprintf(listing,"\nSyntax tree: \n");        
		printTree(syntaxTree);        /* ����Ļ��ݔ�������Z���� */
	}
//#endif
	fclose(source);       /* �P�]�ļ� */
	return 0;
}