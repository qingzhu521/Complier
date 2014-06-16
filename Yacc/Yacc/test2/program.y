%{
#include <math.h>
#include<string.h>
#include<malloc.h>
#include<stdio.h>

void yyerror (char const *);
char *replace(char *source, char *sub, char *rep);
char *add(char*str1,char*str2);
int sCount = 0;
char *s[200];
char *output=" ";
int i;
//char *output="program";
static int error_count=0;


%}

%token NUM ID T_int T_if T_else T_while T_real T_then EQ LEE LAE NE 

%%
program:  compoundstmt
{
printf ("%80s", "program->compoundstmt \n");
s[sCount] = "program";
sCount++;
for(i=sCount-1;i>=0;i--)
{
	printf("%s", s[i]);
}
return 0;
}
;

stmt: ifstmt
{ printf ("%80s", "stmt->ifstmt \n"); 
s[sCount] = "->stmt:ifstmt";
sCount++;
}
|  whilestmt  
{ printf ("%80s", "stmt->whilestmt \n");
s[sCount] = "->stmt:whilestmt";
sCount++;
 }
|  assgstmt  
{ printf ("%80s", "stmt->assgstmt \n"); 
s[sCount] = "->stmt:assgstmt";
sCount++;
}
| assgstmt error
{yyerror("unexpected symbal after assingment");}
|  compoundstmt 
{ printf ("%80s", "stmt->compoundstmt \n");
s[sCount] = "->stmt:compoundstmt";
sCount++;
 }
| error
{yyerror(" invalid segment");
 }
;

compoundstmt: '{' stmts '}' 
{ printf ("%80s", "compoundstmt-> { stmts } \n"); 
s[sCount] = "->compoundstmt:{ stmts }";
sCount++; 
}
| '{'stmts error 
{yyerror(" missing right bracket");
}
| error stmts '}' 
{yyerror(" missing left bracket");
}
;

stmts: stmt stmts   
{ printf ("%80s", "stmts->stmt stmts \n"); 
 s[sCount] = "->stmts: stmt stmts"; 
sCount++;
}
|stmt error stmts
{yyerror(" unexpected symbal after a statement");
}
|   
{ printf ("%80s", "stmts-> \n"); 
s[sCount] = "->stmts:"; 
sCount++;
}
;

ifstmt: T_if '(' boolexpr ')' stmt T_then stmt T_else stmt  
{ printf ("%80s", "ifstmt->T_if ( boolexpr ) stmt T_then stmt T_else stmt \n"); 
s[sCount] = "->ifstmt:T_if ( boolexpr ) stmt T_then stmt T_else stmt";
sCount++;
}
| T_if error '(' boolexpr ')' stmt T_then stmt T_else stmt
{ yyerror (" unexpected identifier after IF");
}
;

whilestmt: T_while '(' boolexpr ')' stmt
{ printf ("%80s", "whilestmt->T_while ( boolexpr ) stmt \n"); 
s[sCount] = "->whilestmt:T_while ( boolexpr ) stmt"; 
sCount++;
}
| T_while error '(' boolexpr ')' stmt
{ yyerror (" unexpected identifier after WHILE");
}
;

assgstmt: ID '=' arithexpr  ';'
{ printf ("%80s", "assgstmt->ID = arithexpr ; \n");
s[sCount] = "->assgstmt:ID = arithexpr ";
sCount++; 
 }
| ID '=' arithexpr  
{ yyerror (" missing ';' in an assignment");
 }	
| ID '=' error arithexpr  ';'
{ yyerror (" invalid assignment");}
| ID error
{ yyerror (" unexpected identifer");}  
;

boolexpr: arithexpr '<' arithexpr 
{ printf ("%80s", "boolexpr->arithexpr < arithexpr \n");
s[sCount] = "->boolexpr:arithexpr < arithexpr"; 
sCount++;
 }
 |arithexpr '>' arithexpr
 |arithexpr LEE arithexpr
 |arithexpr LAE arithexpr
 |arithexpr EQ arithexpr
 |arithexpr NE arithexpr
 |error 
 { yyerror (" invalid bool expression");
 }
