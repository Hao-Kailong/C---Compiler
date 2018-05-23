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
	if(strcmp(node->type,"")==0)
		return;//空结点
	struct Node *child=node->child;
	ExtDef(child);
	ExtDefList(child->next);
}

void ExtDef(struct Node *node)
{
	struct Node *child=node->child;
	Type type=Specifier(child);//类型信息
	if(strcmp(child->next->type,"SEMI")==0)
		return;
	else if(strcmp(child->next->type,"ExtDecList")==0)
		ExtDecList(child->next,type);//继承属性
	else{
		FunDec(child->next,type);
		CompSt(child->next->next,type);//检查返回类型错误
	}
}

void ExtDecList(struct Node *node,Type type)
{
	struct Node *child=node->child;
	struct Record *record=VarDec(child,type);
	insertTable(record,child->line);
	if(child->next==NULL)
		return;
	ExtDecList(child->next->next,type);
}

Type Specifier(struct Node *node)//返回类型信息
{
	struct Node *child=node->child;
	if(strcmp(child->type,"TYPE")==0) {
		Type type=malloc(sizeof(struct Type_));
		type->kind=0;//BASIC
		if(strcmp(child->name,"int")==0)
			type->basic=0;//INT
		else
			type->basic=1;//FLOAT
		return type;
	}
	else
		return StructSpecifier(child);
}

Type StructSpecifier(struct Node *node)
{
	struct Node *child=node->child->next;
	if(strcmp(child->type,"Tag")==0){//查表寻找已定义的类型
		child=child->child;
		struct Record* record=findTable(child->name,2,child->line);
		if(!record)
			return NULL;//未定义
		/*返回结构体*/
		Type type=malloc(sizeof(struct Type_));
		type->kind=2;
		type->structure=record->var;
		return type;
	}
	else {
		char *str=malloc(sizeof(char)*64);
		if(!OptTag(child))
			str=NULL;//无名结构体
		else
			strcpy(str,OptTag(child));
		/*定义新的结构体*/
		if(str){
			struct Record *record=malloc(sizeof(struct Record));
			record->type=2;
			record->next=NULL;
			record->var=malloc(sizeof(struct FieldList_));
			strcpy(record->var->name,str);//结构体名字
			record->var->tail=NULL;
			record->var->type=malloc(sizeof(struct Type_));
			record->var->type->kind=2;
			/*构造域*/
			record->var->type->structure=DefList(child->next->next,1);
			insertTable(record,child->line);
			/*返回结构体*/
			Type type=malloc(sizeof(struct Type_));
			type->kind=2;
			type->structure=record->var;
			return type;
		}
		else{
			/*返回结构体*/
			Type type=malloc(sizeof(struct Type_));
			type->kind=2;
			type->structure=malloc(sizeof(struct FieldList_));
			strcpy(type->structure->name,"");//无名
			type->structure->tail=NULL;
			/*构造域*/
			type->structure->type=malloc(sizeof(struct Type_));
			type->structure->type->kind=2;
			type->structure->type->structure=DefList(child->next->next,1);
			return type;
		}
	}
}

char* OptTag(struct Node *node)
{
	if(strcmp(node->type,"")==0)
		return NULL;//空结点
	struct Node *child=node->child;
	return child->name;
}

//定义变量
struct Record* VarDec(struct Node *node,Type type)
{
	struct Node *child=node->child;
	if(strcmp(child->type,"ID")==0){
		struct Record* record=malloc(sizeof(struct Record));
		record->type=0;
		record->var=malloc(sizeof(struct FieldList_));
		strcpy(record->var->name,child->name);
		record->var->type=type;
		record->var->tail=NULL;
		record->next=NULL;
		return record;
	}
	else{//数组
		struct Record* record=VarDec(child,type);
		int num=child->next->next->valueInt;//数组大小
		//更新类型
		Type t=malloc(sizeof(struct Type_));
		t->kind=1;
		t->array.size=num;
		t->array.elem=record->var->type;
		//更新记录
		record->var->type=t;
		return record;
	}
}

void FunDec(struct Node *node,Type type)
{
	struct Node *child=node->child;
	if(strcmp(child->next->next->type,"RP")==0){//无参函数
		struct Record* record=malloc(sizeof(struct Record));
		record->type=1;
		record->func=malloc(sizeof(struct FuncList_));
		record->next=NULL;
		strcpy(record->func->name,child->name);
		record->func->count=0;
		record->func->args=NULL;
		record->func->returnType=type;
		insertTable(record,child->line);
	}	
	else {
		struct Record* record=malloc(sizeof(struct Record));
		record->type=1;
		record->func=malloc(sizeof(struct FuncList_));
		record->next=NULL;
		strcpy(record->func->name,child->name);
		/*获取参数链表*/
		FieldList args=VarList(child->next->next);
		FieldList cur=args;
		int count=0;
		while(cur){
			++count;
			cur=cur->tail;
		}
		record->func->args=args;
		record->func->count=count;
		record->func->returnType=type;
		insertTable(record,child->line);
	}
}

