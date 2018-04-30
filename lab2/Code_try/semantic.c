#include"semantic.h"

/*语义分析总入口*/
void semantic(struct Node *root)
{
	if(!root)
		return;
	initTable();
	ExtDefList(root->child);
}

void ExtDefList(struct Node *node)
{
	if(!node)
		return;
	struct Node *child=node->child;
	ExtDef(child);
	ExtDefList(child->next);
}

void ExtDef(struct Node *node)
{
	if(!node)
		return;
	struct Node *child=node->child;
	Type t=Specifier(child);
	struct Node *child1=child->next;
	if(strcmp(child1->type,"SEMI")==0)
		return;
	else if(strcmp(child1->type,"ExtDecList")==0)
		ExtDecList(child1,t);
	else{
		FunDec(child1,t);
		CompSt(child1->next,t);//检查类型错误
	}
}

void ExtDecList(struct Node *node,Type type)
{
	if(!node)
		return;
	struct Node *child=node->child;
	Record *record=VarDec(child,type);
	insertTable(record);//插入变量
	if(child->next==NULL)
		return;
	ExtDecList(child->next->next,type);
}

Type Specifier(struct Node *node);
{
	if(!node)
		return NULL;
	struct Node *child=node->child;
	if(strcmp(child->type,"TYPE")==0){
		Type t=malloc(sizeof(Type_));
		t->kind=BASIC;
		if(strcmp(child->name,"INT")==0)
			t->u.basic=0;
		else
			t->u.basic=1;
		return t;
	}
	else
		return StructSpecifier(child);
}

Type StructSpecifier(struct Node *node)
{
	if(!node)
		return NULL;
	struct Node *child=node->child->next;
	if(strcmp(child->type,"Tag")==0)
		return Tag(child);
	else{
		//新结构体类型
		FieldList tmp=malloc(sizeof(FieldList_));
		char *str=OptTag(child);//名字
		Type t=malloc(sizeof(Type_));
		t->kind=STRUCTURE;
		t->u.structure=NULL;
		DefList(child->next->next,t);//类型
		addList(str,t,NULL);//插表
		return t;
	}
}

char* OptTag(struct Node *node)
{
	if(!node)
		return NULL;
	struct Node *child=node->child;
	char *str=malloc(sizeof(char)*64);
	strcpy(str,child->name);
	return str;
}

Type Tag(struct Node *node)
{
	if(!node)
		return NULL;
	struct Node *child=node->child;
	return findType(child->name);
}

//需要递归调用
Record* VarDec(struct Node *node,Type type)
{
	Record* record=vardec(node,type);
	return record;
}
Record* vardec(struct Node *node,Type type)
{
	if(!node)
		return NULL;
	struct Node *child=node->child;
	if(strcmp(child->type,"ID")==0){
		Record* tmp=malloc(sizeof(Record));
		tmp->type=0;//变量
		tmp->r->var=malloc(sizeof(FieldList_));
		tmp->r->var->name=malloc(sizeof(char)*64);
		strcpy(tmp->r->var->name,child->name);
		tmp->r->var->type=type;
		tmp->r->var->tail=NULL;
		tmp->next=NULL;
		return tmp;
	}
	else{//数组
		Record* tmp=VarDec(child,type);
		int num=child->next->next->value.valInt;
		Type t=malloc(sizeof(Type_));
		t->kind=ARRAY;
		t->u.array.size=num;
		t->u.array.elem=tmp->r->var->type;
		tmp->r->var->type=t;//更新类型信息
		return tmp;
	}
}

void FunDec(struct Node *node,Type type)
{
	if(!node)
		return;
	struct Node *child=node->child;
	if(strcmp(child->next->next->type,"RP")==0){
		Record* tmp=malloc(sizeof(Record));
		tmp->type=1;//函数
		tmp->r->func=malloc(sizeof(FuncList_));
		tmp->r->func->name=malloc(sizeof(char)*64);
		strcpy(tmp->r->func->name,child->name);
		tmp->r->func->count=0;
		tmp->r->func->args=NULL;
		tmp->r->func->returnType=type;
		insertTable(tmp);//插入函数
	}	
	else{
		Record* tmp=malloc(sizeof(Record));
		tmp->type=1;
		tmp->r->func=malloc(sizeof(FuncList_));
		tmp->r->func->name=malloc(sizeof(char)*64);
		strcpy(tmp->r->func->name,child->name);
		FieldList args=VarList(child->next->next);//获取参数
		tmp->r->func->args=args;
		int count=0;
		while(args->tail!=NULL){
			++count;
			args=args->tail;
		}
		tmp->r->func->count=count;
		tmp->r->func->returnType=type;
		insertTable(tmp);//插入函数
	}
}

//需要递归调用
FieldList VarList(struct Node *node)
{
	FieldList args=varlist(node);
	return args;
}
FieldList varlist(struct Node *node)
{
	if(!node)
		return NULL;
	struct Node *child=node->child;
	FieldList tmp=ParamDec(child);
	if(child->next==NULL)
		return tmp;
	else{
		FieldList t=varlist(child->next->next);
		tmp->tail=t;//链接
		return tmp;
	}
}

FieldList ParamDec(struct Node *node)
{
	if(!node)
		return NULL;
	struct Node *child=node->child;
	Type t=Specifier(child);
	Record *record=VarDec(child->next,t);
	return record->r->var;
}

void CompSt(struct Node *node,Type type)
{
	
}

addList(char *name,Type type,FieldList tail);//插表
findType(char *str);//根据名字查找类型
insertTable(Recode *r);//插入表格
void DefList(struct Node *node,Type type);
void CompSt(struct Node *node,Type type);//查错
