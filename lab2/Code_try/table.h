#ifndef __TABLE_H__
#define __TABLE_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define TABLESIZE 256

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct FuncList_* FuncList;

//类型
struct Type_
{
	enum { BASIC, ARRAY, STRUCTURE } kind;
	union
	{
	//基本类型
	int basic;//0: INT	1: FLOAT
	//数组类型信息包括元素类型与数组大小构成
	struct { Type elem; int size; } array;
	//结构体类型信息是一个链表
	FieldList structure;
	} u;
};

//变量记录（域、结构体）
struct FieldList_
{
	char* name;
	Type type;// 类型
	FieldList tail;// 下一个域
};

//函数记录
struct FuncList_
{
	char *name;
	int count;//参数个数
	Type returnType;//返回类型
	FieldList args;//参数（借用tail当作next）
};

//符号表的记录
struct Record
{
	int type;//0: var 	1: func
	union record {
		FieldList var;
		FuncList func;
	} *r;
	struct Recode *next;//open hashing
};

//符号表
extern struct Record* table[TABLESIZE];
//结构体类型表
extern FieldList *list;

void initTable();
int hashIndex(char *name);

#endif
