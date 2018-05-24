#include"tree.h"
#include"syntax.tab.h"
#include"table.h"
#include"semantic.h"
#include"IR.h"

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
	if(hasSyntaxError)
		return 1;
	semantic(root);//语义分析
	IR(root);//生成中间代码
	return 0;
}
