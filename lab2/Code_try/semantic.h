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

void ExtDecList(struct Node *node,Type type);

Type Specifier(struct Node *node);

Type StructSpecifier(struct Node *node);

char* OptTag(struct Node *node);

Record* VarDec(struct Node *node,Type type);

Record* vardec(struct Node *node,Type type);

void FunDec(struct Node *node,Type);

void VarList(struct Node *node,FieldList args);

FieldList ParamDec(struct Node *node);

void CompSt(struct Node *node,Type type);

void StmtList(struct Node *node,Type type);

void Stmt(struct Node *node,Type type);

void DefList(struct Node *node,FieldList field,int inStructure);

FieldList Def(struct Node *node,FieldList field,int inStructure);

FieldList DecList(struct Node *node,Type type,FieldList field,int inStructure);

FieldList Dec(struct Node *node,Type type,FieldList field,int inStructure);

Type Exp(struct Node *node);

FieldList Args(struct Node *node);

#endif
