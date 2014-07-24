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
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));   /* �ӑB����һ���Z����,������g+��ʼ�� */
	int k;
	if(t == NULL)
	{/* �@һ���ڃȴ�ӑB�����ϳ����e�` */
		fprintf(listing, "Out of memory error at line %d\n",lineno);
	}
	else
	{
		for (k = 0; k < MAXCHILDREN; k++)
		{
			t->child[k] = NULL;   /* �Z�������к��ӽY�c��ָ��� */
		}
		t->sibling = NULL;  /* �ֵܽY�cҲ��ʼ����� */
		t->nodekind = StmtK;    /* �Y�c��͞��Z����� */
		t->kind.stmt = kind;    /* ��ʼ���Z����� */
		t->lineno = lineno;     /* ��ʼ��Դ���a�Д� */
	}
	return t;       /* �����½������@���Z����ͽY�c */
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
		t->type = Void;         /* ��ʼ�����_ʽ�ķ���ֵ��� */
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
		t->type = Void;             /* ��ʼ��������� */
	}
	return t;
}


/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString( char * s)
{/* �}�u��ǰ��tokenString,�Է�ֹ�䱻�����tokenString���w */
	int n;
	char * t;
	if (s == NULL)
	{/* Դ�ַ�����Մt���ؿ�ֵ,�������˽Y�� */
		return NULL;
	}
	n = strlen(s) + 1;      /* �õ�Ŀ���ַ����Ĵ�С */
	t = malloc(n);          /* ��Ŀ���ַ����ӑB�������g */
	if (t == NULL)
	{/* �ӑB�������gʧ�� */
		fprintf(listing, "Out of memory error at line %d\n",lineno);
	}
	else
	{
		strcpy(t,s);     /* �ɹ��xֵԴ�ַ��� */
	}
	return t;      /* ����Ŀ���ַ��� */
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
		fprintf(listing, " ");   /* �������H��r�ո�,�����Ǡ���ÿһ�Ӻ��ӵĘ���ݔ�� */
	}
}

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void printTree( TreeNode * tree)
{
	int k;
	INDENT;    /* ��ʼݔ���Ŀո� */
	while (tree != NULL)
	{
		printSpaces();    /* ݔ���Z������^ */
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
			case Expressionk:         /* ���_ʽ�Z��,����xֵ�ͺ����{�� */
				fprintf(listing,"Expression\n");
				break;
			case Compoundk:          /* �}���Z��,��춾ֲ����� */
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
			{/* �Д��@���΂��ַ����ַ�����ʽ�ı��_ʽ�ľ��w��� */
			case Operatek:            /* ���g������,����������Token */
				fprintf(listing,"Op: ");
				printToken(tree->attribute.Operater,"\0");
				break;
			case Constk:              /* ������� */
				fprintf(listing,"Const: %d\n",tree->attribute.val);
				break;
			case Identifyk:           /* ���I����� */
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
		{/* ����Y�c���ʹ����� */
			switch(tree->kind.decl)
			{
			case Functionk:       /* ������ */
				if (tree->attribute.name != NULL)
				{
					fprintf(listing,"Function declaration: (ReturnType)%s\n",tree->attribute.name);    /* ���м�ݔ�������ķ���ֵ��ֵ��� */
				}
				else
				{
					fprintf(listing,"Function declaration: Compound-stmt\n");
				}
				break;
			case Variablek:       /* ׃���� */
				fprintf(listing,"Variable declaration: (VarType)%s\n",tree->attribute.name);
				break;
			default:             /* �����R�e���e�`��� */
				fprintf(listing,"Unknown ExpNode kind\n");
				break;
			}
		}
		else   /* �e�`�Y�c */
		{
			fprintf(listing,"Unknown node kind\n");
		}
		for (k = 0; k < MAXCHILDREN; k++)
		{
			printTree(tree->child[k]);           /* ���Z����ĺ�����Ϣ��ݔ�� */
		}
		tree = tree->sibling;       /* �Z�����^ָ�ָ��ǰ�Z������ֵܘ� */
	}
	UNINDENT;   /* �֏͵�֮ǰݔ���Ŀո���ʽ */
}