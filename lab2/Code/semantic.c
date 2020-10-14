#include"semantic.h"
#include<string.h>
#include<stdio.h>
#include<assert.h>

struct HashNode* HashTable[HASH_LENGTH];
int anonymousStrtNum=0;
int currentDepth=0;
struct HashListHead* listHead=NULL;
/*
void freeType(struct Type* t){
	if(t==NULL) return;

	if(t->kind==ARRAY&&t->u.array.elem!=NULL)
		freeType(t->u.array.elem);
	else if(t->kind==STRUCTURE&&t->u.strt!=NULL)
		freeStructure(t->u.strt);
	
	free(t);
}
void freeField(struct FieldList* f){
	if(f==NULL) return;
	if(f->v->name!=NULL)
		free(f->v->name);
	if(f->v->type!=NULL)
		freeType(f->v->type);
	if(f->tail!=NULL)
		freeField(f->tail);
	free(f);
}
void freeStructure(struct Structure* s){
	if(s==NULL) return;
	if(s->name!=NULL)
		free(s->name);
	if(s->structure!=NULL)
		freeField(s->structure);
	free(s);
}
void freeVariable(struct Variable* v){
	if(v==NULL) return;
	if(v->name!=NULL)
		free(v->name);
	if(v->type!=NULL)
		freeType(v->type);
	free(v);
}
void freeFunction(struct Function* f){
	if(f==NULL) return;
	if(f->name!=NULL)
		free(f->name);
	if(f->rtn_type!=NULL)
		freeType(f->rtn_type);
	if(f->attr_list!=NULL){
		for(int i=0;i<f->v_num;i++){
			if(NULL!=*(f->attr_list+i))
				freeVariable(*(f->attr_list+i));
			free(f->attr_list);
		}
	}
	free(f);
}
void freeHashNode(struct HashNode* h){
	if(h==NULL) return;
	if(h->type==VARIABLE&&h->u.v!=NULL)
		freeVariable(h->u.v);
	else if(h->type==STRUCTURE_DEF&&h->u.strt!=NULL)
		freeStructure(h->u.strt);
	else if(h->u.func!=NULL)
		freeFunction(h->u.func);
	if(h->next!=NULL)
		freeHashNode(h->next);
	free(h);
}
*/

/*
void printCurrentDepth(){

	if(listHead==NULL){
		return;
	}
	//printf("depth:%d\n",currentDepth);
	struct HashList* tmp=listHead->list;
	while(tmp!=NULL){
		unsigned int index;
		if(tmp->x->type==VARIABLE){
				//printf("%s,dp:%d\n",tmp->x->u.v->name,tmp->x->depth);
		}
		else if(tmp->x->type==STRUCTURE_DEF){
				printf("%s,dp:%d\n",tmp->x->u.strt->name,tmp->x->depth);
				//index=HashReflect(tmp->x->u.strt->name);
		}
		else if(tmp->x->type==FUNCTION){
				assert(0);
				//printf("%s\n",tmp->x->u.func->name);
				//index=HashReflect(tmp->x->u.func->name);
		}
		else assert(0);
		tmp=tmp->link;
	}
}*/

void removeCurrentDepth(){
	struct HashListHead* delTmp=listHead;
	struct HashList* tmp=listHead->list;
	struct HashList* beforeTmp=listHead->list;
	while(tmp!=NULL){
		unsigned int index;
		if(tmp->x->type==VARIABLE){
				index=HashReflect(tmp->x->u.v->name);
		}
		else if(tmp->x->type==STRUCTURE_DEF){
				index=HashReflect(tmp->x->u.strt->name);
		}
		else if(tmp->x->type==FUNCTION){
				assert(0);
				index=HashReflect(tmp->x->u.func->name);
		}
		else assert(0);
		struct HashNode* p=HashTable[index];
		struct HashNode* beforep=NULL;
		while(p!=tmp->x){
			beforep=p;
			p=p->next;
		}
		assert(p==tmp->x);
		if(beforep==NULL)
			HashTable[index]=HashTable[index]->next;
		else{
			beforep->next=p->next;
		}
	
		//free(tmp->x);//删hash表节点，横向删除
		tmp->x=NULL;
		tmp=tmp->link;
		//free(beforeTmp);//删深度栈链表节点，纵向删除
		beforeTmp=tmp;
	}
	listHead=listHead->nextHead;
	//free(delTmp);//删除深度栈对头，点删除
	//delTmp=NULL;
	
}

unsigned int HashReflect(char* name){
	unsigned int val=0,i;
	for(;*name;++name){
		val=(val<<2)+*name;
		if(i=val&~0x3fff)val=(val^(i>>12))&0x3fff;	
	}
	return val;
}

void HashInit(){
	for(int i=0;i<HASH_LENGTH;i++){
		HashTable[i]=NULL;
	}

}

