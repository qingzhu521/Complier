#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token,token_temp[MAXTOKENTEMP];  /* holds current token,ǰ���r���ԕ���ǰ������token�Ĕ��M */
static char * tokenString_temp[MAXTOKENTEMP];     /* ��ǰ���r����ǰ��token��tokenString */
int i = 0,j = 0;         /* i���ǰ�����ăɂ����攵�M����,��ʼ����0,�ǌ��T����expression()�����ϵ� */
/* j���expression()������Ӌ��'[]'Ƕ�ׂ���,���var���ܕ��ж༉��'[]'Ƕ�� */
int i_first = 0,j_last = 0;                        
/* i_first�����var�Ĕ��M�r����Ȍӵ�expression�ĵ�һ��token����*/
/* j_last�����var�Ĕ��M�r����Ȍӵ�expression������һ��token���� */
int Flag = 0,Flag_var = 0,Flag_FOrD = 0;           /* Flag�Á��Д�var()�����Ƿ��{�õĘ��I */
/* Flag_var�Á��Д�var����Ȍ�expression�Ƿ���赽������һ��token,�Դ_����ȡ��һ��token߀��ȡtoken_temp�е�����һ��token */


/* function prototypes for recursive calls */
/* ���к���ԭ�͵��f�w�{���� */
static TreeNode * declaration_list(void);
static TreeNode * declaration(void);
static TreeNode * var_declaration(void);
static TreeNode * fun_declaration(void);
static TreeNode * params(void);
static TreeNode * param_list(void);
static TreeNode * param(void);
static TreeNode * compound_stmt(void);
static TreeNode * local_declaration(void);
static TreeNode * statement_list(void);
static TreeNode * statement(void);
static TreeNode * expression_stmt(void);
static TreeNode * selection_stmt(void);
static TreeNode * iteration_stmt(void);
static TreeNode * return_stmt(void);
static TreeNode * expression(void);
static TreeNode * var(void);
static TreeNode * simple_expression(void);
static TreeNode * additive_expression(void);
static TreeNode * term(void);
static TreeNode * factor(void);
static TreeNode * call(void);
static TreeNode * args(void);
static TreeNode * arg_list(void);


static void syntaxErrorShow(char * error_message)
{/* Դ���a�����e�`��Ϣ��ʾ */
	fprintf(listing,"\n>>> ");
	fprintf(listing,"Syntax error at line %d: %s",lineno,error_message);
	Error = TRUE;
}

static void match(TokenType expectedToken)
{/* ƥ�䮔ǰToken�Լ�ȡ����һ��Token */
	if (token == expectedToken)  /* ƥ�䮔ǰToken */
	{
		token = getToken();    /* ȡ��һ��Token */
	}
	else
	{/* �e�`: ���F������֮���Token */
		syntaxErrorShow("unexpected token -> ");
		printToken(token,tokenString);
		fprintf(listing,"      ");
	}
}


static TreeNode * declaration_list(void)
{
	TreeNode * t_head = declaration();   /* Դ����ĵ�һ�������Z������^�Y�cָ�ָ�� */
	TreeNode * p = t_head;               /* pָ�Ҳָ���Z������^�Y�c,�Դ���t_headָ������Ƅӵõ������ֵܽY�c */
	while(token != ENDFILE)              /* C-���������б�M�L,ֻҪ�]�е��ļ��Y�����f��߀������������ */
	{
		TreeNode * q = declaration();    /* qָ�ָ����һ���� */
		if (q != NULL)                   /* �@�������� */
		{
			if (t_head == NULL)          /* ����^һ���������� */
			{
				t_head = p = q;          /* �t��qָ����������^�� */
			}
			else                         /* �^һ�������� */
			{
				p->sibling = q;          /* �tqָ����������^�����ֵܽY�c,Ҳ������һ���� */
				p = q;                   /* pָ������Ƅ�һλ,���B�������������������ֵ� */
			}
		}
	}
	return t_head;                      /* ���ش�Դ���a�������õ��Z������^ָ� */
}


static TreeNode * declaration(void)     /* ������,�Дൽ����׃����߀�Ǻ����� */
{	                                    /* ��ǰ�ɂ�token߀�o���Д����׃����߀�Ǻ�����,�����Դˁ핺�������token */
	TreeNode * t_head = NULL;
	if (token == LBRACE)                 /* �����һ��token��'{',�t�f���Ǻ����� */
	{
		t_head = fun_declaration();     /* �M�뺯���������f�w�{�� */
		return t_head;                  /* ����ѽ��Д��Ǻ������t�{��fun_declaration()���� */
	}
	token_temp[0] = token;              /* �����ׂ�token����tokenString */
	tokenString_temp[0] = copyString(tokenString);
	token_temp[1] = getToken();         /* ��ǰȡ�ڶ���token */
	tokenString_temp[1] = copyString(tokenString);
	token_temp[2] = getToken();         /* ��ǰȡ������token */
	tokenString_temp[2] = copyString(tokenString);
	switch(token_temp[2])
	{
	case LPAREN:                       /* ������token�M���Д�,�����'(',�t�Ǻ����� */
		t_head = fun_declaration();
		break;
	case LSQUARE:                        /* �����'['����';',�t��׃���� */
	case SEMT:
		t_head = var_declaration();
		break;
	default:
		syntaxErrorShow("unexpected token -> ");         /* ݔ���@������֮����e�`token��Ϣ */
		printToken(token_temp[2],tokenString_temp[2]);
		token = getToken();          /* ȡ��һ��token�xֵ�otoken׃�� */
		break;
	}
	return t_head;
}


