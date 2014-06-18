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
operators  "+"|"-"|"*"|"/"|"="|">"|"<"|"=="|"<="|">="|"!="
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
printf("\nLN:%d  Pos:%d  ",linenum,charnum);
printf("Type:keywords  Val:%s\n",yytext);
add_word(KEYWORDS,yytext);
charnum++;
return T_int;
}

"if" {Upper(yytext,yyleng);
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:keywords  Val:%s\n",yytext);
add_word(KEYWORDS,yytext);
charnum++;
return T_if;
}

"else" {Upper(yytext,yyleng);
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:keywords  Val:%s\n",yytext);
add_word(KEYWORDS,yytext);
charnum++;
return T_else;
}

"while" {Upper(yytext,yyleng);
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:keywords  Val:%s\n",yytext);
add_word(KEYWORDS,yytext);
charnum++;
return T_while;
}

"real" {Upper(yytext,yyleng);
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:keywords  Val:%s\n",yytext);
add_word(KEYWORDS,yytext);
charnum++;
return T_real;
}

"then" {Upper(yytext,yyleng);
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:keywords  Val:%s\n",yytext);
add_word(KEYWORDS,yytext);
charnum++;
return T_then;
}

{digit} {
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:digit  Val:%s\n",yytext);
add_word(DIGIT,yytext);
charnum++;
return NUM;
}

{realnumber} {
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:realnumber  Val:%s\n",yytext);
add_word(REALNUMBER,yytext);
charnum++;
return NUM;
}

{intnumber} {
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:intnumber  Val:%s\n",yytext);
add_word(INTNUMBER,yytext);
charnum++;
return NUM;
}

{exponent} {
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:exponent  Val:%s\n",yytext);
add_word(EXPONENT,yytext);
charnum++;
return NUM;
}

{fraction} {
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:fraction  Val:%s\n",yytext);
add_word(FRACTION,yytext);
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
add_word(OPERATORS,yytext);
charnum++;
return yytext[0];
}




{identifiers} {
printf("LN:%d  Pos:%d  ",linenum,charnum);
printf("Type:identifiers  Val:%s\n",yytext);
add_word(IDENTIFIERS,yytext);
charnum++;
return ID;
}

%%

//Upper
Upper(char *s,int l){
int i;
for(i=0;i<l;i++)
{s[i]=toupper(s[i]);}
}


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
	
	//char *wn=wp->word_name;
	//int wt=wp->word_type;	

	if(lookup_word(word) != LOOKUP) {
		printf("NOTIC: word %s is already added to symtable \n", word);
		//printf("Symtable value:%s\n",word);
		//printf("SysName:%s,SysType:%d\n\n",wn,wt);
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


yywrap()
 {
  return 0;
}
