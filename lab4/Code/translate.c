#include"translate.h"
#include<string.h>
#include<stdio.h>
#include<assert.h>
void translateTree(struct Node* root){
	if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==29){
		translateExtDefList(root->subNodeList[0]);
	}
	else exit(0);
	printCodes();
}

void translateExtDefList(struct Node* root){
	if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==30&&root->subNodeList[1]->type==29){
		//ExitDefList -> ExtDef ExtDefList
		translateExtDef(root->subNodeList[0]);
		translateExtDefList(root->subNodeList[1]);
	}
	else if(root!=NULL&&root->subNum==0&&strcmp(root->value,"e")==0){
		//ExtDefList -> e
		;
	}
	else assert(0);
}


void translateExtDef(struct Node* root){
	if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==32&&root->subNodeList[1]->type==31&&root->subNodeList[2]->type==4){
		//ExtDef -> Specifier ExtDecList SEMI
		translateSpecifier(root->subNodeList[0]);
		translateExtDecList(root->subNodeList[1]);	
	}
	else if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==32&&root->subNodeList[1]->type==4){
		//ExtDef-> Specifier SEMI
		translateSpecifier(root->subNodeList[0]);
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==32&&root->subNodeList[1]->type==37&&root->subNodeList[2]->type==40){
		//ExtDef -> Specifier FunDec CompSt 
		translateSpecifier(root->subNodeList[0]);
		translateFunDec(root->subNodeList[1]);
		translateCompSt(root->subNodeList[2]);	
	}
	else assert(0);
}

void translateExtDecList(struct Node* root){
	if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==36){
		//ExtDecList -> VarDec
		;//translateVarDec(root->subNodeList[0],false);
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==36&&root->subNodeList[1]->type==5&&root->subNodeList[2]->type==31){
		//ExtDecList -> VarDec COMMA ExtDecList
		//translateVarDec(root->subNodeList[0],false);
		translateExtDecList(root->subNodeList[2]);
	}
	else assert(0);
}

struct Type* translateSpecifier(struct Node* root){
	if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==16){
		//Specifier -> TYPE
		struct Type* rtn=(struct Type*)malloc(sizeof(struct Type));
		rtn->kind=BASIC;
		rtn->u.basic=TYPE(root->subNodeList[0]);
		return rtn;
	}
	else if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==33){
		//Specifier -> StructSpecifier
		return translateStructSpecifier(root->subNodeList[0]);
	}
	else assert(0);
}


struct Type* translateStructSpecifier(struct Node* root){
	if(root!=NULL&&root->subNum==5&&root->subNodeList[0]->type==23&&root->subNodeList[1]->type==34
		&&root->subNodeList[2]->type==21&&root->subNodeList[3]->type==43&&root->subNodeList[4]->type==22){
		//StructSpecifier -> STRUCT OptTag LC DefList RC		
		struct Node* tmpn=OptTag(root->subNodeList[1]);
		struct Structure* strt=(struct Structure*)malloc(sizeof(struct Structure));
		if(tmpn!=NULL){
			strt->name=(char*)malloc(1+strlen(tmpn->value)*sizeof(char));
			strcpy(strt->name,tmpn->value);
			strt->id=0;
		}
			
		else {
			strt->name=NULL;
			strt->id=0;			
		}
		strt->structure=DefList(root->subNodeList[3],true);
		if(strt->structure==NULL){
			return NULL;
		}

		struct Type* rtn=(struct Type*)malloc(sizeof(struct Type));
		rtn->kind=STRUCTURE;
		rtn->u.strt=strt;
		return 	rtn;
	}
	else if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==23&&root->subNodeList[1]->type==35){
		//StructSpecifier -> STRUCT Tag
		struct Node* tmpn=Tag(root->subNodeList[1]);
		struct Structure* strt=HashFind(STRUCTURE_DEF,tmpn->value);
		struct Type* rtn=(struct Type*)malloc(sizeof(struct Type));
		rtn->kind=STRUCTURE;
		rtn->u.strt=strt;
		return rtn;
	}
	else assert(0);

}



