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
	union {
		int basic;//0:INT, 1:FLOAT 
		struct { 
			Type elem;
			int size; 
		} array;//数组
		FieldList structure;//结构体
	};
	int FINAL;
};

//变量、域、结构体
struct FieldList_
{
	char name[64];
	Type type;
	FieldList tail;
};

//函数
struct FuncList_
{
	char name[64];
	Type returnType;
	FieldList args;//参数
	int count;//个数
};

//符号表的记录
struct Record
{
	int type;//0:var, 1:func, 2:structure
	union{
		FieldList var;
		FuncList func;
	};
	struct Record* next;//open hashing
};

//符号表
extern struct Record* table[TABLESIZE];

//初始化符号表
void initTable();
//哈希函数
int hashIndex(char *name);
//判断类型是否相同，采用结构等价
int isEqualType(Type a,Type b);
//查符号表
struct Record* findTable(char *name,int type,int line);
//插入符号表
void insertTable(struct Record *r,int line);


//输出错误信息
void LogError(int NO,int line);

#endif
