#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token,token_temp[MAXTOKENTEMP];  /* holds current token,前看時用以暫存前面若干token的數組 */
static char * tokenString_temp[MAXTOKENTEMP];     /* 在前看時暫存前面token的tokenString */
int i = 0,j = 0;         /* i用於前面聲明的兩個暫存數組的下標,初始化為0,是專門用在expression()函數上的 */
/* j用於expression()函數中計算'[]'嵌套個數,因為var可能會有多級的'[]'嵌套 */
int i_first = 0,j_last = 0;                        
/* i_first為掃描var的數組時的最內層的expression的第一個token的下標*/
/* j_last為掃描var的數組時的最內層的expression的最後一個token的下標 */
int Flag = 0,Flag_var = 0,Flag_FOrD = 0;           /* Flag用來判斷var()函數是否被調用的標誌 */
/* Flag_var用來判斷var中最內層expression是否掃描到了最後一個token,以確定是取下一個token還是取token_temp中的最後一個token */


/* function prototypes for recursive calls */
/* 所有函數原型的遞歸調用聲明 */
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
{/* 源代碼分析錯誤信息提示 */
	fprintf(listing,"\n>>> ");
	fprintf(listing,"Syntax error at line %d: %s",lineno,error_message);
	Error = TRUE;
}

static void match(TokenType expectedToken)
{/* 匹配當前Token以及取到下一個Token */
	if (token == expectedToken)  /* 匹配當前Token */
	{
		token = getToken();    /* 取下一個Token */
	}
	else
	{/* 錯誤: 出現了意料之外的Token */
		syntaxErrorShow("unexpected token -> ");
		printToken(token,tokenString);
		fprintf(listing,"      ");
	}
}


static TreeNode * declaration_list(void)
{
	TreeNode * t_head = declaration();   /* 源程序的第一個聲明由語法樹的頭結點指針指向 */
	TreeNode * p = t_head;               /* p指針也指向語法樹的頭結點,以代替t_head指針向後移動得到更多兄弟結點 */
	while(token != ENDFILE)              /* C-程序由聲明列表組長,只要沒有到文件結束就說明還可能有聲明存在 */
	{
		TreeNode * q = declaration();    /* q指針指向下一個聲明 */
		if (q != NULL)                   /* 這個聲明存在 */
		{
			if (t_head == NULL)          /* 如果頭一個聲明不存在 */
			{
				t_head = p = q;          /* 則將q指向的聲明作為頭聲明 */
			}
			else                         /* 頭一個聲明存在 */
			{
				p->sibling = q;          /* 則q指向的聲明作為頭聲明的兄弟結點,也即是下一個聲明 */
				p = q;                   /* p指針向後移動一位,以連接後面更多的聲明作為其兄弟 */
			}
		}
	}
	return t_head;                      /* 返回此源代碼分析而得的語法樹的頭指針 */
}


static TreeNode * declaration(void)     /* 聲明函數,判斷到底是變量聲明還是函數聲明 */
{	                                    /* 從前兩個token還無法判斷出事變量聲明還是函數聲明,所以以此來暫存第三個token */
	TreeNode * t_head = NULL;
	if (token == LBRACE)                 /* 如果第一個token是'{',則說明是函數聲明 */
	{
		t_head = fun_declaration();     /* 進入函數聲明函數遞歸調用 */
		return t_head;                  /* 如果已經判斷是函數聲明則調用fun_declaration()函數 */
	}
	token_temp[0] = token;              /* 保存首個token及其tokenString */
	tokenString_temp[0] = copyString(tokenString);
	token_temp[1] = getToken();         /* 往前取第二個token */
	tokenString_temp[1] = copyString(tokenString);
	token_temp[2] = getToken();         /* 往前取第三個token */
	tokenString_temp[2] = copyString(tokenString);
	switch(token_temp[2])
	{
	case LPAREN:                       /* 對三個token進行判斷,如果是'(',則是函數聲明 */
		t_head = fun_declaration();
		break;
	case LSQUARE:                        /* 如果是'['或者';',則是變量聲明 */
	case SEMT:
		t_head = var_declaration();
		break;
	default:
		syntaxErrorShow("unexpected token -> ");         /* 輸出這個意料之外的錯誤token信息 */
		printToken(token_temp[2],tokenString_temp[2]);
		token = getToken();          /* 取下一個token賦值給token變量 */
		break;
	}
	return t_head;
}


