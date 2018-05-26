#include"IR.h"

void IR(struct Node *root)
{
	if(!root)
		return;
	initIR();
	trsExtDefList(root->child);
	IRprint(head);//打印中间代码
}

void trsExtDefList(struct Node *node)
{
	if(!strcmp(node->type,""))
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

void trsSpecifier(struct Node *node)
{
	struct Node *child=node->child;
	if(!strcmp(child->type,"TYPE"))
		return;
	printf("Error: Can't Translate Structure Type!\n");
}

char* trsVarDec(struct Node *node,int level,int flag)
{
	static int size=4;//空间
	struct Node *child=node->child;
	if(!strcmp(child->type,"ID"))
		return child->name;
	char* name=trsVarDec(child,level+1,flag);
	int num=child->next->next->valueInt;
    size *= num;	
	if(level)
		return name;
	//添加代码
	Operand o0=newOprStr(name);
	char *str=malloc(sizeof(char)*64);
	sprintf(str,"%d",size);
	Operand o1=newOprStr(str);
	InterCode code=newCode(4,o0,o1,NULL,NULL);//DEC
	if(flag)
		addCode(code);
	return name;
}

void trsFunDec(struct Node *node)
{
	struct Node *child=node->child;
	Operand o0=newOprStr(child->name);
	InterCode code=newCode(1,o0,NULL,NULL,NULL);//FUNCTION
	addCode(code);
	if(!strcmp(child->next->next->type,"RP"))
		return;
	trsVarList(child->next->next);	
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
	//添加代码
	char *name=trsVarDec(child->next,0,0);
	Operand o0=newOprStr(name);
	InterCode code=newCode(16,o0,NULL,NULL,NULL);//PARAM
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
		trsExp(child,NULL);
	else if(!strcmp(child->type,"CompSt"))
		trsCompSt(child);
	else if(!strcmp(child->type,"RETURN")){
		Operand t=newOprRnd();
		trsExp(child->next,t);
		InterCode code=newCode(12,t,NULL,NULL,NULL);//RTN
		addCode(code);
	}
	else if(!strcmp(child->type,"WHILE")){
		Operand l1=newOprRnd();
		Operand l2=newOprRnd();
		Operand l3=newOprRnd();	
		addCode(newCode(0,l1,NULL,NULL,NULL));//LABEL
		trsCond(child->next->next,l2,l3);
		addCode(newCode(0,l2,NULL,NULL,NULL));
		trsStmt(child->next->next->next->next);
		addCode(newCode(10,l1,NULL,NULL,NULL));//GOTO
		addCode(newCode(0,l3,NULL,NULL,NULL));
	} 
	else{
		child=child->next->next;//Exp
		if(!child->next->next->next){
			Operand l1=newOprRnd();
			Operand l2=newOprRnd();
			trsCond(child,l1,l2);
			addCode(newCode(0,l1,NULL,NULL,NULL));
			trsStmt(child->next->next);
			addCode(newCode(0,l2,NULL,NULL,NULL));
		}
		else{
			Operand l1=newOprRnd();
			Operand l2=newOprRnd();
			Operand l3=newOprRnd();
			trsCond(child,l1,l2);
			addCode(newCode(0,l1,NULL,NULL,NULL));
			trsStmt(child->next->next);
			addCode(newCode(10,l3,NULL,NULL,NULL));
			addCode(newCode(0,l2,NULL,NULL,NULL));
			trsStmt(child->next->next->next->next);
			addCode(newCode(0,l3,NULL,NULL,NULL));
		}
	}
}

void trsDefList(struct Node *node)
{
	if(!strcmp(node->type,""))
		return;
	struct Node *child=node->child;
	trsDef(child);
	trsDefList(child->next);
}

void trsDef(struct Node *node)
{
	struct Node *child=node->child;
	trsSpecifier(child);
	trsDecList(child->next);
}

void trsDecList(struct Node *node)
{
	struct Node *child=node->child;
	trsDec(child);
	if(!child->next)
		return;
	trsDecList(child->next->next);
}

void trsDec(struct Node *node)
{
	struct Node *child=node->child;
	char *name=trsVarDec(child,0,1);
	if(!child->next)
		return;
	Operand o1=newOprRnd();
	trsExp(child->next->next,o1);
	Operand o0=newOprStr(name);
	InterCode code=newCode(2,o0,o1,NULL,NULL);//ASSIGN
	addCode(code);
}

//返回结果
Operand trsExp(struct Node *node,Operand place)
{
	struct Node *child=node->child;
	if(child->next && !strcmp(child->next->type,"ASSIGNOP")){
		if(!strcmp(child->child->type,"ID")){
			Operand t1=newOprRnd();
			trsExp(child->next->next,t1);
			Operand t2=newOprStr(child->child->name);
			addCode(newCode(2,t2,t1,NULL,NULL));
			addCode(newCode(2,place,t2,NULL,NULL));
			return place;
		}
		else if(!strcmp(child->child->type,"LP")){
			Operand t1=newOprRnd();
			trsExp(child->next->next,t1);
			Operand t2=trsExp(child,NULL);
			addCode(newCode(2,t2,t1,NULL,NULL));
			addCode(newCode(2,place,t2,NULL,NULL));
			return place;
		}
		else{//数组
			Operand t1=newOprRnd();
			trsExp(child->next->next,t1);
			Operand t2=trsExp(child,NULL);
			addCode(newCode(2,t2,t1,NULL,NULL));
			addCode(newCode(2,place,t2,NULL,NULL));
			return place;
		}
	}
	else if( (child->next && (!strcmp(child->next->type,"AND")
						     ||!strcmp(child->next->type,"OR")
							 ||!strcmp(child->next->type,"RELOP"))
		   ) || !strcmp(child->type,"NOT") 
	){//逻辑比较
		Operand l1=newOprRnd();
		Operand l2=newOprRnd();
		Operand num0=newOprStr("#0");
		addCode(newCode(2,place,num0,NULL,NULL));
		trsCond(node,l1,l2);
		addCode(newCode(0,l1,NULL,NULL,NULL));
		Operand num1=newOprStr("#1");
		addCode(newCode(2,place,num1,NULL,NULL));
		addCode(newCode(0,l2,NULL,NULL,NULL));
		return place;
	}
	else if(child->next && (!strcmp(child->next->type,"PLUS")
						    ||!strcmp(child->next->type,"MINUS")
							||!strcmp(child->next->type,"STAR")
							||!strcmp(child->next->type,"DIV"))
		   ){//算数运算
		Operand t1=newOprRnd();
		Operand t2=newOprRnd();
		trsExp(child,t1);
		trsExp(child->next->next,t2);
		if(!strcmp(child->next->type,"PLUS"))
			addCode(newCode(3,place,t1,t2,NULL));//ADD
		else if(!strcmp(child->next->type,"PLUS"))
			addCode(newCode(4,place,t1,t2,NULL));//SUB
		else if(!strcmp(child->next->type,"PLUS"))
			addCode(newCode(5,place,t1,t2,NULL));//MULT
		else
			addCode(newCode(6,place,t1,t2,NULL));//DV
		return place;
	}
	else if(!strcmp(child->type,"LP"))
		return trsExp(child->next,place);
	else if(!strcmp(child->type,"MINUS")){
		Operand t1=newOprRnd();
		trsExp(child->next,t1);
		Operand num0=newOprStr("#0");
		addCode(newCode(4,place,num0,t1,NULL));
		return place;
	}
	else if(child->next && !strcmp(child->type,"ID")){
		if(!strcmp(child->next->next->type,"RP")){//无参函数
			if(!strcmp(child->name,"read")){
				addCode(newCode(17,place,NULL,NULL,NULL));//READ
				return place;
			}
			Operand t=newOprStr(child->name);
			addCode(newCode(15,place,t,NULL,NULL));//CALL
			return place;
		}
		else{
			Operand args=trsArgs(child->next->next,NULL);
			if(!strcmp(child->name,"write")){
				addCode(newCode(18,args,NULL,NULL,NULL));//WRITE
				return newOprStr("#0");//write函数返回0
			}
			while(args){
				addCode(newCode(14,args,NULL,NULL,NULL));//ARG
				args=args->next;
			}
			Operand t=newOprStr(child->name);
			addCode(newCode(15,place,t,NULL,NULL));
			return place;
		}
	}
	else if(child->next && !strcmp(child->next->type,"LB")){//返回值
		Operand head=NULL;//可优化
		while(child->next){
			Operand tmp=newOprRnd();
			trsExp(child->next->next,tmp);
			tmp->next=head;
			head=tmp;
			child=child->child;
		}
		Operand result=newOprRnd();//偏移
		addCode(newCode(5,result,head,newOprStr("#4"),NULL));
		head=head->next;
		while(head){
			Operand tmp=newOprRnd();
			addCode(newCode(5,tmp,result,head,NULL));
			result=tmp;
			head=head->next;
		}
		Operand addr=newOprRnd();//地址
		Operand id=newOprStr(child->name);
		addCode(newCode(7,addr,id,NULL,NULL));//ADDRESSASSIGN
		Operand realAddr=newOprRnd();
		addCode(newCode(3,realAddr,addr,result,NULL));
		addCode(newCode(8,place,realAddr,NULL,NULL));//STARASSIGN
		return place;
	}
	else if(child->next && !strcmp(child->next->type,"DOT")){
		printf("Error: Can't Translate Structure Type!\n");
		return place;
	}
	else if(!strcmp(child->type,"ID")){
		struct Record *record=findTable(child->name,0,-1);
		Operand t=newOprStr(child->name);
		if(record->var->type->kind==0)
			addCode(newCode(2,place,t,NULL,NULL));
		else if(record->var->type->kind==1)
			addCode(newCode(7,place,t,NULL,NULL));
		else
			printf("Error: Can't Translate Structure Type!\n");
		return place;
	}
	else{//INT FLOAT
		char *str=malloc(sizeof(char)*64);
		str=strcat("#",child->name);
		Operand t=newOprStr(str);
		addCode(newCode(2,place,t,NULL,NULL));
		return place;
	}
}

//返回实参列表
Operand trsArgs(struct Node *node,Operand args)
{
	struct Node *child=node->child;
	Operand t=newOprRnd();
	trsExp(child,t);
	t->next=args;
	if(!child->next)
		return t;
	return trsArgs(child->next->next,t);
}

void trsCond(struct Node *node,Operand labelt,Operand labelf)
{
	struct Node *child=node->child;
	if(child->next && !strcmp(child->next->type,"RELOP")){
		Operand t1=newOprRnd();
		Operand t2=newOprRnd();
		trsExp(child,t1);
		trsExp(child->next->next,t2);
		Operand op=newOprStr(child->next->name);
		addCode(newCode(11,t1,t2,labelt,op));//COND
		addCode(newCode(10,labelf,NULL,NULL,NULL));//GOTO
	}
	else if(!strcmp(child->type,"NOT"))
		trsCond(child,labelf,labelt);
	else if(child->next && !strcmp(child->next->type,"AND")){
		Operand l=newOprRnd();
		trsCond(child,l,labelf);
		addCode(newCode(0,l,NULL,NULL,NULL));
		trsCond(child->next->next,labelt,labelf);
	}
	else if(child->next && !strcmp(child->next->type,"OR")){
		Operand l=newOprRnd();
		trsCond(child,labelt,l);
		addCode(newCode(0,l,NULL,NULL,NULL));//LABEL
		trsCond(child->next->next,labelt,labelf);
	}
	else{
		Operand t=newOprRnd();
		trsExp(node,t);
		addCode(newCode(11,t,newOprStr("#0"),labelt,newOprStr("!=")));//COND
		addCode(newCode(10,labelf,NULL,NULL,NULL));
	}
}




/*=================================================================*/





//初始化
void initIR()
{
	addReadFunc();
	addWriteFunc();
}

//临时变量计数
static int count=0;

//临时
Operand newOprRnd()
{
	Operand opr=malloc(sizeof(struct Operand_));
	char *str=malloc(sizeof(char)*64);
	sprintf(str,"%d",count++);
	strcpy(opr->str,strcat("v",str));
	return opr;
}

//变量
Operand newOprStr(char *str)
{
	Operand opr=malloc(sizeof(struct Operand_));
	strcpy(opr->str,str);
	return opr;
}

//生成中间代码
InterCode newCode(int kind,Operand o0,
		Operand o1,Operand o2,Operand op)
{
	InterCode code=malloc(sizeof(struct InterCode_));
	code->kind=kind;
	code->o0=o0;
	code->o1=o1;
	code->o2=o2;
	code->op=op;
	return code;
}

InterCode head=NULL;
InterCode tail=NULL;

//添加代码
void addCode(InterCode code)
{
	if(!head){
		head=tail=code;
		code->pre=code->next=NULL;
		return;
	}
	tail->next=code;
	code->pre=tail;
	code->next=NULL;
	tail=code;
}

//打印中间代码
void IRprint(InterCode head)
{
	FILE *fp=fopen("out.ir","w");
	while(head){
	switch(head->kind){
	case 0:
		fprintf(fp,"LABEL %s :\n",head->o0->str);
		break;
	case 1:
		fprintf(fp,"FUNCTION %s :\n",head->o0->str);
		break;
	case 2:
		fprintf(fp,"%s := %s\n",head->o0->str,head->o1->str);
		break;
	case 3:
		fprintf(fp,"%s := %s + %s\n",head->o0->str,head->o1->str,head->o2->str);
		break;
	case 4:
		fprintf(fp,"%s := %s - %s\n",head->o0->str,head->o1->str,head->o2->str);
		break;
	case 5:
		fprintf(fp,"%s := %s * %s\n",head->o0->str,head->o1->str,head->o2->str);
		break;
	case 6:
		fprintf(fp,"%s := %s / %s\n",head->o0->str,head->o1->str,head->o2->str);
		break;
	case 7:
		fprintf(fp,"%s := &%s\n",head->o0->str,head->o1->str);
		break;
	case 8:
		fprintf(fp,"%s := *%s\n",head->o0->str,head->o1->str);
		break;
	case 9:
		fprintf(fp,"*%s := %s\n",head->o0->str,head->o1->str);
		break;
	case 10:
		fprintf(fp,"GOTO %s\n",head->o0->str);
		break;
	case 11:
		fprintf(fp,"IF %s %s %s GOTO %s\n",head->o0->str,head->op->str,head->o1->str,head->o2->str);
		break;
	case 12:
		fprintf(fp,"RETURN %s\n",head->o0->str);
		break;
	case 13:
		fprintf(fp,"DEC %s %s\n",head->o0->str,head->o1->str);
		break;
	case 14:
		fprintf(fp,"ARG %s\n",head->o0->str);
		break;
	case 15:
		fprintf(fp,"%s := CALL %s\n",head->o0->str,head->o1->str);
		break;
	case 16:
		fprintf(fp,"PARAM %s\n",head->o0->str);
		break;
	case 17:
		fprintf(fp,"READ %s\n",head->o0->str);
		break;
	case 18:
		fprintf(fp,"WRITE %s\n",head->o0->str);
		break;
	default:
		fprintf(fp,"ERROR\n");
	}
	head=head->next;
	}
}

//添加read函数
void addReadFunc()
{
	Type type=malloc(sizeof(struct Type_));
	type->kind=0;
	type->basic=0;
	//函数
	FuncList read=malloc(sizeof(struct FuncList_));
	strcpy(read->name,"read");
	read->count=0;
	read->args=NULL;
	read->returnType=type;
	struct Record *record=malloc(sizeof(struct Record));
	record->type=1;
	record->func=read;
	record->next=NULL;
	insertTable(record,-1);
}

//添加write函数
void addWriteFunc()
{
	Type type=malloc(sizeof(struct Type_));
	type->kind=0;
	type->basic=0;
	//函数
	FuncList write=malloc(sizeof(struct FuncList_));
	strcpy(write->name,"write");
	write->count=1;
	FieldList args=malloc(sizeof(struct FieldList_));
	strcpy(args->name,"");
	args->type=type;
	args->tail=NULL;
	write->args=args;
	write->returnType=type;
	struct Record *record=malloc(sizeof(struct Record));
	record->type=1;
	record->func=write;
	record->next=NULL;
	insertTable(record,-1);
}
