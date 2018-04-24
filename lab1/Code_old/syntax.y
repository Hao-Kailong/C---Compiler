%{
#include"lex.yy.c"
#include<stdio.h>
#include<stdlib.h>
%}

%locations

%union{
	int type_int;
	float type_float;
	double type_double;
}

%token <type_int> INT
%token <type_float> FLOAT
%token PLUS MINUS STAR DIV AND OR NOT
%token LP RP LB RB LC RC
%token STRUCT RETURN IF ELSE WHILE
%token ID ASSIGNOP RELOP DOT TYPE SEMI COMMA

%type <type_double> Exp

%left LP RP LB RB
%left PLUS MINUS STAR DIV
%left DOT
%left RELOP
%left AND OR
%right NOT
%right ASSIGNOP

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
Program: ExtDefList;
ExtDefList: ExtDef ExtDefList
| /*empty*/;
ExtDef: Specifier ExtDecList SEMI
| Specifier SEMI
| Specifier FunDec CompSt;
ExtDecList: VarDec
| VarDec COMMA ExtDecList;

Specifier: TYPE
| StructSpecifier;
StructSpecifier: STRUCT OptTag LC DefList RC
| STRUCT Tag;
OptTag: ID
| /*empty*/;
Tag: ID;

VarDec: ID
| VarDec LB INT RB;
FunDec: ID LP VarList RP
| ID LP RP;
VarList: ParamDec COMMA VarList
| ParamDec;
ParamDec: Specifier VarDec;

CompSt: LC DefList StmtList RC;
StmtList: Stmt StmtList
| /*empty*/;
Stmt: Exp SEMI
| CompSt
| RETURN Exp SEMI
| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
| IF LP Exp RP Stmt ELSE Stmt
| WHILE LP Exp RP Stmt;

DefList: Def DefList
| /*empty*/;
Def: Specifier DecList SEMI;
DecList: Dec
| Dec COMMA DecList;
Dec: VarDec
| VarDec ASSIGNOP Exp;

Exp: Exp ASSIGNOP Exp
| Exp AND Exp { $$ = 0; }
| Exp OR Exp { $$ = 0; } 
| Exp RELOP Exp { }
| Exp PLUS Exp { $$ = $1 + $3; }
| Exp MINUS Exp { $$ = $1 - $3; }
| Exp STAR Exp { $$ = $1 * $3; }
| Exp DIV Exp { $$ = $1 / $3; }
| LP Exp RP { $$ = $2; }
| MINUS Exp { $$ = -$2; }
| NOT Exp { $$ = 0; }
| ID LP Args RP { $$ = 0; }
| ID LP RP { $$ = 0; }
| Exp LB Exp RB
| Exp DOT ID
| ID { $$ = 0; }
| INT { $$ = $1; }
| FLOAT { $$ = $1; }
Args: Exp COMMA Args
| Exp;

Stmt: error SEMI;
CompSt: error RC;
Exp: error RP { $$ = 0; };
%%
void yyerror(char *msg)
{
	printf("Error type B at Line %d: Mysterious characters \'%s\'\n",yylineno, yytext);
}

struct Node{
	union{
		int valInt;
		float valFloat;
		double valDouble;
	};
	struct Node *head;
};