static TreeNode * var_declaration(void)
{
	TreeNode * t_head = newDeclNode(Variablek);          /* 源程序語法樹中的一個聲明類型結點 */
	if (t_head != NULL)                                  /* 動態創建變量聲明類型結點成功 */
	{
		if (tokenString_temp[0] != 0 && Flag_FOrD == 0)
		{/* 如果這個變量聲明是獨立的一個變量聲明,是在複合語句類型函數聲明之外的變量聲明且在此之前token_temp存儲過非聲明的token */
			switch(token)                                    /* 對當前token進行選擇判斷以確定此變量聲明為INT類型還是VOID類型 */
			{
			case INT:                                        /* 變量聲明為INT類型 */
				t_head->attribute.name = copyString(tokenString_temp[0]);            /* 則將其Operater屬性賦值為INT */
				break;
			case VOID:
				t_head->attribute.name = copyString(tokenString_temp[0]);
				break;
			default:                                         /* 出現了意料之外的token */
				syntaxErrorShow("unexpected token -> ");
				printToken(token,tokenString_temp[0]);
				token = token_temp[1];
				break;
			}
			token = token_temp[1];                           /* 取下一個token */
			if (token == ID)                                 /* 判斷此token是否為符合條件的ID */
			{
				TreeNode * p = newExpNode(Identifyk);        /* 是ID,ID屬於變量表達式,動態創建一個ID表達式結點 */
				if (p != NULL)
				{/* 創建成功 */
					t_head->child[0] = p;                    /* 將這個ID結點作為其第一個孩子 */
					p->attribute.name = tokenString_temp[1];  /* 將這個ID的值賦給ID結點的name屬性 */
				}
			}
			else
			{/* 不是ID,則是意料之外的token,輸出錯誤信息,并取下一個token */
				syntaxErrorShow("unexpected token -> ");
				printToken(token,tokenString_temp[1]);
			}
		    token = token_temp[2];     /* 取下一個token */
		}
		else
		{/* 如果這個變量聲明是在一個複合語句類型的函數聲明當中 */
			switch(token)                                    /* 對當前token進行選擇判斷以確定此變量聲明為INT類型還是VOID類型 */
			{
			case INT:                                        /* 變量聲明為INT類型 */
				t_head->attribute.name = copyString(tokenString);            /* 則將其Operater屬性賦值為INT */
				break;
			case VOID:
				t_head->attribute.name = copyString(tokenString);
				break;
			default:                                         /* 出現了意料之外的token */
				syntaxErrorShow("unexpected token -> ");
				printToken(token,tokenString);
				token = getToken();
				break;
			}
			match(token);                                    /* 匹配當前token,取下一個token */
			if (token == ID)
			{
				TreeNode * p = newExpNode(Identifyk);        /* 是ID,ID屬於變量表達式,動態創建一個ID表達式結點 */
				if (p != NULL)
				{/* 創建成功 */
					t_head->child[0] = p;                    /* 將這個ID結點作為其第一個孩子 */
					p->attribute.name = copyString(tokenString);  /* 將這個ID的值賦給ID結點的name屬性 */
				}
			}
			match(ID);                                       /* 匹配ID,取下一個token */
		}
		if (token == LSQUARE)
		{/* 對變量聲明中可能有的'[NUM]'進行判斷 */
			match(LSQUARE);                             /* 匹配'['token,取下一個token */
			if (token == NUM)
			{
				TreeNode * q = newExpNode(Constk);      /* NUM屬於常量表達式,動態創建一個NUM表達式結點 */
				if (q != NULL)
				{/* 創建成功 */
					t_head->child[1] = q;               /* 將這個NUM結點作為其第二個孩子 */
				    q->attribute.val = atoi(tokenString);  /* 將這個NUM的值賦給NUM結點的val屬性 */
				}
			}
			match(NUM);           /* 匹配NUM,取下一個token */
			match(RSQUARE);       /* 匹配']'token,取下一個token */
		}
		match(SEMT);      /* 匹配token';',取下一個token,代表變量聲明的結點建立、賦值已經完成 */
	}
	memset(tokenString_temp,0,sizeof(tokenString_temp));         /* 重新初始化tokenString_temp數組 */
	return t_head;
}