static TreeNode * var_declaration(void)
{
	TreeNode * t_head = newDeclNode(Variablek);          /* Դ�����Z�����е�һ������ͽY�c */
	if (t_head != NULL)                                  /* �ӑB����׃������ͽY�c�ɹ� */
	{
		if (tokenString_temp[0] != 0 && Flag_FOrD == 0)
		{/* ����@��׃�����Ǫ�����һ��׃����,�����}���Z����ͺ�����֮���׃�������ڴ�֮ǰtoken_temp�惦�^������token */
			switch(token)                                    /* ����ǰtoken�M���x���Д��Դ_����׃������INT���߀��VOID��� */
			{
			case INT:                                        /* ׃������INT��� */
				t_head->attribute.name = copyString(tokenString_temp[0]);            /* �t����Operater�����xֵ��INT */
				break;
			case VOID:
				t_head->attribute.name = copyString(tokenString_temp[0]);
				break;
			default:                                         /* ���F������֮���token */
				syntaxErrorShow("unexpected token -> ");
				printToken(token,tokenString_temp[0]);
				token = token_temp[1];
				break;
			}
			token = token_temp[1];                           /* ȡ��һ��token */
			if (token == ID)                                 /* �Д��token�Ƿ����ϗl����ID */
			{
				TreeNode * p = newExpNode(Identifyk);        /* ��ID,ID���׃�����_ʽ,�ӑB����һ��ID���_ʽ�Y�c */
				if (p != NULL)
				{/* �����ɹ� */
					t_head->child[0] = p;                    /* ���@��ID�Y�c�������һ������ */
					p->attribute.name = tokenString_temp[1];  /* ���@��ID��ֵ�x�oID�Y�c��name���� */
				}
			}
			else
			{/* ����ID,�t������֮���token,ݔ���e�`��Ϣ,��ȡ��һ��token */
				syntaxErrorShow("unexpected token -> ");
				printToken(token,tokenString_temp[1]);
			}
		    token = token_temp[2];     /* ȡ��һ��token */
		}
		else
		{/* ����@��׃��������һ���}���Z����͵ĺ��������� */
			switch(token)                                    /* ����ǰtoken�M���x���Д��Դ_����׃������INT���߀��VOID��� */
			{
			case INT:                                        /* ׃������INT��� */
				t_head->attribute.name = copyString(tokenString);            /* �t����Operater�����xֵ��INT */
				break;
			case VOID:
				t_head->attribute.name = copyString(tokenString);
				break;
			default:                                         /* ���F������֮���token */
				syntaxErrorShow("unexpected token -> ");
				printToken(token,tokenString);
				token = getToken();
				break;
			}
			match(token);                                    /* ƥ�䮔ǰtoken,ȡ��һ��token */
			if (token == ID)
			{
				TreeNode * p = newExpNode(Identifyk);        /* ��ID,ID���׃�����_ʽ,�ӑB����һ��ID���_ʽ�Y�c */
				if (p != NULL)
				{/* �����ɹ� */
					t_head->child[0] = p;                    /* ���@��ID�Y�c�������һ������ */
					p->attribute.name = copyString(tokenString);  /* ���@��ID��ֵ�x�oID�Y�c��name���� */
				}
			}
			match(ID);                                       /* ƥ��ID,ȡ��һ��token */
		}
		if (token == LSQUARE)
		{/* ��׃�����п����е�'[NUM]'�M���Д� */
			match(LSQUARE);                             /* ƥ��'['token,ȡ��һ��token */
			if (token == NUM)
			{
				TreeNode * q = newExpNode(Constk);      /* NUM��춳������_ʽ,�ӑB����һ��NUM���_ʽ�Y�c */
				if (q != NULL)
				{/* �����ɹ� */
					t_head->child[1] = q;               /* ���@��NUM�Y�c������ڶ������� */
				    q->attribute.val = atoi(tokenString);  /* ���@��NUM��ֵ�x�oNUM�Y�c��val���� */
				}
			}
			match(NUM);           /* ƥ��NUM,ȡ��һ��token */
			match(RSQUARE);       /* ƥ��']'token,ȡ��һ��token */
		}
		match(SEMT);      /* ƥ��token';',ȡ��һ��token,����׃�����ĽY�c�������xֵ�ѽ���� */
	}
	memset(tokenString_temp,0,sizeof(tokenString_temp));         /* ���³�ʼ��tokenString_temp���M */
	return t_head;
}


