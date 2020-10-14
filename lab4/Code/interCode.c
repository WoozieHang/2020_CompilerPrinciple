#include"interCode.h"
#include"assert.h"

InterCodes* CodeHead=NULL;
ValueList* ValueHead=NULL;
int ValueNum=0;
int TempNum=0;
int LabelNum=0;
int firstFunc=1;
RELOP getRelop(char* a){
	if(!strcmp(a,">"))
		return G;
	else if(!strcmp(a,"<"))
		return L;
	else if(!strcmp(a,">="))
		return GE;
	else if(!strcmp(a,"<="))
		return LE;
	else if(!strcmp(a,"=="))	
		return EQ;
	else if(!strcmp(a,"!="))
		return NEQ;
	else {assert(0);return 0;}
}

int checkValue(int id){
	ValueList* x=ValueHead;
	while(x!=NULL){
		if(x->id==id)
			return x->isParamStruct;
		x=x->next;
	}
	return 0;

}

int InsertValue(char* name,struct Type* tp,int dimension,int isParamStruct){
	ValueNum=ValueNum+1;
	ValueList* tmp=(ValueList*)malloc(sizeof(ValueList));
	tmp->id=ValueNum;
	tmp->tp=tp;
	tmp->dimension=dimension;
	tmp->isParamStruct=isParamStruct;
	tmp->name=(char*)malloc(sizeof(char)*(1+strlen(name)));
	strcpy(tmp->name,name);
	tmp->next=NULL;
	if(ValueHead==NULL){
		ValueHead=tmp;
	}
	else{
		ValueList* x=ValueHead;
		while(x->next!=NULL)
			x=x->next;
		x->next=tmp;
	}
	return ValueNum;
}

ValueList* findValue(char* name){
	ValueList* x=ValueHead;
	while(x!=NULL){
		if(!strcmp(x->name,name))
			return x;
		x=x->next;
	}
	return NULL;
}

ValueList* findValue2(int id){
	ValueList* x=ValueHead;
	while(x!=NULL){
		if(x->id==id)
			return x;
		x=x->next;
	}
	return NULL;
}


void testCreateCode(){
	CreateLabel(CreateOpLabel(1));
	CreateFunction(CreateOpFunc("ccLover"));
	CreateAssign(CreateOpVar(2),CreateOpTemp(3));
	CreateAdd(CreateOpVar(4),CreateOpConst(5),CreateOpVar(6));
	CreateSub(CreateOpVar(7),CreateOpConst(8),CreateOpVar(9));
	CreateMul(CreateOpVar(10),CreateOpConst(11),CreateOpVar(12));
	CreateDiv(CreateOpVar(13),CreateOpConst(14),CreateOpVar(15));
	CreateAddressAssign(CreateOpTemp(16),CreateOpVar(17));
	CreateContentAssign(CreateOpTemp(18),CreateOpVar(19));
	CreateAssignContent(CreateOpTemp(20),CreateOpVar(21));
	CreateGoto(CreateOpLabel(22));
	CreateRelopGoto(CreateOpLabel(23),CreateOpVar(24),CreateOpVar(25),GE);
	CreateReturn(CreateOpVar(26));
	CreateDec(CreateOpVar(27),28 );
	CreateArg(CreateOpVar(29));
	CreateCall(CreateOpVar(30),CreateOpFunc("woozie lover"));
	CreateParam(CreateOpVar(32));
	CreateRead(CreateOpVar(33));
	CreateWrite(CreateOpVar(34));
	printCodes();
}

Operand* CreateOpAddr(int id){
	Operand* op=(Operand*)malloc(sizeof(Operand));
	op->kind=ADDR;
	op->u.id=id;
	return op;
}

Operand* CreateOpStar(int id,struct Type* tp){
	Operand* op=(Operand*)malloc(sizeof(Operand));
	op->kind=STAR;
	op->u.id=id;
	op->tp=tp;
	return op;
}

Operand* CreateOpVar(int id){
	Operand* op=(Operand*)malloc(sizeof(Operand));
	op->kind=VAR;
	op->u.id=id;
	return op;
}
Operand* CreateOpVaddr(int id){
	Operand* op=(Operand*)malloc(sizeof(Operand));
	op->kind=VADDR;
	op->u.id=id;
	return op;
}


Operand* CreateOpConst(int value){
	Operand* op=(Operand*)malloc(sizeof(Operand));
	op->kind=CONSTANT;
	op->u.value=value;
	return op;
}

Operand* CreateOpTemp(int id){
	Operand* op=(Operand*)malloc(sizeof(Operand));
	op->kind=TEMP;
	op->u.id=id;
	return op;
}

