#include"table.h"
#include"syntax.tab.h"
#include"tree.h"
#include"semantic.h"

int main(int argc, char** argv)
{
	if (argc <= 1) return 1;
	FILE* f = fopen(argv[1], "r");
	if (!f)
	{
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();//词法和语法分析
	if(!hasSyntaxError){
		show(root,0);
		semantic(root);//语义分析
	}
	return 0;
}
