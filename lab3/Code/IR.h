#ifndef __IR_H__
#define __IR_H__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"table.h"
#include"tree.h"

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;

extern InterCode head,tail;

//操作数符
struct Operand_ {
	char str[64];
	struct Operand_* next;//函数参数
};

//中间代码
struct InterCode_
{
	enum{
		LABEL,FUNCTION,ASSIGN,
		ADD,SUB,MULT,DV,
		ADDRESSASSIGN,//=&
		STARASSIGN,//=*
		ASSIGNSTAR,//*=
		GOTO,
		COND,//条件
		RTN,
		DEC,//空间分配
		ARG,CALL,PARAM,
		READ,WRITE
	}kind;
	Operand o0;
	Operand o1;
	Operand o2;
	Operand op;
	struct InterCode_* pre;
	struct InterCode_* next;
};

void IR(struct Node *root);
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
void trsCond(struct Node *node,Operand label1,Operand label2);

/*===========================================*/

void initIR();
Operand newOprRnd();
Operand newOprStr(char *str);
//生成中间代码
InterCode newCode(int kind,Operand o0,
		Operand o1,Operand o2,Operand op);
//添加代码
void addCode(InterCode code);
void addReadFunc();
void addWriteFunc();
void IRprint(InterCode head);

#endif