Operand* CreateOpLabel(int id){
	Operand* op=(Operand*)malloc(sizeof(Operand));
	op->kind=LABEL;
	op->u.id=id;
	return op;
}

Operand* CreateOpFunc(char fname[]){
	Operand* op=(Operand*)malloc(sizeof(Operand));
	op->kind=FUNC;
	op->u.func_name=(char*)malloc(sizeof(char)*(1+strlen(fname)));
	strcpy(op->u.func_name,fname);
	return op;
}


void InsertCode(InterCode* node){
	if(CodeHead==NULL){
		CodeHead=(InterCodes*)malloc(sizeof(InterCodes));
		CodeHead->code=node;
		CodeHead->next=CodeHead;
		CodeHead->prev=CodeHead;
		CodeHead->isEntrance=false;
	}
	else{
		InterCodes* tail=CodeHead->prev;
		tail->next=(InterCodes*)malloc(sizeof(InterCodes));
		tail->next->next=CodeHead;
		CodeHead->prev=tail->next;
		tail->next->prev=tail;
		tail->next->code=node;
		tail->next->isEntrance=false;
	}
}


void CreateLabel(Operand* op){
	//LABEL x
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=LABELDEC;
	node->u.op=op;
	InsertCode(node);
}

void CreateFunction(Operand* op){
	//FUNCTION f
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=FUNCDEC;
	node->u.op=op;
	InsertCode(node);
}

void CreateAssign(Operand* dst,Operand* src){
	//x:=y
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=ASSIGN;
	node->u.assign.left=dst;
	node->u.assign.right=src;
	InsertCode(node);
}

void CreateAdd(Operand* dst,Operand* src1,Operand* src2){
	//x:=y+z
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=ADD;
	node->u.binop.result=dst;
	node->u.binop.op1=src1;
	node->u.binop.op2=src2;
	InsertCode(node);
}

void CreateSub(Operand* dst,Operand* src1,Operand* src2){
	//x:=y-z
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=SUB;
	node->u.binop.result=dst;
	node->u.binop.op1=src1;
	node->u.binop.op2=src2;
	InsertCode(node);
}

void CreateMul(Operand* dst,Operand* src1,Operand* src2){
	//x:=y*z
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=MUL;
	node->u.binop.result=dst;
	node->u.binop.op1=src1;
	node->u.binop.op2=src2;
	InsertCode(node);
}

void CreateDiv(Operand* dst,Operand* src1,Operand* src2){
	//x:=y/z
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=DIV;
	node->u.binop.result=dst;
	node->u.binop.op1=src1;
	node->u.binop.op2=src2;
	InsertCode(node);
}

void CreateAddressAssign(Operand* dst,Operand* src){
	//x:=&y
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=ADDR_ASSIGN;
	node->u.assign.left=dst;
	node->u.assign.right=src;
	InsertCode(node);
}


void CreateContentAssign(Operand* dst,Operand* src){
	//x:=*y
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=CONTENT_ASSIGN;
	node->u.assign.left=dst;
	node->u.assign.right=src;
	InsertCode(node);
}

void CreateAssignContent(Operand* dst,Operand* src){
	//*x:=y
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=ASSIGN_CONTENT;
	node->u.assign.left=dst;
	node->u.assign.right=src;
	InsertCode(node);
}

void CreateGoto(Operand* op){
	//GOTO x
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=GOTO;
	node->u.op=op;
	InsertCode(node);
}

void CreateRelopGoto(Operand* dst,Operand* src1,Operand* src2,RELOP relop){
	// IF x [relop] y Goto z
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=RELOP_GOTO;
	node->u.relop_goto.op1=src1;
	node->u.relop_goto.op2=src2;
	node->u.relop_goto.dst=dst;
	node->u.relop_goto.relop=relop;
	InsertCode(node);
}

void CreateReturn(Operand* op){
	//RETURN x
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=RETURN;
	node->u.op=op;
	InsertCode(node);
}

void CreateDec(Operand* op,int size){
	//DEC x[size]
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=DEC;
	node->u.dec.op=op;
	node->u.dec.size=size;
	InsertCode(node);
}

void CreateArg(Operand* op){
	//ARG x
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=ARG;
	node->u.op=op;
	InsertCode(node);
}

void CreateCall(Operand* dst,Operand* f){
	//x:=CALL f
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=CALL;
	node->u.assign.left=dst;
	node->u.assign.right=f;
	InsertCode(node);
}

void CreateParam(Operand* op){
	//PARAM x
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=PARAM;
	node->u.op=op;
	InsertCode(node);
}

void CreateRead(Operand* op){
	//READ x
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=READ;
	node->u.op=op;
	InsertCode(node);
}

void CreateWrite(Operand* op){
	//WRITE x
	InterCode* node=(InterCode*)malloc(sizeof(InterCode));
	node->kind=WRITE;
	node->u.op=op;
	InsertCode(node);
}

