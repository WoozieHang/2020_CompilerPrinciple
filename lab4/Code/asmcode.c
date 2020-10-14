#include"asmcode.h"

char s0[]="$s0";
char s1[]="$s1";
char s2[]="$s2";
char s3[]="$s3";
char s4[]="$s4";
char s5[]="$s5";
char s6[]="$s6";
char s7[]="$s7";
char t0[]="$t0";
char t1[]="$t1";
char t2[]="$t2";
char t3[]="$t3";
char t4[]="$t4";
char t5[]="$t5";
char t6[]="$t6";
char t7[]="$t7";
char t8[]="$t8";
char t9[]="$t9";
char a0[]="$a0";
char a1[]="$a1";
char a2[]="$a2";
char a3[]="$a3";


int lastRegId=-1;//0-9 means t0-t9,other case -1
RegDescriptor rd[10];//just use for $t
FunctionBlockDescriptor* fbd;
FunctionBlockDescriptor* fbd_index;
InterCodes* current_entrance;
InterCodes* current_code;
FILE* file_index;
bool prepareCall=false;
int ARG_NUM=0;
int ARG_NUM_LATER=0;
bool deBugMode=false;
int stackOffsetForArg=0;
bool isBlockEnd=false;

void deBug(){
	FunctionBlockDescriptor* x=fbd;
	while(x!=NULL){
		deBugFBD(x);
		x=x->next;
	}
}

void deBugFBD(FunctionBlockDescriptor* x){
	printf("%s:\nlocalVarSize:%d\n",x->funcName,x->localVarSize);
	ValueDescriptor* tmp=x->vd;
	while(tmp!=NULL){
		deBugVD(tmp);
		tmp=tmp->next;
	}
	printf("\n");
}

void deBugVD(ValueDescriptor* x){
	if(x->isV)
		printf("v%d",x->id);
	
	else printf("t%d",x->id);

	printf(", size:%d, isArray:%d, isParam:%d\n",x->size,x->isArray,x->isParam);
}

void InitialBlock(){
	//first traversal
	InterCodes* x=CodeHead;
	x->isEntrance=true;
	dealBlock(x);
	while(x->next!=CodeHead){
		x=x->next;
		dealBlock(x);
	}
	//second traversal
	FunctionBlockDescriptor* currentFBD=NULL;

	x=CodeHead;
	fbd=(FunctionBlockDescriptor*)malloc(sizeof(FunctionBlockDescriptor));
	fbd->funcName=x->code->u.op->u.func_name;
	fbd->vd=NULL;
	fbd->next=NULL;
	currentFBD=fbd;

	while(x->next!=CodeHead){
		x=x->next;
		if(x->code->kind==FUNCDEC){
			currentFBD->next=(FunctionBlockDescriptor*)malloc(sizeof(FunctionBlockDescriptor));
			currentFBD=currentFBD->next;
			currentFBD->funcName=x->code->u.op->u.func_name;
			currentFBD->vd=NULL;
			currentFBD->next=NULL;
		}

		dealFuncBlock(x,currentFBD);
		
	}
	
	//third traversal,set fbd->localVarSize
	currentFBD=fbd;
	while(currentFBD!=NULL){
		currentFBD->localVarSize=getLocalVarSize(currentFBD);
		currentFBD=currentFBD->next;
	}

}

void dealValue(Operand* op,FunctionBlockDescriptor* currentFBD,bool isArray,bool isParam,int size){
	bool isV;
	int id=op->u.id;
	if(op->kind==VAR)
		isV=true;
	else if(op->kind==TEMP)
		isV=false;
	else return;

	if(currentFBD->vd==NULL){
		currentFBD->vd=(ValueDescriptor*)malloc(sizeof(ValueDescriptor));
		currentFBD->vd->isV=isV;
		currentFBD->vd->id=id;
		currentFBD->vd->size=size;
		currentFBD->vd->isArray=isArray;
		currentFBD->vd->isParam=isParam;
		currentFBD->vd->next=NULL;
	}
	else{
		ValueDescriptor* x=currentFBD->vd;
		if(x->isV==isV&&x->id==id)
				return;
		while(x->next!=NULL){
			x=x->next;
			if(x->isV==isV&&x->id==id)
				return;
			
		}
		x->next=(ValueDescriptor*)malloc(sizeof(ValueDescriptor));
		x=x->next;
		x->isV=isV;
		x->id=id;
		x->size=size;
		x->isArray=isArray;
		x->isParam=isParam;
		x->next=NULL;
	}

}

void dealFuncBlock(InterCodes* codes,FunctionBlockDescriptor* currentFBD){
	InterCode* x=codes->code;
	switch(x->kind){
		case(LABELDEC):		break;
		case(FUNCDEC):		break;
		case(ASSIGN):		dealValue(x->u.assign.left,currentFBD,false,false,4);break;
		case(ADD):		dealValue(x->u.binop.result,currentFBD,false,false,4);break;
		case(SUB):		dealValue(x->u.binop.result,currentFBD,false,false,4);break;
		case(MUL):		dealValue(x->u.binop.result,currentFBD,false,false,4);break;
		case(DIV):		dealValue(x->u.binop.result,currentFBD,false,false,4);break;
		case(ADDR_ASSIGN):	dealValue(x->u.assign.left,currentFBD,false,false,4);break;
		case(CONTENT_ASSIGN):	dealValue(x->u.assign.left,currentFBD,false,false,4);break;
		case(ASSIGN_CONTENT):	dealValue(x->u.assign.left,currentFBD,false,false,4);break;
		case(GOTO):		break;
		case(RELOP_GOTO):	break;
		case(RETURN):		break;
		case(DEC):		dealValue(x->u.dec.op,currentFBD,true,false,x->u.dec.size);break;
		case(ARG):		break;
		case(CALL):		dealValue(x->u.assign.left,currentFBD,false,false,4);break;
		case(PARAM):		dealValue(x->u.op,currentFBD,false,true,4);break;
		case(READ):		dealValue(x->u.op,currentFBD,false,false,4);break;
		case(WRITE):		break;
		default:		break;
	}	
}



