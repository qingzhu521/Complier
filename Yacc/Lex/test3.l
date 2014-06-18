%{
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "y.tab.h"
int linenum=0;
int charnum=0;
int beginLineNum=0;
int endLineNum=0;
int mul=0;
//NUM ID T_int T_if T_else T_while T_real T_then EQ LEE LAE NE 

//limin
/*
 * Word recognizer with a symbol table.
 */

enum {
	LOOKUP = 0, /* default - looking rather than defining. */
	KEYWORDS,
	DIGIT,
	REALNUMBER,
	INTNUMBER,
	EXPONENT,
	FRACTION,
	OPERATORS,
        IDENTIFIERS
};

int state; 

int add_word(int type, char *word);
int lookup_word(char *word);

%}

letter [A-Z,a-z]
digit [0-9]
intnumber {digit}+
exponent "E"("+"|"-"){digit}+
fraction \.{digit}+
realnumber {digit}+{exponent}|{digit}+{fraction}{exponent}
delimiters [(){};]
operators  "+"|"-"|"*"|"/"|"="|">"|"<"
identifiers {letter}({letter}|{digit})*
keywords "int"|"if"|"else"|"while"|"real"|"then"
error_id ({digit})+({letter})+
error_num ({digit})+\.({identifiers}|{error_id})
whitespace [ ]+
enter [\n]
illegal_sign "~"|"@"|"$"|"%"|"^"|"&"|"#"|"?"|"\\"|"\""|":"|"'"|"¡°"|"¡°"|"¡£"|"¡¶"|"¡·"|"¡­¡­"|"£»"
comment  "//"({letter}|{digit}|{operators}|{whitespace}|{delimiters}|{illegal_sign}|\.)*{enter}
mulcomment "/*"({letter}|{digit}|{operators}|{whitespace}|{delimiters}|{illegal_sign}|\.|{enter})*"*/"


%%

{whitespace} {
}

{enter} {linenum++;charnum=0;
}

{illegal_sign} {printf("---------------e r r o r--------------------\n");
printf("lexial_Error: skip illegal sign :  line:%d-%d %s\n",linenum,charnum,yytext);}

{mulcomment} {printf("skip %d lines--------\n",mul);
linenum=linenum+mul;charnum=0; 
}

{comment} {printf("skip --------\n");
linenum++;charnum=0; 
}

{error_id} {printf("---------------e r r o r--------------------\n");
printf("Lexial_Error:	unexpected identifier in line:%d-%d %s\n",linenum,charnum,yytext);
charnum++;}

{error_num} {printf("---------------e r r o r--------------------\n");
printf("Lexial_Error:	unexpected fraction number in line:%d-%d %s\n",linenum,charnum,yytext);
charnum++;}

"int" {Upper(yytext,yyleng);
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:keywords  Val:%s\n",yytext);
add_word(KEYWORDS, "int");
charnum++;
return T_int;
}

"if" {Upper(yytext,yyleng);
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:keywords  Val:%s\n",yytext);
charnum++;
return T_if;
}

"else" {Upper(yytext,yyleng);
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:keywords  Val:%s\n",yytext);
charnum++;
return T_else;
}

"while" {Upper(yytext,yyleng);
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:keywords  Val:%s\n",yytext);
charnum++;
return T_while;
}

"real" {Upper(yytext,yyleng);
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:keywords  Val:%s\n",yytext);
charnum++;
return T_real;
}

"then" {Upper(yytext,yyleng);
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:keywords  Val:%s\n",yytext);
charnum++;
return T_then;
}

{digit} {
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:digit  Val:%s\n",yytext);
charnum++;
return NUM;
}

{realnumber} {
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:realnumber  Val:%s\n",yytext);
charnum++;
return NUM;
}

{intnumber} {
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:intnumber  Val:%s\n",yytext);
charnum++;
return NUM;
}

{exponent} {
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:exponent  Val:%s\n",yytext);
charnum++;
return NUM;
}

