#include "globals.h"
#include "util.h"
#include "scan.h"

/* states in scanner DFA */
typedef enum
{
	START,INNUM,INID,INLTEQ,INRTEQ,INEQ,INNEQ,COMMENTSTART,INCOMMENT,COMMENTEND,DONE
}StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];    /* 此數組下標範圍: 0--MAXTOKENLEN */

/* BUFLEN = length of the input buffer for source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */
static int EOF_Flag = FALSE;

/* getNextChar fetches the next non-blank character
 * from lineBuf, reading in a new line of lineBuf is exhausted
 */
static int getNextChar(void)
{
	if (!(linepos < bufsize)) /* 當前行下標大於此行長度,即當前行已經的字符已經讀取完,換下一行繼續讀取 */
	{
		lineno++;  /* 行數自加1,讀取下一行內容 */
		if (fgets(lineBuf,BUFLEN - 1,source))  /* 源文件還未讀完,從源文件中繼續讀取內容 */
		{
			if (EchoSource)   /* 可以輸出的一行 */
			{
				fprintf(listing, "%4d: %s",lineno,lineBuf);  /* 在屏幕上輸出這一行,即是每讀取一行便輸出此行后對其分析 */
			}
			bufsize = strlen(lineBuf);  /* 獲取這一行的實際長度 */
			linepos = 0;  /* 數組下標從0開始 */
			return lineBuf[linepos++];  /* 返回當前字符即讀取它,下標加1,接著讀取本行的下一個字符 */
		}
		else
		{
			EOF_Flag = TRUE;    /* 源文件讀取完畢 */
			return EOF;    /* 返回文件結束標誌 */
		}
	}
	else
	{
		return lineBuf[linepos++]; /* 返回當前字符即讀取它,下標加1,接著讀取本行的下一個字符*/
	}
}

/* ungetNextChar backtracks one character in lineBuf */
static void ungetNextChar(void)
{
	if (!EOF_Flag)   /* 只要文件還未讀完 */
	{
		linepos--;   /* 下標自減1意味著回退一步 */
	}
}

/* lookup table of reserved words */
static struct
{
	char* str;
	TokenType tok;
}reservedWords[MAXRESERVED]
= {{"if",IF},{"else",ELSE},{"int",INT},{"return",RETURN},{"void",VOID},{"while",WHILE}};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup(char * s)
{
	int i;  /* 下標變量 */
	for (i = 0; i < MAXRESERVED; i++)
	{
		if (!strcmp(s,reservedWords[i].str))  /* strcmp為字符串比較函數,如果兩個字符串一樣則返回0,否則返回1 */
		{
			return reservedWords[i].tok;   /* 是保留字,則返回 */
		}
	}
	return ID;    /* 不是保留字而是ID */
}

/************************************************************************/
/*             the primary function of the scanner                      */
/************************************************************************/
/* function getToken returns the
 * next token in source file
 */
