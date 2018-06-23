#include"tree.h"
#include"syntax.tab.h"
#include"table.h"
#include"semantic.h"
#include"IR.h"
#include"ASM.h"

int main(int argc, char** argv)
{
	if (argc <= 2) 
		return 1;
	FILE* infp = fopen(argv[1], "r");
	if (!infp){
		perror(argv[1]);
		return 1;
	}
	yyrestart(infp);
	yyparse();//词法和语法分析
	if(hasSyntaxError)
		return 1;
	semantic(root);//语义分析
	IR(root);//生成中间代码
	ASM(argv[2]);//生成汇编代码
	return 0;
}
