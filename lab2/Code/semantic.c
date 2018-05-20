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
	if(strcmp(node->type,"")==0)//空结点
		return;
	struct Node *child=node->child;
	ExtDef(child);
	ExtDefList(child->next);
}

void ExtDef(struct Node *node)
{
	//printf("In ExtDef\n");
	if(!node)
		return;
	struct Node *child=node->child;
	Type t=Specifier(child);//类型信息
	if(strcmp(child->next->type,"SEMI")==0)
		return;
	else if(strcmp(child->next->type,"ExtDecList")==0)
		ExtDecList(child->next,t);//继承属性
	else{
		FunDec(child->next,t);
		CompSt(child->next->next,t);//检查返回类型错误
	}
}

void ExtDecList(struct Node *node,Type type)//使用type类型定义变量
{
	//printf("In ExtDecList\n");
	if(!node)
		return;
	struct Node *child=node->child;
	struct Record *record=VarDec(child,type);//生成新变量的记录
	insertTable(record,child->line);//插入变量
	if(child->next==NULL)
		return;
	ExtDecList(child->next->next,type);
}

Type Specifier(struct Node *node)//返回类型信息
{
	//printf("In Specifier\n");
	if(!node)
		return NULL;
	struct Node *child=node->child;
	if(strcmp(child->type,"TYPE")==0){//基本类型
		Type t=malloc(sizeof(struct Type_));
		t->kind=0;//BASIC
		if(strcmp(child->name,"int")==0)
			t->u.basic=0;//INT
		else
			t->u.basic=1;//FLOAT
		//printf("BASIC type\n");
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
		struct Record* record=findTable(child->name,2,child->line);//查表
		//printf("%s\n",record->var->name);
		if(!record)
			return NULL;//未定义
		else
			return record->var->type;
	}
	else{//新类型
		char *str=OptTag(child);
		struct Record *record=malloc(sizeof(struct Record));
		record->type=2;
		record->next=NULL;
		record->var=malloc(sizeof(struct FieldList_));
		if(str!=NULL)
			strcpy(record->var->name,str);//名字
		record->var->tail=NULL;
		record->var->type=malloc(sizeof(struct Type_));
		record->var->type->kind=2;
		record->var->type->u.structure=malloc(sizeof(struct FieldList_));
		if(str!=NULL)
			strcpy(record->var->type->u.structure->name,str);//类型名
		record->var->type->u.structure->type=malloc(sizeof(struct Type_));
		record->var->type->u.structure->tail=NULL;
		/*将域初始为空*/
		record->var->type->u.structure->type->kind=2;//STRUCTURE
		record->var->type->u.structure->type->u.structure=NULL;
		/*定义结构体的域*/
		record->var->type->u.structure->type->u.structure=DefList(child->next->next,1);//in structure
		FieldList cur=record->var->type->u.structure->type->u.structure;
		//判断重复域
		while(cur->tail!=NULL){
			FieldList post=cur->tail;
			while(post){
				if(strcmp(cur->name,post->name)==0){
					LogError(15,post->line);//Error 15: 结构体重复域
					break;
				}
				post=post->tail;
			}
			//printf("%s\n",cur->name);
			cur=cur->tail;
		}
		if(str!=NULL)
			insertTable(record,child->line);//有名结构体，插表
		return record->var->type;
	}
}

char* OptTag(struct Node *node)
{
	if(!node)
		return NULL;
	if(strcmp(node->type,"")==0)//空结点
		return NULL;
	struct Node *child=node->child;
	if(!child)//空标签
		return NULL;
	char *str=malloc(sizeof(char)*64);
	strcpy(str,child->name);//ID
	return str;
}

//定义变量
struct Record* VarDec(struct Node *node,Type type)
{
	struct Record* record=vardec(node,type);
	return record;
}
struct Record* vardec(struct Node *node,Type type)
{
	if(!node)
		return NULL;
	struct Node *child=node->child;
	if(strcmp(child->type,"ID")==0){
		struct Record* tmp=malloc(sizeof(struct Record));
		tmp->type=0;//变量
		tmp->var=malloc(sizeof(struct FieldList_));
		strcpy(tmp->var->name,child->name);
		tmp->var->type=type;
		tmp->var->tail=NULL;
		tmp->next=NULL;
		return tmp;
	}
	else{//数组
		struct Record* tmp=vardec(child,type);
		int num=child->next->next->value.valInt;//数组大小
		Type t=malloc(sizeof(struct Type_));
		t->kind=1;
		t->u.array.size=num;
		t->u.array.elem=tmp->var->type;
		tmp->var->type=t;//更新类型信息
		return tmp;
	}
}

