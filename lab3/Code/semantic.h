#include"tree.h"

#ifndef SEMANTIC_HEAD
#define SEMANTIC_HEAD

enum HashType {VARIABLE,STRUCTURE_DEF,FUNCTION};
enum TypeKind {BASIC,ARRAY,STRUCTURE};
struct Variable;
struct Type;
struct FieldList;
struct Structure;
struct StructMemeberInfo;
struct HashList;
struct HashListHead;
struct Type{
	enum TypeKind kind;
	union{
		//basic type
		int basic;
		//array type
		struct{struct Type* elem;int size;}array;
		//structure type
		struct Structure* strt;
	}u;
};

struct FieldList{
	struct Variable* v;
	int line;
	struct FieldList* tail;
};

struct Structure{
	char* name;
	int id;//如果是匿名结构体，则记录是第几个被定义的匿名结构体,如果id=0则说明不是匿名结构体
	struct FieldList* structure;
};

struct Variable{
	char* name;
	struct Type* type;
};



struct Function{
	char* name;
	struct Type* rtn_type;
	struct FieldList* attrList;
	int v_num;
};


struct HashNode{
	enum HashType type;
	union{
		//valiable
		struct Variable* v;
		//structure_def
		struct Structure* strt;
		struct Function* func;
		//function
		
	}u;
	int line;
	int depth;
	struct HashNode* next;
};

struct HashList{
	struct HashNode* x;
	struct HashList* link;
};

struct HashListHead{
	struct HashList* list;
	struct HashListHead* nextHead;
};

#define HASH_LENGTH 16384
#endif
/*extern void freeType(struct Type* t);
extern void freeField(struct FieldList* f);
extern void freeStructure(struct Structure* s);
extern void freeVariable(struct Variable* v);
extern void freeFunction(struct Function* f);
extern void freeHashNode(struct HashNode* h);*/
extern int finishSemantic;
extern int anonymousStrtNum;
extern int currentDepth;
extern int SemanticErrorNum;
extern struct HashListHead* listHead;
extern struct HashNode* HashTable[HASH_LENGTH];
extern void removeCurrentDepth();
extern unsigned int HashReflect(char* name);
extern void HashInit();
extern bool HashAdd(enum HashType type,void* u,int line);
extern void* HashFind(enum HashType type,char* name);
extern void semanticAnalyse(struct Node* root);
extern void ExtDefList(struct Node* root);
extern void ExtDef(struct Node* root);
extern struct Type* Specifier(struct Node* root);
extern void ExtDecList(struct Node* root,struct Type* t);
extern struct Function* FunDec(struct Node* root,struct Type* t);
extern void CompSt(struct Node* root,struct Function* f);
extern struct Type* StructSpecifier(struct Node* root);
extern struct Node* OptTag(struct Node* root);
extern struct Node* Tag(struct Node* root);
extern void* VarDec(struct Node* root,bool isStructCall,struct Type* t);
extern struct FieldList* VarList(struct Node* root,int* num);
extern struct Variable* ParamDec(struct Node* root);
extern void StmtList(struct Node* root,struct Function* f);
extern void Stmt(struct Node* root,struct Function* f);
extern struct FieldList* DefList(struct Node* root,bool isStructCall);
extern struct FieldList* Def(struct Node* root,bool isStructCall);
extern struct FieldList* DecList(struct Node* root,bool isStructCall,struct Type* t);
extern struct FieldList* Dec(struct Node* root,bool isStructCall,struct Type* t);
extern struct Type* Exp(struct Node* root);
extern void Args(struct Node* root,struct FieldList* f,char* funcName);
extern int TYPE(struct Node* root);
extern bool isSameType(struct Type* t1,struct Type* t2);
extern int getSize(struct Type* tp);
extern int getOffset(char* member,struct Structure* strt);
extern struct Type* getType(char* member,struct Structure* strt);