/*构造形参并返回头域*/
FieldList VarList(struct Node *node)
{
	struct Node *child=node->child;
	FieldList args=ParamDec(child);
	if(child->next==NULL){
		args->tail=NULL;
		return args;
	}
	args->tail=VarList(child->next->next);
	return args;
}

/*构造当前形参并返回*/
FieldList ParamDec(struct Node *node)
{
	struct Node *child=node->child;
	Type type=Specifier(child);
	struct Record *record=VarDec(child->next,type);
	insertTable(record,child->line);
	FieldList args=record->var;//形参即变量
	return args;
}

void CompSt(struct Node *node,Type type)
{
	struct Node *child=node->child->next;
	DefList(child,0);
	StmtList(child->next,type);
}

void StmtList(struct Node *node,Type type)
{
	if(strcmp(node->type,"")==0)
		return;//空结点
	struct Node *child=node->child;
	Stmt(child,type);
	StmtList(child->next,type);
}

void Stmt(struct Node *node,Type type)
{
	struct Node *child=node->child;
	if(strcmp(child->type,"Exp")==0)
		Exp(child);
	else if(strcmp(child->type,"CompSt")==0)
		CompSt(child,type);
	else if(strcmp(child->type,"RETURN")==0){
		Type tmp=Exp(child->next);
		if(!isEqualType(tmp,type))
			LogError(8,child->line);//Error 8: 返回类型不匹配
	}
	else if(strcmp(child->type,"WHILE")==0){
		Exp(child->next->next);
		Stmt(child->next->next->next->next,type);
	}
	else{
		Exp(child->next->next);
		child=child->next->next->next->next;
		Stmt(child,type);
		if(!child->next)
			return;
		Stmt(child->next->next,type);
	}
}

/*返回头域*/
FieldList DefList(struct Node *node,int inStructure)
{
	if(strcmp(node->type,"")==0)
		return NULL;
	struct Node *child=node->child;
	/*构造当前语句*/
	FieldList head=Def(child,inStructure);
	/*构造下一语句*/
	FieldList cur=head;
	while(cur->tail)
		cur=cur->tail;
	cur->tail=DefList(child->next,inStructure);
	return head;
}

/*构造当前语句，返回头域*/
FieldList Def(struct Node *node,int inStructure)
{
	struct Node *child=node->child;
	Type type=Specifier(child);
	return DecList(child->next,type,inStructure);
}

/*返回头域*/
FieldList DecList(struct Node *node,Type type,int inStructure)
{
	struct Node *child=node->child;
	/*构造当前域*/
	FieldList field=Dec(child,type,inStructure);
	/*构造下一域*/
	if(child->next==NULL){
		field->tail=NULL;
		return field;
	}	
	field->tail=DecList(child->next->next,type,inStructure);
	return field;
}

/*构造当前域并返回*/
FieldList Dec(struct Node *node,Type type,int inStructure)
{
	struct Node *child=node->child;
	struct Record *record=VarDec(child,type);
	insertTable(record,child->line);//插入符号表
	FieldList field=record->var;//域即变量
	if(!child->next)
		return field;
	if(!inStructure){
		Type tmp=Exp(child->next->next);
		if(!isEqualType(type,tmp))
			LogError(5,child->line);//Error 5
		return field;
	}
	LogError(15,child->line);//Error 15: 为结构体域赋值
	return field;
}

int hasLeft(struct Node *exp)
{
	struct Node *child=exp->child;
	if(strcmp(child->type,"ID")==0 && child->next==NULL)
		return 1;
	if(strcmp(child->type,"LP")==0)
		return hasLeft(child->next);
	if(strcmp(child->type,"Exp")==0){
		if(strcmp(child->next->type,"ASSIGNOP")==0)
			return hasLeft(child);
		if(strcmp(child->next->type,"LB")==0)
			return 1;
		if(strcmp(child->next->type,"DOT")==0)
			return 1;
	}	
	return 0;
}

