void yyerror(char *msg);

void addChild(struct Node *f, struct Node *s);

void addNext(struct Node *a, struct Node *b);

struct Node* generateNode(double val,int line,char *type,char *name,struct Node *child,struct Node *next);

void visit(struct Node *node,int rank);

void show(struct Node *node,int rank);
