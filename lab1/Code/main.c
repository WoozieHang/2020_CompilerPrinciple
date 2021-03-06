#include<stdio.h>
#include"tree.h"
extern int yyparse (void);
extern int yyrestart(FILE*);
extern int yydebug;
extern int yylineno;
extern int printId;
int ErrorNum=0;
struct Node* Root=NULL;
int eofLine=1;
int chaNum=0;

int main(int argc,char** argv){
	if(argc<=1) return 1;
	FILE* f=fopen(argv[1],"r");
	if(!f){
		perror(argv[1]);
		return 1;
	}
	yylineno=1;
	ErrorNum=0;
	eofLine=1;
	chaNum==0;
	yyrestart(f);
	//yydebug=1;
	yyparse();
	printId=1;
	if(ErrorNum==0){
		if(chaNum==0){
			;//Root->lineCount=eofLine+1;		
		}
		printTree(Root,0);
	}
	//printf("start release\n");
	releaseTree(&Root);
	//printTree(Root,0);
	return 0;
}
