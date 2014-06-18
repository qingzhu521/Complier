%{
/*
 *a parser that return a quadraplenumber  
 *
 * (tokentype,attributevalue,linenumber,lineposition) 
 */
%}

comments \/\/[A-Za-z0-9]*
digit  [0-9]
number {digit}+(\.{digit}+)?(E[+\-]?{digit}+)?
keyword int|real|if|then|else|while
operators \+|\-|\/|\*|\=|\==|\<|\<=|\>|\>=|\!=
delimiters  \(|\)|\{|\}|\;
identifiers [A-Za-z][A-Za-z0-9]*|{keyword}{digit}+

%%

[\t ]+		/* ignore white space */ ;

{comments}		{ printf("%s: is a comment\n", yytext); }

{digit} |
{number}		{ printf("%s: is a number\n", yytext); }


{keyword}		{ printf("%s: is a keyword\n", yytext); }

{operators}		{ printf("%s: is a operator\n", yytext); }

{delimiters}		{ printf("%s: is a delimiter\n", yytext); }

{identifiers}		{ printf("%s: is a identifier\n", yytext); }

\&.|\n		{ ECHO; /* normal default anyway */ }

%%

main()
{
	yylex();
}