void translateOptTag(struct Node* root){
	if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==3){
		//OptTag -> ID
		;
	}
	else if(root!=NULL&&root->subNum==0&&strcmp(root->value,"e")==0){
		//OptTag -> e
		;
	}
	else assert(0);
	
}

void translateTag(struct Node* root){
	if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==3){
		//Tag -> ID

	}
	else assert(0);

}

Operand* translateVarDec(struct Node* root,int isParam,int siz,struct Type* tp){
	if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==3){
		//VarDec -> ID
		int id;
		if(isParam&&(tp->kind==STRUCTURE))
			id=InsertValue(root->subNodeList[0]->value,tp,siz,true);
			
		else
			id=InsertValue(root->subNodeList[0]->value,tp,siz,false);
		Operand* v=CreateOpVar(id);
		if(isParam)
			CreateParam(v);
		return v;
	}
	else if(root!=NULL&&root->subNum==4&&root->subNodeList[0]->type==36&&root->subNodeList[1]->type==19&&root->subNodeList[2]->type==1&&root->subNodeList[3]->type==20){
		//VarDec -> VarDec LB INT RB	
		if(isParam){
			printf("Cannot translate: Code contains parameters of array type.\n");			
			exit(0);
		}
		else{	
			if(siz!=-1){
				printf("Cannot translate:Code contains variables of multi-dimensional array type.\n");
				exit(0);
			}
			siz=atoi(root->subNodeList[2]->value);
			Operand* op=translateVarDec(root->subNodeList[0],isParam,siz,tp);
			return op;
		}
	
	}
	else assert(0);
}

void translateFunDec(struct Node* root){
	if(root!=NULL&&root->subNum==4&&root->subNodeList[0]->type==3&&root->subNodeList[1]->type==17&&root->subNodeList[2]->type==38&&root->subNodeList[3]->type==18){
		//FunDec -> ID LP VarList RP		
		CreateFunction(CreateOpFunc(root->subNodeList[0]->value));
		translateVarList(root->subNodeList[2]);

	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==3&&root->subNodeList[1]->type==17&&root->subNodeList[2]->type==18){
		//FunDec -> ID LP RP
		CreateFunction(CreateOpFunc(root->subNodeList[0]->value));
	}
	else assert(0);
}

void  translateVarList(struct Node* root){
	if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==39&&root->subNodeList[1]->type==5&&root->subNodeList[2]->type==38){
		//VarList -> ParamDec COMMA VarList
		translateParamDec(root->subNodeList[0]);
		translateVarList(root->subNodeList[2]);
	}
	else if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==39){
		//VarList -> ParamDec
		translateParamDec(root->subNodeList[0]);
	}
	else assert(0);
}

void translateParamDec(struct Node* root){
	if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==32&&root->subNodeList[1]->type==36){
		//ParamDec -> Specifier VarDec
		struct Type* tp=translateSpecifier(root->subNodeList[0]);
		translateVarDec(root->subNodeList[1],true,-1,tp);
	}
	else assert(0);
}

void translateCompSt(struct Node* root){
	if(root!=NULL&&root->subNum==4&&root->subNodeList[0]->type==21&&root->subNodeList[1]->type==43&&root->subNodeList[2]->type==41&&root->subNodeList[3]->type==22){
		//CompSt -> LC DefList StmtList RC
		translateDefList(root->subNodeList[1]);
		translateStmtList(root->subNodeList[2]);
	}
	else assert(0);
}

void translateStmtList(struct Node* root){
	if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==42&&root->subNodeList[1]->type==41){
		//StmtList -> Stmt StmtList
		translateStmt(root->subNodeList[0]);
		translateStmtList(root->subNodeList[1]);
	}
	else if(root!=NULL&&root->subNum==0&&strcmp(root->value,"e")==0){
		//StmtList-> e
		;
	}
	else assert(0);
}

