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

/* allocate global variables,初始化全局變量 */
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
	TreeNode * syntaxTree;   /* 源代碼語法樹指針 */
	char pgm[120];   /* source code file name */
	if (argc != 2)   /* main函數的第一個形參argc表示main函數當中參數個數,其中包括main函數自己 */
	{
		fprintf(stderr, "usage: %s <filename>\n",argv[0]);  /* 讀取參數失敗,退出程序 */
		exit(1);          /* 直接退出程序 */
	}
//	strcpy(pgm,"H:\\compiler\\Debug\\sort.txt");   /* 將main函數第二個參數即傳入的文件名複製給pgm */
	strcpy(pgm,argv[1]);   /* 將main函數第二個參數即傳入的文件名複製給pgm */
	if (strchr(pgm, '.') == NULL)   /* 在文件名中找到字符.首次出現的位置,找不到則返回NULL */
	{
		strcat(pgm,".txt");     /* 為文件名添加txt後綴 */
	}
	if ((source = fopen(pgm,"r")) == NULL)    /* 讀取C-程序文件失敗 */
	{
		fprintf(stderr,"File %s not found\n",pgm);
		exit(1);
	}
	listing = stdout;      /* send listing to screen,系統提供的文件內容輸出格式 */
    fprintf(listing, "\nC- COMPILATION: %s\n",pgm);
//#if NO_PARSE
//    while(getToken() != ENDFILE);
	syntaxTree = parse();             /* 調用parse()函數 */
	if (syntaxTree)
	{/* 如果源代碼語法樹指針不為空 */
		fprintf(listing,"\nSyntax tree: \n");        
		printTree(syntaxTree);        /* 在屏幕上輸出分析語法樹 */
	}
//#endif
	fclose(source);       /* 關閉文件 */
	return 0;
}