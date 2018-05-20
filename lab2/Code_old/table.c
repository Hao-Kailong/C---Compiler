#include"table.h"

int hashIndex(char* name)
{
	int length=strlen(name);
	int sum=0;
	for(int i=0;i<length;++i)
		sum += name[i];
	return sum % TABLESIZE;
}

int main()
{
	return 0;
}
