
#ifndef TRANSLATE_HEAD
#define TRANSLATE_HEAD
#include"tree.h"
#include"interCode.h"
#include"semantic.h"

extern void translateTree(struct Node* root);
extern void translateExtDefList(struct Node* root);
extern void translateExtDef(struct Node* root);
extern void translateExtDecList(struct Node* root);
extern struct Type* translateSpecifier(struct Node* root);
extern struct Type* translateStructSpecifier(struct Node* root);
extern void translateOptTag(struct Node* root);
extern void translateTag(struct Node* root);
extern Operand* translateVarDec(struct Node* root,int isParam,int siz,struct Type* tp);
extern void translateFunDec(struct Node* root);
extern void translateVarList(struct Node* root);
extern void translateParamDec(struct Node* root);
extern void translateCompSt(struct Node* root);
extern void translateStmtList(struct Node* root);
extern void translateStmt(struct Node* root);
extern void translateDefList(struct Node* root);
extern void translateDef(struct Node* root);
extern void translateDecList(struct Node* root,struct Type* tp);
extern void translateDec(struct Node* root,struct Type* tp);
extern void translateCond(struct Node* root,Operand* labelTrue,Operand* labelFalse);
extern Operand* translateExp(struct Node* root);
extern void translateArgs(struct Node* root,ArgsList* arg_list);
#endif
