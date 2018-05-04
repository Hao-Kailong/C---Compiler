#include"table.h"
#include"syntax.tab.h"
#include"tree.h"

struct Record* table[TABLESIZE];//符号表

void initTable()
{
	for(int i=0;i<TABLESIZE;++i)
		table[i]=NULL;
}

int hashIndex(char* name)
{
	int length=strlen(name);
	int sum=0;
	for(int i=0;i<length;++i)
		sum += name[i];
	return sum % TABLESIZE;
}

bool isEqualType(Type a,Type b)
{
	if(a->kind!=b->kind)//类型不同
		return false;
	if(a->kind==a->BASIC&&a->u.basic==b->u.basic)//基本
		return true;
	else if(a->kind==a->ARRAY){//数组
		bool flag=isEqualType(a->u.array.elem,b->u.array.elem);
		return flag;
	}
	else{//结构体
		FieldList f1=a->u.structure->type->u.structure;
		FieldList f2=b->u.structure->type->u.structure;
		while(true){//结构等价
			if(f1==NULL&&f2==NULL)
				return true;
			else if(f1==NULL||f2==NULL)
				return false;
			if(!isEqualType(f1->type,f2->type))
				return false;
			f1=f1->tail;
			f2=f2->tail;
		}
	}
}

Record* findTable(char *name,int type)
{
	int index=hashIndex(name);
	Record *tmp=table[index];
	while(tmp!=NULL){
		if(type==1){//函数
			if(tmp->type==1&&strcmp(tmp->func->name,str)==0)
				return tmp;
		}
		else{//结构体、变量
			if(tmp->type==type&&strcmp(tmp->var->name,str)==0)
				return tmp;
		}
		tmp=tmp->next;
	}
	if(type==0)
		LogError(1);//Error 1: 变量未定义
	else if(type==1)
		LogError(2);//Error 2: 函数未定义
	else
		LogError(17);//Error 17:未定义的结构体
	return NULL;
}

void insertTable(Record *r)
{
	char *str=malloc(sizeof(char)*64);
	if(r->type==0||r->type==2)
		strcpy(str,r->var->name);
	else
		strcpy(str,r->func->name);
	int index=hashIndex(str);
	Record *tmp=table[index];
	while(true){
		if(r->type==0){
			if((tmp->type==0||tmp->type==2)&&strcmp(r->var->name,tmp->var->name)==0){
				LogError(3);//Error 3:变量重定义
				return;
			}	
		}
		else if(r->type==1){
			if(tmp->type==1&&strcmp(r->func->name,tmp->func->name)==0){
				LogError(4);//Error 4:函数重定义
				return;
			}	
		}
		else{
			if((tmp->type==0||tmp->type==2)&&strcmp(r->var->name,tmp->var->name)==0){
				LogError(16);//Error 16:结构体重定义
				return;
			}	
		}
		if(tmp->next==NULL){//正确插入
			tmp->next=r;
			r->next=NULL;
			return;
		}
		tmp=tmp->next;
	}
}

void LogError(int NO)
{
	switch(NO){
		case 1:printf("error 1: undefined variable\n");break;
		case 2:printf("error 2: undefined function\n");break;
		case 3:printf("error 3: redefine variable\n");break;
		case 4:printf("error 4: redefine function\n");break;
		case 5:printf("error 5: type mismatched for assignment\n");break;
		case 6:printf("error 6: left side must be a variable\n");break;
		case 7:printf("error 7: type mismatched for opernds\n");break;
		case 8:printf("error 8: type mismatched for return\n");break;
		case 9:printf("error 9: args for function mismatch\n");break;
		case 10:printf("error 10: not an array\n");break;
		case 11:printf("error 11: not a function\n");break;
		case 12:printf("error 12: not an integer\n");break;
		case 13:printf("error 13: illegal use of '.'\n");break;
		case 14:printf("error 14: non-existent field\n");break;
		case 15:printf("error 15: redefined field\n");break;
		case 16:printf("error 16: redefine structure\n")break;
		case 17:printf("error 17: undefined structure\n");break;
		default:printf("undefined semantic error\n");
	}
}