bool HashAdd(enum HashType type,void* u,int line){
	struct HashNode* tmp=(struct HashNode*) malloc(sizeof(struct HashNode));
	tmp->type=type;
	tmp->depth=currentDepth;
	if(type==VARIABLE){
		tmp->u.v=(struct Variable*) u;
		tmp->line=line;
		unsigned int index=HashReflect(tmp->u.v->name);
		if(HashTable[index]==NULL){
			HashTable[index]=tmp;
			tmp->next=NULL;
		}

		else{
			struct HashNode* x=HashTable[index];
			do{
				if(x->depth==currentDepth&&x->type==VARIABLE&&!strcmp(x->u.v->name,tmp->u.v->name)){
					printf("Error type 3 at Line %d: Redfined variable \"%s\".\n",line,tmp->u.v->name);
					return false;	
				}
				else if(x->type==STRUCTURE_DEF&&!strcmp(x->u.strt->name,tmp->u.v->name)){
					printf("Error type 3 at Line %d: Conflicting variable name \"%s\" with structure defined at line %d.\n"
							,line,tmp->u.v->name,tmp->line);
					return false;
				}
				else if(x->type==FUNCTION&&!strcmp(x->u.func->name,tmp->u.v->name)){
					printf("Error type 3 at Line %d: Conflicting variable name \"%s\" with function defined at line %d.\n"
							,line,tmp->u.v->name,tmp->line);
					return false;
				}
				x=x->next;		
			}while(x!=NULL);
			tmp->next=HashTable[index];
			HashTable[index]=tmp;	
		}
		//对作用域栈进行添加 以便将来从hash table 中删除变量
		if(currentDepth>0){
			//第0层是最外层，不用删除所以不用记录
			struct HashList* tmpl=(struct HashList*)malloc(sizeof(struct HashList));
			tmpl->x=tmp;
			tmpl->link=listHead->list;
			listHead->list=tmpl;
		}
	}

	else if(type==STRUCTURE_DEF){
		tmp->depth=0;
		tmp->u.strt=(struct Structure*) u;
		tmp->line=line;
		if(tmp->u.strt->name==NULL)
			return true;
		unsigned int index=HashReflect(tmp->u.strt->name);
		if(HashTable[index]==NULL){
			HashTable[index]=tmp;
			tmp->next=NULL;
		}

		else{
			struct HashNode* x=HashTable[index];
			do{
				if(x->type==VARIABLE&&!strcmp(x->u.v->name,tmp->u.strt->name)){
					printf("Error type 16 at Line %d: Duplicated name \"%s\".\n",line,tmp->u.strt->name);
					return false;	
				}
				else if(x->type==STRUCTURE_DEF&&!strcmp(x->u.strt->name,tmp->u.strt->name)){
					printf("Error type 16 at Line %d: Duplicated name \"%s\".\n",line,tmp->u.strt->name);
					return false;
				}
				else if(x->type==FUNCTION&&!strcmp(x->u.func->name,tmp->u.strt->name)){
					printf("Error type 16 at Line %d: Duplicated name \"%s\".\n",line,tmp->u.strt->name);
					return false;
				}
				x=x->next;		
			}while(x!=NULL);
			tmp->next=HashTable[index];
			HashTable[index]=tmp;	
		}
		//结构体不用添作用域
		
	}
	else{
		assert(currentDepth==1&&type==FUNCTION);
		tmp->depth=0;//函数必然在第0层
		tmp->u.func=(struct Function*) u;
		tmp->line=line;
		unsigned int index=HashReflect(tmp->u.func->name);
		if(HashTable[index]==NULL){
			HashTable[index]=tmp;
			tmp->next=NULL;
		}

		else{
			struct HashNode* x=HashTable[index];
			do{
				if(x->type==VARIABLE&&!strcmp(x->u.v->name,tmp->u.func->name)){
					printf("Error type 4 at Line %d: Conflicting function name \"%s\" with vairable defined at line %d.\n"
							,line,tmp->u.func->name,tmp->line);
					return false;	
				}
				else if(x->type==STRUCTURE_DEF&&!strcmp(x->u.strt->name,tmp->u.func->name)){
					printf("Error type 4 at Line %d: Conflicting function name \"%s\" with structure defined at line %d.\n"
							,line,tmp->u.func->name,tmp->line);
					return false;
				}
				else if(x->type==FUNCTION&&!strcmp(x->u.func->name,tmp->u.func->name)){
					printf("Error type 4 at Line %d: Redefined function \"%s\".\n",line,tmp->u.func->name);
					return false;
				}
				x=x->next;		
			}while(x!=NULL);
			tmp->next=HashTable[index];
			HashTable[index]=tmp;	
		}
		//函数不需要添加深度栈，因为0层不需要删除
		/*
		if(currentDepth>0&&type!=FUNCTION){
			//第0层是最外层，不用删除所以不用记录
			struct HashList* tmpl=(struct HashList*)malloc(sizeof(struct HashList));
			tmpl->x=tmp;
			tmpl->link=listHead->list;
			listHead->list=tmpl;
		}
		*/
	}

	return true;
}