void dealBlock(InterCodes* codes){
	switch(codes->code->kind){
		case(LABELDEC):		codes->isEntrance=true;break;
		case(FUNCDEC):		codes->isEntrance=true;break;
		case(ASSIGN):		break;
		case(ADD):		break;
		case(SUB):		break;
		case(MUL):		break;
		case(DIV):		break;
		case(ADDR_ASSIGN):	break;
		case(CONTENT_ASSIGN):	break;
		case(ASSIGN_CONTENT):	break;
		case(GOTO):		codes->next->isEntrance=true;break;
		case(RELOP_GOTO):	codes->next->isEntrance=true;break;
		case(RETURN):		codes->next->isEntrance=true;break;
		case(DEC):		break;
		case(ARG):		break;
		case(CALL):		codes->next->isEntrance=true;break;
		case(PARAM):		break;
		case(READ):		break;
		case(WRITE):		break;
		default:		break;
	}	
}


void OutAsm(char* out_file){
	FILE* fp;
	if(out_file==NULL)
		fp=stdout;
	else
		fp=fopen(out_file,"w");

	file_index=fp;
	fprintf(fp,".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\n");
	fprintf(fp,"read:\n  li $v0,4\n  la $a0, _prompt\n  syscall\n  li $v0, 5\n  syscall\n  jr $ra\n");
	fprintf(fp,"write:\n  li $v0, 1\n  syscall\n  li $v0, 4\n  la $a0, _ret\n  syscall\n  move $v0, $0\n  jr $ra\n");
	OutCodes(fp);
	fclose(fp);
	file_index=NULL;
}

void OutCodes(FILE* fp){
	InitialBlock();
	InitialReg();
	firstFunc=1;
	if(deBugMode)
		deBug();
	//int code_index=0;

	InterCodes* x=CodeHead;
	if(x->next->isEntrance)
		isBlockEnd=true;
	else
		isBlockEnd=false;

	current_entrance=x;
	current_code=x;
	if(deBugMode){
		fprintf(fp,"\nstart Compare\n");
		fprintf(fp,"======================\n");
	}
	OutCode(fp,x->code);
	if(deBugMode){
		fprintf(fp,"----------------------\n");
		printCode(fp,x->code);
		fprintf(fp,"======================\n\n\n");
		fprintf(fp,"======================\n");
	}	
	//printf("code_index:%d\n",code_index);
	//code_index=code_index+1;
	while(x->next!=CodeHead){
		x=x->next;
		current_code=x;
		if(x->isEntrance){
			current_entrance=x;
			InitialReg();		
		}

		if(x->next->isEntrance)
			isBlockEnd=true;
		else
			isBlockEnd=false;

		OutCode(fp,x->code);
		if(deBugMode){
			fprintf(fp,"----------------------\n");
			printCode(fp,x->code);
			fprintf(fp,"======================\n\n\n");
			fprintf(fp,"======================\n");
		}
		//printf("code_index:%d\n",code_index);
		//code_index=code_index+1;
	}

}

void InitialReg(){
	for(int i=0;i<10;i++){
		rd[i].isFree=true;
		rd[i].lock=false;
	}
}
void spillReg(){
	for(int i=0;i<10;i++){
		
		if(rd[i].isFree)
			continue;
		if(rd[i].isConstant){
			rd[i].isFree=true;
			continue;
		}		

		bool is_addr=false;
		int offset=getStackOffset(rd[i].isValue,rd[i].id,&is_addr)+stackOffsetForArg;
		char* rtn=getTReg(i);
		if(is_addr==false){
			//spill farthest reg if not array, or needn't spill
			fprintf(file_index,"  sw %s, %d($sp)\n",rtn,offset);
		}
		rd[i].isFree=true;
	}
}


void OutCode(FILE* fp,InterCode* x){
	switch(x->kind){
		case(LABELDEC):		OutLabel(fp,x);break;
		case(FUNCDEC):		OutFunc(fp,x);break;
		case(ASSIGN):		OutAssign(fp,x);break;
		case(ADD):		OutAdd(fp,x);break;
		case(SUB):		OutSub(fp,x);break;
		case(MUL):		OutMul(fp, x);break;
		case(DIV):		OutDiv(fp, x);break;
		case(ADDR_ASSIGN):	OutAddrAssign(fp,x);break;
		case(CONTENT_ASSIGN):	OutContentAssign(fp,x);break;
		case(ASSIGN_CONTENT):	OutAssignContent(fp,x);break;
		case(GOTO):		OutGoto(fp,x);break;
		case(RELOP_GOTO):	OutRelopGoto(fp,x);break;
		case(RETURN):		OutReturn(fp,x);break;
		case(DEC):		OutDec(fp,x);break;
		case(ARG):		OutARG(fp,x);break;
		case(CALL):		OutCall(fp,x);break;
		case(PARAM):		OutParam(fp,x);break;
		case(READ):		OutRead(fp,x);break;
		case(WRITE):		OutWrite(fp,x);break;
		default:		break;
	}	

	//unlock
	for(int i=0;i<10;i++)
		rd[i].lock=false;
}

char* getAReg(int index){
	switch(index){
		case(0):return a0;break;
		case(1):return a1;break;
		case(2):return a2;break;
		case(3):return a3;break;
		default: return NULL;break;
	}


}

char* getSReg(int index){
	switch(index){
		case(0):return s0;break;
		case(1):return s1;break;
		case(2):return s2;break;
		case(3):return s3;break;
		case(4):return s4;break;
		case(5):return s5;break;
		case(6):return s6;break;
		case(7):return s7;break;
		default: return NULL;break;
	}

}

char* getTReg(int index){	
	switch(index){
		case(0):return t0;break;
		case(1):return t1;break;
		case(2):return t2;break;
		case(3):return t3;break;
		case(4):return t4;break;
		case(5):return t5;break;
		case(6):return t6;break;
		case(7):return t7;break;
		case(8):return t8;break;
		case(9):return t9;break;
		default: return NULL;break;
	}

}

bool opMatch(Operand* op,bool isV,int id){
	switch(op->kind){
		case(VAR):return isV&&(id==op->u.id);break;
		case(VADDR):return isV&&(id==op->u.id);break;
		case(CONSTANT):return false;break;
		case(TEMP):return (isV==false)&&(id==op->u.id);break;
		case(LABEL):return false;break;
		case(FUNC):return false;break;
		case(ADDR):return (isV==false)&&(id==op->u.id);break;
		case(STAR):return (isV==false)&&(id==op->u.id);break;
		default:return false;break;
	}

}

