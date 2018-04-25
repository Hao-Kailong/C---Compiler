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
{STRUCT} { printf("%s\n",yytext); }
{RETURN} { printf("%s\n",yytext); }
{IF} { printf("%s\n",yytext); }
{ELSE} { printf("%s\n",yytext); }
{WHILE} { printf("%s\n",yytext); }
{TYPE} { printf("%s\n",yytext); }
{INT} { printf("%s\n",yytext); }
{FLOAT} { printf("%s\n",yytext); }
{ID} { printf("%s\n",yytext); }
{RELOP} { printf("%s\n",yytext); }
{SEMI} { printf("%s\n",yytext); }
{COMMA} { printf("%s\n",yytext); }
{ASSIGNOP} { printf("%s\n",yytext); }
{PLUS} { printf("%s\n",yytext); }
{MINUS} { printf("%s\n",yytext); }
{STAR} { printf("%s\n",yytext); }
{DIV} { printf("%s\n",yytext); }
{AND} { printf("%s\n",yytext); }
{OR} { printf("%s\n",yytext); }
{DOT} { printf("%s\n",yytext); }
{NOT} { printf("%s\n",yytext); }
{LP} { printf("%s\n",yytext); }
{RP} { printf("%s\n",yytext); }
{LB} { printf("%s\n",yytext); }
{RB} { printf("%s\n",yytext); }
{LC} { printf("%s\n",yytext); }
{RC} { printf("%s\n",yytext); }
{EMPTY} {}
{SHORTNOTE} { printf("短注释\n"); }
{LONGNOTE} { printf("长注释\n"); }
\n {}
. { printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno,yytext); }
%%