void* HashFind(enum HashType type,char* name){
	//if(name==NULL&&type==STRUCTURE_DEF)
	if(name==NULL)
		return NULL;
	unsigned int index=HashReflect(name);
		if(HashTable[index]==NULL)
			return NULL;

	if(type==VARIABLE){
		struct HashNode* x=HashTable[index];
		do{
			if(x->type==VARIABLE&&!strcmp(x->u.v->name,name))
				return x->u.v;
			else if(x->type==STRUCTURE_DEF&&!strcmp(x->u.strt->name,name))
				return NULL;
			else if(x->type==FUNCTION&&!strcmp(x->u.func->name,name))
				return NULL;
			x=x->next;		
		}while(x!=NULL);	
	
	}

	else if(type==STRUCTURE_DEF){
		struct HashNode* x=HashTable[index];
		do{
			if(x->type==VARIABLE&&!strcmp(x->u.v->name,name))
				return NULL;
		
			else if(x->type==STRUCTURE_DEF&&!strcmp(x->u.strt->name,name))
				return x->u.strt;

			else if(x->type==FUNCTION&&!strcmp(x->u.func->name,name))
				return NULL;

			x=x->next;		
		}while(x!=NULL);
	}
	else{
		struct HashNode* x=HashTable[index];
		do{
			if(x->type==VARIABLE&&!strcmp(x->u.v->name,name))
				return NULL;
			else if(x->type==STRUCTURE_DEF&&!strcmp(x->u.strt->name,name))
				return NULL;
			else if(x->type==FUNCTION&&!strcmp(x->u.func->name,name))
				return x->u.func;
			x=x->next;		
		}while(x!=NULL);		
	
	}
	return NULL;

}

void semanticAnalyse(struct Node* root){
	if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==29)
		ExtDefList(root->subNodeList[0]);
	else assert(0);
			
}

void ExtDefList(struct Node* root){
	if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==30&&root->subNodeList[1]->type==29){
		//ExitDefList -> ExtDef ExtDefList
		ExtDef(root->subNodeList[0]);
		ExtDefList(root->subNodeList[1]);
	}
	else if(root!=NULL&&root->subNum==0&&strcmp(root->value,"e")==0){
		//ExtDefList -> e
		;
	}
	else assert(0);
}


void ExtDef(struct Node* root){
	if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==32&&root->subNodeList[1]->type==31&&root->subNodeList[2]->type==4){
		//ExtDef -> Specifier ExtDecList SEMI
		struct Type* t=Specifier(root->subNodeList[0]);
		if(t==NULL)
			return;
		else
			ExtDecList(root->subNodeList[1],t);	
	}
	else if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==32&&root->subNodeList[1]->type==4){
		//ExtDef-> Specifier SEMI
		Specifier(root->subNodeList[0]);
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==32&&root->subNodeList[1]->type==37&&root->subNodeList[2]->type==40){
		//ExtDef -> Specifier FunDec CompSt 
		struct Type* t=Specifier(root->subNodeList[0]);
		//对作用域进行处理
		//printf("进入函数前\n");
		//printCurrentDepth();
		currentDepth=currentDepth+1;
		struct HashListHead* tmph=(struct HashListHead*)malloc(sizeof(struct HashListHead));
		tmph->nextHead=listHead;
		tmph->list=NULL;
		listHead=tmph;
		//printf("进入函数后\n");
		//printCurrentDepth();
		struct Function* f=FunDec(root->subNodeList[1],t);
		CompSt(root->subNodeList[2],f);	
		//printf("出来函数前\n");
		//printCurrentDepth();
		removeCurrentDepth();
		currentDepth=currentDepth-1;
		//printf("出来函数后\n");
		//printCurrentDepth();
	}
	else assert(0);
}

void ExtDecList(struct Node* root,struct Type* t){
	if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==36){
		//ExtDecList -> VarDec
		VarDec(root->subNodeList[0],false,t);
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==36&&root->subNodeList[1]->type==5&&root->subNodeList[2]->type==31){
		//ExtDecList -> VarDec COMMA ExtDecList
		VarDec(root->subNodeList[0],false,t);
		ExtDecList(root->subNodeList[2],t);
	}
	else assert(0);
}

struct Type* Specifier(struct Node* root){
	if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==16){
		//Specifier -> TYPE
		struct Type* rtn=(struct Type*)malloc(sizeof(struct Type));
		rtn->kind=BASIC;
		rtn->u.basic=TYPE(root->subNodeList[0]);
		return rtn;
	}
	else if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==33){
		//Specifier -> StructSpecifier
		return StructSpecifier(root->subNodeList[0]);
	}
	else assert(0);
	return NULL;
}

int TYPE(struct Node* root){
	if(root==NULL||root->subNum!=0)
		assert(0);
	else if(!strcmp(root->value,"int"))
		return 0;
	else if(!strcmp(root->value,"float"))
		return 1;
	assert(0);
	return -1;

}

bool CheckRepeat(struct FieldList* f){
	int len=0;
	struct FieldList* x=f;	
	while(x!=NULL){
		len=len+1;
		x=x->tail;
	}
	struct FieldList** tmp=(struct FieldList**)malloc(len*sizeof(struct FieldList*));
	len=0;
	x=f;
	while(x!=NULL){
		tmp[len]=x;
		len=len+1;
		x=x->tail;
	}
	for(int i=1;i<len;i++){
		for(int j=0;j<i;j++){
			if(!strcmp(tmp[i]->v->name,tmp[j]->v->name)){
				printf("Error type 15 at Line %d: Redfined field \"%s\".\n"
						,tmp[i]->line,tmp[i]->v->name);
				return false;
			}
		}
	}
	return true;
}