/*返回类型，出错返回NULL*/
Type Exp(struct Node *node)
{
	struct Node *child=node->child;
	if(child->next!=NULL&&strcmp(child->next->type,"ASSIGNOP")==0){
		if(!hasLeft(child)){ 
			LogError(6,child->line);//Error 6: 无左值
			return NULL;
		}
		Type t1=Exp(child);
		Type t2=Exp(child->next->next);
		if(!t1||!t2)
			return NULL;
		if(!isEqualType(t1,t2)){
			LogError(5,child->line);//Error 5: 赋值类型不匹配
			return NULL;
		}
		return t1;
	}	
	else if(child->next!=NULL
				&&( strcmp(child->next->type,"AND")==0
					||strcmp(child->next->type,"OR")==0 )
   	)//逻辑运算
	{
		Type t1=Exp(child);
		Type t2=Exp(child->next->next);
		if(!t1||!t2)
			return NULL;
		if(t1->kind!=0||t1->basic!=0||t2->kind!=0||t2->basic!=0){
			LogError(7,child->line);//Error 7: 操作数操作符不匹配
			return NULL;
		}
		return t1;
	}
	else if(child->next!=NULL
				&&( strcmp(child->next->type,"PLUS")==0
					||strcmp(child->next->type,"MINUS")==0 
					||strcmp(child->next->type,"STAR")==0 
					||strcmp(child->next->type,"DIV")==0 
					||strcmp(child->next->type,"RELOP")==0 )
   	)//算数运算
	{
		Type t1=Exp(child);
		Type t2=Exp(child->next->next);
		if(!t1||!t2)
			return NULL;
		if(t1->kind!=0||t2->kind!=0){
			LogError(7,child->line);//Error 7
			return NULL;
		}
		if(t1->basic!=t2->basic){
			LogError(7,child->line);//Error 7
			return NULL;
		}
		return t1;
	}
	else if(strcmp(child->type,"LP")==0)
		return Exp(child->next);
	else if(strcmp(child->type,"MINUS")==0){
		Type t=Exp(child->next);
		if(!t)
			return NULL;
		if(t->kind!=0){
			LogError(7,child->line);//Error 7
			return NULL;
		}
		return t;
	}
	else if(strcmp(child->type,"NOT")==0){
		Type t=Exp(child->next);
		if(!t)
			return NULL;
		if(t->kind!=0||t->basic!=0){
			LogError(7,child->line);//Error 7
			return NULL;
		}
		return t;
	}
	else if(child->next!=NULL && strcmp(child->type,"ID")==0){
		if(strcmp(child->next->next->type,"RP")==0){//无参函数
			struct Record *record=findTable(child->name,1,child->line);
			if(!record)
				return NULL;
			if(record->func->count!=0){
				LogError(9,child->line);//Error 9: 实参形参不匹配
				return NULL;
			}
			return record->func->returnType;
		}
		else{
			struct Record *record=findTable(child->name,1,child->line);
			if(!record)
				return NULL;
			FieldList args1=record->func->args;
			FieldList args2=Args(child->next->next);
			while(1){//比较参数是否匹配
				if(args1==NULL&&args2==NULL)
					break;
				if(args1==NULL||args2==NULL){//个数不同
					LogError(9,child->line);
					return NULL;
				}
				if(!isEqualType(args1->type,args2->type)){//类型不同
					LogError(9,child->line);
					return NULL;
				}
				args1=args1->tail;
				args2=args2->tail;
			}
			return record->func->returnType;
		}
	}
	else if(child->next!=NULL && strcmp(child->next->type,"LB")==0){
		Type t1=Exp(child);
		if(!t1)
			return NULL;
		if(t1->kind!=1){
			LogError(10,child->line);//Error 10: 对非数组变量采用[]
			return NULL;
		}
		Type t2=Exp(child->next->next);
		if(!t2)
			return NULL;
		if(t2->kind!=0||t2->basic!=0){
			LogError(12,child->line);//Error 12: 数组下标不正确
			return NULL;
		}
		return t1->array.elem;
	}
	else if(child->next!=NULL && strcmp(child->next->type,"DOT")==0){
		Type t=Exp(child);
		if(!t)
			return NULL;
		if(t->kind!=2){
			LogError(13,child->line);//Error 13: 对非结构体使用.
			return NULL;
		}
		char *fieldName=malloc(sizeof(char)*64);
		strcpy(fieldName,child->next->next->name);
		FieldList field=t->structure->type->structure;
		while(field){
			if(strcmp(field->name,fieldName)==0)
				return field->type;
			field=field->tail;
		}
		LogError(14,child->line);//Error 14: 未定义的域
		return NULL;
	}
	else if(strcmp(child->type,"ID")==0){
		struct Record* record=findTable(child->name,0,child->line);
		if(!record)
			return NULL;
		return record->var->type;
	}
	else if(strcmp(child->type,"INT")==0){
		Type t=malloc(sizeof(struct Type_));
		t->kind=0;
		t->basic=0;
		t->FINAL=1;	
		return t;
	}
	else if(strcmp(child->type,"FLOAT")==0){
		Type t=malloc(sizeof(struct Type_));
		t->kind=0;
		t->basic=1;
		t->FINAL=1;	
		return t;
	}
}

/*返回实参列表*/
FieldList Args(struct Node *node){
	struct Node *child=node->child;
	FieldList args=malloc(sizeof(struct FieldList_));
	args->type=Exp(child);
	if(child->next==NULL){
		args->tail=NULL;
		return args;
	}
	args->tail=Args(child->next->next);
	return args;
}