static TreeNode * fun_declaration(void)
{
	TreeNode * t_head = newDeclNode(Functionk);        /* �ӑB����һ��Դ���a�Z�����еĺ������Y�c */
	if (t_head != NULL)                                /* ����˽Y�c�����ɹ� */
	{
		switch(token)                                  /* ���������ĵ�һ��token�M���x�� */
		{
		case INT:                                      /* �˺������ķ���ֵ��INT */
			t_head->attribute.name = copyString(tokenString_temp[0]);
			break;
		case VOID:                                     /* �˺������ķ���ֵ��VOID */
			t_head->attribute.name = copyString(tokenString_temp[0]);
			break;
		case LBRACE:                                    /* �˺������ǂ��}���Z�� */
			t_head = compound_stmt();
			return t_head;                             /* ������}���Z��t����Ҫ�������M�з���,����ֱ���{��compound_stmt()���� */
			break;
		default:                                       /* ����춺������A�ϵ�token֮��,ݔ���e�`��Ϣ */
			syntaxErrorShow("unexpected token -> ");
			printToken(token,tokenString);
			token = token_temp[1];                     /*  ȡ��һ��token*/
			break;
		}
		token = token_temp[1];                         /* ȡ��һ��token */
		if (token == ID)
		{/* ƥ��ڶ���token�Ƿ��ID */
			TreeNode * p = newExpNode(Identifyk);       /* ��ID,ID���׃�����_ʽ,�ӑB����һ��ID���_ʽ�Y�c */
			if (p != NULL)
			{/* �����ɹ� */
				t_head->child[0] = p;                   /* ���@��ID���_ʽ�Y�c�������һ������ */
			    p->attribute.name = tokenString_temp[1];   /* ��ID��ֵ�x�o�@��ID�Y�c��name���� */
			}
		}
		else
		{/* ��token��������֮�е�ID,ݔ���e�`��Ϣ */
			syntaxErrorShow("unexpected token -> ");
			printToken(token,tokenString_temp[1]);
		}
		token = token_temp[2];                         /* ȡ��һ��token */
		if (token == LPAREN)
		{/* ƥ�������token�Ƿ��'(' */
			token = getToken();
			t_head->child[1] = params();              /* ƥ��ɹ�,�{�ú������ą����ķ��������� */
		}
		else
		{/* ��token��������֮�е�'(',ݔ���e�`��Ϣ */
			syntaxErrorShow("unexpected token -> ");
			printToken(token,tokenString_temp[2]);
		}
		match(RPAREN);                                /* ƥ�亯�����е�����һ��')',��ȡ��һ��token,�����@�������������ꮅ */
	}
    memset(tokenString_temp,0,sizeof(tokenString_temp));     /* ���³�ʼ��tokenString_temp���M */
	return t_head;
}


static TreeNode * params(void)
{
	TreeNode * t_head = NULL;                        /* �������Y�c,���r��� */
	TreeNode * p = NULL;
	switch(token)                                    /* ��params�еĵ�һ��token�M���x�� */
	{
	case INT:                                        /* �������������,�{�Å����б��� */
		t_head = param_list();
		break;
	case VOID:
		p = newExpNode(ParamK);         /* �����������,�ӑB����һ��operater��͵ı��_ʽ */
		if (p != NULL)
		{/* �����ɹ� */
			t_head = p;
		    t_head->attribute.name = copyString(tokenString);           /* �xֵ */
		}
		match(VOID);
		break;
	default:                                         /* ���F������֮���token */
		syntaxErrorShow("unexpected token -> ");
		printToken(token,tokenString);
		token = getToken();                          /* ȡ��һ��token */
		break;
	}
	return t_head;
}

static TreeNode * param_list(void)
{/* �ķ�: 'param{,param}' */
	TreeNode * t_head = param();                     /* �����б���^ָ� */
	TreeNode * p = t_head;                           /* ��ָᘵĹ����Ǵ����^ָ������B�������ą����Y�c,�����o�^ָ� */
	while (token == COMMA)                           /* �����token��',',�t�f������߀�Ѕ��� */
	{
		TreeNode * q = NULL;
		match(COMMA);                                /* ƥ��','token,ȡ��һ��token */
		q = param();                      /* ��һ�������Y�cָ� */
		if (q != NULL)
		{/* ���������������б��Z���� */
			if (t_head == NULL)
			{
				t_head = p = q;
			}
			else
			{
				p->sibling = q;                      /* �����Y�c�˴˻����ֵ� */
				p = q;
			}
		}
	}
	return t_head;
}


static TreeNode * param(void)
{
	TreeNode * t_head = newExpNode(ParamK);       /* ���麯�����Y�c�ڶ������ӵą����Y�c */
	if (t_head != NULL)                             /* �ӑB���������Y�c�ɹ� */
	{
		if (token == INT)                           /* ��������ֵ��INT */
		{
			t_head->attribute.name= copyString(tokenString);
		}
		match(INT);                                /* ƥ��INT,ȡ��һ��token */ 
		if (token == ID)                           /* INT�����token��ID*/
		{
			TreeNode * p = NULL;
			p = newExpNode(Identifyk);      /* �ӑB����һ��id��͵ı��_ʽ�Y�c */
			if (p != NULL)                             /* �����ɹ� */
			{/* �����ɹ� */
				t_head->child[0] = p;                  /* ID�Y�c���酢���Y�c�ĵ�һ������ */
			    p->attribute.name = copyString(tokenString);
			}
		}
		match(ID);                                 /* ƥ��ID,ȡ��һ���Y�c */
		if (token == LSQUARE)                      /* ���ID�����token��'[',�t�f���˅����锵�M���� */
		{                                          /* ��'['��']'���酢���Y�c�ĵڶ����͵��������ӽY�c */
			TreeNode * q = NULL;
			TreeNode * t = NULL;
			q = newExpNode(Operatek);   /* '['��']'�����operater���_ʽ */
			if (q != NULL)
			{/* �����ɹ� */
				t_head->child[1] = q;
				q->attribute.Operater = LSQUARE;
			}
			match(LSQUARE);
			t = newExpNode(Operatek);
			if (t != NULL && token == RSQUARE)
			{/* �����ɹ� */
				t_head->child[2] = t;
				t->attribute.Operater = RSQUARE;
			}
			match(RSQUARE);
		}
	}
	return t_head;
}

static TreeNode * compound_stmt(void)
{
	TreeNode * t_head = newDeclNode(Functionk);        /* �����}���Z���͵ĺ��������^�Y�cָ� */
	if (t_head != NULL)
	{
		t_head->attribute.name = NULL;
	}
	match(LBRACE);                                     /* ƥ��'{'token,ȡ��һ��token */
	if (t_head != NULL)
	{/* �����ɹ� */
		t_head->child[0] = local_declaration();            /* �{�ú���local_declaration() */
	    t_head->child[1] = statement_list();               /* �{�ú���statement_list() */
	}
	match(RBRACE);                                     /* ƥ��'}'token,ȡ��һ��token */
	return t_head;
}

