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
	int onlyRight;//只有右值
	union
	{
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
	int line;
	char name[64];
	Type type;// 类型
	FieldList tail;// 下一个域
};

//函数记录
struct FuncList_
{
	char name[64];
	int count;//参数个数
	Type returnType;//返回类型
	FieldList args;//参数（借用tail当作next）
};

//符号表的记录
struct Record
{
	int type;//0: var 	1: func   2:structure
	union{
		FieldList var;
		FuncList func;
	};
	struct Record* next;//open hashing
};

//符号表
extern struct Record* table[TABLESIZE];

//函数
void initTable();
int hashIndex(char *name);
int isEqualType(Type a,Type b);
struct Record* findTable(char *name,int type,int line);
void insertTable(struct Record *r,int line);
void LogError(int NO,int line);

#endif