bool isUsed(InterCode* x,bool isV,int id){
	switch(x->kind){
		case(LABELDEC):		return false;break;
		case(FUNCDEC):		return false;break;
		case(ASSIGN):		return opMatch(x->u.assign.left,isV,id)||opMatch(x->u.assign.right,isV,id);break;
		case(ADD):		return opMatch(x->u.binop.result,isV,id)||opMatch(x->u.binop.op1,isV,id)||opMatch(x->u.binop.op2,isV,id);break;
		case(SUB):		return opMatch(x->u.binop.result,isV,id)||opMatch(x->u.binop.op1,isV,id)||opMatch(x->u.binop.op2,isV,id);break;
		case(MUL):		return opMatch(x->u.binop.result,isV,id)||opMatch(x->u.binop.op1,isV,id)||opMatch(x->u.binop.op2,isV,id);break;
		case(DIV):		return opMatch(x->u.binop.result,isV,id)||opMatch(x->u.binop.op1,isV,id)||opMatch(x->u.binop.op2,isV,id);break;
		case(ADDR_ASSIGN):	return opMatch(x->u.assign.left,isV,id)||opMatch(x->u.assign.right,isV,id);break;
		case(CONTENT_ASSIGN):	return opMatch(x->u.assign.left,isV,id)||opMatch(x->u.assign.right,isV,id);break;
		case(ASSIGN_CONTENT):	return opMatch(x->u.assign.left,isV,id)||opMatch(x->u.assign.right,isV,id);break;
		case(GOTO):		return false;break;
		case(RELOP_GOTO):	return opMatch(x->u.relop_goto.op1,isV,id)||opMatch(x->u.relop_goto.op2,isV,id);break;
		case(RETURN):		return opMatch(x->u.op,isV,id);break;
		case(DEC):		return false;break;
		case(ARG):		return opMatch(x->u.op,isV,id);break;
		case(CALL):		return opMatch(x->u.assign.left,isV,id);break;
		case(PARAM):		return false;break;
		case(READ):		return opMatch(x->u.op,isV,id);break;
		case(WRITE):		return opMatch(x->u.op,isV,id);break;
		default:		return false;break;
	}
}

int getNextUsedLine(bool isV,int id){
	InterCodes* x=current_entrance;
	int rtn=0;
	while(x!=current_code){
		x=x->next;
		rtn=rtn+1;
	}
	while(x->next!=NULL&&x->next->isEntrance==false){
		x=x->next;
		rtn=rtn+1;
		if(isUsed(x->code,isV,id))
			return rtn;
	}
	return -1;
}

int getLocalVarSize(FunctionBlockDescriptor* f){
	ValueDescriptor* x=f->vd;
	int reg_count=0;
	int param_count=0;
	int rtn=0;
	while(x!=NULL){
		if(x->isArray==false && x->isParam==false){
			if(reg_count>=8)
				rtn=rtn+4;
			reg_count=reg_count+1;	
		}

		else if(x->isArray){
			rtn=rtn+x->size;
		}

		x=x->next;
	}
	return rtn;
}

int getStackOffset(bool isV,int id,bool* is_addr){
	int reg_count=0;
	int param_count=0;
	ValueDescriptor* x=fbd_index->vd;
	int offset=0;
	*is_addr=false;

	while(x!=NULL){
		if(isV!=x->isV||id!=x->id){
			if(x->isArray==false && x->isParam==false){
				if(reg_count>=8)
					offset=offset+4;
				reg_count=reg_count+1;	
			}
			else if(x->isParam){
				param_count=param_count+1;
				
			}
			else if(x->isArray){
				offset=offset+x->size;
			}
		}
		else{
			if(x->isArray==false && x->isParam==false){
				return offset+stackOffsetForArg;
			}
			else if(x->isParam){
				return fbd_index->localVarSize+32+4+(param_count-4)+stackOffsetForArg;
			}
			else if(x->isArray){
				*is_addr=true;
				return offset+stackOffsetForArg;
			}
		}
		x=x->next;
	}
}

