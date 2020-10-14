/*
INT 1 FLOAT 2 ID 3 SEMI 4 COMMA 5 ASSIGNOP 6 RELOP 7 PLUS 8 MINUS 9 STAR 10 
DIV 11 AND 12 OR 13 DOT 14 NOT 15 TYPE 16 LP 17 RP 18 LB 19 RB 20 
LC 21 RC 22 STRUCT 23 RETURN 24 IF 25 ELSE 26 WHILE 27 
Program 28 ExtDefList 29 ExtDef 30 ExtDecList 31 
Specifier 32 StructSpecifier 33 OptTag 34 Tag 35 VarDec 36
FunDec 37 VarList 38 ParamDec 39 CompSt 40 StmtList 41
Stmt 42 DefList 43 Def 44 DecList 45 Dec 46
Exp 47 Args 48
*/
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
typedef int bool;
#define true 1
#define false 0

#ifndef Node_Def
#define Node_Def
struct Node{
	int type;
	char* value;
	int lineCount;
	struct Node** subNodeList;
	int subNum;
	bool isLeaf;
}; 
#endif
extern void printValue(int id,char* value);
extern char* getTagStr(int id);
extern struct Node* createNode(int t,char*v,int lc,struct Node** l,int n,bool islf);
extern void printTree(struct Node* root,int depth);
extern void releaseTree(struct Node**root);
extern int eofLine;
extern int chaNum;
