#include"IR.h"

//用于按地址传实参
static int swch=0;

//中间代码链表
InterCode IRhead=NULL;
InterCode IRtail=NULL;

/*中间代码总入口*/
void IR(struct Node *root)
{
	IRgenerate(root);//生成
	IRopt();//优化
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
	exit(-1);//翻译异常
}

/*返回名字*/
char* trsVarDec(struct Node *node,int level,int flag)//局部变量、函数形参
{//flag为１表示定义局部变量
	static int size=4;//空间
	struct Node *child=node->child;
	if(!strcmp(child->type,"ID"))
		return child->name;
	char* name=trsVarDec(child,level+1,flag);
	if(!flag)//形参
		return name;
	int num=child->next->next->valueInt;
    size *= num;//空间计算	
	if(level)
		return name;
	//数组添加代码
	Operand o=newOprStr(name);
	char *str=malloc(sizeof(char)*64);
	sprintf(str,"%d",size);
	Operand o1=newOprStr(str);
	addCode(newCode(DEC,o,o1,NULL,NULL));
	size=4;//复位
	return name;
}

void trsFunDec(struct Node *node)
{
	struct Node *child=node->child;
	Operand o=newOprStr(child->name);
	addCode(newCode(FUNCTION,o,NULL,NULL,NULL));
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
	Operand o=newOprStr(name);
	addCode(newCode(PARAM,o,NULL,NULL,NULL));
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
		Operand t=newOprRnd(0);
		trsExp(child->next,t);
		addCode(newCode(RTN,t,NULL,NULL,NULL));
	}
	else if(!strcmp(child->type,"WHILE")){//循环语句
		Operand l1=newOprRnd(1);
		Operand l2=newOprRnd(1);
		Operand l3=newOprRnd(1);	
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
			Operand l1=newOprRnd(1);
			Operand l2=newOprRnd(1);
			trsCond(child,l1,l2);
			addCode(newCode(LABEL,l1,NULL,NULL,NULL));
			trsStmt(child->next->next);
			addCode(newCode(LABEL,l2,NULL,NULL,NULL));
		}
		else{
			Operand l1=newOprRnd(1);
			Operand l2=newOprRnd(1);
			Operand l3=newOprRnd(1);
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
	Operand t=newOprRnd(0);
	trsExp(child->next->next,t);
	Operand o=newOprStr(name);
	addCode(newCode(ASSIGN,o,t,NULL,NULL));
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
			Operand t1=newOprRnd(0);
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
			Operand t1=newOprRnd(0);
			trsExp(exp,t1);
			Operand t2=trsExp(child,NULL);//数组地址
			addCode(newCode(ASSIGNSTAR,t2,t1,NULL,NULL));
			if(place){
				addCode(newCode(STARASSIGN,place,t2,NULL,NULL));
				return place;
			}
			place=newOprRnd(0);
			addCode(newCode(STARASSIGN,place,t2,NULL,NULL));
			return place;
		}
		else{//未知情况
			printf("Error: Can't Translate Structure Type!\n");
			exit(-1);
		}
	}
	else if( (child->next && (!strcmp(child->next->type,"AND")
						     ||!strcmp(child->next->type,"OR")
							 ||!strcmp(child->next->type,"RELOP"))
		   ) || !strcmp(child->type,"NOT") 
	){//逻辑比较
		Operand l1=newOprRnd(1);
		Operand l2=newOprRnd(1);
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
		Operand t1=newOprRnd(0);
		Operand t2=newOprRnd(0);
		trsExp(child,t1);
		trsExp(child->next->next,t2);
		if(!strcmp(child->next->type,"PLUS"))
			addCode(newCode(ADD,place,t1,t2,NULL));
		else if(!strcmp(child->next->type,"MINUS"))
			addCode(newCode(SUB,place,t1,t2,NULL));
		else if(!strcmp(child->next->type,"STAR"))
			addCode(newCode(MULT,place,t1,t2,NULL));
		else
			addCode(newCode(DV,place,t1,t2,NULL));
		return place;
	}
	else if(!strcmp(child->type,"LP"))
		return trsExp(child->next,place);
	else if(!strcmp(child->type,"MINUS")){
		Operand t1=newOprRnd(0);
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
			if(!place)
				place=newOprRnd(0);
			addCode(newCode(CALL,place,newOprStr(child->name),NULL,NULL));
			return place;
		}
		else{
			Operand args=trsArgs(child->next->next,NULL);//实参列表（与形参反序）
			if(!strcmp(child->name,"write")){
				addCode(newCode(WRITE,args,NULL,NULL,NULL));
				if(place){
					addCode(newCode(ASSIGN,place,newOprStr("#0"),NULL,NULL));
					return place;
				}
				return newOprStr("#0");
			}
			while(args){
				addCode(newCode(ARG,args,NULL,NULL,NULL));
				args=args->next;
			}
			if(!place)
				place=newOprRnd(0);
			addCode(newCode(CALL,place,newOprStr(child->name),NULL,NULL));
			return place;
		}
	}
	/*数组，返回目标地址*/
	else if(child->next && !strcmp(child->next->type,"LB")){
		Operand head,tail;
		head=tail=NULL;
		while(child->next){//数组各个维度
			Operand tmp=newOprRnd(0);
			trsExp(child->next->next,tmp);
			if(!head){
				head=tail=tmp;
				tail->next=NULL;
			}
			else{
				tail->next=tmp;
				tail=tmp;
				tail->next=NULL;
			}
			child=child->child;
		}
		/*跳出循环后child指向ID*/
		struct Record *record=findTable(child->name,0,-1);
		Type t=record->var->type;
		int width=4;//宽度

		Operand offset=NULL;//偏移
		while(head){
			char *num=malloc(sizeof(char)*64);
			sprintf(num,"#%d",width);//立即数: 宽度
			Operand tmpResult=newOprRnd(0);
			addCode(newCode(MULT,tmpResult,head,newOprStr(num),NULL));
			if(offset)
				addCode(newCode(ADD,offset,offset,tmpResult,NULL));
			else{
				offset=newOprRnd(0);
				addCode(newCode(ADD,offset,newOprStr("#0"),tmpResult,NULL));
			}
			width *= t->array.size;
			head=head->next;
			t=t->array.elem;
		}

		Operand addr=NULL;//数组首地址
		if(!record->var->pos){						/*变量*/
			char *buf=malloc(sizeof(char)*64);
			strcpy(buf,"&");
			strcat(buf,child->name);
			addr=newOprStr(buf);
		}
		else addr=newOprStr(child->name);			/*形参*/

		Operand realAddr=newOprRnd(0);//目标地址
		addCode(newCode(ADD,realAddr,addr,offset,NULL));
		if(place){
			addCode(newCode(STARASSIGN,place,realAddr,NULL,NULL));
			return place;
		}
		return realAddr;
	}
	else if(child->next && !strcmp(child->next->type,"DOT")){
		printf("Error: Can't Translate Structure Type!\n");
		exit(-1);
	}
	else if(!strcmp(child->type,"ID")){//ID
		Operand t=NULL;
		if(swch){//实参
			struct Record *record=findTable(child->name,0,-1);
			if(record->var->type->kind==1){//数组
				if(record->var->pos==0){//传址
					char *buf=malloc(sizeof(char)*64);
					strcpy(buf,"&");
					strcat(buf,child->name);
					t=newOprStr(buf);
				}
				else{//传值
					char *buf=malloc(sizeof(char)*64);
					strcpy(buf,child->name);
					t=newOprStr(buf);
				}
			}
			else t=newOprStr(child->name);
		}
		else t=newOprStr(child->name);//变量
		if(place){
			addCode(newCode(ASSIGN,place,t,NULL,NULL));
			return place;
		}
		return t;
	}
	else{ 
		char *buf=malloc(sizeof(char)*64);
		strcpy(buf,"#");
		strcat(buf,child->name);//数字
		if(place){
			addCode(newCode(ASSIGN,place,newOprStr(buf),NULL,NULL));
			return place;
		}
		return newOprStr(buf);
	}
}

