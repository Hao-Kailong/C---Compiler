%{
#include<string.h>
#include"syntax.tab.h"
#include"tree.h"
extern int hasError;
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

SHORTNOTE \/\/.*\n
LONGNOTE \/\*[^(\/\*)(\*\/)]*\*\/
%%
{STRUCT} {yylval.NodeStar=generateNode(0,yylineno,"STRUCT","",NULL,NULL);return STRUCT;}
{RETURN} {yylval.NodeStar=generateNode(0,yylineno,"RETURN","",NULL,NULL);return RETURN;}
{IF} {yylval.NodeStar=generateNode(0,yylineno,"IF","",NULL,NULL);return IF;}
{ELSE} {yylval.NodeStar=generateNode(0,yylineno,"ELSE","",NULL,NULL);return ELSE;}
{WHILE} {yylval.NodeStar=generateNode(0,yylineno,"WHILE","",NULL,NULL);return WHILE;}
{TYPE} {
yylval.NodeStar=generateNode(0,yylineno,"TYPE",yytext,NULL,NULL);
return TYPE;
}
{INT} {
yylval.NodeStar=generateNode(atoi(yytext),yylineno,"INT",yytext,NULL,NULL);
return INT;
}
{FLOAT} {
yylval.NodeStar=generateNode(atof(yytext),yylineno,"FLOAT",yytext,NULL,NULL);
return FLOAT;
}
{ID} {
yylval.NodeStar=generateNode(0,yylineno,"ID",yytext,NULL,NULL);
return ID; 
}
{RELOP} {
yylval.NodeStar=generateNode(0,yylineno,"RELOP",yytext,NULL,NULL);
return RELOP; 
}
{SEMI} {yylval.NodeStar=generateNode(0,yylineno,"SEMI","",NULL,NULL);return SEMI;}
{COMMA} {yylval.NodeStar=generateNode(0,yylineno,"COMMA","",NULL,NULL);return COMMA;}
{ASSIGNOP} {yylval.NodeStar=generateNode(0,yylineno,"ASSIGNOP","",NULL,NULL);return ASSIGNOP;}
{PLUS} {yylval.NodeStar=generateNode(0,yylineno,"PLUS","",NULL,NULL);return PLUS;}
{MINUS} {yylval.NodeStar=generateNode(0,yylineno,"MINUS","",NULL,NULL);return MINUS;}
{STAR} {yylval.NodeStar=generateNode(0,yylineno,"STAR","",NULL,NULL);return STAR;}
{DIV} {yylval.NodeStar=generateNode(0,yylineno,"DIV","",NULL,NULL);return DIV;}
{AND} {yylval.NodeStar=generateNode(0,yylineno,"AND","",NULL,NULL);return AND;}
{OR} {yylval.NodeStar=generateNode(0,yylineno,"OR","",NULL,NULL);return OR;}
{DOT} {yylval.NodeStar=generateNode(0,yylineno,"DOT","",NULL,NULL);return DOT;}
{NOT} {yylval.NodeStar=generateNode(0,yylineno,"NOT","",NULL,NULL);return NOT;}
{LP} {yylval.NodeStar=generateNode(0,yylineno,"LP","",NULL,NULL);return LP;}
{RP} {yylval.NodeStar=generateNode(0,yylineno,"RP","",NULL,NULL);return RP;}
{LB} {yylval.NodeStar=generateNode(0,yylineno,"LB","",NULL,NULL);return LB;}
{RB} {yylval.NodeStar=generateNode(0,yylineno,"RB","",NULL,NULL);return RB;}
{LC} {yylval.NodeStar=generateNode(0,yylineno,"LC","",NULL,NULL);return LC;}
{RC} {yylval.NodeStar=generateNode(0,yylineno,"RC","",NULL,NULL);return RC;}
{EMPTY} {}
{SHORTNOTE} {}
{LONGNOTE} {}
\n { yycolumn=1; }
. { hasError=1;printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno,yytext); }
%%
