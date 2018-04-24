%{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"lex.yy.c"
struct Node;
#define YYSTYPE Node
%}

%locations

%token <Node> INT
%type <Node> Exp

%%
Exp: INT { setNode(&$$,$1.valInt,"Exp",NULL,&$1,NULL); }
%%

void yyerror(char *msg)
{
printf("Error type B at Line %d\n",yylineno);
}

struct Node{
	union{
		int valInt;
		float valFloat;
		double valDouble;
	};
	char type[64];
	char name[64];
	Node *child;
	Node *next;
};

void addChild(Node *f, Node *s)
{
	f->child=s;
}

void addNext(Node *a, Node *b)
{
	a->next=b;	
} 

void setNode(struct Node *node,double val,char *type,char *name,struct Node *child,struct Node *next)
{
	if(!node)
		return;
	if(type!=NULL)
	{
		if(strcmp(type,"INT")==0) node->valInt=(int)val;
		else if(strcmp(type,"FLOAT")==0) node->valFloat=(float)val;
		else node->valDouble=val;
	}
	if(type!=NULL) strcpy(node->type,type);
	else node->type=NULL;
	if(name!=NULL) strcpy(node->name,name);
	else node->name=NULL;
	node->child=child;
	node->next=next;	
}

void visit(struct Node *node,int rank)
{
	for(int i=0;i<rank*spaceSize;++i)
		printf(" ");
	printf("%s\n",node->type);
}

#define spaceSize 2
void show(struct Node *node,int rank)
{
	if(!node)
		return;
	visit(node,rank);
	Node *child=node->child;	
	while(child)
	{
		show(child,rank+1);
		child=child->next;
	}
}