void printCodes(){
	FILE* fp=fopen("interCode.ir","w");
	InterCodes* x=CodeHead;
	printCode(fp,x->code);
	while(x->next!=CodeHead){
		x=x->next;
		printCode(fp,x->code);
	}
	fclose(fp);
}

void printOp(FILE* fp,Operand* op){
	switch(op->kind){
		case(VAR):fprintf(fp,"v%d",op->u.id);break;
		case(VADDR):fprintf(fp,"&v%d",op->u.id);break;
		case(CONSTANT):fprintf(fp,"#%d",op->u.value);break;
		case(TEMP):fprintf(fp,"t%d",op->u.id);break;
		case(LABEL):fprintf(fp,"label%d",op->u.id);break;
		case(FUNC):fprintf(fp,"%s",op->u.func_name);break;
		case(ADDR):fprintf(fp,"&t%d",op->u.id);break;
		case(STAR):fprintf(fp,"*t%d",op->u.id);break;
		default:break;
	}
}

void printRelop(FILE* fp,RELOP r){
	switch(r){
		case(EQ):fprintf(fp,"==");break;
		case(NEQ):fprintf(fp,"!=");break;
		case(G):fprintf(fp,">");break;
		case(GE):fprintf(fp,">=");break;
		case(L):fprintf(fp,"<");break;
		case(LE):fprintf(fp,"<=");break;
		default:break;
	}
}


void printCode(FILE* fp,InterCode* x){
	switch(x->kind){
		case(LABELDEC):		fprintf(fp,"LABEL ");printOp(fp,x->u.op);fprintf(fp," :");break;
		case(FUNCDEC):		if(!firstFunc)fprintf(fp,"\n");else firstFunc=0;fprintf(fp,"FUNCTION ");printOp(fp,x->u.op);fprintf(fp," :");break;
		case(ASSIGN):		printOp(fp,x->u.assign.left);fprintf(fp," := ");printOp(fp,x->u.assign.right);break;
		case(ADD):		printOp(fp,x->u.binop.result);fprintf(fp," := ");printOp(fp,x->u.binop.op1);fprintf(fp," + ");printOp(fp,x->u.binop.op2);break;
		case(SUB):		printOp(fp,x->u.binop.result);fprintf(fp," := ");printOp(fp,x->u.binop.op1);fprintf(fp," - ");printOp(fp,x->u.binop.op2);break;
		case(MUL):		printOp(fp,x->u.binop.result);fprintf(fp," := ");printOp(fp,x->u.binop.op1);fprintf(fp," * ");printOp(fp,x->u.binop.op2);break;
		case(DIV):		printOp(fp,x->u.binop.result);fprintf(fp," := ");printOp(fp,x->u.binop.op1);fprintf(fp," / ");printOp(fp,x->u.binop.op2);break;
		case(ADDR_ASSIGN):	printOp(fp,x->u.assign.left);fprintf(fp," := &");printOp(fp,x->u.assign.right);break;
		case(CONTENT_ASSIGN):	printOp(fp,x->u.assign.left);fprintf(fp," := *");printOp(fp,x->u.assign.right);break;
		case(ASSIGN_CONTENT):	fprintf(fp,"*");printOp(fp,x->u.assign.left);fprintf(fp," := ");printOp(fp,x->u.assign.right);break;
		case(GOTO):		fprintf(fp,"GOTO ");printOp(fp,x->u.op);break;
		case(RELOP_GOTO):	fprintf(fp,"IF ");printOp(fp,x->u.relop_goto.op1);fprintf(fp," ");printRelop(fp,x->u.relop_goto.relop);fprintf(fp," ");
					printOp(fp,x->u.relop_goto.op2);fprintf(fp," GOTO ");printOp(fp,x->u.relop_goto.dst);break;
		case(RETURN):		fprintf(fp,"RETURN ");printOp(fp,x->u.op);break;
		case(DEC):		fprintf(fp,"DEC ");printOp(fp,x->u.dec.op);fprintf(fp," %d",x->u.dec.size);break;
		case(ARG):		fprintf(fp,"ARG ");printOp(fp,x->u.op);break;
		case(CALL):		printOp(fp,x->u.assign.left);fprintf(fp," := CALL ");printOp(fp,x->u.assign.right);break;
		case(PARAM):		fprintf(fp,"PARAM ");printOp(fp,x->u.op);break;
		case(READ):		fprintf(fp,"READ ");printOp(fp,x->u.op);break;
		case(WRITE):		fprintf(fp,"WRITE ");printOp(fp,x->u.op);break;
		default:		break;
	}
	fprintf(fp,"\n");
	
}