static TreeNode * local_declaration(void)
{
	TreeNode * t_head = NULL;                          /* һ�_ʼ�ı�������һ���Y�c��� */
	TreeNode * p;                                      /* ���o�^�Y�c�������Ƅ����B�����������ֵܽY�c */
	while((token == INT) || (token == VOID))           /* ��һ��token��INT����VOID,�f��һ���ǂ�׃���� */
	{
		TreeNode * q;
		q = var_declaration();                         /* �{��׃�����������������Y�c */
		if (q != NULL)                                 /* �Y�c�����ɹ� */
		{
			if (t_head == NULL)                        /* �����һ���Y�c��� */
			{
				t_head = p = q;                        /* �t���@������յĽY�c���鱾��һ�B������һ���Y�c */
			}
			else
			{
				p->sibling = q;                        /* �����һ���Y�c�ѽ������,�t���@��Ҳ����յĽY�c���������ֵ� */
				p = q;                                 /* ʹ����ָ��ǰ�Y�c��ָ�p�����Ƅ�һλ */
			}
		}
	}
	return t_head;
}


static TreeNode * statement_list(void)
{
	TreeNode * t_head = NULL;                          /* һ�_ʼ���Z���б�ĵ�һ���Y�c��� */
	TreeNode * p;                                      /* ���o�^�Y�c�������Ƅ����B�����������ֵܽY�c */
	while((token == IF) || (token == WHILE) || (token == RETURN) || (token == ID) || (token == NUM) || (token == LPAREN) || (token == LBRACE))           
	{ /* ��һ��token��IF����HWILE����RETURN����ID����NUM����'('����'{',�f��һ�����Z�� */
		TreeNode * q;
		q = statement();                               /* �{���Z�亯�����������Y�c */
		if (q != NULL)                                 /* �Y�c�����ɹ� */
		{
			if (t_head == NULL)                        /* �����һ���Y�c��� */
			{
				t_head = p = q;                        /* �t���@������յĽY�c���鱾��һ�B���Z���һ���Y�c */
			}
			else
			{
				p->sibling = q;                        /* �����һ���Y�c�ѽ������,�t���@��Ҳ����յĽY�c���������ֵ� */
				p = q;                                 /* ʹ����ָ��ǰ�Y�c��ָ�p�����Ƅ�һλ */
			}
		}
	}
	return t_head;
}


static TreeNode * statement(void)
{
	TreeNode * t_head = NULL;
	switch(token)                 /* ���Z��ĵ�һ��token�M���x�����Д�����һ��͵��Z�� */
	{
	case IF:
		t_head = selection_stmt();      /* ��if�x���Z�� */
		break;
	case WHILE:
		t_head = iteration_stmt();      /* ��whileѭ�h�Z�� */
		break;
	case RETURN:                        /* ��return�����Z�� */
		t_head = return_stmt();
		break;
	case ID:
	case NUM:
	case LPAREN: 
		t_head = expression_stmt();     /* �Ǳ��_ʽ�Z�� */
		break;
	case LBRACE:
		t_head = compound_stmt();       /* ���}���Z�� */
		break;
	default:                            /* ���F������֮���token */
		syntaxErrorShow("unexpected token -> ");
		printToken(token,tokenString);
		token = getToken();
		break;
	}
	return t_head;
}


static TreeNode * expression_stmt(void)
{
	TreeNode * t_head = newStmtNode(Expressionk);               /* �ӑB����һ�����_ʽ�Z��Y�c */
	if ((token == ID) || (token == NUM) || (token == LPAREN) && (t_head != NULL))   /* ����һ��token��ID����NUM����'(',�t�f�����Z�����_ʽ�Z�� */
	{
		t_head->child[0] = expression();
	}
	match(SEMT);                                                /* ƥ��';'token,ȡ��һ��token */
	return t_head;
}


static TreeNode * selection_stmt(void)
{
	TreeNode * t_head = newStmtNode(IfK);                      /* �ӑB����һ��if�Z��Y�c */
	match(IF);                                                 /* ƥ��if,ȡ��һ��token */
	match(LPAREN);                                             /* ƥ��'('token,ȡ��һ��token */
	if (t_head != NULL)                                        /* if�Z��Y�c�����ɹ� */
	{
		t_head->child[0] = expression();                       /* if�l���Д���_ʽ�Y�c�������һ������ */
	}
	match(RPAREN);                                             /* ƥ��')'token,ȡ��һ��token */
	if (t_head != NULL)
	{
		t_head->child[1] = statement();                        /* �l������r���Z��Y�c������ڶ������� */
	}
	if (token == ELSE)                                         /* ����@��if�������else */
	{
		match(ELSE);                                           /* ƥ��else,ȡ��һ��token */
		if (t_head != NULL)
		{
			t_head->child[2] = statement();                    /* �l����ٕr���Z��Y�c��������������� */
		}
	}
	return t_head;
}


static TreeNode * iteration_stmt(void)
{
	TreeNode * t_head = newStmtNode(WhileK);                  /* �ӑB����һ��while�Z��Y�c */
	match(WHILE);                                             /* ƥ��while,ȡ��һ��token */
	match(LPAREN);                                            /* ƥ��'('token,ȡ��һ��token */
	if (t_head != NULL)                                       /* ���while�Z��Y�c�����ɹ� */
	{
		t_head->child[0] = expression();                      /* ��ѭ�h�Д��l�����_ʽ�Y�c�������һ������ */
	}
	match(RPAREN);                                            /* ƥ��')'token,ȡ��һ��token */
	if (t_head != NULL)
	{
		t_head->child[1] = statement();                       /* ��ѭ�h�Z��Y�c������ڶ������� */
	}
	return t_head;
}


