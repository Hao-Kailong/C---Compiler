#include"semantic.h"

void initTable()
{
	for(int i=0;i<TABLESIZE;++i){
		table[i].r=NULL;
		table[i].next=NULL;
	}
}

/*语义分析总入口*/
void semantic(struct Node *root)
{
	if(!root)
		return;
	initTable();
	ExtDefList(root->child);
}

void ExtDefList(struct Node *node)
{
	if(!node)
		return;
	struct Node *child=node->child;
	ExtDef(child);
	ExtDefList(child->next);
}

void ExtDef(struct Node *node)
{
	
}
