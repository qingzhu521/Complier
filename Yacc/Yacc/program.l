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