static TreeNode * return_stmt(void)
{
	TreeNode * t_head = newStmtNode(Returnk);                /* �ӑB����һ��return�Z��Y�c */
	match(RETURN);                                           /* ƥ��return,ȡ��һ��token */
	if ((token == ID) || (token == NUM) || (token == LPAREN) && (t_head != NULL))  /* �����expression��return�Z��Y�c�����ɹ� */
	{
		t_head->child[0] = expression();                     /* ���˷��صı��_ʽ�Y�c�������һ������ */
	}
	match(SEMT);                                             /* ƥ��';'token,ȡ��һ��token */
	return t_head; 
}


static TreeNode * expression(void)
{
	TreeNode * t_head = NULL;                       /* ���_ʽ�^ָ�ָ��,�д��������Д� */
	Flag = 0;                                       /* ÿ���{��expression()��������ʼ��Flag,��ʾ���{���˴˺��� */
	if (token == ID)                         
	{/* �����һ��token��ID�tҪһֱ������貢�����������ÿһ��token,ֱ��var�����ꮅ,��var�����Л]���xֵ̖'=' */
		/* ������xֵ̖'=',�t�f���@�����_ʽ��var = expression,��t����simple-expression */
		/* �����һ��token�Ͳ���ID�t�@�����_ʽ����simple-expression */
		Flag_FOrD = 1;                              /* ��ʾtoken_temp���M�惦�˷�����token */
		i = 0;                                      /* ÿ���{��expression()��������ʼ����i,��ζ��token_temp���M���Ǐ��^�_ʼ���� */
		if (i_first != 0 && j_last != 0)
		{/* ����@���ǵ�һ�γ��Fexpression���_ʼ����var����r,�����F��expressionǶ�׵���r */
			token_temp[i] = token_temp[i_first];
			tokenString_temp[i] = tokenString_temp[i_first];
			if (i_first < j_last)
			{/* �@��expressionǰ���expression��ֹһ��token,�@��ID߀������Ȍ�expression������һ��token */
				i++;/* ȡ���ڶ���token */
				token_temp[i] = token_temp[i_first + 1];
				tokenString_temp[i] = tokenString_temp[i_first + 1];
			    token = token_temp[i];
			}
		}
		else
		{/* �@�ǵ�һ�γ��Fexpression���_ʼ����var����r,�����_ʼ�M��var��Ȍ�expression����r */
			token_temp[i] = token;                     /* �����һ��IDtoken */
			tokenString_temp[i] = copyString(tokenString);    /* �����@��token��tokenString */
			i++;                                       /* ȡ�ڶ���token,�����Ƿ��'[' */
			token_temp[i] = getToken();
			tokenString_temp[i] = copyString(tokenString);
			token = token_temp[i];
			if (token == LSQUARE)/* ����ڶ���token�_����'[',�t�f��var�ǂ����M�Y��,��ȥ���@��var�Ƿ��Д��MǶ��,��t�f��var=ID */
			{
				int k;
				j = 0;
				while (token != RSQUARE)             /* �Д��@��token�Ƿ��']',������Ǆt�^�m����,����Ǆtֹͣ���� */
				{/* �^�mѭ�h�����Д� */
					i++;
					token_temp[i] = getToken();
					tokenString_temp[i] = copyString(tokenString);
					token = token_temp[i];
					if (token == RSQUARE)
					{
						j_last = i - 1;              /* ����var��Ȍ�expression������һ��token����,�����ڵ�һ��']'ǰ��token�� */
					}
				}
				for (k = 2; k < i; k++)
				{/* ���var���H�H��һ��ID,�t�ڵ�һ��']'token���_ʼ��'['token֮�g��ǰ�����Д� */
					if (token_temp[k] == LSQUARE)       /* ������F'['token,�t�f��Ƕ����һ��'[]' */
					{
						j++;                            /* Ƕ�׵Ă��� */
					}
				}
				for (k = 0; k < j; k++)
				{/* ����'[]'��Ƕ�ה�,����һ��']'�����ÿһ��']'���貢�Д��Ƿ���e */
					i++;
					token_temp[i] = getToken();
					tokenString_temp[i] = copyString(tokenString);
					token = token_temp[i];
					if (token != RSQUARE)            /* ����']',���F������֮���token */
					{
						syntaxErrorShow("unexpected token -> ");
						printToken(token,tokenString_temp[i]);
					}
				}
				/* ������var���е��ַ�,�������xȡһ��token,�Д����Ƿ��'=',�Ķ����Д��@��expression�ǺηN��� */
				i++;
				token_temp[i] = getToken();
				tokenString_temp[i] = copyString(tokenString);
				token = token_temp[i];
			}
		}
		if (token == ASSIGN)
		{/* var֮��ĵ�һ��token���xֵ̖'=',var: ID['['expression']'] */
			TreeNode * q = newExpNode(Operatek);         /* �ӑB����һ��operater��͵ı��_ʽ�Y�c */
			if (q != NULL)
			{/* �����ɹ� */
				t_head = q;                              /* �t�@�����_ʽ�Y�c�����@��operater��͵� */
				t_head->attribute.Operater = token;      
				t_head->child[0] = var();                /* ���ĵ�һ���������xֵ̖'='ǰ���׃�� */
				if (Flag_var == 0)
				{/* ���֮ǰ߀δ�{���^var()����,����token_temp���M��߀�]�д惦�κε�token */
					match(ASSIGN);                      /* ȡ'='�������һ��token */
				}
				else
				{/* ���token_temp���M��߀��token�҄t�^�m����ȡ */
					i_first++;
					token = token_temp[i_first];
				}
				t_head->child[1] = expression();         /* ���ĵڶ����������xֵ̖'='����ı��_ʽ */
			}
		}
		else
		{/* ���var֮��ĵ�һ��token�����xֵ̖'=' */
			token = token_temp[0];                       /* ��token�֏͞�expression�ĵ�һ��token */
			t_head = simple_expression();                /* �t�@�����_ʽ�麆�α��_ʽ */
		}
	}
	else
	{/* �����һ��token����ID */
		t_head = simple_expression();                   /* �t�@�����_ʽ�麆�α��_ʽ */
	}
	return t_head;
}