struct Type* StructSpecifier(struct Node* root){
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
			anonymousStrtNum=anonymousStrtNum+1;
			strt->id=anonymousStrtNum;
				
		}
		strt->structure=DefList(root->subNodeList[3],true);
		if(strt->structure==NULL){
			return NULL;
		}
		bool ok=CheckRepeat(strt->structure);
		if(!ok){
			return NULL;
		}

		if(strt->name!=NULL){
			ok=HashAdd(STRUCTURE_DEF,strt,tmpn->lineCount);
		}
		else ok=true;
		if(ok){
				struct Type* rtn=(struct Type*)malloc(sizeof(struct Type));
				rtn->kind=STRUCTURE;
				rtn->u.strt=strt;
				return 	rtn;
		}

		else{
			//如果结构体重定义，那么只返回空
			//freeStructure(strt);
			return NULL;
		}
	}
	else if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==23&&root->subNodeList[1]->type==35){
		//StructSpecifier -> STRUCT Tag
		struct Node* tmpn=Tag(root->subNodeList[1]);
		struct Structure* strt=HashFind(STRUCTURE_DEF,tmpn->value);
		if(strt==NULL){
			printf("Error type 17 at Line %d: Undefined structure \"%s\".\n",tmpn->lineCount,tmpn->value);
			return NULL;
		}
		else{
			struct Type* rtn=(struct Type*)malloc(sizeof(struct Type));
			rtn->kind=STRUCTURE;
			rtn->u.strt=strt;
			return rtn;
		}
	}
	else assert(0);

}





struct Node* OptTag(struct Node* root){
	if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==3){
		//OptTag -> ID
		return root->subNodeList[0];
	}
	else if(root!=NULL&&root->subNum==0&&strcmp(root->value,"e")==0){
		//OptTag -> e
		return NULL;
	}
	else assert(0);
	
}

struct Node* Tag(struct Node* root){
	if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==3){
		//Tag -> ID
		return root->subNodeList[0];
	}
	else assert(0);
	return NULL;

}

void* VarDec(struct Node* root,bool isStructCall,struct Type* t){
	if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==3){
		//VarDec -> ID
		if(isStructCall){
			struct FieldList* rtn=(struct FieldList*)malloc(sizeof(struct FieldList));
			rtn->v=(struct Variable*)malloc(sizeof(struct Variable));
			rtn->v->name=(char*)malloc(1+sizeof(char)*strlen(root->subNodeList[0]->value));
			strcpy(rtn->v->name,root->subNodeList[0]->value);
			rtn->v->type=t;
			rtn->tail=NULL;
			rtn->line=root->subNodeList[0]->lineCount;
			return rtn;
		}

		else{
			struct Variable* v=(struct Variable*)malloc(sizeof(struct Variable));
			v->name=(char*)malloc(1+sizeof(char)*strlen(root->subNodeList[0]->value));
			strcpy(v->name,root->subNodeList[0]->value);
			v->type=t;
			bool ok=HashAdd(VARIABLE,v,root->subNodeList[0]->lineCount);
			/*
			if(ok){
				printf("successfully add variable %s of type %d",v->name,v->type->u.basic);
			}
			*/
			return v;
		}	
	}
	else if(root!=NULL&&root->subNum==4&&root->subNodeList[0]->type==36&&root->subNodeList[1]->type==19&&root->subNodeList[2]->type==1&&root->subNodeList[3]->type==20){
		//VarDec -> VarDec LB INT RB	
		if(isStructCall){
			struct Type* tt=(struct Type*)malloc(sizeof(struct Type));
			tt->kind=ARRAY;
			tt->u.array.size=atoi(root->subNodeList[2]->value);
			tt->u.array.elem=t;
			return 	VarDec(root->subNodeList[0],true,tt);
		}

		else{
			struct Type* tt=(struct Type*)malloc(sizeof(struct Type));
			tt->kind=ARRAY;
			tt->u.array.size=atoi(root->subNodeList[2]->value);
			tt->u.array.elem=t;
			return VarDec(root->subNodeList[0],false,tt);
		}	
	}
	else assert(0);
}

struct Function* FunDec(struct Node* root,struct Type* t){
	if(root!=NULL&&root->subNum==4&&root->subNodeList[0]->type==3&&root->subNodeList[1]->type==17&&root->subNodeList[2]->type==38&&root->subNodeList[3]->type==18){
		//FunDec -> ID LP VarList RP		
		//ID(root->subNodeList[0]);
		struct Function* f=(struct Function*)malloc(sizeof(struct Function));
		f->name=root->subNodeList[0]->value;
		f->rtn_type=t;
		f->v_num=0;
		f->attrList=VarList(root->subNodeList[2],&f->v_num);
		bool ok=HashAdd(FUNCTION,f,root->lineCount);
		return f;
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==3&&root->subNodeList[1]->type==17&&root->subNodeList[2]->type==18){
		//FunDec -> ID LP RP
		struct Function* f=(struct Function*)malloc(sizeof(struct Function));
		f->name=root->subNodeList[0]->value;
		f->rtn_type=t;
		f->v_num=0;
		f->attrList=NULL;
		bool ok=HashAdd(FUNCTION,f,root->lineCount);
		return f;
	}
	else assert(0);
}

