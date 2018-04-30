#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"tree.h"
#include"table.h"

void semantic(struct Node *root);
void ExtDefList(struct Node *node);
void ExtDef(struct Node *node);

#endif