//返回实参列表头
Operand trsArgs(struct Node *node,Operand args)//args表示实参头
{
	struct Node *child=node->child;
	Operand t=newOprRnd(0);
	swch=1;//on
	trsExp(child,t);
	swch=0;//off
	t->next=args;//与形参反序
	if(!child->next)
		return t;
	return trsArgs(child->next->next,t);
}

//条件语句
void trsCond(struct Node *node,Operand labelt,Operand labelf)
{
	struct Node *child=node->child;
	if(child->next && !strcmp(child->next->type,"RELOP")){
		Operand t1=newOprRnd(0);
		Operand t2=newOprRnd(0);
		trsExp(child,t1);
		trsExp(child->next->next,t2);
		Operand op=newOprStr(child->next->name);
		addCode(newCode(COND,t1,t2,labelt,op));
		addCode(newCode(GOTO,labelf,NULL,NULL,NULL));
	}
	else if(!strcmp(child->type,"NOT"))
		trsCond(child,labelf,labelt);
	else if(child->next && !strcmp(child->next->type,"AND")){
		Operand l=newOprRnd(1);
		trsCond(child,l,labelf);
		addCode(newCode(LABEL,l,NULL,NULL,NULL));
		trsCond(child->next->next,labelt,labelf);
	}
	else if(child->next && !strcmp(child->next->type,"OR")){
		Operand l=newOprRnd(1);
		trsCond(child,labelt,l);
		addCode(newCode(LABEL,l,NULL,NULL,NULL));
		trsCond(child->next->next,labelt,labelf);
	}
	else{
		Operand t=newOprRnd(0);
		trsExp(node,t);
		addCode(newCode(COND,t,newOprStr("#0"),labelt,newOprStr("!=")));//从左至右依次
		addCode(newCode(GOTO,labelf,NULL,NULL,NULL));
	}
}



