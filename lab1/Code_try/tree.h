extern int yylineno;
extern char* yytext;
void yyerror(char *msg);

struct Node{
	union{
		int valInt;
		float valFloat;
		double valDouble;
	};
	int line;
	char type[64];
	char name[64];
	struct Node *child;
	struct Node *next;
};