char* getReg(Operand* op){
	int id=op->u.id;
	bool isV;
	bool isConstant;
	int reg_index=0;//save in reg s0 to s7
	//int stack_index=0;//save in stack, use reg t0 to t9
	int param_index=0;//save in a0 to a3 or in stack


	if(op->kind==CONSTANT){
		for(int i=0;i<10;i++){
			if(rd[i].lock==false&&(rd[i].isFree || (rd[i].isConstant&&(i!=lastRegId)))){
				//have free $t
				rd[i].isFree=false;
				rd[i].isConstant=true;
				rd[i].isValue=false;
				rd[i].id=-1;
				rd[i].nextUsedLine=0;
				rd[i].lock=true;
				char* rtn=getTReg(i);
				fprintf(file_index,"  li %s, %d\n",rtn,op->u.value);
				lastRegId=i;
				return rtn;
			}
		}
		//have no free $t, spill the farthest value into stack
		int farthest_index=0;
		for(;farthest_index<10;farthest_index++)
			if(rd[farthest_index].lock==false)
				break;
	
		int farthest_line=rd[farthest_index].nextUsedLine;
		if(farthest_line!=-1)
			for(int i=farthest_index+1;i<10;i++){
				if(rd[i].lock==false&&rd[i].nextUsedLine==-1){
					farthest_line=-1;
					farthest_index=i;
					break;						
				}
				else if(rd[i].lock==false&&rd[i].nextUsedLine>farthest_line){
					farthest_line=rd[i].nextUsedLine;
					farthest_index=i;
				}
			}
		bool is_addr=false;
		int offset=getStackOffset(rd[farthest_index].isValue,rd[farthest_index].id,&is_addr);
		char* rtn=getTReg(farthest_index);
		if(is_addr==false){
			//spill farthest reg if not array, or needn't spill
			fprintf(file_index,"  sw %s, %d($sp)\n",rtn,offset);
		}
		rd[farthest_index].nextUsedLine=0;
		rd[farthest_index].lock=true;
		rd[farthest_index].isValue=false;
		rd[farthest_index].id=-1;
		rd[farthest_index].isConstant=true;
		fprintf(file_index,"  li %s, %d\n",rtn,op->u.value);
		lastRegId=farthest_index;
		return rtn;
	}
	

	if(op->kind==VAR||op->kind==VADDR)
		isV=true;
	else isV=false;
	
	ValueDescriptor* x=fbd_index->vd;

	while(x!=NULL){
		if(isV==x->isV&&id==x->id){
			if(x->isArray==false && x->isParam==false && reg_index<8){
				lastRegId=-1;			
				return getSReg(reg_index);
			
			}			
			else if((x->isArray==false && x->isParam==false)||(x->isParam==true&&param_index>=4)||(x->isArray)){
				for(int i=0;i<10;i++){
					bool checkFree=rd[i].isFree || (rd[i].isConstant&&(i!=lastRegId));
					if(checkFree==false&&rd[i].isValue==isV&&rd[i].id==id){
						//have allocated $t
						lastRegId=i;
						rd[i].nextUsedLine=getNextUsedLine(isV,id);
						rd[i].lock=true;
						return getTReg(i);					
					}
				}

				for(int i=0;i<10;i++){
					bool checkFree=rd[i].isFree || (rd[i].isConstant&&(i!=lastRegId));

					if(checkFree && rd[i].lock==false){
						//have free $t
						rd[i].isFree=false;
						rd[i].isValue=isV;
						rd[i].id=id;
						rd[i].isConstant=false;
						rd[i].nextUsedLine=getNextUsedLine(isV,id);
						rd[i].lock=true;
						bool is_addr=false;
						int offset=getStackOffset(isV,id,&is_addr);//local variable's offset,array's offset,param5+'s offset in stack;
						char* rtn=getTReg(i);
						if(is_addr)
							fprintf(file_index,"  addi %s, $sp, %d\n",rtn,offset);
						
						else
							fprintf(file_index,"  lw %s, %d($sp)\n",rtn,offset);
						lastRegId=i;
						return rtn;
					}
				}

				//have no free $t, spill the farthest value into stack
				int farthest_index=0;
				for(;farthest_index<10;farthest_index++)
					if(rd[farthest_index].lock==false)
						break;
	
				int farthest_line=rd[farthest_index].nextUsedLine;

				if(farthest_line!=-1)
					for(int i=farthest_index+1;i<10;i++){
						if(rd[i].lock==false&&rd[i].nextUsedLine==-1){
							farthest_line=-1;
							farthest_index=i;
							break;						
						}
						else if(rd[i].lock==false&&rd[i].nextUsedLine>farthest_line){
							farthest_line=rd[i].nextUsedLine;
							farthest_index=i;
						}
					}
				bool is_addr=false;
				int offset=getStackOffset(rd[farthest_index].isValue,rd[farthest_index].id,&is_addr);
				char* rtn=getTReg(farthest_index);
				if(is_addr==false){
					//spill farthest reg if not array, or needn't spill
					fprintf(file_index,"  sw %s, %d($sp)\n",rtn,offset);
				}
				rd[farthest_index].isValue=isV;
				rd[farthest_index].id=id;
				rd[farthest_index].nextUsedLine=getNextUsedLine(isV,id);
				rd[farthest_index].lock=true;		
				rd[farthest_index].isConstant=false;

				//load reg
				is_addr=false;
				offset=getStackOffset(isV,id,&is_addr);//local variable's offset,array's offset,param5+'s offset in stack;
				if(is_addr)
					fprintf(file_index,"  addi %s, $sp, %d\n",rtn,offset);
						
				else
					fprintf(file_index,"  lw %s, %d($sp)\n",rtn,offset);
				lastRegId=farthest_index;
				return rtn;
			}
			else{
				lastRegId=-1;				
				return getAReg(param_index);
			}
		}
		else{
			if(x->isArray==false && x->isParam==false){
				reg_index=reg_index+1;
			}
			
			else if(x->isParam==true)
				param_index=param_index+1;
		}
		x=x->next;
	}
	return NULL;
}

void OutRead(FILE* fp,InterCode* x){

	fprintf(fp,"  addi $sp, $sp, -4\n");
	fprintf(fp,"  sw $ra, 0($sp)\n");
	fprintf(fp,"  jal read\n");
	fprintf(fp,"  lw $ra, 0($sp)\n");
	fprintf(fp,"  addi $sp, $sp, 4\n");
	
	char* regString=getReg(x->u.op);

	if(x->u.op->kind==STAR)
		fprintf(fp,"  sw $v0, 0(%s)\n",regString);
	else
		fprintf(fp,"  move %s, $v0\n",regString);
	
	if(isBlockEnd||1)
		spillReg();
}

void OutWrite(FILE* fp,InterCode* x){

	char* regString=getReg(x->u.op);
	
	fprintf(fp,"  addi $sp, $sp, -4\n");
	fprintf(fp,"  sw $a0, 0($sp)\n");
	
	

	if(x->u.op->kind==STAR)
		fprintf(fp,"  lw $a0, 0(%s)\n",regString);

	else	
		fprintf(fp,"  move $a0, %s\n",regString);

	
	fprintf(fp,"  addi $sp, $sp, -4\n");
	fprintf(fp,"  sw $ra, 0($sp)\n");

	fprintf(fp,"  jal write\n");

	fprintf(fp,"  lw $ra, 0($sp)\n");
	fprintf(fp,"  addi $sp, $sp, 4\n");
	
	fprintf(fp,"  lw $a0, 0($sp)\n");
	fprintf(fp,"  addi $sp, $sp, 4\n");

	if(isBlockEnd||1)
		spillReg();
}

void OutParam(FILE* fp,InterCode* x){
	if(isBlockEnd||1)
		spillReg();
}

void OutCall(FILE* fp,InterCode* x){
	if(prepareCall==false){
		fprintf(fp,"  addi $sp, $sp, -16\n");
		fprintf(fp,"  sw $a0, ($sp)\n");
		fprintf(fp,"  sw $a1, 4($sp)\n");
		fprintf(fp,"  sw $a2, 8($sp)\n");
		fprintf(fp,"  sw $a3, 12($sp)\n");

		ARG_NUM=0;
	}
	else prepareCall=false;
	
	fprintf(fp,"  addi $sp, $sp, -4\n");
	fprintf(fp,"  sw $ra, ($sp)\n");

	spillReg();

	fprintf(fp,"  jal %s\n",x->u.assign.right->u.func_name);
	fprintf(fp,"  lw $ra, ($sp)\n");	
	fprintf(fp,"  addi $sp, $sp, 4\n");	
	
	if(ARG_NUM>4){
		fprintf(fp,"  addi $sp, $sp, %d\n",4*(ARG_NUM-4));		
	}

	

	fprintf(fp,"  lw $a3, 12($sp)\n");
	fprintf(fp,"  lw $a2, 8($sp)\n");
	fprintf(fp,"  lw $a1, 4($sp)\n");
	fprintf(fp,"  lw $a0, ($sp)\n");
	fprintf(fp,"  addi $sp, $sp, 16\n");
	
	stackOffsetForArg=0;

	char* leftReg=getReg(x->u.assign.left);

	if(x->u.assign.left->kind==STAR)
		fprintf(fp,"  sw $v0, 0(%s)\n",leftReg);
	else
		fprintf(fp,"  move %s, $v0\n",leftReg);
	
	if(isBlockEnd||1)
		spillReg();

}

