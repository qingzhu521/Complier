/************************************************************************/
/* File: util.c                                                         */
/* Utility function implementation                                      */
/* for the C- compiler                                                  */
/* Compiler Construction: Principles and Practice                       */
/************************************************************************/


#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */

void printToken(TokenType token, const char* tokenString)
{
	switch(token)
	{
	case IF:
	case ELSE:
	case INT:
	case RETURN:
	case VOID:
	case WHILE:
		fprintf(listing, "reserved word: %s\n",tokenString);
		break;
	case PLUS:
		fprintf(listing, "+\n");
		break;
	case MINUS:
		fprintf(listing, "-\n");
		break;
	case TIMES:
		fprintf(listing, "*\n");
		break;
	case OVER:
		fprintf(listing, "/\n");
		break;
	case LT:
		fprintf(listing, "<\n");
		break;
	case LTEQ:
		fprintf(listing, "<=\n");
		break;
	case RT:
		fprintf(listing, ">\n");
		break;
	case RTEQ:
		fprintf(listing, ">=\n");
		break;
	case EQ:
		fprintf(listing, "==\n");
		break;
	case NEQ:
		fprintf(listing, "!=\n");
		break;
	case ASSIGN:
		fprintf(listing, "=\n");
		break;
	case SEMT:
		fprintf(listing, ";\n");
		break;
	case COMMA:
		fprintf(listing, ",\n");
		break;
	case LPAREN:
		fprintf(listing, "(\n");
		break;
	case RPAREN:
		fprintf(listing, ")\n");
		break;
	case LSQUARE:
		fprintf(listing, "[\n");
		break;
	case RSQUARE:
		fprintf(listing, "]\n");
		break;
	case LBRACE:
		fprintf(listing, "{\n");
		break;
	case RBRACE:
		fprintf(listing, "}\n");
		break;
	case LNOTE:
		fprintf(listing, "/*\n");
		break;
	case RNOTE:
		fprintf(listing, "*\\n");
		break;
	case ENDFILE:
		fprintf(listing, "EOF\n");
		break;
	case NUM:
		fprintf(listing, "NUM, val = %s\n", tokenString);
		break;
	case ID:
		fprintf(listing, "ID, name = %s\n", tokenString);
		break;
	case ERROR:
		fprintf(listing, "ERROR: %s\n", tokenString);
		break;
	default: /* should never happen */
		fprintf(listing, "Unknown token: %d\n", token);
	}
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind)
{
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));   /* 動態創建一棵語法樹,分配空間+初始化 */
	int k;
	if(t == NULL)
	{/* 這一行在內存動態分配上出了錯誤 */
		fprintf(listing, "Out of memory error at line %d\n",lineno);
	}
	else
	{
		for (k = 0; k < MAXCHILDREN; k++)
		{
			t->child[k] = NULL;   /* 語法樹所有孩子結點均指向空 */
		}
		t->sibling = NULL;  /* 兄弟結點也初始化為空 */
		t->nodekind = StmtK;    /* 結點類型為語句類型 */
		t->kind.stmt = kind;    /* 初始化語句類型 */
		t->lineno = lineno;     /* 初始化源代碼行數 */
	}
	return t;       /* 返回新建立的這個語句類型結點 */
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode * newExpNode(Expkind kind)
{
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
	int k;
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n",lineno);
	}
	else
	{
		for (k = 0; k < MAXCHILDREN; k++)
		{
			t->child[k] = NULL;
		}
		t->sibling = NULL;
		t->nodekind = ExpK;
		t->kind.exp = kind;
		t->lineno = lineno;
		t->type = Void;         /* 初始化表達式的返回值類型 */
	}
	return t;
}

/* Function newDeclNode creates a new declaration
 * node for syntax tree construction
 */

TreeNode * newDeclNode(Declkind kind)
{
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
	int k;
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n",lineno);
	}
	else
	{
		for (k = 0; k < MAXCHILDREN; k++)
		{
			t->child[k] = NULL;
		}
		t->sibling = NULL;
		t->nodekind = DeclK;
		t->kind.decl = kind;
		t->lineno = lineno;
		t->type = Void;             /* 初始化聲明的類型 */
	}
	return t;
}