struct FieldList*  VarList(struct Node* root,int* num){
	if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==39&&root->subNodeList[1]->type==5&&root->subNodeList[2]->type==38){
		//VarList -> ParamDec COMMA VarList
		struct FieldList* rtn=(struct FieldList*)malloc(sizeof(struct FieldList));
		rtn->line=root->subNodeList[0]->subNodeList[1]->lineCount;
		rtn->v=ParamDec(root->subNodeList[0]);
		*num=*num+1;
		rtn->tail=VarList(root->subNodeList[2],num);
		return rtn;
	}
	else if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==39){
		//VarList -> ParamDec
		struct FieldList* rtn=(struct FieldList*)malloc(sizeof(struct FieldList));
		rtn->line=root->subNodeList[0]->subNodeList[1]->lineCount;
		rtn->v=ParamDec(root->subNodeList[0]);
		*num=*num+1;
		rtn->tail=NULL;
		return rtn;
	}
	else assert(0);
	return NULL;
}

struct Variable* ParamDec(struct Node* root){
	if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==32&&root->subNodeList[1]->type==36){
		//ParamDec -> Specifier VarDec
		struct Type* t=Specifier(root->subNodeList[0]);
		return VarDec(root->subNodeList[1],false,t);
	}
	else assert(0);
}

void CompSt(struct Node* root,struct Function* f){
	if(root!=NULL&&root->subNum==4&&root->subNodeList[0]->type==21&&root->subNodeList[1]->type==43&&root->subNodeList[2]->type==41&&root->subNodeList[3]->type==22){
		//CompSt -> LC DefList StmtList RC
		DefList(root->subNodeList[1],false);
		StmtList(root->subNodeList[2],f);
	}
	else assert(0);
}

void StmtList(struct Node* root,struct Function* f){
	if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==42&&root->subNodeList[1]->type==41){
		//StmtList -> Stmt StmtList
		Stmt(root->subNodeList[0],f);
		StmtList(root->subNodeList[1],f);
	}
	else if(root!=NULL&&root->subNum==0&&strcmp(root->value,"e")==0){
		//StmtList-> e
		;
	}
	else assert(0);
}

void Stmt(struct Node* root,struct Function* f){
	if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==4){
		//Stmt -> Exp SEMI
		Exp(root->subNodeList[0]);
	}
	else if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==40){
		//Stmt -> CompSt
		//对作用域进行处理
		//printf("进入块前\n");
		//printCurrentDepth();
		currentDepth=currentDepth+1;
		struct HashListHead* tmph=(struct HashListHead*)malloc(sizeof(struct HashListHead));
		tmph->nextHead=listHead;
		tmph->list=NULL;
		listHead=tmph;
		//printf("进入块后\n");
		//printCurrentDepth();
		CompSt(root->subNodeList[0],f);
		//printf("出来块前\n");
		//printCurrentDepth();
		removeCurrentDepth();
		currentDepth=currentDepth-1;
		//printf("出来块后\n");
		//printCurrentDepth();
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==24&&root->subNodeList[1]->type==47&&root->subNodeList[2]->type==4){
		//Stmt -> RETURN Exp SEMI
		struct Type* t=Exp(root->subNodeList[1]);
		bool ok;
		if(t==NULL||f==NULL)
			ok=false;
		else 
			 ok=isSameType(t,f->rtn_type);//检查返回值是否一致

		if(!ok)
			printf("Error type 8 at Line %d: Type mismatched for return.\n",root->subNodeList[0]->lineCount);
	}
	else if(root!=NULL&&root->subNum==5&&root->subNodeList[0]->type==25&&root->subNodeList[1]->type==17
		&&root->subNodeList[2]->type==47&&root->subNodeList[3]->type==18&&root->subNodeList[4]->type==42){
		//Stmt -> IF LP Exp RP Stmt
		Exp(root->subNodeList[2]);
		Stmt(root->subNodeList[4],f);
	}
	else if(root!=NULL&&root->subNum==7&&root->subNodeList[0]->type==25&&root->subNodeList[1]->type==17
		&&root->subNodeList[2]->type==47&&root->subNodeList[3]->type==18&&root->subNodeList[4]->type==42
		&&root->subNodeList[5]->type==26&&root->subNodeList[6]->type==42){
		//Stmt -> IF LP Exp RP Stmt ELSE Stmt
		Exp(root->subNodeList[2]);
		Stmt(root->subNodeList[4],f);
		Stmt(root->subNodeList[6],f);
	}
	else if(root!=NULL&&root->subNum==5&&root->subNodeList[0]->type==27&&root->subNodeList[1]->type==17
		&&root->subNodeList[2]->type==47&&root->subNodeList[3]->type==18&&root->subNodeList[4]->type==42){
		//Stmt -> WHILE LP Exp RP Stmt
		Exp(root->subNodeList[2]);
		Stmt(root->subNodeList[4],f);
	}
	else assert(0);
}

struct FieldList* DefList(struct Node* root,bool isStructCall){
	