static TreeNode * var(void)
{
	TreeNode * t_head = newExpNode(Identifyk);          /* �ӑB����һ��ID��ͱ��_ʽ�Y�c */
	TreeNode * p = t_head;                              /* �����^�Y�cָᘲ��������B����������е�һϵ�к��ӽY�c */
	if (t_head != NULL)                               
	{/* �����ɹ� */
		t_head->attribute.name = tokenString_temp[0];       /* �xֵ */
	}
	if (i > 1 && t_head != NULL)                    /* ���token_temp[]���M�б����token�����2,�t�f��varһ���Д��M�Y�� */
	{
		TreeNode * p_index = NULL;                  /* ÿһ�����M����ָ� */
		TreeNode * q = NULL;
		TreeNode * t = NULL;
		int k;
		for (k = 1; k < (j + 1) * 2; k++)
		{/* �ĵ�һ��ID֮��ĵ�һ��token�_ʼ�Д�,ֱ��var�е�����һ��'['��ֹ(��j��Ƕ�׾���j+1��'[') */
			if (token_temp[k] == ID || token_temp[k] == LSQUARE)
			{/* ��һ��ID֮���ÿһ��tokenҪ�N��'[',Ҫ�N��ID */
				if (token_temp[k] == ID)
				{/* �����ID,�t�f���ǂ�Ƕ�� */
					p_index = newExpNode(IndexK);
					if (p_index != NULL)
					{
						p->sibling = p_index;         /* ���@һ�Ӕ��M����ָ����������ֵ� */
					}
					q = newExpNode(Identifyk);        /* �ӑB����һ��ID���_ʽ�Y�c�����Ƕ�ה��M��ID */
					if (q != NULL)
					{/* �����ɹ� */
						q->attribute.name = tokenString_temp[k];    
						p_index->child[0] = q;       /* ���˽Y�c����var�^�Y�c�ĵ�һ������,֮���ÿһ��Ƕ��ID����������һ�ӵĵ�һ������ */
					    p = q;                 /* pָ�λ�������Ƅ�һλ */
					}
				}
			}
			else
			{/* ���F�˼Ȳ���'['Ҳ����ID������֮���token */
				syntaxErrorShow("unexpected token -> ");
				printToken(token_temp[k],tokenString_temp[k]);
			}
		}
		i_first = k;                         /* ����var��Ȍ�expression�ĵ�һ��token��,��������һ��'['����ĵ�һ��token�� */
		token = token_temp[k];               /* ȡvar��Ȍ�expression�ĵ�һ��token */
		Flag_var = 1;                        /* ��ʾ���{��expression()���������{����var()������var����ʽ��:ID[expression] */
		t = newExpNode(IndexK);              /* ����Ӕ��M����ָ� */
		if (t != NULL)
		{/* �����ɹ� */
			p->sibling = t;                  
		}
		t->child[0] = expression();           /* ����һ��Ƕ���Y����˞�һ��expression,������������һ��Ƕ�׵ĵ�һ������ */
		Flag_var = 0;                         /* ��ʾexpression�е�var�ѽ������ꮅ */
		i_first = j_last = 0;                 /* ���³�ʼ�� */
		return t_head;                        /* �o�����^�m��ȥ,ֱ�ӷ����^ָ� */
	}
	if (Flag_var == 1 && i_first == j_last)
	{/* ���߀��var����Ȍ�expression���������ѽ�����������һ��token */
		token = token_temp[j_last + 2 + j];    /* �tȡ��var��ĵ�һ��token */
		if (token != ASSIGN && token != SEMT)
		{/* ���var��ĵ�һ��token����'='��';',�t�f������߀���M��Ӌ����߱��^��expression */
			Flag_var = 2;                      /* ���I�@��expression�c�����expression�ǁK���P�S�ҕ��M��expression��ѭ�h�� */
		}
		else
		{/* ��t�f��var�ѽ������ꮅ */
			Flag_var = 0;
		    i_first = j_last = 0;
		}
		return t_head;
	}
	else if (Flag_var == 1 && i_first != j_last)
	{/* ���߀��var����Ȍ�expression��������߀δ��������һ��token,�t�^�m����ȡtoken */
		i_first++;
	}
	token = token_temp[i];                   /* ƥ������var����,ȡ��һ��token */
	return t_head;
}