int argNumLater(){
	InterCodes* x=current_code;
	int num=0;
	while(x->next!=NULL){
		x=x->next;
		if(x->code->kind!=CALL)
			num=num+1;
		else break;	
	}
	return num;

}

void OutARG(FILE* fp, InterCode* x){
	char* RegString=getReg(x->u.op);
	bool is_ax=(RegString[1]=='a');

	if(prepareCall==false){
		fprintf(fp,"  addi $sp, $sp, -16\n");
		fprintf(fp,"  sw $a0, ($sp)\n");
		fprintf(fp,"  sw $a1, 4($sp)\n");
		fprintf(fp,"  sw $a2, 8($sp)\n");
		fprintf(fp,"  sw $a3, 12($sp)\n");
		stackOffsetForArg=16;
		prepareCall=true;
		ARG_NUM_LATER=argNumLater();
		ARG_NUM=1+ARG_NUM_LATER;
	}
	else{
		ARG_NUM_LATER=ARG_NUM_LATER-1;
	}
	
	
	
	if(ARG_NUM_LATER>=4){
		stackOffsetForArg=stackOffsetForArg+4;

		fprintf(fp,"  addi $sp, $sp, -4\n");
		if(x->u.op->kind==STAR){
			//save v1
			fprintf(fp,"  addi $sp, $sp, -4\n");
			fprintf(fp,"  sw $v1, ($sp)\n");

			//push stack
			fprintf(fp,"  lw $v1, 0(%s)\n",RegString);
			fprintf(fp,"  sw $v1, 4($sp)\n");
			
			//pop v1
			fprintf(fp,"  lw $v1, ($sp)\n");
			fprintf(fp,"  addi $sp, $sp, 4\n");
			
		}
		else{
			if(is_ax){
				int a_id=RegString[2]-'0';
				int a_offset=stackOffsetForArg-16+4*a_id;
				fprintf(fp,"  addi $sp, $sp, -4\n");
				fprintf(fp,"  sw $v1, ($sp)\n");
				fprintf(fp,"  lw $v1, %d($sp)\n",a_offset+4);
				fprintf(fp,"  sw $v1, 4($sp)\n");
				fprintf(fp,"  lw $v1, ($sp)\n");
				fprintf(fp,"  addi $sp, $sp, 4\n");
			}
			
			else
				fprintf(fp,"  sw %s, ($sp)\n",RegString);
		}
	}
	else{
		if(x->u.op->kind==STAR){
			fprintf(fp,"  lw  %s, 0(%s)\n",getAReg(ARG_NUM_LATER),RegString);
		
		}
		else{
			if(is_ax){
				int a_id=RegString[2]-'0';
				int a_offset=stackOffsetForArg-16+4*a_id;
				fprintf(fp,"  lw %s, %d($sp)\n",getAReg(ARG_NUM_LATER),a_offset);
			}

			else
				fprintf(fp,"  move %s, %s\n",getAReg(ARG_NUM_LATER),RegString);
		}
	}
	
}

void OutDec(FILE* fp,InterCode* x){
	if(isBlockEnd||1)
		spillReg();
}


void OutReturn(FILE* fp,InterCode* x){
	char* regString=getReg(x->u.op);
	if(x->u.op->kind==STAR)
		fprintf(fp,"  lw $v0, 0(%s)\n",regString);
	else
		fprintf(fp,"  move $v0, %s\n",regString);
	
	spillReg();

	if(fbd_index->localVarSize>0)
		fprintf(fp,"  addi $sp, $sp, %d\n",fbd_index->localVarSize);

	fprintf(fp,"  lw $s7, 28($sp)\n");
	fprintf(fp,"  lw $s6, 24($sp)\n");
	fprintf(fp,"  lw $s5, 20($sp)\n");
	fprintf(fp,"  lw $s4, 16($sp)\n");
	fprintf(fp,"  lw $s3, 12($sp)\n");
	fprintf(fp,"  lw $s2, 8($sp)\n");
	fprintf(fp,"  lw $s1, 4($sp)\n");
	fprintf(fp,"  lw $s0, ($sp)\n");
	fprintf(fp,"  addi $sp, $sp, 32\n");

	fprintf(fp,"  jr $ra\n");
}

void OutRelopGoto(FILE* fp,InterCode* x){
	spillReg();
	char* regLeft=getReg(x->u.relop_goto.op1);
	char* regRight=getReg(x->u.relop_goto.op2);
	
	if(x->u.relop_goto.op1->kind==STAR){
		if(x->u.relop_goto.op2->kind==STAR){
			Operand tmp;
			tmp.kind=CONSTANT;
			tmp.u.value=0;
			char* regTmp1=getReg(&tmp);
			char* regTmp2=getReg(&tmp);
			fprintf(fp,"  lw %s, (%s)\n",regTmp1,regLeft);
			fprintf(fp,"  lw %s, (%s)\n",regTmp2,regRight);
			fprintf(fp,"  ");
			OutRelop(fp,x->u.relop_goto.relop);
			fprintf(fp,"  %s, %s, ",regTmp1,regTmp2);
			
		}
			
		else{
			Operand tmp;
			tmp.kind=CONSTANT;
			tmp.u.value=0;
			char* regTmp=getReg(&tmp);
			fprintf(fp,"  lw %s, (%s)\n",regTmp,regLeft);
			fprintf(fp,"  ");
			OutRelop(fp,x->u.relop_goto.relop);
			fprintf(fp,"  %s, %s, ",regTmp,regRight);
		}
	}
	else{
		if(x->u.relop_goto.op2->kind==STAR){
			Operand tmp;
			tmp.kind=CONSTANT;
			tmp.u.value=0;
			char* regTmp=getReg(&tmp);
			fprintf(fp,"  lw %s, (%s)\n",regTmp,regRight);
			fprintf(fp,"  ");
			OutRelop(fp,x->u.relop_goto.relop);
			fprintf(fp,"  %s, %s, ",regLeft,regTmp);
		}
			
		else{
			fprintf(fp,"  ");
			OutRelop(fp,x->u.relop_goto.relop);
			fprintf(fp," %s, %s, ",regLeft,regRight);
		}
	}
	OutOp(fp,x->u.relop_goto.dst);
	fprintf(fp,"\n");
}