	if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==44&&root->subNodeList[1]->type==43){
		//DefList -> Def DefList
		if(isStructCall){
			struct FieldList* tmpf=Def(root->subNodeList[0],true);
			struct FieldList* index=tmpf;
			if(index==NULL){
				return NULL;
			}
			while(index->tail!=NULL)
				index=index->tail;
			index->tail=DefList(root->subNodeList[1],true);
			return tmpf;
		}
		else{
			Def(root->subNodeList[0],false);
			DefList(root->subNodeList[1],false);
			return NULL;
		}
	}
	else if(root!=NULL&&root->subNum==0&&strcmp(root->value,"e")==0){
		//DefList -> e
		return NULL;
	}
	else assert(0);
}

struct FieldList* Def(struct Node* root,bool isStructCall){
	if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==32&&root->subNodeList[1]->type==45&&root->subNodeList[2]->type==4){
		//Def -> Specifier DecList SEMI
		if(isStructCall){
			struct Type* t=Specifier(root->subNodeList[0]);
			return DecList(root->subNodeList[1],true,t);
		}
		else{
			struct Type* t=Specifier(root->subNodeList[0]);
			DecList(root->subNodeList[1],false,t);
			return NULL;
		}
		
	}
	else assert(0);
}

struct FieldList* DecList(struct Node* root,bool isStructCall,struct Type* t){
	if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==46){
		//DecList -> Dec
		if(isStructCall){
			return Dec(root->subNodeList[0],true,t);
		}
		else{
			Dec(root->subNodeList[0],false,t);
			return NULL;
		}
	}

	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==46&&root->subNodeList[1]->type==5&&root->subNodeList[2]->type==45){
		//DecList -> Dec COMMA DecList
		if(isStructCall){
			struct FieldList* rtn;
			rtn=Dec(root->subNodeList[0],true,t);
			if(rtn!=NULL)
				rtn->tail=DecList(root->subNodeList[2],true,t);
			return rtn;
		}
		else{
			Dec(root->subNodeList[0],false,t);
			DecList(root->subNodeList[2],false,t);
			return NULL;
		}
	}
	else assert(0);
}

struct FieldList* Dec(struct Node* root,bool isStructCall,struct Type* t){
	if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==36){
		//Dec -> VarDec
		if(isStructCall)
			return VarDec(root->subNodeList[0],true,t);
		else{
			VarDec(root->subNodeList[0],false,t);
			return NULL;
		}	
	}

	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==36&&root->subNodeList[1]->type==6&&root->subNodeList[2]->type==47){
		//Dec -> VarDec ASSIGNOP Exp
		if(isStructCall){
			printf("Error type 15 at line %d:Initialize field when defining a structure\n",root->subNodeList[1]->lineCount);
			return NULL;
		}
		else{
			VarDec(root->subNodeList[0],false,t);//先设置变量的类型
			struct Type* tmp=Exp(root->subNodeList[2]);//再检查是否类型一致
	
			if(!isSameType(tmp,t))
				printf("Error type 5 at Line %d: Type mismatched for assignment.\n",root->subNodeList[1]->lineCount);

			return NULL;
		}
	}
	else assert(0);
}

bool isSameType(struct Type* t1,struct Type* t2){
	if(t1==NULL||t2==NULL)
		return false;
	if(t1->kind!=t2->kind)	
		return false;
	else if(t1->kind==BASIC){
		return (t1->u.basic==t2->u.basic);
	}
	else if(t1->kind==ARRAY)
		return (isSameType(t1->u.array.elem,t2->u.array.elem)&&t1->u.array.size==t2->u.array.size);
	else if(t1->kind==STRUCTURE){
		if(t1->u.strt->name!=NULL)
			return !strcmp(t1->u.strt->name,t2->u.strt->name);
		else{
			assert(t1->u.strt->id>0&&t2->u.strt->id>0);
			return t1->u.strt->id==t2->u.strt->id;
		}
	}
	assert(0);
	return false;
}

bool checkLeftValue(struct Node* root){
	if(root!=NULL&&root->subNum==4&&root->subNodeList[0]->type==47
		&&root->subNodeList[1]->type==19&&root->subNodeList[2]->type==47&&root->subNodeList[3]->type==20){
		//Exp -> Exp LB Exp RB
		return true;
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47
		&&root->subNodeList[1]->type==14&&root->subNodeList[2]->type==3){
		//Exp -> Exp DOT ID
		return true;
	}
	else if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==3){
		//Exp -> ID
		return true;
	}
	return false;
}

