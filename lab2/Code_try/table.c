#include"table.h"
#include"syntax.tab.h"
#include"tree.h"

struct Record table[TABLESIZE];//符号表

int hashIndex(char* name)
{
	int length=strlen(name);
	int sum=0;
	for(int i=0;i<length;++i)
		sum += name[i];
	return sum % TABLESIZE;
}

