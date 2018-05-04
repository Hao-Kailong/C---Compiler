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
	Type t=Specifier(child);//类型信息
	struct Node *child1=child->next;
	if(strcmp(child1->type,"SEMI")==0)
		return;
	else if(strcmp(child1->type,"ExtDecList")==0)
		ExtDecList(child1,t);//继承属性
	else{
		FunDec(child1,t);
		CompSt(child1->next,t);//检查返回类型错误
	}
}

void ExtDecList(struct Node *node,Type type)//使用type类型定义变量
{
	if(!node)
		return;
	struct Node *child=node->child;
	Record *record=VarDec(child,type);//生成新变量的记录
	insertTable(record);//插入变量
	if(child->next==NULL)
		return;
	ExtDecList(child->next->next,type);
}

Type Specifier(struct Node *node)//返回类型信息
{
	if(!node)
		return NULL;
	struct Node *child=node->child;
	if(strcmp(child->type,"TYPE")==0){//基本类型
		Type t=malloc(sizeof(Type_));
		t->kind=t->BASIC;
		if(strcmp(child->name,"INT")==0)
			t->u.basic=0;//INT
		else
			t->u.basic=1;//FLOAT
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
	if(strcmp(child->type,"Tag")==0){//查表寻找已定义的类型
		child=child->child;//下一层
		if(!child)
			return NULL;
		Record *record=findTable(child->name,2);//查表
		if(!record)
			return NULL;//未定义
		else
			return record->var->type;
	}
	else{//新类型
		char *str=OptTag(child);
		Record *record=malloc(sizeof(Record));
		record->type=2;
		record->var=malloc(sizeof(FieldList_));
		strcpy(record->var->name,str);//名字
		record->var->type=malloc(sizeof(Type_));
		record->var->type->kind=record->var->type->STRUCTURE;
		record->var->type->u.structure=malloc(sizeof(FieldList_));
		strcpy(record->var->type->u.structure->name,str);//类型名
		record->var->type->u.structure->type=malloc(sizeof(Type_));
		record->var->type->u.structure->tail=NULL;
		/*将域初始为空*/
		record->var->type->u.structure->type->kind=record->var->type->u.structure->type->STRUCTURE;
		record->var->type->u.structure->type->u.structure=NULL;
		/*域的开头*/
		FieldList tmp=record->var->type->u.structure->type->u.structure;
		/*定义结构体的域*/
		DefList(child->next->next,tmp,1);//in structure
		if(str!=NULL)
			insertTable(record);//有名结构体，插表
		return record->var->type;
	}
}

char* OptTag(struct Node *node)
{
	if(!node)
		return NULL;
	struct Node *child=node->child;
	if(!child)//空标签
		return NULL;
	char *str=malloc(sizeof(char)*64);
	strcpy(str,child->name);//ID
	return str;
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
		tmp->var=malloc(sizeof(FieldList_));
		strcpy(tmp->var->name,child->name);
		tmp->var->type=type;
		tmp->var->tail=NULL;
		tmp->next=NULL;
		return tmp;
	}
	else{//数组
		Record* tmp=vardec(child,type);
		int num=child->next->next->value.valInt;//数组大小
		Type t=malloc(sizeof(Type_));
		t->kind=t->ARRAY;
		t->u.array.size=num;
		t->u.array.elem=tmp->var->type;
		tmp->var->type=t;//更新类型信息
		return tmp;
	}
}

void FunDec(struct Node *node,Type type)
{
	if(!node)
		return;
	struct Node *child=node->child;
	if(strcmp(child->next->next->type,"RP")==0){//无参函数
		Record* tmp=malloc(sizeof(Record));
		tmp->type=1;//函数
		tmp->func=malloc(sizeof(FuncList_));
		strcpy(tmp->func->name,child->name);
		tmp->func->count=0;
		tmp->func->args=NULL;
		tmp->func->returnType=type;
		insertTable(tmp);//插入函数
	}	
	else{
		Record* tmp=malloc(sizeof(Record));
		tmp->type=1;
		tmp->func=malloc(sizeof(FuncList_));
		strcpy(tmp->func->name,child->name);
		/*获取参数链表*/
		FieldList args=malloc(sizeof(FuncList_));
		VarList(child->next->next,args);
		tmp->func->args=args;
		int count=0;//统计参数个数
		while(args->tail!=NULL){
			++count;
			args=args->tail;
		}
		tmp->func->count=count;
		tmp->func->returnType=type;
		insertTable(tmp);//插入函数
	}
}