struct Type* Exp(struct Node* root){
	if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==6&&root->subNodeList[2]->type==47){
		//Exp -> Exp ASSIGNOP Exp
		bool isLeftValue=checkLeftValue(root->subNodeList[0]);
		struct Type* t1=Exp(root->subNodeList[0]);
		struct Type* t2=Exp(root->subNodeList[2]);
		if(!isLeftValue){
			//如果不是左值
			printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n"
			,root->subNodeList[1]->lineCount);
		}
		
		if(t1==NULL||t2==NULL)
			return NULL;
		
		else if(!isSameType(t1,t2))
			printf("Error type 5 at Line %d: Type mismatched for assignment.\n",root->subNodeList[1]->lineCount);
		
		else if(isLeftValue)
			return t1;
		
		else 
			return NULL;
	}

	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==12&&root->subNodeList[2]->type==47){
		//Exp -> Exp AND Exp
		struct Type* t1=Exp(root->subNodeList[0]);
		struct Type* t2=Exp(root->subNodeList[2]);
		if(t1==NULL||t2==NULL)
			return NULL;
		else if(t1->kind==BASIC&&t1->u.basic==0&&isSameType(t1,t2))
			return t1;
		else{
			printf("Error type 7 at Line %d: Type mismatched for operands.\n",root->subNodeList[1]->lineCount);
			return NULL;
		}

	}

	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==13&&root->subNodeList[2]->type==47){
		//Exp -> Exp OR Exp
		struct Type* t1=Exp(root->subNodeList[0]);
		struct Type* t2=Exp(root->subNodeList[2]);
		if(t1==NULL||t2==NULL)
			return NULL;
		else if(t1->kind==BASIC&&t1->u.basic==0&&isSameType(t1,t2))
			return t1;
		else{
			printf("Error type 7 at Line %d: Type mismatched for operands.\n",root->subNodeList[1]->lineCount);
			return NULL;
		}
	}

	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==7&&root->subNodeList[2]->type==47){
		//Exp -> Exp RELOP Exp
		struct Type* t1=Exp(root->subNodeList[0]);
		struct Type* t2=Exp(root->subNodeList[2]);
		if(t1==NULL||t2==NULL)
			return NULL;
		else if(t1->kind==BASIC&&isSameType(t1,t2)){
			struct Type* t=(struct Type*)malloc(sizeof(struct Type));
			t->kind==BASIC;
			t->u.basic=0;
			return t;
		}
		else{
			printf("Error type 7 at Line %d: Type mismatched for operands.\n",root->subNodeList[1]->lineCount);
			return NULL;
		}
	}

	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==8&&root->subNodeList[2]->type==47){
		//Exp -> Exp PLUS Exp
		struct Type* t1=Exp(root->subNodeList[0]);
		struct Type* t2=Exp(root->subNodeList[2]);
		if(t1==NULL||t2==NULL)
			return NULL;
		else if(t1->kind==BASIC&&isSameType(t1,t2)){
			return t1;
		}
		else{
			printf("Error type 7 at Line %d: Type mismatched for operands.\n",root->subNodeList[1]->lineCount);
			return NULL;
		}
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==9&&root->subNodeList[2]->type==47){
		//Exp -> Exp MINUS Exp
		struct Type* t1=Exp(root->subNodeList[0]);
		struct Type* t2=Exp(root->subNodeList[2]);
		if(t1==NULL||t2==NULL)
			return NULL;
		else if(t1->kind==BASIC&&isSameType(t1,t2)){
			return t1;
		}
		else{
			printf("Error type 7 at Line %d: Type mismatched for operands.\n",root->subNodeList[1]->lineCount);
			return NULL;
		}
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==10&&root->subNodeList[2]->type==47){
		//Exp -> Exp STAR Exp
		struct Type* t1=Exp(root->subNodeList[0]);
		struct Type* t2=Exp(root->subNodeList[2]);
		if(t1==NULL||t2==NULL)
			return NULL;
		else if(t1->kind==BASIC&&isSameType(t1,t2)){
			return t1;
		}
		else{
			printf("Error type 7 at Line %d: Type mismatched for operands.\n",root->subNodeList[1]->lineCount);
			return NULL;
		}
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==11&&root->subNodeList[2]->type==47){
		//Exp -> Exp DIV Exp
		struct Type* t1=Exp(root->subNodeList[0]);
		struct Type* t2=Exp(root->subNodeList[2]);
		if(t1==NULL||t2==NULL)
			return NULL;
		else if(t1->kind==BASIC&&isSameType(t1,t2)){
			return t1;
		}
		else{
			printf("Error type 7 at Line %d: Type mismatched for operands.\n",root->subNodeList[1]->lineCount);
			return NULL;
		}
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==17&&root->subNodeList[1]->type==47&&root->subNodeList[2]->type==18){
		//Exp -> LP Exp RP
		return Exp(root->subNodeList[1]);
	}
	else if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==9&&root->subNodeList[1]->type==47){
		//Exp -> MINUS Exp
		struct Type* t=Exp(root->subNodeList[1]);
		if(t==NULL)
			return NULL;
		else if(t->kind==BASIC){
			return t;
		}
		else{
			printf("Error type 7 at Line %d: Type mismatched for operands.\n",root->subNodeList[0]->lineCount);
			return NULL;
		}
	}
	else if(root!=NULL&&root->subNum==2&&root->subNodeList[0]->type==15&&root->subNodeList[1]->type==47){
		//Exp -> NOT Exp
		struct Type* t=Exp(root->subNodeList[1]);
		if(t==NULL)
			return NULL;
		else if(t->kind==BASIC&&t->u.basic==0){
			return t;
		}
		else{
			printf("Error type 7 at Line %d: Type mismatched for operands.\n",root->subNodeList[0]->lineCount);
			return NULL;
		}
	}
	else if(root!=NULL&&root->subNum==4&&root->subNodeList[0]->type==3&&root->subNodeList[1]->type==17&&root->subNodeList[2]->type==48&&root->subNodeList[3]->type==18){
		//Exp -> ID LP Args RP
		//ID(root->subNodeList[0]);
		struct Function* f=HashFind(FUNCTION,root->subNodeList[0]->value);
		if(f==NULL){
			if(HashFind(VARIABLE,root->subNodeList[0]->value)!=NULL)
				printf("Error type 11 at Line %d: \"%s\" is not a function.\n",root->lineCount,root->subNodeList[0]->value);
				
			else
				printf("Error type 2 at Line %d: Undefined function \"%s\".\n",root->lineCount,root->subNodeList[0]->value);
			return NULL;
		}
		Args(root->subNodeList[2],f->attrList,f->name);//检查参数对不对
		
		return f->rtn_type;
	
	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==3&&root->subNodeList[1]->type==17&&root->subNodeList[2]->type==18){
		//Exp -> ID LP RP
		//ID(root->subNodeList[0]);
		struct Function* f=HashFind(FUNCTION,root->subNodeList[0]->value);
		if(f==NULL){
			if(HashFind(VARIABLE,root->subNodeList[0]->value)!=NULL)
				printf("Error type 11 at Line %d: \"%s\" is not a function.\n",root->lineCount,root->subNodeList[0]->value);
				
			else
				printf("Error type 2 at Line %d: Undefined function \"%s\".\n",root->lineCount,root->subNodeList[0]->value);
			return NULL;
		}
		if(f->v_num!=0)
			printf("Error type 9 at Line %d: Arguments of Function \"%s\" are not compatible.\n"
				,root->lineCount,f->name);
		return f->rtn_type;
	}
	else if(root!=NULL&&root->subNum==4&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==19&&root->subNodeList[2]->type==47&&root->subNodeList[3]->type==20){
		//Exp -> Exp LB Exp RB
		struct Type* t1=Exp(root->subNodeList[0]);
		struct Type* t2=Exp(root->subNodeList[2]);
		if(t1==NULL||t2==NULL)
			return NULL;
		else if(t2->kind!=BASIC||t2->u.basic!=0){
			printf("Error type 12 at Line %d: Variable type in \"[]\" must be an interger.\n",root->subNodeList[2]->lineCount);
			return NULL;
		}
		else if(t1->kind!=ARRAY){
			printf("Error type 10 at Line %d: ID before \"[]\" is not an array.\n",root->subNodeList[0]->lineCount);
			return NULL;
		}
		else return t1->u.array.elem;

	}
	else if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==14&&root->subNodeList[2]->type==3){
		//Exp -> Exp DOT ID
		struct Type* t=Exp(root->subNodeList[0]);
		if(t==NULL)
			return NULL;
		else if(t->kind!=STRUCTURE){
			printf("Error type 13 at Line %d: The type of variable before \".\" must be structure.\n"
				,root->subNodeList[0]->lineCount);
			return NULL;
		}
		else{
			struct Type* tt=NULL;
			struct FieldList* f=t->u.strt->structure;
			while(f!=NULL){
				if(!strcmp(f->v->name,root->subNodeList[2]->value)){
					tt=f->v->type;
					break;
				}
				f=f->tail;
			}

			if(tt==NULL){
				printf("Error type 14 at Line %d: Non-existent field \"%s\".\n"
					,root->subNodeList[2]->lineCount,root->subNodeList[2]->value);
				return NULL;
			}
			else return tt;
		}
		//ID(root->subNodeList[2]);
	}
	else if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==3){
		//Exp -> ID
		struct Variable* v=HashFind(VARIABLE,root->subNodeList[0]->value);
		if(v==NULL){
			printf("Error type 1 at Line %d: Undefined variable \"%s\".\n"
					,root->subNodeList[0]->lineCount,root->subNodeList[0]->value);
			return NULL;
		}
		else return v->type;
	}
	else if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==1){
		//Exp -> INT
		//INT(root->subNodeList[0]);
		struct Type* rtn=(struct Type*)malloc(sizeof(struct Type));
		rtn->kind==BASIC;
		rtn->u.basic=0;
		return rtn;
	}
	else if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==2){
		//Exp -> FLOAT
		//FLOAT(root->subNodeList[0]);
		struct Type* rtn=(struct Type*)malloc(sizeof(struct Type));
		rtn->kind==BASIC;
		rtn->u.basic=1;
		return rtn;
	}
	else assert(0);
}

void Args(struct Node* root,struct FieldList* f,char* funcName){
	if(root!=NULL&&root->subNum==3&&root->subNodeList[0]->type==47&&root->subNodeList[1]->type==5
		&&root->subNodeList[2]->type==48){
		//Args -> Exp COMMA Args
		struct Type* t=Exp(root->subNodeList[0]);
		if(f==NULL||!isSameType(t,f->v->type)){;
			printf("Error type 9 at Line %d: Arguments of Function \"%s\" are not compatible.\n"
				,root->lineCount,funcName);
			return ;
		}

		Args(root->subNodeList[2],f->tail,funcName);
	}

	else if(root!=NULL&&root->subNum==1&&root->subNodeList[0]->type==47){
		//Args -> Exp
		struct Type* t=Exp(root->subNodeList[0]);
		if(f==NULL||!isSameType(t,f->v->type)||(f->tail!=NULL)){
			printf("Error type 9 at Line %d: Arguments of Function \"%s\" are not compatible.\n"
				,root->lineCount,funcName);
			return ;
		}

	}
	else assert(0);
}