void translateStmt(struct Node* root){
	if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==4){
		//Stmt -> Exp SEMI
		translateExp(root->subNodeList[0]);
	}
	else if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==40){
		//Stmt -> CompSt
		translateCompSt(root->subNodeList[0]);
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==24&&root->subNodeList[1]->type==47&&root->subNodeList[2]->type==4){
		//Stmt -> RETURN Exp SEMI
		Operand* op=translateExp(root->subNodeList[1]);
		CreateReturn(op);
	}
	else if(root!=NULL&&root->subNum==5&&root->subNodeList[0]->type==25&&root->subNodeList[1]->type==17
		&&root->subNodeList[2]->type==47&&root->subNodeList[3]->type==18&&root->subNodeList[4]->type==42){
		//Stmt -> IF LP Exp RP Stmt
		LabelNum=LabelNum+1;
		Operand* label1=CreateOpLabel(LabelNum);
		LabelNum=LabelNum+1;
		Operand* label2=CreateOpLabel(LabelNum);
		translateCond(root->subNodeList[2],label1,label2);
		CreateLabel(label1);
		translateStmt(root->subNodeList[4]);
		CreateLabel(label2);
	}
	else if(root!=NULL&&root->subNum==7&&root->subNodeList[0]->type==25&&root->subNodeList[1]->type==17
		&&root->subNodeList[2]->type==47&&root->subNodeList[3]->type==18&&root->subNodeList[4]->type==42
		&&root->subNodeList[5]->type==26&&root->subNodeList[6]->type==42){
		//Stmt -> IF LP Exp RP Stmt ELSE Stmt
		LabelNum=LabelNum+1;
		Operand* label1=CreateOpLabel(LabelNum);
		LabelNum=LabelNum+1;
		Operand* label2=CreateOpLabel(LabelNum);
		LabelNum=LabelNum+1;
		Operand* label3=CreateOpLabel(LabelNum);
		translateCond(root->subNodeList[2],label1,label2);
		CreateLabel(label1);
		translateStmt(root->subNodeList[4]);
		CreateGoto(label3);
		CreateLabel(label2);
		translateStmt(root->subNodeList[6]);
		CreateLabel(label3);	
	}
	else if(root!=NULL&&root->subNum==5&&root->subNodeList[0]->type==27&&root->subNodeList[1]->type==17
		&&root->subNodeList[2]->type==47&&root->subNodeList[3]->type==18&&root->subNodeList[4]->type==42){
		//Stmt -> WHILE LP Exp RP Stmt
		LabelNum=LabelNum+1;
		Operand* label1=CreateOpLabel(LabelNum);
		LabelNum=LabelNum+1;
		Operand* label2=CreateOpLabel(LabelNum);
		LabelNum=LabelNum+1;
		Operand* label3=CreateOpLabel(LabelNum);
		CreateLabel(label1);
		translateCond(root->subNodeList[2],label2,label3);
		CreateLabel(label2);
		translateStmt(root->subNodeList[4]);
		CreateGoto(label1);
		CreateLabel(label3);		
	}
	else assert(0);
}

void translateDefList(struct Node* root){
	
	if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==44&&root->subNodeList[1]->type==43){
		//DefList -> Def DefList
		translateDef(root->subNodeList[0]);
		translateDefList(root->subNodeList[1]);

	}
	else if(root!=NULL&&root->subNum==0&&strcmp(root->value,"e")==0){
		//DefList -> e
		;
	}
	else assert(0);
}

void translateDef(struct Node* root){
	if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==32&&root->subNodeList[1]->type==45&&root->subNodeList[2]->type==4){
		//Def -> Specifier DecList SEMI
		struct Type* tp=translateSpecifier(root->subNodeList[0]);
		translateDecList(root->subNodeList[1],tp);		
	}
	else assert(0);
}