void OutGoto(FILE* fp,InterCode* x){
	spillReg();
	fprintf(fp,"  j ");
	OutOp(fp,x->u.op);
	fprintf(fp,"\n");
}

void OutAssignContent(FILE* fp,InterCode* x){
	char* regLeft=getReg(x->u.assign.left);
	char* regRight=getReg(x->u.assign.right);

	fprintf(fp,"  sw %s, 0(%s)\n",regRight,regLeft);
	
	if(isBlockEnd||1)
		spillReg();
}

void OutContentAssign(FILE* fp, InterCode* x){
	char* regLeft=getReg(x->u.assign.left);
	char* regRight=getReg(x->u.assign.right);

	fprintf(fp,"  lw %s, 0(%s)\n",regLeft,regRight);

	if(isBlockEnd||1)
		spillReg();
}

void OutAddrAssign(FILE* fp, InterCode* x){
	char* regLeft=getReg(x->u.assign.left);
	char* regRight=getReg(x->u.assign.right);

	fprintf(fp,"  move %s, %s\n",regLeft,regRight);

	if(isBlockEnd||1)
		spillReg();
}


void OutLabel(FILE* fp,InterCode* x){
	fprintf(fp,"label%d:\n",x->u.op->u.id);
}

void OutFunc(FILE* fp,InterCode* x){
	if(!firstFunc){
		fprintf(fp,"\n");
		fbd_index=fbd_index->next;
	}
	else {
		firstFunc=0;
		fbd_index=fbd;
	}
	
	fprintf(fp,"%s:\n",x->u.op->u.func_name);
	fprintf(fp,"  addi $sp, $sp, -32\n");
	fprintf(fp,"  sw $s0, ($sp)\n");
	fprintf(fp,"  sw $s1, 4($sp)\n");
	fprintf(fp,"  sw $s2, 8($sp)\n");
	fprintf(fp,"  sw $s3, 12($sp)\n");
	fprintf(fp,"  sw $s4, 16($sp)\n");
	fprintf(fp,"  sw $s5, 20($sp)\n");
	fprintf(fp,"  sw $s6, 24($sp)\n");
	fprintf(fp,"  sw $s7, 28($sp)\n");
	if(fbd_index->localVarSize>0)
		fprintf(fp,"  addi $sp, $sp, -%d\n",fbd_index->localVarSize);
}

void OutAssign(FILE* fp,InterCode* x){
	char* regLeft=getReg(x->u.assign.left);
	char* regRight=getReg(x->u.assign.right);

	if(x->u.assign.left->kind==STAR){
		if(x->u.assign.right->kind==STAR){
			Operand tmp;
			tmp.kind=CONSTANT;
			tmp.u.value=0;
			char* regTmp=getReg(&tmp);
			fprintf(fp,"  lw %s, (%s)\n",regTmp,regRight);
			fprintf(fp,"  sw %s, (%s)\n",regTmp,regLeft);
			
		}
		else 
			fprintf(fp," sw %s, (%s)\n",regRight,regLeft);
	}
	else{
		if(x->u.assign.right->kind==STAR){
			fprintf(fp,"  lw %s, (%s)\n",regLeft,regRight);
		}

		else{
			fprintf(fp,"  move %s, %s\n",regLeft,regRight);
		}
	}

	spillReg();
	

}


void OutAdd(FILE* fp, InterCode* x){
	char* regResult=getReg(x->u.binop.result);
	char* regLeft=getReg(x->u.binop.op1);
	char* regRight=getReg(x->u.binop.op2);
	
	if(x->u.binop.result->kind==STAR){
		if(x->u.binop.op1->kind==STAR){
			if(x->u.binop.op2->kind==STAR){
				//prepare tmp reg
				fprintf(fp,"  addi $sp, $sp, -4\n");
				fprintf(fp,"  sw $v1, (sp)\n");
				
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp1=getReg(&tmp);
				char* regTmp2=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp1,regLeft);
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp2,regRight);
				fprintf(fp,"  add $v1, %s, %s\n",regTmp1,regTmp2);
				fprintf(fp,"  sw $v1, 0(%s)\n",regResult);
				
				//pop v1
				fprintf(fp,"  lw $v1, ($sp)\n");
				fprintf(fp,"  addi $sp, $sp, 4\n");
			}
			else{
				//prepare tmp reg
				fprintf(fp,"  addi $sp, $sp, -4\n");
				fprintf(fp,"  sw $v1, (sp)\n");
				
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp1=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp1,regLeft);
				fprintf(fp,"  add $v1, %s, %s\n",regTmp1,regRight);
				fprintf(fp,"  sw $v1, 0(%s)\n",regResult);
				
				//pop v1
				fprintf(fp,"  lw $v1, ($sp)\n");
				fprintf(fp,"  addi $sp, $sp, 4\n");
			}
		}
		else{
			if(x->u.binop.op2->kind==STAR){
				//prepare tmp reg
				fprintf(fp,"  addi $sp, $sp, -4\n");
				fprintf(fp,"  sw $v1, (sp)\n");
				
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp2=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp2,regRight);
				fprintf(fp,"  add $v1, %s, %s\n",regLeft,regTmp2);
				fprintf(fp,"  sw $v1, 0(%s)\n",regResult);
				
				//pop v1
				fprintf(fp,"  lw $v1, ($sp)\n");
				fprintf(fp,"  addi $sp, $sp, 4\n");
			}
			else{
				//prepare tmp reg
				fprintf(fp,"  addi $sp, $sp, -4\n");
				fprintf(fp,"  sw $v1, (sp)\n");
				
				//start compute
				fprintf(fp,"  add $v1, %s, %s\n",regLeft,regRight);
				fprintf(fp,"  sw $v1, 0(%s)\n",regResult);
				
				//pop v1
				fprintf(fp,"  lw $v1, ($sp)\n");
				fprintf(fp,"  addi $sp, $sp, 4\n");
			}
		}
	}
	else{
		if(x->u.binop.op1->kind==STAR){
			if(x->u.binop.op2->kind==STAR){				
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp1=getReg(&tmp);
				char* regTmp2=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp1,regLeft);
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp2,regRight);
				fprintf(fp,"  add %s, %s, %s\n",regResult,regTmp1,regTmp2);

			}
			else{
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp1=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp1,regLeft);
				fprintf(fp,"  add %s, %s, %s\n",regResult,regTmp1,regRight);
			}
		}
		else{
			if(x->u.binop.op2->kind==STAR){
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp2=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp2,regRight);
				fprintf(fp,"  add %s, %s, %s\n",regResult,regLeft,regTmp2);
			}
			else{
				
				//start compute
				fprintf(fp,"  add %s, %s, %s\n",regResult,regLeft,regRight);
			}
		}
	}
	if(isBlockEnd||1)
		spillReg();
}

