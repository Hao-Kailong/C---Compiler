#include"IR.h"

//中间代码链表
InterCode IRhead=NULL;
InterCode IRtail=NULL;

/*中间代码总入口*/
void IR(struct Node *root,FILE *fp)
{
	IRgenerate(root);//生成
	IRopt();//优化
	IRprint(fp);//打印
}

void IRgenerate(struct Node *root)
{
	if(!root)
		return;
	trsExtDefList(root->child);
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
	trsSpecifier(child);
	if(strcmp(child->next->type,"FunDec"))
		return;//没有全局变量
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

/*返回名字*/
char* trsVarDec(struct Node *node,int level,int flag)//局部变量、函数形参
{//flag为１表示定义局部变量
	static int size=4;//空间
	struct Node *child=node->child;
	if(!strcmp(child->type,"ID"))
		return child->name;
	char* name=trsVarDec(child,level+1,flag);
	int num=child->next->next->valueInt;
    size *= num;//空间计算	
	if(level)
		return name;
	if(!flag)//形参
		return name;
	//数组添加代码
	Operand o0=newOprStr(name);
	char *str=malloc(sizeof(char)*64);
	sprintf(str,"%d",size);
	Operand o1=newOprStr(str);
	addCode(newCode(DEC,o0,o1,NULL,NULL));
	return name;
}

void trsFunDec(struct Node *node)
{
	struct Node *child=node->child;
	Operand o0=newOprStr(child->name);
	addCode(newCode(FUNCTION,o0,NULL,NULL,NULL));
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

//函数形参，值或地址
void trsParamDec(struct Node *node)
{
	struct Node *child=node->child;
	trsSpecifier(child);
	char *name=trsVarDec(child->next,0,0);
	Operand o0=newOprStr(name);
	addCode(newCode(PARAM,o0,NULL,NULL,NULL));
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
		addCode(newCode(RTN,t,NULL,NULL,NULL));
	}
	else if(!strcmp(child->type,"WHILE")){
		Operand l1=newOprRnd();
		Operand l2=newOprRnd();
		Operand l3=newOprRnd();	
		addCode(newCode(LABEL,l1,NULL,NULL,NULL));
		trsCond(child->next->next,l2,l3);
		addCode(newCode(LABEL,l2,NULL,NULL,NULL));
		trsStmt(child->next->next->next->next);
		addCode(newCode(GOTO,l1,NULL,NULL,NULL));
		addCode(newCode(LABEL,l3,NULL,NULL,NULL));
	} 
	else{
		child=child->next->next;//Exp
		if(!child->next->next->next){
			Operand l1=newOprRnd();
			Operand l2=newOprRnd();
			trsCond(child,l1,l2);
			addCode(newCode(LABEL,l1,NULL,NULL,NULL));
			trsStmt(child->next->next);
			addCode(newCode(LABEL,l2,NULL,NULL,NULL));
		}
		else{
			Operand l1=newOprRnd();
			Operand l2=newOprRnd();
			Operand l3=newOprRnd();
			trsCond(child,l1,l2);
			addCode(newCode(LABEL,l1,NULL,NULL,NULL));
			trsStmt(child->next->next);
			addCode(newCode(GOTO,l3,NULL,NULL,NULL));
			addCode(newCode(LABEL,l2,NULL,NULL,NULL));
			trsStmt(child->next->next->next->next);
			addCode(newCode(LABEL,l3,NULL,NULL,NULL));
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
	char *name=trsVarDec(child,0,1);//局部变量
	if(!child->next)
		return;
	Operand t=newOprRnd();
	trsExp(child->next->next,t);
	Operand o0=newOprStr(name);
	addCode(newCode(ASSIGN,o0,t,NULL,NULL));
}

/*返回结果操作数符*/
Operand trsExp(struct Node *node,Operand place)//place即结果
{
	struct Node *child=node->child;
	if(child->next && !strcmp(child->next->type,"ASSIGNOP")){//Exp ASSIGNOP Exp
		struct Node *exp=child->next->next;//计算的公式
		/*剥离括号*/
		while(!strcmp(child->child->type,"LP"))
			child=child->child->next;//最终为Exp
		/*开始赋值*/
		if(!strcmp(child->child->type,"ID")){//ID
			Operand t1=newOprRnd();
			trsExp(exp,t1);
			Operand t2=newOprStr(child->child->name);
			addCode(newCode(ASSIGN,t2,t1,NULL,NULL));
			if(place){
				addCode(newCode(ASSIGN,place,t2,NULL,NULL));
				return place;
			}
			return t2;
		}
		else if(!strcmp(child->child->next->type,"LB")){//数组
			Operand t1=newOprRnd();
			trsExp(exp,t1);
			Operand t2=trsExp(child,NULL);//数组地址
			addCode(newCode(ASSIGNSTAR,t2,t1,NULL,NULL));
			if(place){
				addCode(newCode(STARASSIGN,place,t2,NULL,NULL));
				return place;
			}
			place=newOprRnd();
			addCode(newCode(STARASSIGN,place,t2,NULL,NULL));
			return place;
		}
		else{//未知情况
			printf("Error In Exp Assign!\n");
			return NULL;
		}
	}
	else if( (child->next && (!strcmp(child->next->type,"AND")
						     ||!strcmp(child->next->type,"OR")
							 ||!strcmp(child->next->type,"RELOP"))
		   ) || !strcmp(child->type,"NOT") 
	){//逻辑比较
		Operand l1=newOprRnd();
		Operand l2=newOprRnd();
		addCode(newCode(ASSIGN,place,newOprStr("#0"),NULL,NULL));
		trsCond(node,l1,l2);
		addCode(newCode(LABEL,l1,NULL,NULL,NULL));
		addCode(newCode(ASSIGN,place,newOprStr("#1"),NULL,NULL));
		addCode(newCode(LABEL,l2,NULL,NULL,NULL));
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
			addCode(newCode(ADD,place,t1,t2,NULL));
		else if(!strcmp(child->next->type,"PLUS"))
			addCode(newCode(SUB,place,t1,t2,NULL));
		else if(!strcmp(child->next->type,"PLUS"))
			addCode(newCode(MULT,place,t1,t2,NULL));
		else
			addCode(newCode(DV,place,t1,t2,NULL));
		return place;
	}
	else if(!strcmp(child->type,"LP"))
		return trsExp(child->next,place);
	else if(!strcmp(child->type,"MINUS")){
		Operand t1=newOprRnd();
		trsExp(child->next,t1);
		addCode(newCode(SUB,place,newOprStr("#0"),t1,NULL));
		return place;
	}
	else if(child->next && !strcmp(child->type,"ID")){
		if(!strcmp(child->next->next->type,"RP")){//无参函数
			if(!strcmp(child->name,"read")){
				addCode(newCode(READ,place,NULL,NULL,NULL));
				return place;
			}
			addCode(newCode(CALL,place,newOprStr(child->name),NULL,NULL));
			return place;
		}
		else{
			Operand args=trsArgs(child->next->next,NULL);//实参
			if(!strcmp(child->name,"write")){
				addCode(newCode(WRITE,args,NULL,NULL,NULL));
				if(place){
					addCode(newCode(ASSIGN,place,newOprStr("#0"),NULL,NULL));
					return place;
				}
				return newOprStr("#0");
			}
			while(args){
				addCode(newCode(14,args,NULL,NULL,NULL));//ARG
				args=args->next;
			}
			addCode(newCode(CALL,place,newOprStr(child->name),NULL,NULL));
			return place;
		}
	}
	else if(child->next && !strcmp(child->next->type,"LB")){//数组
		/*返回目标地址*/
		Operand head=NULL;//记录各个维度，还可以采用符号表的实现方式
		while(child->next){
			Operand tmp=newOprRnd();
			trsExp(child->next->next,tmp);
			tmp->next=head;
			head=tmp;
			child=child->child;
		}
		/*跳出循环后child指向ID*/
		Operand offset=newOprRnd();//偏移
		addCode(newCode(MULT,offset,head,newOprStr("#4"),NULL));
		head=head->next;
		while(head){
			Operand tmp=newOprRnd();
			addCode(newCode(MULT,tmp,offset,head,NULL));
			offset=tmp;
			head=head->next;
		}
		Operand addr=newOprRnd();//数组首地址
		Operand name=newOprStr(child->name);
		addCode(newCode(ADDRESSASSIGN,addr,id,NULL,NULL));
		Operand realAddr=newOprRnd();//目标地址
		addCode(newCode(ADD,realAddr,addr,offset,NULL));
		if(place){
			addCode(newCode(STARASSIGN,place,realAddr,NULL,NULL));
			return place;
		}
		return realAddr;
	}
	else if(child->next && !strcmp(child->next->type,"DOT")){
		printf("Error: Can't Translate Structure Type!\n");
		return place;
	}
	else if(!strcmp(child->type,"ID")){//ID
		Operand t=newOprStr(child->name);
		if(place){
			addCode(newCode(ASSIGN,place,t,NULL,NULL));
			return place;
		}
		return t;
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



Operand newOprRnd()
{
	static int count=0;//计数
	Operand opr=malloc(sizeof(struct Operand_));
	char *str=malloc(sizeof(char)*64);
	sprintf(str,"%d",count++);
	strcpy(opr->str,strcat("v",str));
	return opr;
}

Operand newOprStr(char *str)
{
	Operand opr=malloc(sizeof(struct Operand_));
	strcpy(opr->str,str);
	return opr;
}

//生成中间代码
InterCode newCode(enum codeKind  kind,Operand o0,
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

//添加代码
void addCode(InterCode code)
{
	if(!IRhead){
		IRhead=IRtail=code;
		code->pre=code->next=NULL;
		return;
	}
	IRtail->next=code;
	code->pre=IRtail;
	IRtail=code;
	IRtail->next=NULL;
}

void IRopt()
{
	/*优化中间代码*/
}

//打印中间代码
void IRprint(FILE *fp)
{
	InterCode head=IRhead;
	while(head){
		switch(head->kind){
			case LABEL:
				fprintf(fp,"LABEL %s :\n",head->o0->str);
				break;
			case FUNCTION:
				fprintf(fp,"FUNCTION %s :\n",head->o0->str);
				break;
			case ASSIGN:
				fprintf(fp,"%s := %s\n",head->o0->str,head->o1->str);
				break;
			case ADD:
				fprintf(fp,"%s := %s + %s\n",head->o0->str,head->o1->str,head->o2->str);
				break;
			case SUB:
				fprintf(fp,"%s := %s - %s\n",head->o0->str,head->o1->str,head->o2->str);
				break;
			case MULT:
				fprintf(fp,"%s := %s * %s\n",head->o0->str,head->o1->str,head->o2->str);
				break;
			case DV:
				fprintf(fp,"%s := %s / %s\n",head->o0->str,head->o1->str,head->o2->str);
				break;
			case ADDRESSASSIGN:
				fprintf(fp,"%s := &%s\n",head->o0->str,head->o1->str);
				break;
			case STARASSIGN:
				fprintf(fp,"%s := *%s\n",head->o0->str,head->o1->str);
				break;
			case ASSIGNSTAR:
				fprintf(fp,"*%s := %s\n",head->o0->str,head->o1->str);
				break;
			case GOTO:
				fprintf(fp,"GOTO %s\n",head->o0->str);
				break;
			case COND:
				fprintf(fp,"IF %s %s %s GOTO %s\n",head->o0->str,head->op->str,head->o1->str,head->o2->str);
				break;
			case RTN:
				fprintf(fp,"RETURN %s\n",head->o0->str);
				break;
			case DEC:
				fprintf(fp,"DEC %s %s\n",head->o0->str,head->o1->str);
				break;
			case ARG:
				fprintf(fp,"ARG %s\n",head->o0->str);
				break;
			case CALL:
				fprintf(fp,"%s := CALL %s\n",head->o0->str,head->o1->str);
				break;
			case PARAM:
				fprintf(fp,"PARAM %s\n",head->o0->str);
				break;
			case READ:
				fprintf(fp,"READ %s\n",head->o0->str);
				break;
			case WRITE:
				fprintf(fp,"WRITE %s\n",head->o0->str);
				break;
			default:
				fprintf(fp,"ERROR\n");
		}
		head=head->next;
	}
}