/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString( char * s)
{/* 複製當前的tokenString,以防止其被後面的tokenString覆蓋 */
	int n;
	char * t;
	if (s == NULL)
	{/* 源字符串為空則返回空值,函數到此結束 */
		return NULL;
	}
	n = strlen(s) + 1;      /* 得到目標字符串的大小 */
	t = malloc(n);          /* 為目標字符串動態創建空間 */
	if (t == NULL)
	{/* 動態創建空間失敗 */
		fprintf(listing, "Out of memory error at line %d\n",lineno);
	}
	else
	{
		strcpy(t,s);     /* 成功賦值源字符串 */
	}
	return t;      /* 返回目標字符串 */
}


/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno += 2
#define UNINDENT indentno -= 2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
	int k;
	for (k = 0; k < indentno; k++)
	{
		fprintf(listing, " ");   /* 根據實際情況空格,實在是爲了每一層孩子的樹形輸出 */
	}
}

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void printTree( TreeNode * tree)
{
	int k;
	INDENT;    /* 起始輸出的空格 */
	while (tree != NULL)
	{
		printSpaces();    /* 輸出語法樹的頭 */
		if (tree->nodekind == StmtK)
		{
			switch(tree->kind.stmt)
			{
			case IfK:
				fprintf(listing,"If\n");
				break;
			case WhileK:
				fprintf(listing,"While\n");
				break;
			case Returnk:
				fprintf(listing,"Return\n");
				break;
			case Expressionk:         /* 表達式語句,用於賦值和函數調用 */
				fprintf(listing,"Expression\n");
				break;
			case Compoundk:          /* 複合語句,用於局部聲明等 */
				fprintf(listing,"Compound\n");
				break;
			default:
				fprintf(listing,"Unknown ExpNode kind\n");
				break;
			}
		}
		else if(tree->nodekind == ExpK)
		{
			switch(tree->kind.exp)
			{/* 判斷這個單個字符或字符串形式的表達式的具體類型 */
			case Operatek:            /* 算術操作符,對應相應的Token */
				fprintf(listing,"Op: ");
				printToken(tree->attribute.Operater,"\0");
				break;
			case Constk:              /* 常量類型 */
				fprintf(listing,"Const: %d\n",tree->attribute.val);
				break;
			case Identifyk:           /* 標誌符類型 */
				fprintf(listing,"Id: %s\n",tree->attribute.name);
				break;
			case ParamK:
				fprintf(listing,"ParamType: %s\n",tree->attribute.name);
				break;
			case IndexK:
				fprintf(listing,"Index: \n");
				break;
			case ActualPK:
				fprintf(listing,"Actual Param: \n");
				break;
			default:
				fprintf(listing,"Unknown ExpNode kind\n");
				break;
			}
		}
		else if (tree->nodekind == DeclK)
		{/* 如果結點類型使聲明類型 */
			switch(tree->kind.decl)
			{
			case Functionk:       /* 函數聲明 */
				if (tree->attribute.name != NULL)
				{
					fprintf(listing,"Function declaration: (ReturnType)%s\n",tree->attribute.name);    /* 首行即輸出此聲明的返回值或值類型 */
				}
				else
				{
					fprintf(listing,"Function declaration: Compound-stmt\n");
				}
				break;
			case Variablek:       /* 變量聲明 */
				fprintf(listing,"Variable declaration: (VarType)%s\n",tree->attribute.name);
				break;
			default:             /* 不能識別的錯誤類型 */
				fprintf(listing,"Unknown ExpNode kind\n");
				break;
			}
		}
		else   /* 錯誤結點 */
		{
			fprintf(listing,"Unknown node kind\n");
		}
		for (k = 0; k < MAXCHILDREN; k++)
		{
			printTree(tree->child[k]);           /* 把語法樹的孩子信息都輸出 */
		}
		tree = tree->sibling;       /* 語法樹頭指針指向前語法樹的兄弟樹 */
	}
	UNINDENT;   /* 恢復到之前輸出的空格形式 */
}