void translateDecList(struct Node* root,struct Type* tp){
	if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==46){
		//DecList -> Dec
		translateDec(root->subNodeList[0],tp);
	}

	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==46&&root->subNodeList[1]->type==5&&root->subNodeList[2]->type==45){
		//DecList -> Dec COMMA DecList
		translateDec(root->subNodeList[0],tp);
		translateDecList(root->subNodeList[2],tp);
	}
	else assert(0);
}

void translateDec(struct Node* root,struct Type* tp){
	if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==36){
		//Dec -> VarDec
		Operand* op=translateVarDec(root->subNodeList[0],false,-1,tp);
		ValueList* vl=findValue2(op->u.id);
		if(vl->dimension!=-1&&tp->kind==BASIC){
			//one dimension INT array
			CreateDec(op,4*vl->dimension);
		}
		else if(vl->dimension!=-1){
			// one dimension structure array
			CreateDec(op,getSize(tp)*vl->dimension);
		}
		else if(tp->kind!=BASIC){
			//structure variable declare
			CreateDec(op,getSize(tp));
		}
	}

	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==36&&root->subNodeList[1]->type==6&&root->subNodeList[2]->type==47){
		//Dec -> VarDec ASSIGNOP Exp
		Operand* op1=translateVarDec(root->subNodeList[0],false,-1,tp);
		Operand* op2=translateExp(root->subNodeList[2]);
		CreateAssign(op1,op2);
	}
	else assert(0);
}

void translateCond(struct Node* root,Operand* labelTrue,Operand* labelFalse){
	if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==12&&root->subNodeList[2]->type==47){
		//Exp -> Exp AND Exp
		LabelNum=LabelNum+1;
		Operand* label1=CreateOpLabel(LabelNum);
		translateCond(root->subNodeList[0],label1,labelFalse);
		CreateLabel(label1);
		translateCond(root->subNodeList[2],labelTrue,labelFalse);
		
	}

	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==13&&root->subNodeList[2]->type==47){
		//Exp -> Exp OR Exp
		LabelNum=LabelNum+1;
		Operand* label1=CreateOpLabel(LabelNum);
		translateCond(root->subNodeList[0],labelTrue,label1);
		CreateLabel(label1);
		translateCond(root->subNodeList[2],labelTrue,labelFalse);
	}

	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==7&&root->subNodeList[2]->type==47){
		//Exp -> Exp RELOP Exp
		Operand* t1=translateExp(root->subNodeList[0]);
		Operand* t2=translateExp(root->subNodeList[2]);
		RELOP rp=getRelop(root->subNodeList[1]->value);
		CreateRelopGoto(labelTrue,t1,t2,rp);
		CreateGoto(labelFalse);
		
	}
	else if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==15&&root->subNodeList[1]->type==47){
		//Exp -> NOT Exp
		translateCond(root,labelFalse,labelTrue);
	}
	else{
		Operand* t1=translateExp(root);
		CreateRelopGoto(labelTrue,t1,CreateOpConst(0),NEQ);
		CreateGoto(labelFalse);
	}

}