static TreeNode * fun_declaration(void)
{
	TreeNode * t_head = newDeclNode(Functionk);        /* 動態創建一個源代碼語法樹中的函數聲明結點 */
	if (t_head != NULL)                                /* 如果此結點創建成功 */
	{
		switch(token)                                  /* 對函數聲明的第一個token進行選擇 */
		{
		case INT:                                      /* 此函數聲明的返回值為INT */
			t_head->attribute.name = copyString(tokenString_temp[0]);
			break;
		case VOID:                                     /* 此函數聲明的返回值為VOID */
			t_head->attribute.name = copyString(tokenString_temp[0]);
			break;
		case LBRACE:                                    /* 此函數聲明是個複合語句 */
			t_head = compound_stmt();
			return t_head;                             /* 如果是複合語句則不需要在往下進行分享,而是直接調用compound_stmt()函數 */
			break;
		default:                                       /* 不屬於函數聲明預料的token之列,輸出錯誤信息 */
			syntaxErrorShow("unexpected token -> ");
			printToken(token,tokenString);
			token = token_temp[1];                     /*  取下一個token*/
			break;
		}
		token = token_temp[1];                         /* 取下一個token */
		if (token == ID)
		{/* 匹配第二個token是否為ID */
			TreeNode * p = newExpNode(Identifyk);       /* 是ID,ID屬於變量表達式,動態創建一個ID表達式結點 */
			if (p != NULL)
			{/* 創建成功 */
				t_head->child[0] = p;                   /* 將這個ID表達式結點作為其第一個孩子 */
			    p->attribute.name = tokenString_temp[1];   /* 將ID的值賦給這個ID結點的name屬性 */
			}
		}
		else
		{/* 此token不是意料之中的ID,輸出錯誤信息 */
			syntaxErrorShow("unexpected token -> ");
			printToken(token,tokenString_temp[1]);
		}
		token = token_temp[2];                         /* 取下一個token */
		if (token == LPAREN)
		{/* 匹配第三個token是否為'(' */
			token = getToken();
			t_head->child[1] = params();              /* 匹配成功,調用函數聲明的參數文法分析函數 */
		}
		else
		{/* 此token不是意料之中的'(',輸出錯誤信息 */
			syntaxErrorShow("unexpected token -> ");
			printToken(token,tokenString_temp[2]);
		}
		match(RPAREN);                                /* 匹配函數聲明中的最後一個')',并取下一個token,代表這個函數聲明分析完畢 */
	}
    memset(tokenString_temp,0,sizeof(tokenString_temp));     /* 重新初始化tokenString_temp數組 */
	return t_head;
}


static TreeNode * params(void)
{
	TreeNode * t_head = NULL;                        /* 聲明參數結點,暫時為空 */
	TreeNode * p = NULL;
	switch(token)                                    /* 對params中的第一個token進行選擇 */
	{
	case INT:                                        /* 函數參數不為空,調用參數列表函數 */
		t_head = param_list();
		break;
	case VOID:
		p = newExpNode(ParamK);         /* 函數參數為空,動態創建一個operater類型的表達式 */
		if (p != NULL)
		{/* 創建成功 */
			t_head = p;
		    t_head->attribute.name = copyString(tokenString);           /* 賦值 */
		}
		match(VOID);
		break;
	default:                                         /* 出現了意料之外的token */
		syntaxErrorShow("unexpected token -> ");
		printToken(token,tokenString);
		token = getToken();                          /* 取下一個token */
		break;
	}
	return t_head;
}

static TreeNode * param_list(void)
{/* 文法: 'param{,param}' */
	TreeNode * t_head = param();                     /* 參數列表的頭指針 */
	TreeNode * p = t_head;                           /* 此指針的功能是代替頭指針往後連接其他的參數結點,并保護頭指針 */
	while (token == COMMA)                           /* 如果此token為',',則說明後面還有參數 */
	{
		TreeNode * q = NULL;
		match(COMMA);                                /* 匹配','token,取下一個token */
		q = param();                      /* 下一個參數結點指針 */
		if (q != NULL)
		{/* 完整建立起參數列表語法樹 */
			if (t_head == NULL)
			{
				t_head = p = q;
			}
			else
			{
				p->sibling = q;                      /* 參數結點彼此互為兄弟 */
				p = q;
			}
		}
	}
	return t_head;
}


