%{
#include"syntax.tab.h"
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
FLOAT [0-9]+\.[0-9]+
ID (_|[a-zA-Z])[_a-zA-Z0-9]*
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|(>=)|(<=)|(==)|(!=)
PLUS \+
MINUS -
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
TYPE int|float
EMPTY \r|\t|" "
%%
{STRUCT} { return STRUCT; }
{RETURN} { return RETURN; }
{IF} { return IF; }
{ELSE} { return ELSE; }
{WHILE} { return WHILE; }
{TYPE} { return TYPE; }
{INT} {
yylval.type_int=atoi(yytext);
return INT;
}
{FLOAT} {
yylval.type_float=atof(yytext);
return FLOAT;
}
{ID} { return ID; }
{SEMI} { return SEMI; }
{COMMA} { return COMMA; }
{ASSIGNOP} { return ASSIGNOP; }
{RELOP} { return RELOP; }
{PLUS} { return PLUS; }
{MINUS} { return MINUS; }
{STAR} { return STAR; }
{DIV} { return DIV; }
{AND} { return AND; }
{OR} { return OR; }
{DOT} { return DOT; }
{NOT} { return NOT; }
{LP} { return LP; }
{RP} { return RP; }
{LB} { return LB; }
{RB} { return RB; }
{LC} { return LC; }
{RC} { return RC; }
{EMPTY} {}
\n { yycolumn=1; }
. { printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno,yytext); }
%%