Operand* translateExp(struct Node* root){
	if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==6&&root->subNodeList[2]->type==47){
		//Exp -> Exp ASSIGNOP Exp
		Operand* op1=translateExp(root->subNodeList[0]);
		Operand* op2=translateExp(root->subNodeList[2]);
		if((op1->kind==VAR||op1->kind==STAR)&&((op2->kind==VAR||op2->kind==STAR))){
			if(op1->kind==VAR&&op2->kind==VAR){
				//v = v
				ValueList* vl=findValue2(op1->u.id);
				if(vl->tp->kind==STRUCTURE || vl->dimension!=-1){
					//ASSIGNOP between structure or array
					ValueList* vl2=findValue2(op2->u.id);
					int dim=1;
					int totalSize=0;
					if(vl->dimension>0)
						dim=vl->dimension;
					if(vl2->dimension<dim)
						dim=vl2->dimension; //min(size(array1),size(array2))
					totalSize=getSize(vl->tp)*dim;
					TempNum=TempNum+1;
					Operand* t1=CreateOpTemp(TempNum);
					TempNum=TempNum+1;
					Operand* t2=CreateOpTemp(TempNum);
	
					for(int i=0;i<totalSize;i=i+4){
						if(vl->tp->kind==STRUCTURE&&vl->isParamStruct){
							CreateAdd(t1,op1,CreateOpConst(i));
						}
						else{
							CreateAdd(t1,CreateOpVaddr(op1->u.id),CreateOpConst(i));
						}
						if(vl2->tp->kind==STRUCTURE&&vl2->isParamStruct){
							CreateAdd(t2,op2,CreateOpConst(i));
						}
						else{
							CreateAdd(t2,CreateOpVaddr(op2->u.id),CreateOpConst(i));
						}
						CreateAssign(CreateOpStar(t1->u.id,NULL),CreateOpStar(t2->u.id,NULL));
					}
				}
				else
					CreateAssign(op1,op2);
			}
			else if(op1->kind==STAR&&op2->kind==STAR){
				//*t = *t
				if(op1->tp->kind!=BASIC){
					//ASSIGNOP between structure or array
					int dim=1;
					int totalSize=0;
					if(op1->tp->kind==ARRAY)
						dim=op1->tp->u.array.size;
					if(op2->tp->kind==ARRAY&&op2->tp->u.array.size<dim)
						dim=op2->tp->u.array.size;//min(size(array1),size(array2))
					
					totalSize=getSize(op1->tp)*dim;
					TempNum=TempNum+1;
					Operand* t1=CreateOpTemp(TempNum);
					TempNum=TempNum+1;
					Operand* t2=CreateOpTemp(TempNum);
					for(int i=0;i<totalSize;i=i+4){
						CreateAdd(t1,CreateOpTemp(op1->u.id),CreateOpConst(i));
						CreateAdd(t2,CreateOpTemp(op2->u.id),CreateOpConst(i));
						CreateAssign(CreateOpStar(t1->u.id,NULL),CreateOpStar(t2->u.id,NULL));
					}
				}
				else
					CreateAssign(op1,op2);
			}
			else if(op1->kind==VAR&&op2->kind==STAR){
				//v = *t
				ValueList* vl=findValue2(op1->u.id);
				if(vl->tp->kind==STRUCTURE || vl->dimension!=-1){
					//ASSIGNOP between structure or array
					int dim=1;
					int totalSize=0;
					if(vl->dimension>0)
						dim=vl->dimension;
					if(op2->tp->kind==ARRAY&&op2->tp->u.array.size<dim)
						dim=op2->tp->u.array.size;//min(size(array1),size(array2))

					totalSize=getSize(vl->tp)*dim;
					TempNum=TempNum+1;
					Operand* t1=CreateOpTemp(TempNum);
					TempNum=TempNum+1;
					Operand* t2=CreateOpTemp(TempNum);
	
					for(int i=0;i<totalSize;i=i+4){
						if(vl->tp->kind==STRUCTURE&&vl->isParamStruct){
							CreateAdd(t1,op1,CreateOpConst(i));
						}
						else{
							CreateAdd(t1,CreateOpVaddr(op1->u.id),CreateOpConst(i));
						}
						CreateAdd(t2,CreateOpTemp(op2->u.id),CreateOpConst(i));
						CreateAssign(CreateOpStar(t1->u.id,NULL),CreateOpStar(t2->u.id,NULL));
					}
				}
				else
					CreateAssign(op1,op2);
			}
			else{
				//*t = v
				if(op1->tp->kind!=BASIC){
					//ASSIGNOP between structure or array
					ValueList* vl2=findValue2(op2->u.id);
					int dim=1;
					int totalSize=0;
					if(op1->tp->kind==ARRAY)
						dim=op1->tp->u.array.size;
					if(vl2->dimension<dim)
						dim=vl2->dimension; //min(size(array1),size(array2))

					totalSize=getSize(op1->tp)*dim;
					TempNum=TempNum+1;
					Operand* t1=CreateOpTemp(TempNum);
					TempNum=TempNum+1;
					Operand* t2=CreateOpTemp(TempNum);
					for(int i=0;i<totalSize;i=i+4){
						CreateAdd(t1,CreateOpTemp(op1->u.id),CreateOpConst(i));
						if(vl2->tp->kind==STRUCTURE&&vl2->isParamStruct){
							CreateAdd(t2,op2,CreateOpConst(i));
						}
						else{
							CreateAdd(t2,CreateOpVaddr(op2->u.id),CreateOpConst(i));
						}
						CreateAssign(CreateOpStar(t1->u.id,NULL),CreateOpStar(t2->u.id,NULL));
					}
				}
				else
					CreateAssign(op1,op2);
			}
		}
		else
			CreateAssign(op1,op2);
		return op1;
	}

	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==12&&root->subNodeList[2]->type==47){
		//Exp -> Exp AND Exp
		LabelNum=LabelNum+1;
		Operand* label1=CreateOpLabel(LabelNum);
		LabelNum=LabelNum+1;
		Operand* label2=CreateOpLabel(LabelNum);
		TempNum=TempNum+1;
		Operand* t=CreateOpTemp(TempNum);
		CreateAssign(t,CreateOpConst(0));
		translateCond(root,label1,label2);
		CreateLabel(label1);
		CreateAssign(t,CreateOpConst(1));
		CreateLabel(label2);
		return t;
	}

	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==13&&root->subNodeList[2]->type==47){
		//Exp -> Exp OR Exp
		LabelNum=LabelNum+1;
		Operand* label1=CreateOpLabel(LabelNum);
		LabelNum=LabelNum+1;
		Operand* label2=CreateOpLabel(LabelNum);
		TempNum=TempNum+1;
		Operand* t=CreateOpTemp(TempNum);
		CreateAssign(t,CreateOpConst(0));
		translateCond(root,label1,label2);
		CreateLabel(label1);
		CreateAssign(t,CreateOpConst(1));
		CreateLabel(label2);
		return t;
	}

	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==7&&root->subNodeList[2]->type==47){
		//Exp -> Exp RELOP Exp
		LabelNum=LabelNum+1;
		Operand* label1=CreateOpLabel(LabelNum);
		LabelNum=LabelNum+1;
		Operand* label2=CreateOpLabel(LabelNum);
		TempNum=TempNum+1;
		Operand* t=CreateOpTemp(TempNum);
		CreateAssign(t,CreateOpConst(0));
		translateCond(root,label1,label2);
		CreateLabel(label1);
		CreateAssign(t,CreateOpConst(1));
		CreateLabel(label2);
		return t;
	}

	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==8&&root->subNodeList[2]->type==47){
		//Exp -> Exp PLUS Exp
		Operand* t1=translateExp(root->subNodeList[0]);
		Operand* t2=translateExp(root->subNodeList[2]);
		TempNum=TempNum+1;
		Operand* t3=CreateOpTemp(TempNum);
		CreateAdd(t3,t1,t2);
		return t3;
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==9&&root->subNodeList[2]->type==47){
		//Exp -> Exp MINUS Exp		
		Operand* t1=translateExp(root->subNodeList[0]);
		Operand* t2=translateExp(root->subNodeList[2]);
		TempNum=TempNum+1;
		Operand* t3=CreateOpTemp(TempNum);
		CreateSub(t3,t1,t2);
		return t3;
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==10&&root->subNodeList[2]->type==47){
		//Exp -> Exp STAR Exp
		Operand* t1=translateExp(root->subNodeList[0]);
		Operand* t2=translateExp(root->subNodeList[2]);
		TempNum=TempNum+1;
		Operand* t3=CreateOpTemp(TempNum);
		CreateMul(t3,t1,t2);
		return t3;
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==11&&root->subNodeList[2]->type==47){
		//Exp -> Exp DIV Exp
		Operand* t1=translateExp(root->subNodeList[0]);
		Operand* t2=translateExp(root->subNodeList[2]);
		TempNum=TempNum+1;
		Operand* t3=CreateOpTemp(TempNum);
		CreateDiv(t3,t1,t2);
		return t3;
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==17&&root->subNodeList[1]->type==47&&root->subNodeList[2]->type==18){
		//Exp -> LP Exp RP
		translateExp(root->subNodeList[1]);
	}
	else if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==9&&root->subNodeList[1]->type==47){
		//Exp -> MINUS Exp
		Operand* src=translateExp(root->subNodeList[1]);
		TempNum=TempNum+1;
		Operand* dst=CreateOpTemp(TempNum);		
		CreateSub(dst,CreateOpConst(0),src);
		return dst;
	}
	else if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==15&&root->subNodeList[1]->type==47){
		//Exp -> NOT Exp
		LabelNum=LabelNum+1;
		Operand* label1=CreateOpLabel(LabelNum);
		LabelNum=LabelNum+1;
		Operand* label2=CreateOpLabel(LabelNum);
		TempNum=TempNum+1;
		Operand* t=CreateOpTemp(TempNum);
		CreateAssign(t,CreateOpConst(0));
		translateCond(root,label1,label2);
		CreateLabel(label1);
		CreateAssign(t,CreateOpConst(1));
		CreateLabel(label2);
		return t;

	}
	else if(root!=NULL&&root->subNum==4&&root->subNodeList[0]->type==3&&root->subNodeList[1]->type==17&&root->subNodeList[2]->type==48&&root->subNodeList[3]->type==18){
		//Exp -> ID LP Args RP
		ArgsList* arg=(ArgsList*)malloc(sizeof(ArgsList));
		translateArgs(root->subNodeList[2],arg);
		if(!strcmp(root->subNodeList[0]->value,"write")){
			CreateWrite(arg->op);
			return CreateOpConst(0);
		}
		else{
			TempNum=TempNum+1;
			Operand* t=CreateOpTemp(TempNum);
			ArgsList* x=arg;
			int len=0;
			if(x!=NULL){
				len=len+1;
				while(x->next!=NULL){
					x=x->next;
					len=len+1;
				}
			}
			ArgsList** argListArray=(ArgsList**)malloc(sizeof(ArgsList*)*len);
			x=arg;
			for(int i=0;i<len;i++){
				argListArray[i]=x;
				x=x->next;
			}
			for(int i=len-1;i>=0;i--){
				if((argListArray[i]->op->kind==VAR)&&(findValue2(argListArray[i]->op->u.id)->tp->kind==STRUCTURE)){
						CreateArg(CreateOpVaddr(argListArray[i]->op->u.id));
				}
				else
					CreateArg(argListArray[i]->op);
			}
			/*while(x!=NULL){
				if((x->op->kind==VAR)&&(findValue2(x->op->u.id)->tp->kind==STRUCTURE)){
						CreateArg(CreateOpVaddr(x->op->u.id));
				}
				else
					CreateArg(x->op);
				x=x->next;
			}*/
			Operand* f=CreateOpFunc(root->subNodeList[0]->value);		
			CreateCall(t, f);
			return t;
		}
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==3&&root->subNodeList[1]->type==17&&root->subNodeList[2]->type==18){
		//Exp -> ID LP RP
		TempNum=TempNum+1;
		Operand* t=CreateOpTemp(TempNum);
		if(!strcmp(root->subNodeList[0]->value,"read"))
			CreateRead(t);
		else{
			Operand* f=CreateOpFunc(root->subNodeList[0]->value);		
			CreateCall(t, f);
		}
		return t;
			
	}
	else if(root!=NULL&&root->subNum==4&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==19&&root->subNodeList[2]->type==47&&root->subNodeList[3]->type==20){
		//Exp -> Exp LB Exp RB
		Operand* op1=translateExp(root->subNodeList[0]);
		Operand* op2=translateExp(root->subNodeList[2]);
		if(op1->kind==VAR){
			ValueList* vl=findValue2(op1->u.id);
			int sz=getSize(vl->tp);
			TempNum=TempNum+1;
			Operand* t1=CreateOpTemp(TempNum);
			CreateMul(t1,op2,CreateOpConst(sz));
			TempNum=TempNum+1;
			Operand* t2=CreateOpTemp(TempNum);
			CreateAddressAssign(t2,op1);
			TempNum=TempNum+1;
			Operand* t3=CreateOpTemp(TempNum);
			CreateAdd(t3,t2,t1);
			return CreateOpStar(t3->u.id,vl->tp);
		}
		else if(op1->kind==STAR){
			int sz=getSize(op1->tp->u.array.elem);
			TempNum=TempNum+1;
			Operand* t1=CreateOpTemp(TempNum);
			CreateMul(t1,op2,CreateOpConst(sz));
			TempNum=TempNum+1;
			Operand* t3=CreateOpTemp(TempNum);
			CreateAdd(t3,CreateOpTemp(op1->u.id),t1);
			return CreateOpStar(t3->u.id,op1->tp);
		}	
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==14&&root->subNodeList[2]->type==3){
		//Exp -> Exp DOT ID
		Operand* op1=translateExp(root->subNodeList[0]);
		if(op1->kind==VAR){
			ValueList* vl=findValue2(op1->u.id);
			int offset=getOffset(root->subNodeList[2]->value,vl->tp->u.strt);
			if(vl->isParamStruct){
				TempNum=TempNum+1;
				Operand* t1=CreateOpTemp(TempNum);
				CreateAdd(t1,op1,CreateOpConst(offset));
				return CreateOpStar(t1->u.id,getType(root->subNodeList[2]->value,vl->tp->u.strt));
			}
			else{
				TempNum=TempNum+1;
				Operand* t1=CreateOpTemp(TempNum);
				CreateAddressAssign(t1,op1);
				TempNum=TempNum+1;
				Operand* t2=CreateOpTemp(TempNum);
				CreateAdd(t2,t1,CreateOpConst(offset));
				return CreateOpStar(t2->u.id,getType(root->subNodeList[2]->value,vl->tp->u.strt));
			}
			
		}
		else if(op1->kind==STAR){
			int offset=getOffset(root->subNodeList[2]->value,op1->tp->u.strt);
			TempNum=TempNum+1;
			Operand* t1=CreateOpTemp(TempNum);
			TempNum=TempNum+1;
			Operand* t2=CreateOpTemp(TempNum);
			CreateAdd(t2,CreateOpTemp(op1->u.id),CreateOpConst(offset));
			return CreateOpStar(t2->u.id,getType(root->subNodeList[2]->value,op1->tp->u.strt));
		}
		else return CreateOpConst(-1);
	}
	else if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==3){
		//Exp -> ID
		ValueList* vl=findValue(root->subNodeList[0]->value);
		if(vl->id==-1){
			//id=InsertValue(root->subNodeList[0]->value);
			assert(0);
			//semantic error
		}
		Operand* v=CreateOpVar(vl->id);
		return v;
		
	}
	else if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==1){
		//Exp -> INT
		//INT(root->subNodeList[0]);
		int x=atoi(root->subNodeList[0]->value);
		return CreateOpConst(x);
	}
	else if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==2){
		//Exp -> FLOAT
		return CreateOpConst(-1);
	}
	else assert(0);
}

void translateArgs(struct Node* root,ArgsList* arg_list){
	if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==5
		&&root->subNodeList[2]->type==48){
		//Args -> Exp COMMA Args
		Operand* op=translateExp(root->subNodeList[0]);
		arg_list->op=op;
		arg_list->next=(ArgsList*)malloc(sizeof(ArgsList));
		translateArgs(root->subNodeList[2],arg_list->next);
	}

	else if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==47){
		//Args -> Exp
		Operand* op=translateExp(root->subNodeList[0]);
		arg_list->op=op;
		arg_list->next=NULL;
	}
	else assert(0);
}