void OutSub(FILE* fp,InterCode* x){
	char* regResult=getReg(x->u.binop.result);
	char* regLeft=getReg(x->u.binop.op1);
	char* regRight=getReg(x->u.binop.op2);
	
	if(x->u.binop.result->kind==STAR){
		if(x->u.binop.op1->kind==STAR){
			if(x->u.binop.op2->kind==STAR){
				//prepare tmp reg
				fprintf(fp,"  addi $sp, $sp, -4\n");
				fprintf(fp,"  sw $v1, (sp)\n");
				
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp1=getReg(&tmp);
				char* regTmp2=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp1,regLeft);
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp2,regRight);
				fprintf(fp,"  sub $v1, %s, %s\n",regTmp1,regTmp2);
				fprintf(fp,"  sw $v1, 0(%s)\n",regResult);
				
				//pop v1
				fprintf(fp,"  lw $v1, ($sp)\n");
				fprintf(fp,"  addi $sp, $sp, 4\n");
			}
			else{
				//prepare tmp reg
				fprintf(fp,"  addi $sp, $sp, -4\n");
				fprintf(fp,"  sw $v1, (sp)\n");
				
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp1=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp1,regLeft);
				fprintf(fp,"  sub $v1, %s, %s\n",regTmp1,regRight);
				fprintf(fp,"  sw $v1, 0(%s)\n",regResult);
				
				//pop v1
				fprintf(fp,"  lw $v1, ($sp)\n");
				fprintf(fp,"  addi $sp, $sp, 4\n");
			}
		}
		else{
			if(x->u.binop.op2->kind==STAR){
				//prepare tmp reg
				fprintf(fp,"  addi $sp, $sp, -4\n");
				fprintf(fp,"  sw $v1, (sp)\n");
				
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp2=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp2,regRight);
				fprintf(fp,"  sub $v1, %s, %s\n",regLeft,regTmp2);
				fprintf(fp,"  sw $v1, 0(%s)\n",regResult);
				
				//pop v1
				fprintf(fp,"  lw $v1, ($sp)\n");
				fprintf(fp,"  addi $sp, $sp, 4\n");
			}
			else{
				//prepare tmp reg
				fprintf(fp,"  addi $sp, $sp, -4\n");
				fprintf(fp,"  sw $v1, (sp)\n");
				
				//start compute
				fprintf(fp,"  sub $v1, %s, %s\n",regLeft,regRight);
				fprintf(fp,"  sw $v1, 0(%s)\n",regResult);
				
				//pop v1
				fprintf(fp,"  lw $v1, ($sp)\n");
				fprintf(fp,"  addi $sp, $sp, 4\n");
			}
		}
	}
	else{
		if(x->u.binop.op1->kind==STAR){
			if(x->u.binop.op2->kind==STAR){				
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp1=getReg(&tmp);
				char* regTmp2=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp1,regLeft);
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp2,regRight);
				fprintf(fp,"  sub %s, %s, %s\n",regResult,regTmp1,regTmp2);

			}
			else{
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp1=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp1,regLeft);
				fprintf(fp,"  sub %s, %s, %s\n",regResult,regTmp1,regRight);
			}
		}
		else{
			if(x->u.binop.op2->kind==STAR){
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp2=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp2,regRight);
				fprintf(fp,"  sub %s, %s, %s\n",regResult,regLeft,regTmp2);
			}
			else{
				
				//start compute
				fprintf(fp,"  sub %s, %s, %s\n",regResult,regLeft,regRight);
			}
		}
	}
	if(isBlockEnd||1)
		spillReg();
}