;

boolop: '<' 
{ 
s[sCount] = "->boolop:< "; 
sCount++;
output=add(a,output);
 }
| '>' 
{ 
s[sCount] = "->boolop:> ";
sCount++; 
output=add(a,output);
}
| LEE
{ 
s[sCount] = "->boolop:LEE";
sCount++; 
output=add(a,output); 
}
| LAE
{ 
s[sCount] = "->boolop:LAE ";
sCount++; 
output=add(a,output);
}
| EQ  
{ 
s[sCount] = "->boolop:EQ"; 
sCount++;
output=add(a,output);
}
;

arithexpr: multexpr arithexprprime    
{ printf ("%80s", "arithexpr->multexpr arithexprprime \n");
s[sCount] = "->arithexpr:multexpr arithexprprime"; 
sCount++;
 }
| error arithexprprime
{yyerror(" invalid arithmatic expression");}

;

arithexprprime: '+' multexpr arithexprprime  
{ printf ("%80s", "arithexprprime-> + multexpr arithexprprime \n"); 
s[sCount] = "->arithexprprime: + multexpr arithexprprime"; 
sCount++;
}
| '+' error simpleexpr multexprprime         
{ yyerror(" invalid arithmatic expression"); 
}
|  '-' multexpr arithexprprime    
{ printf ("%80s","arithexprprime-> - multexpr arithexprprime \n");
s[sCount] = "->arithexprprime:- multexpr arithexprprime"; 
sCount++;
 }
| '-' error simpleexpr multexprprime         
{ yyerror(" invalid arithmatic expression"); 
}
|  { printf ("%80s", "arithexprprime-> \n");
s[sCount] = "->arithexprprime:"; 
sCount++;
 }
;

multexpr: simpleexpr multexprprime   
{ printf ("%80s", "multexpr-> simpleexpr multexprprime \n");
s[sCount] = "->multexpr: simpleexpr multexprprime";
sCount++;  
}
|error multexprprime
{yyerror(" invalid arithmatic expression");}
;

multexprprime: '*' simpleexpr multexprprime   
{ printf ("%80s", "multexprprime-> * simpleexpr multexprprime \n"); 
s[sCount] = "->multexprprime: '*' simpleexpr multexprprime"; 
sCount++;
}
| '*' error simpleexpr multexprprime  
{ yyerror(" invalid arithmatic expression"); 
}  
|  '/' simpleexpr multexprprime     
{ printf ("%80s", "multexprprime-> / simpleexpr multexprprime \n");
s[sCount] = "->multexprprime: / simpleexpr multexprprime"; 
sCount++; 
}
| '/' error simpleexpr multexprprime         
{ yyerror(" invalid arithmatic expression"); 
}
|    
{ printf ("%80s", "multexprprime-> \n"); 
s[sCount] = "->multexprprime:"; 
sCount++;
}
;

simpleexpr: ID   
{ printf ("%80s", "simpleexpr-> ID \n"); 
s[sCount] = "->simpleexpr: ID ";
sCount++; 
}
| NUM   
{ printf ("%80s", "simpleexpr-> NUM \n");
s[sCount] = "->simpleexpr: NUM"; 
sCount++;
 }
| '(' arithexpr ')'  
{ printf ("%80s", "simpleexpr-> ( arithexpr ) \n");
s[sCount] = "->simpleexpr: ( arithexpr )"; 
sCount++;
 }
| error arithexpr ')'
{yyerror(" missing ("); }
;

%%

#include <stdio.h>
int
main (void)
{
//yydebug = 1;
return yyparse ();
}

#include <stdio.h>
/* Called by yyparse on error. */
void
yyerror (char const *s)
{
	error_count=error_count+1;
	printf("%80s","-------------E R R O R---------  ");
	printf("%40s%1d :%2s\n","error ",error_count,s);
	printf ("%s\n", s);
}

char *add(char*str1,char*str2)
{
	char*str=str1;
	while (*str)
		*str++;
	while(*str2)
		*str++=*str2++;
	*str='\0';
	return str1;
}