{fraction} {
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:fraction  Val:%s\n",yytext);
charnum++;
return NUM;
}

{delimiters} {
//if(yytext[0]=='(')
	//bracket_count=bracket_count+1;
//if(yytext[0]==')')
	//bracket_count=bracket_count-1;
//if(yytext[0]=='{')
	//bracket_count=Bracket_count+1;
//if(yytext[0]=='}')
	//bracket_count=Bracket_count-1;
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:delimiters  Val:%s\n",yytext);
charnum++;
return yytext[0];
}

{operators} {
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:operators  Val:%s\n",yytext);
charnum++;
return yytext[0];
}

"==" {
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:operators  Val:%s\n",yytext);
charnum++;
return EQ;
}

"<=" {
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:operators  Val:%s\n",yytext);
charnum++;
return LEE;
}

">=" {
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:operators  Val:%s\n",yytext);
charnum++;
return LAE;
}

"!=" {
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:operators  Val:%s\n",yytext);
charnum++;
return NE;
}

{identifiers} {
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:identifiers  Val:%s\n",yytext);
charnum++;
return ID;
}




\n	{ state = LOOKUP; }	/* end of line, return to default state */

^keywords	{ state = KEYWORDS; }
^digit	        { state = DIGIT; }
^realnumber	{ state = REALNUMBER; }
^intnumber	{ state = INTNUMBER; }
^exponent	{ state = EXPONENT; }
^fraction	{ state = FRACTION; }
^operators	{ state = OPERATORS; }
^identifiers	{ state = IDENTIFIERS; }



[a-zA-Z]+  {
		/* a normal word, define it or look it up */
	     if(state != LOOKUP) {
	        /* define the current word */
	     	add_word(state, yytext);
                
	     } else {
		switch(lookup_word(yytext)) {
		case KEYWORDS: printf("%s: keywords\n", yytext); break;
		case IDENTIFIERS: printf("%s: identifiers\n", yytext); break;
		default:
			printf("%s:  don't recognize\n", yytext);
			break;
		}
            }
          }

[0-9]+  {
		/* a normal word, define it or look it up */
	     if(state != LOOKUP) {
	        /* define the current word */
	     	add_word(state, yytext);
	     } else {
		switch(lookup_word(yytext)) {
		case DIGIT: printf("%s: digit\n", yytext); break;
		case REALNUMBER: printf("%s: realnumber\n", yytext); break;
		case INTNUMBER: printf("%s: intnumber\n", yytext); break;
		case EXPONENT: printf("%s: exponent\n", yytext); break;
		case FRACTION: printf("%s: fraction\n", yytext); break;
		default:
			printf("%s:  don't recognize\n", yytext);
			break;
		}
            }
          }

.	/* ignore anything else */ ;






%%

//Upper
Upper(char *s,int l){
int i;
for(i=0;i<l;i++)
{s[i]=toupper(s[i]);}
}

yywrap()
{
   return 0;
}

//limin
/* define a linked list of words and types */
struct word {
	char *word_name;
	int word_type;
	struct word *next;
};

struct word *word_list; /* first element in word list */

extern void *malloc();

int
add_word(int type, char *word)
{
	struct word *wp;	

	if(lookup_word(word) != LOOKUP) {
		printf("!!! warning: word %s already defined \n", word);
		return 0;
	}
	
	/* word not there, allocate a new entry and link it on the list */

	wp = (struct word *) malloc(sizeof(struct word));

	wp->next = word_list;

	/* have to copy the word itself as well */
	
	wp->word_name = (char *) malloc(strlen(word)+1);
	strcpy(wp->word_name, word);
	wp->word_type = type;
	word_list = wp;
	return 1;	/* it worked */
}
int
lookup_word(char *word)
{
	struct word *wp = word_list;

	/* search down the list looking for the word */
	for(; wp; wp = wp->next) {
		if(strcmp(wp->word_name, word) == 0)
			return wp->word_type;
	}

	return LOOKUP;	/* not found */
}
