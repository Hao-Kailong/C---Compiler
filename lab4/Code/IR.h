#ifndef __IR_H__
#define __IR_H__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"table.h"
#include"tree.h"

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;

//中间代码链表
extern InterCode IRhead,IRtail;

//操作数、操作符
struct Operand_ {
	char str[64];
	int value;
	struct Operand_* next;
};

//代码类型
enum codeKind{
	LABEL,FUNCTION,ASSIGN,		//2
	ADD,SUB,MULT,DV,			//6
	ADDRESSASSIGN,//=&
	STARASSIGN,//=*
	ASSIGNSTAR,//*=				
	GOTO,						//10
	COND,//条件
	RTN,
	DEC,//空间分配				//13
	ARG,CALL,PARAM, 			//16
	READ,WRITE					//18
};

//中间代码
struct InterCode_
{
	enum codeKind kind;
	Operand o0;//o0 := o1 op o2, 从左至右
	Operand o1;
	Operand o2;
	Operand op;
	struct InterCode_* pre;
	struct InterCode_* next;
};

void IR(struct Node *root,char *outfile);
void IRgenerate(struct Node *root);
void IRopt();
void IRprint(char *outfile);

void trsExtDefList(struct Node *node);
void trsExtDef(struct Node *node);
void trsSpecifier(struct Node *node);
char* trsVarDec(struct Node *node,int level,int flag);
void trsFunDec(struct Node *node);
void trsVarList(struct Node *node);
void trsParamDec(struct Node *node);
void trsCompSt(struct Node *node);
void trsStmtList(struct Node *node);
void trsStmt(struct Node *node);
void trsDefList(struct Node *node);
void trsDef(struct Node *node);
void trsDecList(struct Node *node);
void trsDec(struct Node *node);
Operand trsExp(struct Node *node,Operand place);
Operand trsArgs(struct Node *node,Operand args);
void trsCond(struct Node *node,Operand labelt,Operand labelf);

Operand newOprRnd(int kind);//0: var    1: label
Operand newOprStr(char *str);
InterCode newCode(enum codeKind kind,Operand o0,
	Operand o1,Operand o2,Operand op);
void addCode(InterCode code);

#endif