void VarList(struct Node *node,FieldList args)
{
	if(!node)
		return; 
	struct Node *child=node->child;
	args=ParamDec(child);
	if(child->next==NULL)
		return;
	VarList(node->next->next,args->tail);
}

FieldList ParamDec(struct Node *node)
{
	if(!node)
		return NULL;
	struct Node *child=node->child;
	Type type=Specifier(child);
	Record *record=VarDec(child->next,type);
	return record->var;//返回变量
}

void CompSt(struct Node *node,Type type)
{
	if(!node)
		return NULL;
	struct Node *child=node->child->next;
	DefList(child,NULL);//不在结构体中
	StmtList(child->next,type);
}

void StmtList(struct Node *node,Type type)
{
	if(!node)
		return;
	struct Node *child=node->child;
	Stmt(child,type);
	StmtList(child->next,type);
}

void Stmt(struct Node *node,Type type)
{
	if(!node)
		return;
	struct Node *child=node->child;
	if(strcmp(child->type,"Exp")==0){
		Exp(child);
	}		
	else if(strcmp(child->type,"CompSt")==0){
		CompSt(child,type);
	}
	else if(strcmp(child->type,"RETURN")==0){
		Type t=Exp(child->next);
		if(!isEqualType(t,type))
			LogError(8);//Error 8: 返回类型不匹配
	}
	else if(strcmp(child->type,"WHILE")==0){
		Exp(child->next->next);
		Stmt(child->next->next->next->next,type);
	}
	else{
		Exp(child->next->next);
		child=child->next->next->next->next;//First Stmt
		Stmt(child,type);
		if(child->next!=NULL)
			Stmt(child->next->next,type);
	}
}

void DefList(struct Node *node,FieldList field,int inStructure)
{
	if(!node)
		return;
	struct Node *child=node->child;
	FieldList fieldNext=Def(child,field,inStructure);//下面的域
	DefList(child->next,fieldNext,inStructure);
}

FieldList Def(struct Node *node,FieldList field,int inStructure)
{
	if(!node)
		return NULL;
	struct Node *child=node->child;
	Type type=Specifier(child);
	return DecList(child->next,type,field,inStructure);
}

FieldList DecList(struct Node *node,Type type,FieldList field,int inStructure)
{
	if(!node)
		return NULL;
	struct Node *child=node->child;
	fieldNext=Dec(child,type,field,inStructure);//下一个域
	if(child->next==NULL)
		return fieldNext;
	else
		return DecList(child->next->next,type,fieldNext,inStructure);
}

FieldList Dec(struct Node *node,Type type,FieldList field,int inStructure)
{
	if(!node)
		return NULL;
	struct Node *child=node->child;
	Record *record=VarDec(child,type);
	if(!inStructure){//不在结构体内
		insertTable(record);
		return NULL;
	}
	if(child->next==NULL){//在结构体内
		field=malloc(sizeof(FieldList_));
		strcpy(field->name,record->var->name);
		field->type=record->var->type;
		field->tail=NULL;
		return field->tail;	
	}
	else{
		Type tmp=Exp(child->next->next);
		if(!isEqualType(type,tmp))
			LogError(5);//Error 5: 赋值类型不匹配
		field=malloc(sizeof(FieldList_));
		strcpy(field->name,record->var->name);
		field->type=record->var->type;
		field->tail=NULL;
		return field->tail;	
	}
}

