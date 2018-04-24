%{
#include"syntax.tab.h"
#include<string.h>
#include"tree.h"
%}

%{
int yycolumn = 1;
#define YY_USER_ACTION \
yylloc.first_line = yylloc.last_line = yylineno; \
yylloc.first_column = yycolumn; \
yylloc.last_column = yycolumn + yyleng - 1; \
yycolumn += yyleng;
%}

%option yylineno
yylineno=1;

INT 0|([1-9][0-9]*)
%%

{INT} {
yylval.valInt=atoi(yytext);
yylval.type="INT";
strcpy(yylval.name,yytext);
yylval.child=NULL;
yylval.next=NULL;
yylval
return INT;
}
\n { yycolumn=1; }
. { printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno,yytext); }
%%