void FunDec(struct Node *node,Type type)
{
	//printf("In FunDec\n");
	if(!node)
		return;
	struct Node *child=node->child;
	if(strcmp(child->next->next->type,"RP")==0){//无参函数
		struct Record* tmp=malloc(sizeof(struct Record));
		tmp->type=1;//函数
		tmp->func=malloc(sizeof(struct FuncList_));
		strcpy(tmp->func->name,child->name);
		//printf("Copy Name Success\n");
		tmp->func->count=0;
		tmp->func->args=NULL;
		tmp->func->returnType=type;
		tmp->next=NULL;
		insertTable(tmp,child->line);//插入函数
		//printf("Insert Table Success\n");
	}	
	else{//带参函数
		struct Record* tmp=malloc(sizeof(struct Record));
		tmp->type=1;
		tmp->func=malloc(sizeof(struct FuncList_));
		strcpy(tmp->func->name,child->name);
		/*获取参数链表*/
		FieldList args=NULL;
		VarList(child->next->next,args);
		tmp->func->args=args;
		int count=0;//统计参数个数
		while(args!=NULL){
			++count;
			args=args->tail;
		}
		tmp->func->count=count;
		//printf("%d\n",count);
		tmp->func->returnType=type;
		insertTable(tmp,child->line);//插入函数
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
	struct Record *record=VarDec(child->next,type);
	/*函数形参也要插入符号表*/
	insertTable(record,child->line);
	return record->var;//返回变量
}

void CompSt(struct Node *node,Type type)
{
	//printf("In CompSt\n");
	if(!node)
		return;
	struct Node *child=node->child->next;
	DefList(child,0);//不在结构体中
	StmtList(child->next,type);
}

void StmtList(struct Node *node,Type type)
{
	//printf("In StmtList\n");
	if(!node)
		return;
	if(strcmp(node->type,"")==0)//空结点
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
		if(!t)
			return;
		if(!isEqualType(t,type))
			LogError(8,child->line);//Error 8: 返回类型不匹配
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

FieldList DefList(struct Node *node,int inStructure)
{
	//printf("In DefList\n");
	if(!node)
		return NULL;
	if(strcmp(node->type,"")==0)//空结点
		return NULL;
	struct Node *child=node->child;
	FieldList result=Def(child,inStructure);//结果
	if(!inStructure)
		return DefList(child->next,inStructure);
	FieldList cur=result;
	if(!cur)
		return NULL;
	while(cur->tail!=NULL) 
		cur=cur->tail;
	cur->tail=DefList(child->next,inStructure);//尾部添加
	return result;
}

FieldList Def(struct Node *node,int inStructure)
{
	//printf("In Def\n");
	if(!node)
		return NULL;
	struct Node *child=node->child;
	Type type=Specifier(child);
	return DecList(child->next,type,inStructure);
}

FieldList DecList(struct Node *node,Type type,int inStructure)
{
	//printf("In DecList\n");
	if(!node)
		return NULL;
	struct Node *child=node->child;
	FieldList result=Dec(child,type,inStructure);
	if(child->next==NULL)
		return result;
	else{
		if(!inStructure)
			return DecList(child->next->next,type,inStructure);
		FieldList cur=result;
		if(!cur)
			return result;
		while(cur->tail!=NULL)
			cur=cur->tail;
		cur->tail=DecList(child->next->next,type,inStructure);
	}	
	return result;
}

FieldList Dec(struct Node *node,Type type,int inStructure)
{
	//printf("In Dec\n");
	if(!node)
		return NULL;
	struct Node *child=node->child;
	struct Record *record=VarDec(child,type);
	//printf("%s\n",record->var->name);
	if(!inStructure){//不在结构体内
		insertTable(record,child->line);
		if(child->next!=NULL){
			Type tmp=Exp(child->next->next);
			if(!tmp)
				return NULL;	
			//printf("May Appear\n");
			if(!isEqualType(type,tmp))
				LogError(5,child->line);//Error 5
			//printf("Should Appear\n");
		}
		return NULL;
	}
	else{//在结构体内
		if(child->next==NULL){
			FieldList result=malloc(sizeof(struct FieldList_));
			strcpy(result->name,record->var->name);
			result->type=record->var->type;
			result->tail=NULL;
			result->line=child->line;
			return result;	
		}
		else{
			LogError(15,child->line);//Error 15: 为结构体域赋值
			FieldList result=malloc(sizeof(struct FieldList_));
			strcpy(result->name,record->var->name);
			result->type=record->var->type;
			result->tail=NULL;
			result->line=child->line;
			return result;	
		}
	}
}

Type Exp(struct Node *node)
{
	//printf("In Exp\n");
	if(!node)
		return NULL;
	struct Node *child=node->child;
	if(child->next!=NULL&&strcmp(child->next->type,"ASSIGNOP")==0){
		Type t1=Exp(child);
		if(!t1)
			return NULL;
		if(t1->kind==0&&t1->onlyRight==1){
			LogError(6,child->line);//Error 6: 赋值号左边出现常数
			return t1;
		}
		Type t2=Exp(child->next->next);
		if(!t2)
			return NULL;
		if(!isEqualType(t1,t2))
			LogError(5,child->line);//Error 5: 赋值类型不匹配
		return t1;
	}	
	else if(child->next!=NULL&&(strcmp(child->next->type,"AND")==0||strcmp(child->next->type,"OR")==0)){
		Type t1=Exp(child);
		Type t2=Exp(child->next->next);
		if(!t1||!t2)
			return NULL;
		if(t1->kind!=0||t1->u.basic!=0||t2->kind!=0||t2->u.basic!=0)
			LogError(7,child->line);//Error 7: 操作数类型不匹配
		return t1;
	}
	else if(strcmp(child->type,"MINUS")==0)
		return Exp(child->next);
	else if(strcmp(child->type,"NOT")==0){
		Type t=Exp(child->next);
		if(!t)
			return NULL;
		if(t->kind!=0||t->u.basic==1)
			LogError(7,child->line);//Error 7
		return t;
	}
	else if(strcmp(child->type,"ID")==0){
		if(child->next==NULL){
			struct Record *record=findTable(child->name,0,child->line);
			if(!record)
				return NULL;
			else
				return record->var->type;
		}
		else if(strcmp(child->next->next->type,"RP")==0){//无参函数
			struct Record *record=findTable(child->name,1,child->line);
			if(!record)
				return NULL;
			else{
				if(record->func->count!=0)
					LogError(9,child->line);//Error 9: 实参形参不匹配
				return record->func->returnType;
			}
		}
		else{//带参函数
			struct Record *record=findTable(child->name,1,child->line);
			if(!record)
				return NULL;
			else{
				FieldList args=Args(child->next->next);
				FieldList args1=record->func->args;
				while(1){//比较参数类型是否匹配
					//printf("~~~\n");
					if(args==NULL&&args1==NULL)
						break;
					else if(args==NULL||args1==NULL){//参数个数不同
						LogError(9,child->line);
						break;
					}
					if(!isEqualType(args->type,args1->type)){//参数类型不同
						LogError(9,child->line);
						break;
					}
					printf("Should Appear\n");
					args=args->tail;
					args1=args1->tail;
				}
				return record->func->returnType;
			}
		}
	}
	else if(child->next!=NULL&&strcmp(child->next->type,"LB")==0){
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
		if(t2->kind!=0||t2->u.basic==1){
			LogError(12,child->line);//Error 12: 数组下标不正确
			return NULL;
		}
		return t1->u.array.elem;
	}
	else if(child->next!=NULL&&strcmp(child->next->type,"DOT")==0){//结构体访问
		Type t=Exp(child);
		if(!t)
			return NULL;
		if(t->kind!=2){
			LogError(13,child->line);//Error 13: 对非结构体使用.
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
		LogError(14,child->line);//Error 14: 访问结构体中未定义的域
		return NULL;
	}
	else if(strcmp(child->type,"INT")==0){
		Type t=malloc(sizeof(struct Type_));
		t->kind=0;
		t->onlyRight=1;//只有右值
		t->u.basic=0;	
		return t;
	}
	else if(strcmp(child->type,"FLOAT")==0){
		Type t=malloc(sizeof(struct Type_));
		t->kind=0;
		t->onlyRight=1;//只有右值
		t->u.basic=1;	
		return t;
	}
	else{
		Type t1=Exp(child);
		Type t2=Exp(child->next->next);
		if(!t1||!t2)
			return NULL;
		if(t1->kind!=0||t2->kind!=0)
			LogError(7,child->line);//Error 7
		if(t1->u.basic!=t2->u.basic)
			LogError(7,child->line);
		return t1;
	}
}

FieldList Args(struct Node *node){
	if(!node)
		return NULL;
	struct Node *child=node->child;
	FieldList args=malloc(sizeof(struct FieldList_));
	args->type=Exp(child);
	args->tail=NULL;
	if(child->next==NULL)
		return args;
	args->tail=Args(child->next->next);
	return args;
}