static TreeNode * simple_expression(void)
{
	TreeNode * t_head = additive_expression();                    /* �ӑB�������α��_ʽ���^�Y�c */
	if (Flag_var == 2 && t_head != NULL)
	{/* ���t_head�����ɹ����Ѓɂ���expressionͨ�^���^���B�ӳ�һ�����w,���˱���ǰ�攵�M���c�������`�����������token */
		Flag_var = 0;                /* �t���˳�ǰ��var()�������{���ұ�ʾǰ���var�ѽ������ꮅ */
		return t_head;
	}
	/* �����һ���ӷ����_ʽ֮����F��'<'��'<='��'>'��'>='��'=='��'!='������֮һ,�t�f������߀�мӷ����_ʽ�c��һ���M�б��^ */
	if ((token == LT) || (token == LTEQ) || (token == RT) || (token == RTEQ) || (token == EQ) || (token == NEQ) && (t_head != NULL))
	{
		TreeNode * p = newExpNode(Operatek);              /* �ӑB�������^��̖��operater��ͱ��_ʽ�Y�c */
		if (p != NULL)
		{/* �����ɹ� */
			p->child[0] = t_head;                    /* �ѵ�һ���ӷ����_ʽ����operater�ĵ�һ������ */
			p->attribute.Operater = token;           /* �xֵ */
			t_head = p;                              /* ʹt_headָ�ָ��operater�Y�c */
			if (Flag_var == 1)
			{/* �����һ��var߀δ������,�����������token_temp���M�е�token߀δ�����ꮅ */
				i_first++;                         /* �t���{��match()����,��ֱ�ӏĔ��M��ȡ����һ��token�M����һ���ķ��� */
				token = token_temp[i_first];
			}
			else
			{
			    match(token);                            /* ƥ�䮔ǰtoken,ȡ��һ��token */
			}
			p->child[1] = additive_expression();     /* �ѵڶ����ӷ����_ʽ����operater�ĵڶ������� */
		}
	}
	return t_head;
}


static TreeNode * additive_expression(void)
{
	TreeNode * t_head = term();                     /* �ӑBterm���_ʽ���^�Y�c */
	if (Flag_var == 2 && t_head != NULL)
	{
		return t_head;                              /* �����Ȍӵ��f�w�{�� */
	}
	while ((token == PLUS) || (token == MINUS))     /* ���һ���˷����_ʽ֮����F��'+'��'-',�t�f���@���˷����_ʽ��߀�г˷����_ʽ */
	{/* �M��ѭ�h */
		TreeNode * p = newExpNode(Operatek);        /* �ӑB�����Ӝp��̖��operater��ͱ��_ʽ�Y�c */
		if (p != NULL)
		{/* �����ɹ� */
			p->child[0] = t_head;                   /* ��operaterǰ��ĳ˷����_ʽ����operater�ĵ�һ������ */
			p->attribute.Operater = token;          /* �xֵ */
			t_head = p;                             /* ʹt_headָ�ָ��operater�Y�c */
			if (Flag_var == 1)
			{
				i_first++;
				token = token_temp[i_first];
			}
			else
			{
				match(token);                            /* ƥ�䮔ǰtoken,ȡ��һ��token */
			}
			p->child[1] = term();              /* ��operater����ĳ˷����_ʽ����operater�ĵڶ������� */
		}
	}
	return t_head;
}


static TreeNode * term(void)
{
	TreeNode * t_head = factor();                  /* �ӑBfactor���_ʽ���^�Y�c */
	if (Flag_var == 2 && t_head != NULL)
	{
		return t_head;                             /* �����Ȍӵ��f�w�{�� */
	}
	while ((token == TIMES) || (token == OVER))    
	{
		TreeNode * p = newExpNode(Operatek);
		if (p != NULL)
		{/* �����ɹ� */
			p->child[0] = t_head;                  /* ��operaterǰ���factor����operater�ĵ�һ������ */
			p->attribute.Operater = token;         /* �xֵ */
			t_head = p;                            /* ʹt_headָ�ָ��operater�Y�c */
			if (Flag_var == 1)
			{/* �����һ��var߀δ������,�����������token_temp���M�е�token߀δ�����ꮅ */
				i_first++;                         /* �t���{��match()����,��ֱ�ӏĔ��M��ȡ����һ��token�M����һ���ķ��� */
				token = token_temp[i_first];
			}
			else
			{
				match(token);                            /* ƥ�䮔ǰtoken,ȡ��һ��token */
			}
			p->child[1] = factor();           /* ��operater�����Ԫ������operater�ĵڶ������� */
		}
	}
	return t_head;
}

