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

int isEqualType(Type a,Type b)
{
	//printf("In IsEqualType\n");
	if(a->kind!=b->kind)//类型不同
		return 0;
	if(a->kind==0){//基本
		if(a->u.basic==b->u.basic)
			return 1;
		return 0;
	}
	else if(a->kind==1){//数组
		int flag=isEqualType(a->u.array.elem,b->u.array.elem);
		return flag;
	}
	else{//结构体
		FieldList f1=a->u.structure->type->u.structure;
		FieldList f2=b->u.structure->type->u.structure;
		while(1){//结构等价
			if(f1==NULL&&f2==NULL)
				return 1;
			else if(f1==NULL||f2==NULL)
				return 0;
			if(!isEqualType(f1->type,f2->type))
				return 0;
			f1=f1->tail;
			f2=f2->tail;
		}
	}
}

struct Record* findTable(char *name,int NO,int line)
{
	int index=hashIndex(name);
	struct Record *tmp=table[index];
	while(tmp!=NULL){
		if(NO==1){//函数
			if(tmp->type==1&&strcmp(tmp->func->name,name)==0)
				return tmp;
		}
		else{//结构体、变量
			if(tmp->type==NO&&strcmp(tmp->var->name,name)==0)
				return tmp;
		}
		tmp=tmp->next;
	}
	if(NO==0)
		LogError(1,line);//Error 1: 变量未定义
	else if(NO==1)
		LogError(2,line);//Error 2: 函数未定义
	else
		LogError(17,line);//Error 17:未定义的结构体
	return NULL;
}

void insertTable(struct Record *r,int line)
{
	//printf("In InsertTable\n");
	char str[64];
	if(r->type==0||r->type==2)
		strcpy(str,r->var->name);
	else
		strcpy(str,r->func->name);
	//printf("%s\n",str);
	int index=hashIndex(str);
	//printf("%d\n",index);
	struct Record *tmp=table[index];
	if(tmp==NULL){
		tmp=r;
		tmp->next=NULL;
		table[index]=tmp;
		return;
	}
	while(1){
		if(r->type==0){
			//printf("Should Appear");
			if((tmp->type==0||tmp->type==2)&&strcmp(r->var->name,tmp->var->name)==0){
				LogError(3,line);//Error 3:变量重定义
				return;
			}	
		}
		else if(r->type==1){
			if(tmp->type==1&&strcmp(r->func->name,tmp->func->name)==0){
				LogError(4,line);//Error 4:函数重定义
				return;
			}	
		}
		else{
			if((tmp->type==0||tmp->type==2)&&strcmp(r->var->name,tmp->var->name)==0){
				LogError(16,line);//Error 16:结构体重定义
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

void LogError(int NO,int line)
{
	switch(NO){
		case 1:printf("error 1 at line %d: undefined variable\n",line);break;
		case 2:printf("error 2 at line %d: undefined function\n",line);break;
		case 3:printf("error 3 at line %d: redefine variable\n",line);break;
		case 4:printf("error 4 at line %d: redefine function\n",line);break;
		case 5:printf("error 5 at line %d: type mismatched for assignment\n",line);break;
		case 6:printf("error 6 at line %d: left side must be a variable\n",line);break;
		case 7:printf("error 7 at line %d: type mismatched for operands\n",line);break;
		case 8:printf("error 8 at line %d: type mismatched for return\n",line);break;
		case 9:printf("error 9 at line %d: args for function mismatch\n",line);break;
		case 10:printf("error 10 at line %d: not an array\n",line);break;
		case 11:printf("error 11 at line %d: not a function\n",line);break;
		case 12:printf("error 12 at line %d: not an integer\n",line);break;
		case 13:printf("error 13 at line %d: illegal use of '.'\n",line);break;
		case 14:printf("error 14 at line %d: non-existent field\n",line);break;
		case 15:printf("error 15 at line %d: redefined field\n",line);break;
		case 16:printf("error 16 at line %d: redefine structure\n",line);break;
		case 17:printf("error 17 at line %d: undefined structure\n",line);break;
		default:printf("undefined semantic error\n");
	}
}
