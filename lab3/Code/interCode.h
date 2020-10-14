#ifndef INTER_CODE_HEAD
#define INTER_CODE_HEAD
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include"semantic.h"
typedef struct Operand Operand;
typedef struct InterCode InterCode;
typedef struct InterCodes InterCodes;
typedef struct ValueList ValueList;
typedef struct ArgsList ArgsList;
typedef enum{EQ,NEQ,G,GE,L,LE}RELOP;

struct Operand{
	enum {VAR,CONSTANT,TEMP,LABEL,FUNC,ADDR,STAR,VADDR}kind;
	union{
		int id;//存储第几个变量、标号等
		int value;	
		char* func_name;
	}u;
	struct Type* tp;//represent the type of "*x"
};

struct InterCode{
	enum {LABELDEC,FUNCDEC,ASSIGN,ADD,SUB,MUL,DIV,ADDR_ASSIGN,CONTENT_ASSIGN,ASSIGN_CONTENT,GOTO,RELOP_GOTO,RETURN,DEC,ARG,CALL,PARAM,READ,WRITE}kind;
	union{
		struct Operand *op;
		struct{Operand *right,*left;}assign;
		struct{Operand *result,*op1,*op2;}binop;
		struct{Operand *op1,*op2, *dst;RELOP relop;}relop_goto;
		struct{Operand *op;int size;}dec;
		
	}u;
};

struct InterCodes{
	InterCode *code;
	InterCodes *prev,*next;
};

struct ValueList{
		int id;
		char* name;
		struct Type* tp;
		int dimension;
		int isParamStruct;
		ValueList* next;
};

struct ArgsList{
	Operand* op;
	ArgsList* next;
};

extern InterCodes* CodeHead;
extern ValueList* ValueHead;
extern int ValueNum;
extern int TempNum;
extern int LabelNum;
extern int firstFunc;
extern RELOP getRelop(char* a);
extern int InsertValue(char* name,struct Type* tp,int dimension,int isParamStruct);
extern ValueList* findValue(char* name);
extern ValueList* findValue2(int id);
extern int checkValue(int id);
extern void InsertCode(InterCode* node);
extern void CreateLabel(Operand* op);
extern void CreateFunction(Operand* op);
extern void CreateAssign(Operand* dst,Operand* src);
extern void CreateAdd(Operand* dst,Operand* src1,Operand* src2);
extern void CreateSub(Operand* dst,Operand* src1,Operand* src2);
extern void CreateMul(Operand* dst,Operand* src1,Operand* src2);
extern void CreateDiv(Operand* dst,Operand* src1,Operand* src2);
extern void CreateAddressAssign(Operand* dst,Operand* src);
extern void CreateContentAssign(Operand* dst,Operand* src);
extern void CreateAssignContent(Operand* dst,Operand* src);
extern void CreateGoto(Operand* op);
extern void CreateRelopGoto(Operand* dst,Operand* src1,Operand* src2,RELOP relop);
extern void CreateReturn(Operand* op);
extern void CreateDec(Operand* op,int size);
extern void CreateArg(Operand* op);
extern void CreateCall(Operand* dst,Operand* f);
extern void CreateParam(Operand* op);
extern void CreateRead(Operand* op);
extern void CreateWrite(Operand* op);
extern void printCodes();
extern void printOp(FILE* fp,Operand* op);
extern void printCode(FILE* fp,InterCode* x);
extern void printRelop(FILE* fp,RELOP r);
extern void testCreateCode();
extern Operand* CreateOpVar(int id);
extern Operand* CreateOpVaddr(int id);
extern Operand* CreateOpConst(int value);
extern Operand* CreateOpTemp(int id);
extern Operand* CreateOpLabel(int id);
extern Operand* CreateOpFunc(char fname[]);
extern Operand* CreateOpAddr(int id);
extern Operand* CreateOpStar(int id,struct Type* tp);
#endif
