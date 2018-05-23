#include"tree.h"
#define spaceSize 2

//报告语法错误
void yyerror(char *msg)
{
	printf("Error type B at Line %d near %d\n",yylineno,yycolumn);
}

//添加子结点
void addChild(struct Node *f, struct Node *s)
{
	f->child=s;
}

//添加兄弟结点
void addNext(struct Node *a, struct Node *b)
{
	a->next=b;	
} 

//生成语法树结点
struct Node* generateNode(double val,int line,char *type,char *name,struct Node *child,struct Node *next)
{
	struct Node *node=(struct Node*)malloc(sizeof(struct Node));
	if(type!=NULL)
	{
		if(strcmp(type,"INT")==0) node->valueInt=(int)val;
		else node->valueFloat=(float)val;
	}
	node->line=line;	
	strcpy(node->type,type);
	strcpy(node->name,name);
	node->child=child;
	node->next=next;	
	return node;
}

//访问当前结点
void visit(struct Node *node,int rank)
{
	if(!strcmp(node->type,""))//空
		return;
	for(int i=0;i<rank*spaceSize;++i)
		printf(" ");
	//特殊词法单元
	if(!strcmp(node->type,"ID"))
		printf("%s: %s\n",node->type,node->name);
	else if(!strcmp(node->type,"TYPE"))
		printf("%s: %s\n",node->type,node->name);
	else if(!strcmp(node->type,"INT"))
		printf("%s: %d\n",node->type,node->valueInt);	
	else if(!strcmp(node->type,"FLOAT"))
		printf("%s: %f\n",node->type,node->valueFloat);	
	else if(node->child==NULL)//词法单元无需输出行号
		printf("%s\n",node->type);
	//语法单元
	else
		printf("%s (%d)\n",node->type,node->line);
}

//输出语法树
void show(struct Node *node,int rank)
{
	if(!node)
		return;
	visit(node,rank);
	struct Node *child=node->child;	
	while(child)
	{
		show(child,rank+1);
		child=child->next;
	}
}