TokenType getToken(void)
{   /* index for storing into tokenString */
	int tokenStringIndex = 0;
	/* holds current token to be returned */
	TokenType currentToken;
	/* current state - always begins at SRART */
	StateType state = START;
	/* flag to indicate save to tokenString */
	int save;
	while (state != DONE)
	{
		int ch = getNextChar();
		save = TRUE;
		switch(state)
		{
		case START:
			if(isdigit(ch))
				state = INNUM;   /* 首字符為數字則進入數字判斷狀態 */
			else if(isalpha(ch))
				state = INID;    /* 首字符為字符則進入ID判斷狀態 */
			else if(ch == ' ' || ch == '\n' ||ch == '\t')
				save = FALSE;    /* 首字符為空格、換行符、製錶符則進入ID判斷狀態,且不予保存 */
			else if(ch == '<')
				state = INLTEQ;  /* 首字符為'<'則進入'<='判斷狀態 */
			else if(ch == '>')
				state = INRTEQ;  /* 首字符為'>'則進入'>='判斷狀態 */
			else if(ch == '=')
				state = INEQ; /* 首字符為'='則進入'=='判斷狀態 */
			else if(ch == '!')
				state = INNEQ;   /* 首字符為'!'則進入'!='判斷狀態 */
			else if(ch == '/')
			{                    /* 首字符為'/'則進入注釋判斷狀態 */
				state = COMMENTSTART;      /* 進入註釋判斷狀態 */
				if (getNextChar() == '*')
				{
					save = FALSE;            /* 說明進入了註釋語句,故而不予保存 */ 
					state = INCOMMENT;    /* 進入註釋狀態 */
				}
				else
				{
					currentToken = OVER;    /* 說明前一個讀取的字符是專用字符/ */
					state = DONE;           /* 進入結束狀態 */
					ungetNextChar();        /* 證明這個'/'只是專用字符OVER,所以要把之前前看的位於'/'後面的字符回退回去 */
				}
			}
			else
			{
				state = DONE;   /* 因為是首字符是專用字符,故而直接進入結束狀態 */
				switch(ch)      /* 判斷到底是哪個專用字符 */
				{
				case EOF:
					save = FALSE;
					currentToken = ENDFILE;
					break;
				case '+':
					currentToken = PLUS;
					break;
				case '-':
					currentToken = MINUS;
					break;
				case '*':
					currentToken = TIMES;
					break;
				case ';':
					currentToken = SEMT;
					break;
				case ',':
					currentToken = COMMA;
					break;
				case '(':
					currentToken = LPAREN;
					break;
				case ')':
					currentToken = RPAREN;
					break;
				case '[':
					currentToken = LSQUARE;
					break;
				case ']':
					currentToken = RSQUARE;
					break;
				case '{':
					currentToken = LBRACE;
					break;
				case '}':
					currentToken = RBRACE;
					break;
				default:            
					currentToken = ERROR;            /* 錯誤的token */
					break;
				}
			}
			break;
		case INNUM:
			if (!isdigit(ch))
			{  /* backup in the input,利用回退技術來得到正確的digit */
				ungetNextChar();        /* 通過緩衝區數組下標自減1實現回退一個字符,即將當前字符放棄 */
				save = FALSE;           /* 當前讀取字符不予保存 */
				state = DONE;           /* 進入完成狀態 */
				currentToken = NUM;     /* 表明這個NUM讀取完畢 */
			}
			break;
		case INID:
			if (!isalpha(ch))
			{  /* backup in the input,意思同上 */
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = ID;
			}
			break;
		case INLTEQ:
			state = DONE;            /* 不論在字符<後面出現的是什麽字符都會進入到完成狀態 */
			if (ch != '=')           /* 如果這個字符是字符=以外的字符則回退 */
			{  /* backup in the input,意思同第一處註釋 */
				ungetNextChar();
				save = FALSE;
				currentToken = LT;    /* 表明上一個讀取的字符是專用字符< */
			}
			else
			{
				currentToken = LTEQ;  /* 表明這個token是專用字符<= */
			}
			break;
		case INRTEQ:
			state = DONE;            /* 不論在字符<後面出現的是什麽字符都會進入到完成狀態 */
			if (ch != '=')           /* 如果這個字符是字符=以外的字符則回退 */
			{  /* backup in the input,意思同第一處註釋 */
				ungetNextChar();
				save = FALSE;
				currentToken = RT;    /* 表明上一個讀取的字符是專用字符> */
			}
			else
			{
				currentToken = RTEQ;  /* 表明這個token是專用字符>= */
			}
			break;
		case INEQ:
			state = DONE;            /* 不論在字符<後面出現的是什麽字符都會進入到完成狀態 */
			if (ch != '=')           /* 如果這個字符是字符=以外的字符則回退 */
			{  /* backup in the input,意思同第一處註釋 */
				ungetNextChar();
				save = FALSE;
				currentToken = ASSIGN;    /* 表明上一個讀取的字符是專用字符= */
			}
			else
			{
				currentToken = EQ;  /* 表明這個token是專用字符== */
			}
			break;
		case INNEQ:
			state = DONE;            /* 不論在字符<後面出現的是什麽字符都會進入到完成狀態 */
			if (ch != '=')           /* 如果這個字符是字符=以外的字符則回退 */
			{  /* backup in the input,意思同第一處註釋 */
				ungetNextChar();
				save = FALSE;
				currentToken = ERROR;    /* 表明上一個讀取的字符是錯誤字符! */
			}
			else
			{
				currentToken = NEQ;  /* 表明這個token是專用字符!= */
			}
			break;
		case COMMENTSTART:          /* 由於這個狀態在字符/出現時已經利用前看技術跳過,故而此處不做任何處理 */
			break;
		case INCOMMENT:
			save = FALSE;           /* 註釋字符不予保存 */
			if (ch == '*')
			{
				state = COMMENTEND;        /*  進入註釋結束判斷狀態*/
			}
			else
			{
				state = INCOMMENT;        /* 依然在註釋當中狀態 */
			}
			break;
		case COMMENTEND:
			save = FALSE;                /* 註釋字符不予保存 */
			if (ch == '*')
			{
				state = COMMENTEND;      /* 依然在註釋結束判斷狀態 */
			}
			else if (ch == '/')
			{
				state = START;           /* 重新進入開始狀態 */
			}
			else
			{
				state = INCOMMENT;      /* 返回至註釋當中狀態 */
			}
			break;
		case DONE:         /* 實際上這個狀態是進不到while循環里的 */
		default:      /* should never happen,不可能出現的錯誤狀態 */
			fprintf(listing, "Scanner Bug: state = %d\n",state);        /* 掃描器的bug */
			state = DONE;               
			currentToken = ERROR;
			break;
		}
		if ((save) && (tokenStringIndex <= MAXTOKENLEN))   /*  字符可取且這個token長度不超過規定範圍*/
		{
			tokenString[tokenStringIndex++] = (char)ch;     /* 將字符保存至tokenString中以形成完整的token */
		}
		if (state == DONE)
		{
			tokenString[tokenStringIndex] = '\0';      /* 為此數組賦予結束標誌,表明以得到一個完整的token */
			if(currentToken == ID)                     /* 如果此token為identify(ID)則判斷是否為保留字 */
				currentToken = reservedLookup(tokenString);       /* 得到此token的標記 */
		}
	}
	if (TraceScan)
	{
		fprintf(listing, "\t%d: ",lineno);    /* 在屏幕上輸出行數 */
		printToken(currentToken, tokenString);    /* 在屏幕上輸出掃描成功的token */
	}
	return currentToken;        /* 返回當前token */
}   /* end getToken */