void OutMul(FILE* fp, InterCode* x){
	char* regResult=getReg(x->u.binop.result);
	char* regLeft=getReg(x->u.binop.op1);
	char* regRight=getReg(x->u.binop.op2);
	
	if(x->u.binop.result->kind==STAR){
		if(x->u.binop.op1->kind==STAR){
			if(x->u.binop.op2->kind==STAR){
				//prepare tmp reg
				fprintf(fp,"  addi $sp, $sp, -4\n");
				fprintf(fp,"  sw $v1, (sp)\n");
				
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp1=getReg(&tmp);
				char* regTmp2=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp1,regLeft);
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp2,regRight);
				fprintf(fp,"  mul $v1, %s, %s\n",regTmp1,regTmp2);
				fprintf(fp,"  sw $v1, 0(%s)\n",regResult);
				
				//pop v1
				fprintf(fp,"  lw $v1, ($sp)\n");
				fprintf(fp,"  addi $sp, $sp, 4\n");
			}
			else{
				//prepare tmp reg
				fprintf(fp,"  addi $sp, $sp, -4\n");
				fprintf(fp,"  sw $v1, (sp)\n");
				
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp1=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp1,regLeft);
				fprintf(fp,"  mul $v1, %s, %s\n",regTmp1,regRight);
				fprintf(fp,"  sw $v1, 0(%s)\n",regResult);
				
				//pop v1
				fprintf(fp,"  lw $v1, ($sp)\n");
				fprintf(fp,"  addi $sp, $sp, 4\n");
			}
		}
		else{
			if(x->u.binop.op2->kind==STAR){
				//prepare tmp reg
				fprintf(fp,"  addi $sp, $sp, -4\n");
				fprintf(fp,"  sw $v1, (sp)\n");
				
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp2=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp2,regRight);
				fprintf(fp,"  mul $v1, %s, %s\n",regLeft,regTmp2);
				fprintf(fp,"  sw $v1, 0(%s)\n",regResult);
				
				//pop v1
				fprintf(fp,"  lw $v1, ($sp)\n");
				fprintf(fp,"  addi $sp, $sp, 4\n");
			}
			else{
				//prepare tmp reg
				fprintf(fp,"  addi $sp, $sp, -4\n");
				fprintf(fp,"  sw $v1, (sp)\n");
				
				//start compute
				fprintf(fp,"  mul $v1, %s, %s\n",regLeft,regRight);
				fprintf(fp,"  sw $v1, 0(%s)\n",regResult);
				
				//pop v1
				fprintf(fp,"  lw $v1, ($sp)\n");
				fprintf(fp,"  addi $sp, $sp, 4\n");
			}
		}
	}
	else{
		if(x->u.binop.op1->kind==STAR){
			if(x->u.binop.op2->kind==STAR){				
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp1=getReg(&tmp);
				char* regTmp2=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp1,regLeft);
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp2,regRight);
				fprintf(fp,"  mul %s, %s, %s\n",regResult,regTmp1,regTmp2);

			}
			else{
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp1=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp1,regLeft);
				fprintf(fp,"  mul %s, %s, %s\n",regResult,regTmp1,regRight);
			}
		}
		else{
			if(x->u.binop.op2->kind==STAR){
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp2=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp2,regRight);
				fprintf(fp,"  mul %s, %s, %s\n",regResult,regLeft,regTmp2);
			}
			else{
				
				//start compute
				fprintf(fp,"  mul %s, %s, %s\n",regResult,regLeft,regRight);
			}
		}
	}
	if(isBlockEnd||1)
		spillReg();

}

void OutDiv(FILE* fp, InterCode* x){
	char* regResult=getReg(x->u.binop.result);
	char* regLeft=getReg(x->u.binop.op1);
	char* regRight=getReg(x->u.binop.op2);
	
	if(x->u.binop.result->kind==STAR){
		if(x->u.binop.op1->kind==STAR){
			if(x->u.binop.op2->kind==STAR){
				//prepare tmp reg
				fprintf(fp,"  addi $sp, $sp, -4\n");
				fprintf(fp,"  sw $v1, (sp)\n");
				
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp1=getReg(&tmp);
				char* regTmp2=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp1,regLeft);
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp2,regRight);
				fprintf(fp,"  div %s, %s\n",regTmp1,regTmp2);
				fprintf(fp,"  mflo $v1\n");
				fprintf(fp,"  sw $v1, 0(%s)\n",regResult);
				
				//pop v1
				fprintf(fp,"  lw $v1, ($sp)\n");
				fprintf(fp,"  addi $sp, $sp, 4\n");
			}
			else{
				//prepare tmp reg
				fprintf(fp,"  addi $sp, $sp, -4\n");
				fprintf(fp,"  sw $v1, (sp)\n");
				
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp1=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp1,regLeft);
				fprintf(fp,"  div %s, %s\n",regTmp1,regRight);
				fprintf(fp,"  mflo $v1\n");
				fprintf(fp,"  sw $v1, 0(%s)\n",regResult);
				
				//pop v1
				fprintf(fp,"  lw $v1, ($sp)\n");
				fprintf(fp,"  addi $sp, $sp, 4\n");
			}
		}
		else{
			if(x->u.binop.op2->kind==STAR){
				//prepare tmp reg
				fprintf(fp,"  addi $sp, $sp, -4\n");
				fprintf(fp,"  sw $v1, (sp)\n");
				
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp2=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp2,regRight);
				fprintf(fp,"  div %s, %s\n",regLeft,regTmp2);
				fprintf(fp,"  mflo $v1\n");
				fprintf(fp,"  sw $v1, 0(%s)\n",regResult);
				
				//pop v1
				fprintf(fp,"  lw $v1, ($sp)\n");
				fprintf(fp,"  addi $sp, $sp, 4\n");
			}
			else{
				//prepare tmp reg
				fprintf(fp,"  addi $sp, $sp, -4\n");
				fprintf(fp,"  sw $v1, (sp)\n");
				
				//start compute
				fprintf(fp,"  div %s, %s\n",regLeft,regRight);
				fprintf(fp,"  mflo $v1\n");
				fprintf(fp,"  sw $v1, 0(%s)\n",regResult);
				
				//pop v1
				fprintf(fp,"  lw $v1, ($sp)\n");
				fprintf(fp,"  addi $sp, $sp, 4\n");
			}
		}
	}
	else{
		if(x->u.binop.op1->kind==STAR){
			if(x->u.binop.op2->kind==STAR){				
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp1=getReg(&tmp);
				char* regTmp2=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp1,regLeft);
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp2,regRight);
				fprintf(fp,"  div %s, %s\n",regTmp1,regTmp2);
				fprintf(fp,"  mflo %s\n",regResult);

			}
			else{
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp1=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp1,regLeft);
				fprintf(fp,"  div %s, %s\n",regTmp1,regRight);
				fprintf(fp,"  mflo %s\n",regResult);
			}
		}
		else{
			if(x->u.binop.op2->kind==STAR){
				Operand tmp;
				tmp.kind=CONSTANT;
				tmp.u.value=0;
				char* regTmp2=getReg(&tmp);
				
				//start load and compute
				fprintf(fp,"  lw %s, 0(%s)\n",regTmp2,regRight);
				fprintf(fp,"  div %s, %s\n",regLeft,regTmp2);
				fprintf(fp,"  mflo %s\n",regResult);
			}
			else{
				
				//start compute
				fprintf(fp,"  div %s, %s\n",regLeft,regRight);
				fprintf(fp,"  mflo %s\n",regResult);
			}
		}
	}
	if(isBlockEnd||1)
		spillReg();
}


void OutOp(FILE* fp,Operand* op){
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

void OutRelop(FILE* fp,RELOP r){
	switch(r){
		case(EQ):fprintf(fp,"beq");break;
		case(NEQ):fprintf(fp,"bne");break;
		case(G):fprintf(fp,"bgt");break;
		case(GE):fprintf(fp,"bge");break;
		case(L):fprintf(fp,"blt");break;
		case(LE):fprintf(fp,"ble");break;
		default:break;
	}
}



