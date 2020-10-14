#include"tree.h"
int printId=1;
struct Node* createNode(int t,char*v,int lc,struct Node** l,int n,bool islf){
	struct Node* rtn=(struct Node*)malloc(sizeof(struct Node));
	rtn->type=t;
	if(v!=NULL){
		rtn->value=(char*)malloc(strlen(v)*sizeof(char)+1);
		strcpy(rtn->value,v);
	}
	else rtn->value=NULL;
	if(islf)
		rtn->lineCount=lc;
	else{
		rtn->lineCount=lc;//
		if(n>0)
		rtn->lineCount=l[0]->lineCount;
	}
	if(n>0&&l!=NULL){
		rtn->subNodeList=(struct Node**)malloc(n*sizeof(struct Node*));
		for(int i=0;i<n;i++){
			rtn->subNodeList[i]=(struct Node*)malloc(sizeof(struct Node));
			*rtn->subNodeList[i]=*l[i];
		}
	}
	else rtn->subNodeList=NULL;
	rtn->subNum=n;
	rtn->isLeaf=islf;
}

void printTree(struct Node* root,int depth){
	//printf("root==NULL?:%d ",root==NULL);
	if(root!=NULL&&(root->isLeaf==true||root->subNum>0)){		
		//printf("%d ",printId);
		printId=printId+1;
		if(root->isLeaf==true){
			for(int i=0;i<depth;i++){
				printf("  ");
			}
			printf("%s",getTagStr(root->type));//call a switch,making Str from int
			printValue(root->type,root->value);
		}
		else{
			for(int i=0;i<depth;i++){
				printf("  ");
			}
			printf("%s (%d)\n",getTagStr(root->type),root->lineCount);
			for(int i=0;i<root->subNum;i++){
				printTree(root->subNodeList[i],depth+1);
			}
		}
		
	}

}

void releaseTree(struct Node**root){
	if((*root)!=NULL){
		for(int i=0;i<(*root)->subNum;i++){
				releaseTree(&(*root)->subNodeList[i]);
		}
		free(*root);
		(*root)=NULL;
	}
}

void printValue(int id,char* value){
	switch(id){
	case(1): printf(": %d\n",atoi(value));break;
	case(2): printf(": %f\n",atof(value));break;
	case(3): printf(": %s\n",value);break;
	case(16): printf(": %s\n",value);break;
	default: printf("\n");break;
	}

}



char* getTagStr(int id){
	switch(id){
	case(1): return "INT";break;
	case(2): return "FLOAT";break;
	case(3): return "ID";break;
	case(4): return "SEMI";break;
	case(5): return "COMMA";break;                           
	case(6): return "ASSIGNOP";break;
	case(7): return "RELOP";break;
	case(8): return "PLUS";break;
	case(9): return "MINUS";break;
	case(10): return "STAR";break;
	case(11): return "DIV";break;
	case(12): return "AND";break;
	case(13): return "OR";break;
	case(14): return "DOT";break;
	case(15): return "NOT";break;
	case(16): return "TYPE";break;
	case(17): return "LP";break;
	case(18): return "RP";break;
	case(19): return "LB";break;
	case(20): return "RB";break;
	case(21): return "LC";break;
	case(22): return "RC";break;
	case(23): return "STRUCT";break;
	case(24): return "RETURN";break;
	case(25): return "IF";break;
	case(26): return "ELSE";break;
	case(27): return "WHILE";break;
	case(28): return "Program";break;
	case(29): return "ExtDefList";break;
	case(30): return "ExtDef";break;
	case(31): return "ExtDecList";break;
	case(32): return "Specifier";break;
	case(33): return "StructSpecifier";break;
	case(34): return "OptTag";break;
	case(35): return "Tag";break;
	case(36): return "VarDec";break;
	case(37): return "FunDec";break;
	case(38): return "VarList";break;
	case(39): return "ParamDec";break;
	case(40): return "CompSt";break;
	case(41): return "StmtList";break;
	case(42): return "Stmt";break;
	case(43): return "DefList";break;
	case(44): return "Def";break;
	case(45): return "DecList";break;
	case(46): return "Dec";break;
	case(47): return "Exp";break;
	case(48): return "Args";break;
	default: return "error";break;
	}
}


