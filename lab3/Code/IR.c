#include"IR.h"

void IR(struct Node *root)
{
	if(!root)
		return;
	initIR();
	trsExtDefList(root->child);
}

void trsExtDefList(struct Node *node)
{
	if(strcmp(node->type,"")==0)
		return;
	struct Node *child=node->child;
	trsExtDef(child);
	trsExtDefList(child->next);
}

void trsExtDef(struct Node *node)
{
	struct Node *child=node->child;
	if(strcmp(child->next->type,"FunDec"))
		return;
	trsFunDec(child->next);
	trsCompSt(child->next->next);
}

void trsExtDecList(struct Node *node){
	return;
}

void trsSpecifier(struct Node *node)
{
	struct Node *child=node->child;
	if(!strcmp(child->type,"TYPE"))
		return;
	printf("Error: Can't Translate Structure Type\n");
}

void trsStructSpecifier(struct Node *node){
	return;
}

void trsOptTag(struct Node *node){
	return;
}

void trsTag(struct Node *node){
	return;
}

void trsVarDec(struct Node *node)
{
	struct Node *child=node->child;
	if(!strcmp(child->type,"ID")){
			
	}
}

void trsFunDec(struct Node *node)
{
	struct Node *child=node->child;
	if(!strcmp(child->next->next,"RP")){
		output(cat(3,"FUNCTION ",child->name," :\n"));	
	}
	else{
		trsVarList(child->next->next);	
		output(cat(3,"FUNCTION ",child->name," :\n"));	
	}
}

void trsVarList(struct Node *node)
{
	struct Node *child=node->child;
	trsParamDec(child);
	if(!child->next)
		return;
	trsVarList(child->next->next);
}

void trsParamDec(struct Node *node)
{
	struct Node *child=node->child;
	trsSpecifier(child);
	trsVarDec(child->next);
}

void trsCompSt(struct Node *node)
{
	struct Node *child=node->child->next;
	trsDefList(child);
	trsStmtList(child->next);
}

void trsStmtList(struct Node *node)
{
	if(!strcmp(node->type,""))
		return;
	struct Node *child=node->child;
	trsStmt(child);
	trsStmtList(child->next);
}

void trsStmt(struct Node *node)
{
	struct Node *child=node->child;
	if(!strcmp(child->type,"Exp"))
		trsExp(child);
	else if(!strcmp(child->type,"CompSt"))
		trsCompSt(child);
	else if
}

void trsExp(struct Node *node)
{
	struct Node *child=node->child;
	if(child->next && !strcmp(child->next->type,"ASSIGNOP")){
		
	}
	else if(child->next && (!strcmp(child->next->type,"AND")
						    ||!strcmp(child->next->type,"OR")
							||!strcmp(child->next->type,"RELOP"))
		   ){//逻辑比较
		l1=newLabel();
		l2=newLabel();
		output(place := #0);
		trsCond(node,l1,l2);
		output(LABEL l1 place := #1);	
	}
	else if(child->next && (!strcmp(child->next->type,"PLUS")
						    ||!strcmp(child->next->type,"MINUS")
							||!strcmp(child->next->type,"STAR")
							||!strcmp(child->next->type,"DIV"))
		   ){//算数运算
		t1=newTemp();
		t2=newTemp();
		trsExp(child,t1);
		trsExp(child->next->next,t2);
		output(cat(place := t1 + t2))	;
	}
	else if(!strcmp(child->type,"LP"))
		trsExp(child->next);
	else if(!strcmp(child->type,"NOT")){
	}
}

void initIR()
{
	Type type=malloc(sizeof(struct Type_));
	type->kind=0;
	type->basic=0;
	//添加read函数
	FuncList read=malloc(sizeof(struct FuncList_));
	strcpy(read->name,"read");
	read->count=0;
	read->args=NULL;
	read->returnType=type;
	struct Record *record=malloc(sizeof(struct Record));
	record->type=1;
	record->func=read;
	record->next=NULL:
	insertTable(record,-1);
	//添加write函数
	FuncList write=malloc(sizeof(struct FuncList_));
	strcpy(write->name,"write");
	write->count=1;
	FieldList args=malloc(sizeof(struct FieldList_));
	args->name=NULL;
	args->type=type;
	args->tail=NULL;
	read->args=args;
	read->returnType=type;
	record=malloc(sizeof(struct Record));
	record->type=1;
	record->func=write;
	record->next=NULL:
	insertTable(record,-1);
}