static TreeNode * factor(void)
{
	TreeNode * t_head = NULL;                       /* ��һ���Z������͵�ָ��ָ�,Ҫ������һ��token����r���܄������w�Y�c */
	if (token == NUM)
	{/* �����һ��token��NUM */
		t_head = newExpNode(Constk);                /* �t����һ��������͵ĽY�c,ʹt_headָ���� */
		if (t_head != NULL)
		{/* �����ɹ� */
			if (Flag_var == 0)
			{/* ����ǵ�һ��var֮���NUM,�tֱ�����ַ��D����int�͵Ĕ��� */
				t_head->attribute.val = atoi(tokenString);
			}
			else
			{/* ���߀�ǵ�һ��var�����е�NUM�t���ڔ��MtokenString_temp��ȡ�������Ĕ��� */
				t_head->attribute.val = atoi(tokenString_temp[i_first]);
				if (i_first == j_last)
				{/* ����ѽ���������var��Ȍ�expression������һ��token */
					token = token_temp[j_last + 2 + j];         /* �tȡvar�����һ��token */
					if (token != ASSIGN && token != SEMT)
					{/* ����@��token����'='��';' */
						Flag_var = 2;                       /* �t�����Ȍӵ�var()�����{�Ñ�ԓ�����f�w�{�� */
					}
				}
				else
				{/* ��t�t�^�m����ȡtoken */
					i_first++;
					token = token_temp[i_first];
				}
			}
		}
		if (Flag_var == 0)
		{/* �������var������,token_temp���M�е�token�ѽ�ȡ�� */
			match(NUM);                         /* ƥ��NUM,ȡ��һ��token */
		}
	}
	else if (token == LPAREN)
	{/* �����һ��token��'(' */
		if (Flag_var == 1)
		{
			i_first++;
			token = token_temp[i_first];
		}
		else
		{
			match(LPAREN);                       /* ƥ��'('token */
		}
		t_head = expression();               /* t_headָ��һ��expression */
		if (Flag_var == 0)
		{
			match(RPAREN);                       /* ƥ��')'token */
		}
		if (i_first == j_last)
		{
			token = token_temp[j_last + 2 + j];
		}
	}
	else if (token == ID && Flag == 0)
	{/* �����һ��token��ID,�tҪ��ID�����token�M���Д� */
		if (token_temp[1] == LPAREN)
		{/* ���ID�����token��'(',�t�f���@��factor��һ�������{��: ID(args) */
			t_head = call();
		}
		else
		{/* �������'(',�t�f����һ��׃��var */
			t_head = var();
		}
		Flag = 1;                            /* �f��֮ǰ�ѽ��{����var()������߀�]���{��expression()���� */
	}
	else if (token == ID && Flag == 1)
	{/* Flag==1������֮ǰvar()�����ăȌ�var()�����{�� */
		t_head = newExpNode(Identifyk);
		if (t_head != NULL)
		{
			if (Flag_var == 0)
			{
				t_head->attribute.name = copyString(tokenString);
			}
			else
			{
				t_head->attribute.name = tokenString_temp[i_first];
			}
		}
		if (Flag_var == 1)
		{
			if (i_first < j_last)
			{
				i_first++;
				token = token_temp[i_first];
			}
			else
			{
				token = token_temp[j_last + 2 + j];
			}
		}
		else
		{
			match(ID);
		}
	}
	return t_head;
}


static TreeNode * call(void)
{
	TreeNode * t_head = newExpNode(Identifyk);                /* �ӑB����һ��operater��ͱ��_ʽ�Y�c */
	if (t_head != NULL)
	{/* �����ɹ� */
		t_head->attribute.name = tokenString_temp[0];                   /* ��ID��stringֵ�x�o�@��operater�Y�c��operater���� */
	}
	token = token_temp[1];                                    /* ȡID�����һ��token */
	if (Flag_var == 0)
	{
		match(LPAREN);                                            /* ƥ��'('token,ȡ��һ��token */
	}
	else
	{
		i_first++;
		token = token_temp[i_first];
	}
	if (token == RPAREN)
	{
		if (Flag_var == 1)
		{
			if (i_first == j_last)
			{
				token = token_temp[j_last + 2 + j];
			}
			else
			{
				i_first++;
				token = token_temp[i_first];
			}
		}
		else
		{
			match(RPAREN);
		}
		return t_head;
	}
	else
	{
		if (t_head != NULL)
		{/* �����ɹ� */
			TreeNode * p = NULL;
			p = newExpNode(ActualPK);
			t_head->sibling = p;
			p->child[0] = arg_list();                            /* �����{�õČ��������^�Y�c�ĵ�һ������,���Ќ��������ֵ��P�S */
		}
	}
	if (Flag_var == 1)
	{
		if (i_first == j_last)
		{
			token = token_temp[j_last + 2 + j];
		}
		else
		{
			i_first++;
			token = token_temp[i_first];
		}
	}
	else
	{
		match(RPAREN);                                            /* ƥ��')'token,ȡ��һ��token */
	}
	return t_head;
}

static TreeNode * arg_list(void)
{
	TreeNode * t_head = expression();                         /* �ӑB����һ��expression�Y�cָ� */
	TreeNode * p = t_head;                                    /* ���ot_head�Ҵ������B��������ֵ�expression */
	while(token == COMMA)                                     /* ���expression֮���token��',',�t�M������expression��ѭ�h�B�� */
	{
		TreeNode * q = NULL;
		if (Flag_var == 0)
		{
			match(COMMA);                                         /* ƥ��','token,ȡ��һ��token */
		}
		else
		{
			i_first++;
			token = token_temp[i_first];
		}
		q = expression();                          /* ��һ��expression */
		if (q != NULL)
		{/* �����ɹ� */
			if (t_head == NULL)
			{/* ���ǰ���expression����ʧ�� */
				t_head = p = q;              /* �t���@�������ɹ��Ĵ�����ǰ�愓��ʧ���� */
			}
			else
			{
				p->sibling = q;             /* ��t�Ͱ��@�������ɹ�������ǰ�愓���ɹ���expression���ֵ� */
				p = q;                      /* pָ������Ƅ�һλ */
			}
		}
	}
	return t_head;
}


/************************************************************************/
/*                   the primary function of the parser                 */
/************************************************************************/ 
/* Function parser returns the newly
 * constructed syntax tree
 */
TreeNode * parse(void)
{
	TreeNode * t_head;                  /* Դ���a�Z������^ָ� */
	token = getToken();                 /* ȡ����һ��token */
	t_head = declaration_list();        /* �M��Դ���a�Z��������^�� */
	if (token != ENDFILE)
	{/* Դ���a�����ꮅ��߀�]�е��ļ��Y��λ��,�t�f��Դ���a��ǰ�Y����,��������;���F�˲������ϵ��e�` */
		syntaxErrorShow("Code ends before file\n");
	}
	return t_head;                      /* ���������Z������^ָ� */
}