/*=================================================================*/



//标签、临时变量
Operand newOprRnd(int kind)//0: var    1: label
{
	static int countVar=0;//计数
	static int countLabel=0;
	Operand opr=malloc(sizeof(struct Operand_));
	char *num=malloc(sizeof(char)*64);
	if(!kind)
		sprintf(num,"%d",countVar++);
	else sprintf(num,"%d",countLabel++);
	char *buf=malloc(sizeof(char)*64);
	if(!kind)
		strcpy(buf,"t_v");
	else strcpy(buf,"label");
	strcat(buf,num);//名称
	strcpy(opr->str,buf);
	return opr;
}

Operand newOprStr(char *str)
{
	Operand opr=malloc(sizeof(struct Operand_));
	strcpy(opr->str,str);
	return opr;
}

//生成中间代码
InterCode newCode(enum codeKind  kind,Operand o,
	Operand o1,Operand o2,Operand op)
{
	InterCode code=malloc(sizeof(struct InterCode_));
	code->kind=kind;
	code->o=o;
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

//打印中间代码
void IRprint(char *outfile)
{
	FILE* fp = fopen(outfile, "w");
	if (!fp){
		perror(outfile);
		return;
	}
	InterCode head=IRhead;
	while(head){
		switch(head->kind){
			case LABEL:
				fprintf(fp,"LABEL %s :\n",head->o->str);
				break;
			case FUNCTION:
				fprintf(fp,"FUNCTION %s :\n",head->o->str);
				break;
			case ASSIGN:
				fprintf(fp,"%s := %s\n",head->o->str,head->o1->str);
				break;
			case ADD:
				fprintf(fp,"%s := %s + %s\n",head->o->str,head->o1->str,head->o2->str);
				break;
			case SUB:
				fprintf(fp,"%s := %s - %s\n",head->o->str,head->o1->str,head->o2->str);
				break;
			case MULT:
				fprintf(fp,"%s := %s * %s\n",head->o->str,head->o1->str,head->o2->str);
				break;
			case DV:
				fprintf(fp,"%s := %s / %s\n",head->o->str,head->o1->str,head->o2->str);
				break;
			case ADDRESSASSIGN:
				fprintf(fp,"%s := &%s\n",head->o->str,head->o1->str);
				break;
			case STARASSIGN:
				fprintf(fp,"%s := *%s\n",head->o->str,head->o1->str);
				break;
			case ASSIGNSTAR:
				fprintf(fp,"*%s := %s\n",head->o->str,head->o1->str);
				break;
			case GOTO:
				fprintf(fp,"GOTO %s\n",head->o->str);
				break;
			case COND:
				fprintf(fp,"IF %s %s %s GOTO %s\n",head->o->str,head->op->str,head->o1->str,head->o2->str);
				break;
			case RTN:
				fprintf(fp,"RETURN %s\n",head->o->str);
				break;
			case DEC:
				fprintf(fp,"DEC %s %s\n",head->o->str,head->o1->str);
				break;
			case ARG:
				fprintf(fp,"ARG %s\n",head->o->str);
				break;
			case CALL:
				fprintf(fp,"%s := CALL %s\n",head->o->str,head->o1->str);
				break;
			case PARAM:
				fprintf(fp,"PARAM %s\n",head->o->str);
				break;
			case READ:
				fprintf(fp,"READ %s\n",head->o->str);
				break;
			case WRITE:
				fprintf(fp,"WRITE %s\n",head->o->str);
				break;
			default:
				fprintf(fp,"ERROR\n");
		}
		head=head->next;
	}
}


//寻找临时变量在中间代码的位置
InterCode findInterCode(Operand o,InterCode stop)
{
	InterCode cur=IRhead;
	InterCode result=NULL;
	while(cur && cur!=stop){ 	//当前代码前的最后一次出现
		if(!strcmp(cur->o->str,o->str))
			result=cur;
		cur=cur->next;
	}
	return result;
}

//判断是否为冗余变量
int isRedundent(Operand o)
{
	InterCode cur=IRhead;
	while(cur){
		if(cur->o1 && !strcmp(cur->o1->str,o->str))
			return 0;
		if(cur->o2 && !strcmp(cur->o2->str,o->str))
			return 0;
		if( (cur->kind==RTN 	//出现在RETURN等
		  || cur->kind==READ || cur->kind==WRITE || cur->kind==PARAM
		  || cur->kind==ARG || cur->kind==GOTO || cur->kind==FUNCTION
		  || cur->kind==LABEL || cur->kind==CALL || cur->kind==COND)
		  && !strcmp(cur->o->str,o->str))
			return 0;
		cur=cur->next;
	}
	return 1;
}

/*优化中间代码*/
void IRopt()
{	
	/*Exp优化*/
	InterCode cur=IRhead;
	while(cur){
		if(cur->kind==ASSIGN	//ASSIGN 
		  && cur->o1->str[0]=='t' && cur->o1->str[1]=='_' && cur->o1->str[2]=='v') { 
			InterCode pre=findInterCode(cur->o1,cur);
			if(pre->kind==ASSIGN)
				cur->o1=pre->o1;
			if(pre->kind==ADD||pre->kind==SUB||pre->kind==MULT||pre->kind==DV){
				cur->kind=pre->kind;
				cur->o1=pre->o1;
				cur->o2=pre->o2;
			}
		}
		if(cur->kind==ADD||cur->kind==SUB||cur->kind==MULT||cur->kind==DV){	//算数运算
		  	if(cur->o1->str[0]=='t' && cur->o1->str[1]=='_' && cur->o1->str[2]=='v'){
				InterCode pre=findInterCode(cur->o1,cur);
				if(pre->kind==ASSIGN)
					cur->o1=pre->o1;
			}
		  	if(cur->o2->str[0]=='t' && cur->o2->str[1]=='_' && cur->o2->str[2]=='v'){
				InterCode pre=findInterCode(cur->o2,cur);
				if(pre->kind==ASSIGN)
					cur->o2=pre->o1;
			}
		}
		cur=cur->next;
	}
	/*去除冗余变量*/
	cur=IRhead;
	while(cur){
		if( (cur->kind==ASSIGN
		  || cur->kind==ADD || cur->kind==SUB || cur->kind==MULT || cur->kind==DV)
	      && isRedundent(cur->o) ){
			cur->pre->next=cur->next;
			cur->next->pre=cur->pre;
		}
		cur=cur->next;
	}
}












