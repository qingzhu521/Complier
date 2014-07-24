#include "globals.h"
#include "util.h"
#include "scan.h"

/* states in scanner DFA */
typedef enum
{
	START,INNUM,INID,INLTEQ,INRTEQ,INEQ,INNEQ,COMMENTSTART,INCOMMENT,COMMENTEND,DONE
}StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];    /* �˔��M�˹���: 0--MAXTOKENLEN */

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
	if (!(linepos < bufsize)) /* ��ǰ���˴�춴����L��,����ǰ���ѽ����ַ��ѽ��xȡ��,�Q��һ���^�m�xȡ */
	{
		lineno++;  /* �Д��Լ�1,�xȡ��һ�Ѓ��� */
		if (fgets(lineBuf,BUFLEN - 1,source))  /* Դ�ļ�߀δ�x��,��Դ�ļ����^�m�xȡ���� */
		{
			if (EchoSource)   /* ����ݔ����һ�� */
			{
				fprintf(listing, "%4d: %s",lineno,lineBuf);  /* ����Ļ��ݔ���@һ��,����ÿ�xȡһ�б�ݔ�����к������ */
			}
			bufsize = strlen(lineBuf);  /* �@ȡ�@һ�еČ��H�L�� */
			linepos = 0;  /* ���M�ˏ�0�_ʼ */
			return lineBuf[linepos++];  /* ���خ�ǰ�ַ����xȡ��,�˼�1,�����xȡ���е���һ���ַ� */
		}
		else
		{
			EOF_Flag = TRUE;    /* Դ�ļ��xȡ�ꮅ */
			return EOF;    /* �����ļ��Y�����I */
		}
	}
	else
	{
		return lineBuf[linepos++]; /* ���خ�ǰ�ַ����xȡ��,�˼�1,�����xȡ���е���һ���ַ�*/
	}
}

