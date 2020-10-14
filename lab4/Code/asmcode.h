#ifndef ASM_CODE_HEAD
#define ASM_CODE_HEAD
#include"interCode.h"

typedef struct RegDescriptor RegDescriptor;
typedef struct FunctionBlockDescriptor FunctionBlockDescriptor;
typedef struct ValueDescriptor ValueDescriptor;
typedef struct IntList IntList;

struct RegDescriptor{
	bool isFree;
	bool isValue;//v 
	bool isConstant;//#
	int id;
	int nextUsedLine;//next position where variable used in current base block
	bool lock;//lock reg when translate interCode line
};

struct FunctionBlockDescriptor{
	char* funcName;
	ValueDescriptor* vd;
	int localVarSize;
	FunctionBlockDescriptor* next;
};

struct ValueDescriptor{
	bool isV;
	int id;
	int size;//4,8,12,...
	bool isArray;
	bool isParam;
	ValueDescriptor* next;
};

extern char s0[];
extern char s1[];
extern char s2[];
extern char s3[];
extern char s4[];
extern char s5[];
extern char s6[];
extern char s7[];
extern char t0[];
extern char t1[];
extern char t2[];
extern char t3[];
extern char t4[];
extern char t5[];
extern char t6[];
extern char t7[];
extern char t8[];
extern char t9[];
extern char a0[];
extern char a1[];
extern char a2[];
extern char a3[];
extern int lastRegId;
extern RegDescriptor rd[10];//just use for $t
extern FunctionBlockDescriptor* fbd;
extern FunctionBlockDescriptor* fbd_index;
extern InterCodes* current_entrance;
extern InterCodes* current_code;
extern FILE* file_index;
extern bool prepareCall;
extern int ARG_NUM;
extern int ARG_NUM_LATER;
extern bool deBugMode;
extern int stackOffsetForArg;
extern bool isBlockEnd;

extern void deBug();
extern void deBugFBD(FunctionBlockDescriptor* x);
extern void deBugVD(ValueDescriptor* x);
extern void InitialBlock();
extern int getLocalVarSize(FunctionBlockDescriptor* f);
extern void dealValue(Operand* op,FunctionBlockDescriptor* currentFBD,bool isArray,bool isParam,int size);
extern void dealFuncBlock(InterCodes* codes,FunctionBlockDescriptor* currentFBD);
extern void dealBlock(InterCodes* codes);
extern void spillReg();
extern void InitialReg();
extern void OutAsm(char* out_path);
extern void OutCodes(FILE* fp);
extern void OutCode(FILE* fp,InterCode* x);
extern char* getSReg(int index);
extern char* getAReg(int index);
extern char* getTReg(int index);
extern bool opMatch(Operand* op,bool isV,int id);
extern bool isUsed(InterCode* x,bool isV,int id);
extern int getNextUsedLine(bool isV,int id);
extern int getStackOffset(bool isV,int id,bool* is_addr);
extern char* getReg(Operand* op);
extern void OutRead(FILE* fp,InterCode* x);
extern void OutWrite(FILE* fp,InterCode* x);
extern void OutParam(FILE* fp,InterCode* x);
extern void OutCall(FILE* fp,InterCode* x);
extern int argNumLater();
extern void OutARG(FILE* fp, InterCode* x);
extern void OutDec(FILE* fp,InterCode* x);
extern void OutReturn(FILE* fp,InterCode* x);
extern void OutRelopGoto(FILE* fp,InterCode* x);
extern void OutGoto(FILE* fp,InterCode* x);
extern void OutAssignContent(FILE* fp,InterCode* x);
extern void OutContentAssign(FILE* fp, InterCode* x);
extern void OutAddrAssign(FILE* fp, InterCode* x);
extern void OutLabel(FILE* fp,InterCode* x);
extern void OutFunc(FILE* fp,InterCode* x);
extern void OutAssign(FILE* fp,InterCode* x);
extern void OutAdd(FILE* fp, InterCode* x);
extern void OutSub(FILE* fp,InterCode* x);
extern void OutMul(FILE* fp, InterCode* x);
extern void OutDiv(FILE* fp, InterCode* x);
extern void OutOp(FILE* fp,Operand* op);
extern void OutRelop(FILE* fp,RELOP r);

#endif
