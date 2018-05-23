#ifndef __TREE_H__
#define __TREE_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void yyrestart(FILE *f);
int yyparse();

extern int hasSyntaxError;//词法语法错误
extern int yylineno;//行号
extern int yycolumn;//列号

//语法树节点
struct Node{
	union{
		int valueInt;
		float valueFloat;
	};
	int line;
	char type[64];
	char name[64];
	struct Node *child;
	struct Node *next;
};

//根结点
extern struct Node *root;

//打印错误信息
void yyerror(char *msg);
//添加子节点
void addChild(struct Node *f, struct Node *s);
//添加兄弟节点
void addNext(struct Node *a, struct Node *b);
//生成新节点
struct Node* generateNode(double val,int line,char *type,char *name,struct Node *child,struct Node *next);
//访问当前节点
void visit(struct Node *node,int rank);
//前序遍历整个树
void show(struct Node *node,int rank);

#endif
