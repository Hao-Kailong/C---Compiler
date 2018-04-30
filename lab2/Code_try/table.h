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
	int basic;
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
	Type *args;//参数类型
};

//符号表的记录
struct Record
{
	union record {
		FuncList func;
		FieldList var;
	} *r;
	struct Recode *next;//open hashing
};

extern struct Record table[TABLESIZE];

int hashIndex(char *name);

#endif