static TreeNode * param(void)
{
	TreeNode * t_head = newExpNode(ParamK);       /* 作為函數聲明結點第二個孩子的參數結點 */
	if (t_head != NULL)                             /* 動態創建參數結點成功 */
	{
		if (token == INT)                           /* 參數返回值為INT */
		{
			t_head->attribute.name= copyString(tokenString);
		}
		match(INT);                                /* 匹配INT,取下一個token */ 
		if (token == ID)                           /* INT後面的token為ID*/
		{
			TreeNode * p = NULL;
			p = newExpNode(Identifyk);      /* 動態創建一個id類型的表達式結點 */
			if (p != NULL)                             /* 創建成功 */
			{/* 創建成功 */
				t_head->child[0] = p;                  /* ID結點作為參數結點的第一個孩子 */
			    p->attribute.name = copyString(tokenString);
			}
		}
		match(ID);                                 /* 匹配ID,取下一個結點 */
		if (token == LSQUARE)                      /* 如果ID後面的token為'[',則說明此參數為數組參數 */
		{                                          /* 將'['、']'作為參數結點的第二個和第三個孩子結點 */
			TreeNode * q = NULL;
			TreeNode * t = NULL;
			q = newExpNode(Operatek);   /* '['、']'均屬於operater表達式 */
			if (q != NULL)
			{/* 創建成功 */
				t_head->child[1] = q;
				q->attribute.Operater = LSQUARE;
			}
			match(LSQUARE);
			t = newExpNode(Operatek);
			if (t != NULL && token == RSQUARE)
			{/* 創建成功 */
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
	TreeNode * t_head = newDeclNode(Functionk);        /* 創建複合語句型的函數聲明的頭結點指針 */
	if (t_head != NULL)
	{
		t_head->attribute.name = NULL;
	}
	match(LBRACE);                                     /* 匹配'{'token,取下一個token */
	if (t_head != NULL)
	{/* 創建成功 */
		t_head->child[0] = local_declaration();            /* 調用函數local_declaration() */
	    t_head->child[1] = statement_list();               /* 調用函數statement_list() */
	}
	match(RBRACE);                                     /* 匹配'}'token,取下一個token */
	return t_head;
}

static TreeNode * local_declaration(void)
{
	TreeNode * t_head = NULL;                          /* 一開始的本地聲明第一個結點為空 */
	TreeNode * p;                                      /* 保護頭結點并向後移動以連接起其他的兄弟結點 */
	while((token == INT) || (token == VOID))           /* 第一個token為INT或者VOID,說明一定是個變量聲明 */
	{
		TreeNode * q;
		q = var_declaration();                         /* 調用變量聲明函數生成相應結點 */
		if (q != NULL)                                 /* 結點創建成功 */
		{
			if (t_head == NULL)                        /* 如果第一個結點為空 */
			{
				t_head = p = q;                        /* 則把這個不為空的結點作為本地一連串聲明第一個結點 */
			}
			else
			{
				p->sibling = q;                        /* 如果第一個結點已經不為空,則把這個也不為空的結點作為他的兄弟 */
				p = q;                                 /* 使本來指向當前結點的指針p向後移動一位 */
			}
		}
	}
	return t_head;
}


static TreeNode * statement_list(void)
{
	TreeNode * t_head = NULL;                          /* 一開始的語句列表的第一個結點為空 */
	TreeNode * p;                                      /* 保護頭結點并向後移動以連接起其他的兄弟結點 */
	while((token == IF) || (token == WHILE) || (token == RETURN) || (token == ID) || (token == NUM) || (token == LPAREN) || (token == LBRACE))           
	{ /* 第一個token為IF或者HWILE或者RETURN或者ID或者NUM或者'('或者'{',說明一定是語句 */
		TreeNode * q;
		q = statement();                               /* 調用語句函數生成相應結點 */
		if (q != NULL)                                 /* 結點創建成功 */
		{
			if (t_head == NULL)                        /* 如果第一個結點為空 */
			{
				t_head = p = q;                        /* 則把這個不為空的結點作為本地一連串語句第一個結點 */
			}
			else
			{
				p->sibling = q;                        /* 如果第一個結點已經不為空,則把這個也不為空的結點作為他的兄弟 */
				p = q;                                 /* 使本來指向當前結點的指針p向後移動一位 */
			}
		}
	}
	return t_head;
}


static TreeNode * statement(void)
{
	TreeNode * t_head = NULL;
	switch(token)                 /* 對語句的第一個token進行選擇以判斷是哪一類型的語句 */
	{
	case IF:
		t_head = selection_stmt();      /* 是if選擇語句 */
		break;
	case WHILE:
		t_head = iteration_stmt();      /* 是while循環語句 */
		break;
	case RETURN:                        /* 是return返回語句 */
		t_head = return_stmt();
		break;
	case ID:
	case NUM:
	case LPAREN: 
		t_head = expression_stmt();     /* 是表達式語句 */
		break;
	case LBRACE:
		t_head = compound_stmt();       /* 是複合語句 */
		break;
	default:                            /* 出現了意料之外的token */
		syntaxErrorShow("unexpected token -> ");
		printToken(token,tokenString);
		token = getToken();
		break;
	}
	return t_head;
}


static TreeNode * expression_stmt(void)
{
	TreeNode * t_head = newStmtNode(Expressionk);               /* 動態創建一個表達式語句結點 */
	if ((token == ID) || (token == NUM) || (token == LPAREN) && (t_head != NULL))   /* 當第一個token為ID或者NUM或者'(',則說明此語句為表達式語句 */
	{
		t_head->child[0] = expression();
	}
	match(SEMT);                                                /* 匹配';'token,取下一個token */
	return t_head;
}


static TreeNode * selection_stmt(void)
{
	TreeNode * t_head = newStmtNode(IfK);                      /* 動態創建一個if語句結點 */
	match(IF);                                                 /* 匹配if,取下一個token */
	match(LPAREN);                                             /* 匹配'('token,取下一個token */
	if (t_head != NULL)                                        /* if語句結點創建成功 */
	{
		t_head->child[0] = expression();                       /* if條件判斷表達式結點作為其第一個孩子 */
	}
	match(RPAREN);                                             /* 匹配')'token,取下一個token */
	if (t_head != NULL)
	{
		t_head->child[1] = statement();                        /* 條件為真時的語句結點作為其第二個孩子 */
	}
	if (token == ELSE)                                         /* 如果這個if後面跟有else */
	{
		match(ELSE);                                           /* 匹配else,取下一個token */
		if (t_head != NULL)
		{
			t_head->child[2] = statement();                    /* 條件為假時的語句結點作為其第三個孩子 */
		}
	}
	return t_head;
}


static TreeNode * iteration_stmt(void)
{
	TreeNode * t_head = newStmtNode(WhileK);                  /* 動態創建一個while語句結點 */
	match(WHILE);                                             /* 匹配while,取下一個token */
	match(LPAREN);                                            /* 匹配'('token,取下一個token */
	if (t_head != NULL)                                       /* 如果while語句結點創建成功 */
	{
		t_head->child[0] = expression();                      /* 將循環判斷條件表達式結點作為其第一個孩子 */
	}
	match(RPAREN);                                            /* 匹配')'token,取下一個token */
	if (t_head != NULL)
	{
		t_head->child[1] = statement();                       /* 將循環語句結點作為其第二個孩子 */
	}
	return t_head;
}


static TreeNode * return_stmt(void)
{
	TreeNode * t_head = newStmtNode(Returnk);                /* 動態創建一個return語句結點 */
	match(RETURN);                                           /* 匹配return,取下一個token */
	if ((token == ID) || (token == NUM) || (token == LPAREN) && (t_head != NULL))  /* 如果有expression且return語句結點創建成功 */
	{
		t_head->child[0] = expression();                     /* 將此返回的表達式結點作為其第一個孩子 */
	}
	match(SEMT);                                             /* 匹配';'token,取下一個token */
	return t_head; 
}


static TreeNode * expression(void)
{
	TreeNode * t_head = NULL;                       /* 表達式頭指針指空,有待於下面的判斷 */
	Flag = 0;                                       /* 每次調用expression()函數都初始化Flag,表示又調用了此函數 */
	if (token == ID)                         
	{/* 如果第一個token為ID則要一直往後掃描并保存所掃描的每一個token,直到var掃描完畢,看var後面有沒有賦值號'=' */
		/* 如果有賦值號'=',則說明這個表達式是var = expression,否則就是simple-expression */
		/* 如果第一個token就不是ID則這個表達式就是simple-expression */
		Flag_FOrD = 1;                              /* 表示token_temp數組存儲了非聲明的token */
		i = 0;                                      /* 每次調用expression()函數都初始化下標i,意味著token_temp數組總是從頭開始暫存 */
		if (i_first != 0 && j_last != 0)
		{/* 如果這不是第一次出現expression中開始便是var的情況,即出現了expression嵌套的情況 */
			token_temp[i] = token_temp[i_first];
			tokenString_temp[i] = tokenString_temp[i_first];
			if (i_first < j_last)
			{/* 這個expression前面的expression不止一個token,這個ID還不是最內層expression的最後一個token */
				i++;/* 取出第二個token */
				token_temp[i] = token_temp[i_first + 1];
				tokenString_temp[i] = tokenString_temp[i_first + 1];
			    token = token_temp[i];
			}
		}
		else
		{/* 這是第一次出現expression中開始便是var的情況,即是開始進入var最內層expression的情況 */
			token_temp[i] = token;                     /* 保存第一個IDtoken */
			tokenString_temp[i] = copyString(tokenString);    /* 保存這個token的tokenString */
			i++;                                       /* 取第二個token,看其是否為'[' */
			token_temp[i] = getToken();
			tokenString_temp[i] = copyString(tokenString);
			token = token_temp[i];
			if (token == LSQUARE)/* 如果第二個token確實為'[',則說明var是個數組結構,再去看這個var是否有數組嵌套,否則說明var=ID */
			{
				int k;
				j = 0;
				while (token != RSQUARE)             /* 判斷這個token是否為']',如果不是則繼續掃描,如果是則停止掃描 */
				{/* 繼續循環掃描判斷 */
					i++;
					token_temp[i] = getToken();
					tokenString_temp[i] = copyString(tokenString);
					token = token_temp[i];
					if (token == RSQUARE)
					{
						j_last = i - 1;              /* 保存var最內層expression的最後一個token的下標,即是在第一個']'前的token下標 */
					}
				}
				for (k = 2; k < i; k++)
				{/* 如果var不僅僅是一個ID,則在第一個']'token和開始的'['token之間往前掃描判斷 */
					if (token_temp[k] == LSQUARE)       /* 如果出現'['token,則說明嵌套了一個'[]' */
					{
						j++;                            /* 嵌套的個數 */
					}
				}
				for (k = 0; k < j; k++)
				{/* 根據'[]'的嵌套數,對第一個']'後面的每一個']'掃描并判斷是否出錯 */
					i++;
					token_temp[i] = getToken();
					tokenString_temp[i] = copyString(tokenString);
					token = token_temp[i];
					if (token != RSQUARE)            /* 不是']',出現了意料之外的token */
					{
						syntaxErrorShow("unexpected token -> ");
						printToken(token,tokenString_temp[i]);
					}
				}
				/* 掃描完var所有的字符,再往後讀取一個token,判斷其是否為'=',從而來判斷這個expression是何種類型 */
				i++;
				token_temp[i] = getToken();
				tokenString_temp[i] = copyString(tokenString);
				token = token_temp[i];
			}
		}
		if (token == ASSIGN)
		{/* var之後的第一個token是賦值號'=',var: ID['['expression']'] */
			TreeNode * q = newExpNode(Operatek);         /* 動態創建一個operater類型的表達式結點 */
			if (q != NULL)
			{/* 創建成功 */
				t_head = q;                              /* 則這個表達式結點就是這個operater類型的 */
				t_head->attribute.Operater = token;      
				t_head->child[0] = var();                /* 它的第一個孩子是賦值號'='前面的變量 */
				if (Flag_var == 0)
				{/* 如果之前還未調用過var()函數,即是token_temp數組中還沒有存儲任何的token */
					match(ASSIGN);                      /* 取'='後面的下一個token */
				}
				else
				{/* 如果token_temp數組中還有token且則繼續往下取 */
					i_first++;
					token = token_temp[i_first];
				}
				t_head->child[1] = expression();         /* 它的第二個孩子是賦值號'='後面的表達式 */
			}
		}
		else
		{/* 如果var之後的第一個token不是賦值號'=' */
			token = token_temp[0];                       /* 把token恢復為expression的第一個token */
			t_head = simple_expression();                /* 則這個表達式為簡單表達式 */
		}
	}
	else
	{/* 如果第一個token不是ID */
		t_head = simple_expression();                   /* 則這個表達式為簡單表達式 */
	}
	return t_head;
}


static TreeNode * var(void)
{
	TreeNode * t_head = newExpNode(Identifyk);          /* 動態創建一個ID類型表達式結點 */
	TreeNode * p = t_head;                              /* 保存頭結點指針并代替它連接下面可能有的一系列孩子結點 */
	if (t_head != NULL)                               
	{/* 創建成功 */
		t_head->attribute.name = tokenString_temp[0];       /* 賦值 */
	}
	if (i > 1 && t_head != NULL)                    /* 如果token_temp[]數組中保存的token數大於2,則說明var一定有數組結構 */
	{
		TreeNode * p_index = NULL;                  /* 每一個數組的下標指針 */
		TreeNode * q = NULL;
		TreeNode * t = NULL;
		int k;
		for (k = 1; k < (j + 1) * 2; k++)
		{/* 從第一個ID之後的第一個token開始判斷,直到var中的最後一個'['為止(有j個嵌套就有j+1個'[') */
			if (token_temp[k] == ID || token_temp[k] == LSQUARE)
			{/* 第一個ID之後的每一個token要麼是'[',要麼是ID */
				if (token_temp[k] == ID)
				{/* 如果是ID,則說明是個嵌套 */
					p_index = newExpNode(IndexK);
					if (p_index != NULL)
					{
						p->sibling = p_index;         /* 把這一層數組的下標指針作為它的兄弟 */
					}
					q = newExpNode(Identifyk);        /* 動態創建一個ID表達式結點來保存此嵌套數組的ID */
					if (q != NULL)
					{/* 創建成功 */
						q->attribute.name = tokenString_temp[k];    
						p_index->child[0] = q;       /* 將此結點作為var頭結點的第一個孩子,之後的每一個嵌套ID都作為它上一層的第一個孩子 */
					    p = q;                 /* p指針位置往下移動一位 */
					}
				}
			}
			else
			{/* 出現了既不是'['也不是ID的意料之外的token */
				syntaxErrorShow("unexpected token -> ");
				printToken(token_temp[k],tokenString_temp[k]);
			}
		}
		i_first = k;                         /* 保存var最內層expression的第一個token下標,即是最後一個'['後面的第一個token下標 */
		token = token_temp[k];               /* 取var最內層expression的第一個token */
		Flag_var = 1;                        /* 表示在調用expression()函數後又調用了var()函數且var的形式為:ID[expression] */
		t = newExpNode(IndexK);              /* 最外層數組的下標指針 */
		if (t != NULL)
		{/* 創建成功 */
			p->sibling = t;                  
		}
		t->child[0] = expression();           /* 最後一層嵌套裏面的下標為一個expression,將其作為最後一層嵌套的第一個孩子 */
		Flag_var = 0;                         /* 表示expression中的var已經分析完畢 */
		i_first = j_last = 0;                 /* 重新初始化 */
		return t_head;                        /* 無需再繼續下去,直接返回頭指針 */
	}
	if (Flag_var == 1 && i_first == j_last)
	{/* 如果還在var的最內層expression分析中且已經到了其最後一個token */
		token = token_temp[j_last + 2 + j];    /* 則取到var後的第一個token */
		if (token != ASSIGN && token != SEMT)
		{/* 如果var後的第一個token不是'='或';',則說明後面還有進行計算或者比較的expression */
			Flag_var = 2;                      /* 標誌這個expression與後面的expression是並列關係且會進入expression的循環中 */
		}
		else
		{/* 否則說明var已經分析完畢 */
			Flag_var = 0;
		    i_first = j_last = 0;
		}
		return t_head;
	}
	else if (Flag_var == 1 && i_first != j_last)
	{/* 如果還在var的最內層expression分析中且還未到其最後一個token,則繼續往後取token */
		i_first++;
	}
	token = token_temp[i];                   /* 匹配完了var以後,取下一個token */
	return t_head;
}


static TreeNode * simple_expression(void)
{
	TreeNode * t_head = additive_expression();                    /* 動態創建簡單表達式的頭結點 */
	if (Flag_var == 2 && t_head != NULL)
	{/* 如果t_head創建成功且有兩個個expression通過比較符連接成一個整體,爲了避免前面數組下標與被後面誤會為其所需的token */
		Flag_var = 0;                /* 則先退出前面var()函數的調用且表示前面的var已經分析完畢 */
		return t_head;
	}
	/* 如果第一個加法表達式之後出現了'<'、'<='、'>'、'>='、'=='、'!='的其中之一,則說明後面還有加法表達式與第一個進行比較 */
	if ((token == LT) || (token == LTEQ) || (token == RT) || (token == RTEQ) || (token == EQ) || (token == NEQ) && (t_head != NULL))
	{
		TreeNode * p = newExpNode(Operatek);              /* 動態創建比較符號的operater類型表達式結點 */
		if (p != NULL)
		{/* 創建成功 */
			p->child[0] = t_head;                    /* 把第一個加法表達式作為operater的第一個孩子 */
			p->attribute.Operater = token;           /* 賦值 */
			t_head = p;                              /* 使t_head指針指向operater結點 */
			if (Flag_var == 1)
			{/* 如果第一個var還未分析完,即最初暫存在token_temp數組中的token還未分析完畢 */
				i_first++;                         /* 則不調用match()函數,而直接從數組中取出下一個token進行下一步的分析 */
				token = token_temp[i_first];
			}
			else
			{
			    match(token);                            /* 匹配當前token,取下一個token */
			}
			p->child[1] = additive_expression();     /* 把第二個加法表達式作為operater的第二個孩子 */
		}
	}
	return t_head;
}


static TreeNode * additive_expression(void)
{
	TreeNode * t_head = term();                     /* 動態term表達式的頭結點 */
	if (Flag_var == 2 && t_head != NULL)
	{
		return t_head;                              /* 跳出內層的遞歸調用 */
	}
	while ((token == PLUS) || (token == MINUS))     /* 如果一個乘法表達式之後出現了'+'或'-',則說明這個乘法表達式後還有乘法表達式 */
	{/* 進入循環 */
		TreeNode * p = newExpNode(Operatek);        /* 動態創建加減符號的operater類型表達式結點 */
		if (p != NULL)
		{/* 創建成功 */
			p->child[0] = t_head;                   /* 把operater前面的乘法表達式作為operater的第一個孩子 */
			p->attribute.Operater = token;          /* 賦值 */
			t_head = p;                             /* 使t_head指針指向operater結點 */
			if (Flag_var == 1)
			{
				i_first++;
				token = token_temp[i_first];
			}
			else
			{
				match(token);                            /* 匹配當前token,取下一個token */
			}
			p->child[1] = term();              /* 把operater後面的乘法表達式作為operater的第二個孩子 */
		}
	}
	return t_head;
}


static TreeNode * term(void)
{
	TreeNode * t_head = factor();                  /* 動態factor表達式的頭結點 */
	if (Flag_var == 2 && t_head != NULL)
	{
		return t_head;                             /* 跳出內層的遞歸調用 */
	}
	while ((token == TIMES) || (token == OVER))    
	{
		TreeNode * p = newExpNode(Operatek);
		if (p != NULL)
		{/* 創建成功 */
			p->child[0] = t_head;                  /* 把operater前面的factor作為operater的第一個孩子 */
			p->attribute.Operater = token;         /* 賦值 */
			t_head = p;                            /* 使t_head指針指向operater結點 */
			if (Flag_var == 1)
			{/* 如果第一個var還未分析完,即最初暫存在token_temp數組中的token還未分析完畢 */
				i_first++;                         /* 則不調用match()函數,而直接從數組中取出下一個token進行下一步的分析 */
				token = token_temp[i_first];
			}
			else
			{
				match(token);                            /* 匹配當前token,取下一個token */
			}
			p->child[1] = factor();           /* 把operater後面的元素作為operater的第二個孩子 */
		}
	}
	return t_head;
}

static TreeNode * factor(void)
{
	TreeNode * t_head = NULL;                       /* 聲明一個語法樹類型的指空指針,要根據第一個token的情況才能創建具體結點 */
	if (token == NUM)
	{/* 如果第一個token為NUM */
		t_head = newExpNode(Constk);                /* 則創建一個常量類型的結點,使t_head指向它 */
		if (t_head != NULL)
		{/* 創建成功 */
			if (Flag_var == 0)
			{/* 如果是第一個var之後的NUM,則直接由字符轉化為int型的數字 */
				t_head->attribute.val = atoi(tokenString);
			}
			else
			{/* 如果還是第一個var分析中的NUM則是在數組tokenString_temp中取到對應的數字 */
				t_head->attribute.val = atoi(tokenString_temp[i_first]);
				if (i_first == j_last)
				{/* 如果已經分析到了var最內層expression的最後一個token */
					token = token_temp[j_last + 2 + j];         /* 則取var後的下一個token */
					if (token != ASSIGN && token != SEMT)
					{/* 如果這個token不是'='和';' */
						Flag_var = 2;                       /* 則表明內層的var()函數調用應該跳出遞歸調用 */
					}
				}
				else
				{/* 否則則繼續往下取token */
					i_first++;
					token = token_temp[i_first];
				}
			}
		}
		if (Flag_var == 0)
		{/* 如果不在var分析中,token_temp數組中的token已經取完 */
			match(NUM);                         /* 匹配NUM,取下一個token */
		}
	}
	else if (token == LPAREN)
	{/* 如果第一個token為'(' */
		if (Flag_var == 1)
		{
			i_first++;
			token = token_temp[i_first];
		}
		else
		{
			match(LPAREN);                       /* 匹配'('token */
		}
		t_head = expression();               /* t_head指向一個expression */
		if (Flag_var == 0)
		{
			match(RPAREN);                       /* 匹配')'token */
		}
		if (i_first == j_last)
		{
			token = token_temp[j_last + 2 + j];
		}
	}
	else if (token == ID && Flag == 0)
	{/* 如果第一個token為ID,則要對ID後面的token進行判斷 */
		if (token_temp[1] == LPAREN)
		{/* 如果ID後面的token為'(',則說明這個factor為一個函數調用: ID(args) */
			t_head = call();
		}
		else
		{/* 如果不是'(',則說明是一個變量var */
			t_head = var();
		}
		Flag = 1;                            /* 說明之前已經調用了var()函數且還沒有調用expression()函數 */
	}
	else if (token == ID && Flag == 1)
	{/* Flag==1表明是之前var()函數的內層var()函數調用 */
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
	TreeNode * t_head = newExpNode(Identifyk);                /* 動態創建一個operater類型表達式結點 */
	if (t_head != NULL)
	{/* 創建成功 */
		t_head->attribute.name = tokenString_temp[0];                   /* 將ID的string值賦給這個operater結點的operater屬性 */
	}
	token = token_temp[1];                                    /* 取ID後的下一個token */
	if (Flag_var == 0)
	{
		match(LPAREN);                                            /* 匹配'('token,取下一個token */
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
		{/* 創建成功 */
			TreeNode * p = NULL;
			p = newExpNode(ActualPK);
			t_head->sibling = p;
			p->child[0] = arg_list();                            /* 函數調用的實參作為頭結點的第一個孩子,所有實參皆是兄弟關係 */
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
		match(RPAREN);                                            /* 匹配')'token,取下一個token */
	}
	return t_head;
}

static TreeNode * arg_list(void)
{
	TreeNode * t_head = expression();                         /* 動態創建一個expression結點指針 */
	TreeNode * p = t_head;                                    /* 保護t_head且代替它連接後面的兄弟expression */
	while(token == COMMA)                                     /* 如果expression之後的token為',',則進入後面expression的循環連接 */
	{
		TreeNode * q = NULL;
		if (Flag_var == 0)
		{
			match(COMMA);                                         /* 匹配','token,取下一個token */
		}
		else
		{
			i_first++;
			token = token_temp[i_first];
		}
		q = expression();                          /* 下一個expression */
		if (q != NULL)
		{/* 創建成功 */
			if (t_head == NULL)
			{/* 如果前面的expression創建失敗 */
				t_head = p = q;              /* 則將這個創建成功的代替它前面創建失敗的 */
			}
			else
			{
				p->sibling = q;             /* 否則就把這個創建成功的作為前面創建成功的expression的兄弟 */
				p = q;                      /* p指針向後移動一位 */
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
	TreeNode * t_head;                  /* 源代碼語法樹的頭指針 */
	token = getToken();                 /* 取出第一個token */
	t_head = declaration_list();        /* 進入源代碼語法樹分析過程 */
	if (token != ENDFILE)
	{/* 源代碼分析完畢後還沒有到文件結束位置,則說明源代碼提前結束了,可能是中途出現了不可意料的錯誤 */
		syntaxErrorShow("Code ends before file\n");
	}
	return t_head;                      /* 返回整個語法樹的頭指針 */
}