Type Exp(struct Node *node)
{
	if(!node)
		return NULL;
	struct Node *child=node->child;
	if(child->next!=NULL&&strcmp(child->next->type,"ASSIGNOP")==0){
		Type t1=Exp(child);
		if(t1->kind==t1->BASIC&&t1->u.onlyRight==1){
			LogError(6);//Error 6: 赋值号左边出现常数
			return t1;
		}
		Type t2=Exp(child->next->next);
		if(!isEqualType(t1,t2))
			LogError(5);//Error 5: 赋值类型不匹配
		return t1;
	}	
	else if(child->next!=NULL&&(strcmp(child->next->type,"AND")==0||strcmp(child->next->type,"OR")==0)){
		Type t1=Exp(child);
		Type t2=Exp(child->next->next);
		if(t1->kind!=t1->BASIC||t1->u.basic!=0||t2->kind!=t2->BASIC||t2->u.basic!=0)
			LogError(7);//Error 7: 操作数类型不匹配
		return t1;
	}
	else if(child->next->next!=NULL&&strcmp(child->next->next->type,"Exp")==0){
		Type t1=Exp(child);
		Type t2=Exp(child->next->next);
		if(t1->kind!=t1->BASIC||t2->kind!=t2->BASIC)
			LogError(7);//Error 7
		return t1;
	}
	else if(strcmp(child->type,"MINUS")==0)
		return Exp(child->next);
	else if(strcmp(child->type,"NOT")==0){
		Type t=Exp(child->next);
		if(t->kind!=t->BASIC||t->u.basic==1)
			LogError(7);//Error 7
		return t;
	}
	else if(strcmp(child->type,"ID")==0){
		if(child->next==NULL){
			Record *record=findTable(child->name,0);
			if(!record)
				return NULL;
			else
				return record->var->type;
		}
		else if(strcmp(child->next->next->type,"RP")==0){//无参函数
			Record *record=findTable(child->name,1);
			if(!record)
				return NULL;
			else{
				if(record->func->count!=0)
					LogError(9);//Error 9: 实参形参不匹配
				return record->func->type;
			}
		}
		else{//带参函数
			Record *record=findTable(child->name,1);
			if(!record)
				return NULL;
			else{
				FieldList args=Args(child->next->next);
				FieldList args1=record->func->args;
				while(true){//比较参数类型是否匹配
					if(args==NULL&&args1==NULL)
						break;
					else if(args==NULL||args1==NULL){//参数个数不同
						LogError(9);
						break;
					}
					if(!isEqualType(args->type,args1->type)){//参数类型不同
						LogError(9);
						break;
					}
					args=args->tail;
					args1=args1->tail;
				}
				return record->func->type;
			}
		}
	}
	else if(child->next!=NULL&&strcmp(child->next->type,"LB")==0){
		Type t1=Exp(child);
		if(t1->kind!=t1->ARRAY){
			LogError(10);//Error 10: 对非数组变量采用[]
			return NULL;
		}
		Type t2=Exp(child->next->next);
		if(t2->kind!=t2->BASIC||t2->u.basic==1){
			LogError(12);//Error 12: 数组下标不正确
			return NULL;
		}
		return t1->u.array.elem;
	}
	else if(child->next!=NULL&&strcmp(child->next->type,"DOT")==0){//结构体访问
		Type t=Exp(child);
		if(t->kind!=t->STRUCTURE){
			LogError(13);//Error 13: 对非结构体使用.
			return NULL;
		}
		char fieldName[64];//域名
		strcpy(fieldName,child->next->next->name);
		FieldList field=t->u.structure->type->u.structure;
		while(field){
			if(strcmp(field->name,fieldName)==0)
				return field->type;
			field=field->tail;
		}
		LogError(14);//Error 14: 访问结构体中未定义的域
		return NULL;
	}
	else if(strcmp(child->type,"INT")==0){
		Type t=malloc(sizeof(Type_));
		t->kind=t->BASIC;
		t->u.onlyRight=1;//只有右值
		t->u.basic=0;	
		return t;
	}
	else{
		Type t=malloc(sizeof(Type_));
		t->kind=t->BASIC;
		t->u.onlyRight=1;//只有右值
		t->u.basic=1;	
		return t;
	}
}

FieldList Args(struct Node *node){
	if(!node)
		return NULL;
	struct Node *child=node->child;
	FieldList args=malloc(sizeof(FieldList_));
	args->type=Exp(child);
	args->tail=NULL;
	if(child->next==NULL)
		return args;
	FieldList argsNext=Args(child->next->next);
	args->tail=argsNext;
	return args;
}