/* ungetNextChar backtracks one character in lineBuf */
static void ungetNextChar(void)
{
	if (!EOF_Flag)   /* ֻҪ�ļ�߀δ�x�� */
	{
		linepos--;   /* ���Ԝp1��ζ������һ�� */
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
	int i;  /* ��׃�� */
	for (i = 0; i < MAXRESERVED; i++)
	{
		if (!strcmp(s,reservedWords[i].str))  /* strcmp���ַ������^����,����ɂ��ַ���һ�ӄt����0,��t����1 */
		{
			return reservedWords[i].tok;   /* �Ǳ�����,�t���� */
		}
	}
	return ID;    /* ���Ǳ����ֶ���ID */
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
				state = INNUM;   /* ���ַ��锵�քt�M�딵���Д��B */
			else if(isalpha(ch))
				state = INID;    /* ���ַ����ַ��t�M��ID�Д��B */
			else if(ch == ' ' || ch == '\n' ||ch == '\t')
				save = FALSE;    /* ���ַ���ո񡢓Q�з����u�l���t�M��ID�Д��B,�Ҳ��豣�� */
			else if(ch == '<')
				state = INLTEQ;  /* ���ַ���'<'�t�M��'<='�Д��B */
			else if(ch == '>')
				state = INRTEQ;  /* ���ַ���'>'�t�M��'>='�Д��B */
			else if(ch == '=')
				state = INEQ; /* ���ַ���'='�t�M��'=='�Д��B */
			else if(ch == '!')
				state = INNEQ;   /* ���ַ���'!'�t�M��'!='�Д��B */
			else if(ch == '/')
			{                    /* ���ַ���'/'�t�M��ע��Д��B */
				state = COMMENTSTART;      /* �M���]��Д��B */
				if (getNextChar() == '*')
				{
					save = FALSE;            /* �f���M�����]��Z��,�ʶ����豣�� */ 
					state = INCOMMENT;    /* �M���]ጠ�B */
				}
				else
				{
					currentToken = OVER;    /* �f��ǰһ���xȡ���ַ��ǌ����ַ�/ */
					state = DONE;           /* �M��Y����B */
					ungetNextChar();        /* �C���@��'/'ֻ�ǌ����ַ�OVER,����Ҫ��֮ǰǰ����λ�'/'������ַ����˻�ȥ */
				}
			}
			else
			{
				state = DONE;   /* ��������ַ��ǌ����ַ�,�ʶ�ֱ���M��Y����B */
				switch(ch)      /* �Дൽ�����Ă������ַ� */
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
					currentToken = ERROR;            /* �e�`��token */
					break;
				}
			}
			break;
		case INNUM:
			if (!isdigit(ch))
			{  /* backup in the input,���û��˼��g��õ����_��digit */
				ungetNextChar();        /* ͨ�^���n�^���M���Ԝp1���F����һ���ַ�,������ǰ�ַ��ŗ� */
				save = FALSE;           /* ��ǰ�xȡ�ַ����豣�� */
				state = DONE;           /* �M����ɠ�B */
				currentToken = NUM;     /* �����@��NUM�xȡ�ꮅ */
			}
			break;
		case INID:
			if (!isalpha(ch))
			{  /* backup in the input,��˼ͬ�� */
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = ID;
			}
			break;
		case INLTEQ:
			state = DONE;            /* ��Փ���ַ�<������F����ʲ���ַ������M�뵽��ɠ�B */
			if (ch != '=')           /* ����@���ַ����ַ�=������ַ��t���� */
			{  /* backup in the input,��˼ͬ��һ̎�]� */
				ungetNextChar();
				save = FALSE;
				currentToken = LT;    /* ������һ���xȡ���ַ��ǌ����ַ�< */
			}
			else
			{
				currentToken = LTEQ;  /* �����@��token�ǌ����ַ�<= */
			}
			break;
		case INRTEQ:
			state = DONE;            /* ��Փ���ַ�<������F����ʲ���ַ������M�뵽��ɠ�B */
			if (ch != '=')           /* ����@���ַ����ַ�=������ַ��t���� */
			{  /* backup in the input,��˼ͬ��һ̎�]� */
				ungetNextChar();
				save = FALSE;
				currentToken = RT;    /* ������һ���xȡ���ַ��ǌ����ַ�> */
			}
			else
			{
				currentToken = RTEQ;  /* �����@��token�ǌ����ַ�>= */
			}
			break;
		case INEQ:
			state = DONE;            /* ��Փ���ַ�<������F����ʲ���ַ������M�뵽��ɠ�B */
			if (ch != '=')           /* ����@���ַ����ַ�=������ַ��t���� */
			{  /* backup in the input,��˼ͬ��һ̎�]� */
				ungetNextChar();
				save = FALSE;
				currentToken = ASSIGN;    /* ������һ���xȡ���ַ��ǌ����ַ�= */
			}
			else
			{
				currentToken = EQ;  /* �����@��token�ǌ����ַ�== */
			}
			break;
		case INNEQ:
			state = DONE;            /* ��Փ���ַ�<������F����ʲ���ַ������M�뵽��ɠ�B */
			if (ch != '=')           /* ����@���ַ����ַ�=������ַ��t���� */
			{  /* backup in the input,��˼ͬ��һ̎�]� */
				ungetNextChar();
				save = FALSE;
				currentToken = ERROR;    /* ������һ���xȡ���ַ����e�`�ַ�! */
			}
			else
			{
				currentToken = NEQ;  /* �����@��token�ǌ����ַ�!= */
			}
			break;
		case COMMENTSTART:          /* ����@����B���ַ�/���F�r�ѽ�����ǰ�����g���^,�ʶ���̎�����κ�̎�� */
			break;
		case INCOMMENT:
			save = FALSE;           /* �]��ַ����豣�� */
			if (ch == '*')
			{
				state = COMMENTEND;        /*  �M���]ጽY���Д��B*/
			}
			else
			{
				state = INCOMMENT;        /* ��Ȼ���]ጮ��Р�B */
			}
			break;
		case COMMENTEND:
			save = FALSE;                /* �]��ַ����豣�� */
			if (ch == '*')
			{
				state = COMMENTEND;      /* ��Ȼ���]ጽY���Д��B */
			}
			else if (ch == '/')
			{
				state = START;           /* �����M���_ʼ��B */
			}
			else
			{
				state = INCOMMENT;      /* �������]ጮ��Р�B */
			}
			break;
		case DONE:         /* ���H���@����B���M����whileѭ�h��� */
		default:      /* should never happen,�����ܳ��F���e�`��B */
			fprintf(listing, "Scanner Bug: state = %d\n",state);        /* ��������bug */
			state = DONE;               
			currentToken = ERROR;
			break;
		}
		if ((save) && (tokenStringIndex <= MAXTOKENLEN))   /*  �ַ���ȡ���@��token�L�Ȳ����^Ҏ������*/
		{
			tokenString[tokenStringIndex++] = (char)ch;     /* ���ַ�������tokenString�����γ�������token */
		}
		if (state == DONE)
		{
			tokenString[tokenStringIndex] = '\0';      /* ��˔��M�x��Y�����I,�����Եõ�һ��������token */
			if(currentToken == ID)                     /* �����token��identify(ID)�t�Д��Ƿ�鱣���� */
				currentToken = reservedLookup(tokenString);       /* �õ���token�Ę�ӛ */
		}
	}
	if (TraceScan)
	{
		fprintf(listing, "\t%d: ",lineno);    /* ����Ļ��ݔ���Д� */
		printToken(currentToken, tokenString);    /* ����Ļ��ݔ������ɹ���token */
	}
	return currentToken;        /* ���خ�ǰtoken */
}   /* end getToken */

