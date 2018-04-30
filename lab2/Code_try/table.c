#include"table.h"
#include"syntax.tab.h"
#include"tree.h"

struct Record* table[TABLESIZE];//符号表
FieldList *list;//类型表

void initTable()
{
	for(int i=0;i<TABLESIZE;++i){
		table[i].r=NULL;
		table[i].next=NULL;
	}
	list=NULL;
}

int hashIndex(char* name)
{
	int length=strlen(name);
	int sum=0;
	for(int i=0;i<length;++i)
		sum += name[i];
	return sum % TABLESIZE;
}

