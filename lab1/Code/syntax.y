
%{
	#include "lex.yy.c" 
	#include"tree.h"
	#define MAX_SUBNODE_NUM 30
	int tmp_type=0;
	int tmp_lineNum=0;
	struct Node* tmp_treeNode[MAX_SUBNODE_NUM];
	int tmp_NodeNum=0;
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
	extern int ErrorNum;
	extern struct Node* Root;
	extern int yyerror(char const *msg);
	extern int yylineno;
%}
%define api.value.type {struct Node*}
%token INT 1
%token FLOAT 2 
%token ID 3 SEMI 4 COMMA 5
%token ASSIGNOP 6
%token RELOP 7
%token PLUS 8 MINUS 9 STAR 10 DIV 11 AND 12 OR 13 DOT 14 NOT 15 
%token TYPE 16
%token LP 17 RP 18 LB 19 RB 20 LC 21 RC 22
%token STRUCT 23 RETURN 24 IF 25 ELSE 26 WHILE 27 
%right ASSIGNOP 
%left OR
%left AND
%left RELOP
%left PLUS MINUS 
%left STAR DIV
%right NOT
%left DOT LP RP LB RB COMMA
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%
Program : ExtDefList {tmp_treeNode[0]=$1;$$=createNode(28,NULL,yylineno,tmp_treeNode,1,false);Root=$$;};
ExtDefList : ExtDef ExtDefList {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;$$=createNode(29,NULL,yylineno,tmp_treeNode,2,false);}
	| /*empty*/ {$$=createNode(29,"e",yylineno,NULL,0,false);};
ExtDef : Specifier ExtDecList SEMI {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(30,NULL,yylineno,tmp_treeNode,3,false);}
	| Specifier SEMI {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;$$=createNode(30,NULL,yylineno,tmp_treeNode,2,false);}
	| Specifier FunDec CompSt {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(30,NULL,yylineno,tmp_treeNode,3,false);};
ExtDecList : VarDec {tmp_treeNode[0]=$1;$$=createNode(31,NULL,yylineno,tmp_treeNode,1,false);}
	| VarDec COMMA ExtDecList {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(31,NULL,yylineno,tmp_treeNode,3,false);};
Specifier : TYPE {tmp_treeNode[0]=$1;$$=createNode(32,NULL,yylineno,tmp_treeNode,1,false);}
	| StructSpecifier {tmp_treeNode[0]=$1;$$=createNode(32,NULL,yylineno,tmp_treeNode,1,false);};
StructSpecifier : STRUCT OptTag LC DefList RC {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;tmp_treeNode[3]=$4;tmp_treeNode[4]=$5;$$=createNode(33,NULL,yylineno,tmp_treeNode,5,false);}
	| STRUCT Tag {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;$$=createNode(33,NULL,yylineno,tmp_treeNode,2,false);};
OptTag : ID {tmp_treeNode[0]=$1;$$=createNode(34,NULL,yylineno,tmp_treeNode,1,false);}
	| /*empty*/ {$$=createNode(34,"e",yylineno,NULL,0,false);};
Tag : ID {tmp_treeNode[0]=$1;$$=createNode(35,NULL,yylineno,tmp_treeNode,1,false);};
VarDec : ID {tmp_treeNode[0]=$1;$$=createNode(36,NULL,yylineno,tmp_treeNode,1,false);}
	| VarDec LB INT RB {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;tmp_treeNode[3]=$4;$$=createNode(36,NULL,yylineno,tmp_treeNode,4,false);};
FunDec : ID LP VarList RP {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;tmp_treeNode[3]=$4;$$=createNode(37,NULL,yylineno,tmp_treeNode,4,false);}
	| ID LP RP {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(37,NULL,yylineno,tmp_treeNode,3,false);}
	| error RP {/*printf("error FunDec\n");*/}; 
VarList : ParamDec COMMA VarList {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(38,NULL,yylineno,tmp_treeNode,3,false);}
	| ParamDec {tmp_treeNode[0]=$1;$$=createNode(38,NULL,yylineno,tmp_treeNode,1,false);};
ParamDec : Specifier VarDec {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;$$=createNode(39,NULL,yylineno,tmp_treeNode,2,false);}
	| error COMMA {/*printf("ParaDec error\n");*/};
CompSt : LC DefList StmtList RC {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;tmp_treeNode[3]=$4;$$=createNode(40,NULL,yylineno,tmp_treeNode,4,false);}
	| error RC {/*printf("CompSt error\n");*/};
StmtList : Stmt StmtList {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;$$=createNode(41,NULL,yylineno,tmp_treeNode,2,false);}
	| /*empty*/ {$$=createNode(41,"e",yylineno,NULL,0,false);};
Stmt : Exp SEMI {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;$$=createNode(42,NULL,yylineno,tmp_treeNode,2,false);}
	| CompSt {tmp_treeNode[0]=$1;$$=createNode(42,NULL,yylineno,tmp_treeNode,1,false);}
	| RETURN Exp SEMI {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(42,NULL,yylineno,tmp_treeNode,3,false);}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;tmp_treeNode[3]=$4;tmp_treeNode[4]=$5;$$=createNode(42,NULL,yylineno,tmp_treeNode,5,false);}
	| IF LP Exp RP Stmt ELSE Stmt {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;tmp_treeNode[3]=$4;tmp_treeNode[4]=$5;tmp_treeNode[5]=$6;tmp_treeNode[6]=$7;$$=createNode(42,NULL,yylineno,tmp_treeNode,7,false);}
	| WHILE LP Exp RP Stmt {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;tmp_treeNode[3]=$4;tmp_treeNode[4]=$5;$$=createNode(42,NULL,yylineno,tmp_treeNode,5,false);}
	| error SEMI {/*printf("Stmt error\n");*/};
DefList : Def DefList {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;$$=createNode(43,NULL,yylineno,tmp_treeNode,2,false);}
	| /*empty*/ {$$=createNode(43,"e",yylineno,NULL,0,false);};
Def : Specifier DecList SEMI {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(44,NULL,yylineno,tmp_treeNode,3,false);};
DecList : Dec {tmp_treeNode[0]=$1;$$=createNode(45,NULL,yylineno,tmp_treeNode,1,false);}
	| Dec COMMA DecList {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(45,NULL,yylineno,tmp_treeNode,3,false);};
Dec : VarDec {tmp_treeNode[0]=$1;$$=createNode(46,NULL,yylineno,tmp_treeNode,1,false);}
	| VarDec ASSIGNOP Exp {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(46,NULL,yylineno,tmp_treeNode,3,false);};
Exp : Exp ASSIGNOP Exp {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(47,NULL,yylineno,tmp_treeNode,3,false);}
	| Exp AND Exp {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(47,NULL,yylineno,tmp_treeNode,3,false);}
	| Exp OR Exp {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(47,NULL,yylineno,tmp_treeNode,3,false);}
	| Exp RELOP Exp {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(47,NULL,yylineno,tmp_treeNode,3,false);}
	| Exp PLUS Exp {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(47,NULL,yylineno,tmp_treeNode,3,false);}
	| Exp MINUS Exp {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(47,NULL,yylineno,tmp_treeNode,3,false);}
	| Exp STAR Exp {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(47,NULL,yylineno,tmp_treeNode,3,false);}
	| Exp DIV Exp {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(47,NULL,yylineno,tmp_treeNode,3,false);}
	| LP Exp RP {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(47,NULL,yylineno,tmp_treeNode,3,false);}
	| MINUS Exp {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;$$=createNode(47,NULL,yylineno,tmp_treeNode,2,false);}
	| NOT Exp {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;$$=createNode(47,NULL,yylineno,tmp_treeNode,2,false);}
	| ID LP Args RP {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;tmp_treeNode[3]=$4;$$=createNode(47,NULL,yylineno,tmp_treeNode,4,false);}
	| ID LP RP {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(47,NULL,yylineno,tmp_treeNode,3,false);}
	| Exp LB Exp RB {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;tmp_treeNode[3]=$4;$$=createNode(47,NULL,yylineno,tmp_treeNode,4,false);}
	| Exp DOT ID {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(47,NULL,yylineno,tmp_treeNode,3,false);}
	| ID {tmp_treeNode[0]=$1;$$=createNode(47,NULL,yylineno,tmp_treeNode,1,false);}
	| INT {tmp_treeNode[0]=$1;$$=createNode(47,NULL,yylineno,tmp_treeNode,1,false);}
	| FLOAT {tmp_treeNode[0]=$1;$$=createNode(47,NULL,yylineno,tmp_treeNode,1,false);};
Args : Exp COMMA Args {tmp_treeNode[0]=$1;tmp_treeNode[1]=$2;tmp_treeNode[2]=$3;$$=createNode(48,NULL,yylineno,tmp_treeNode,3,false);}
	| Exp {tmp_treeNode[0]=$1;$$=createNode(48,NULL,yylineno,tmp_treeNode,1,false);};
%%
	int yyerror(char const *msg){
		if(yylval!=NULL&&yylval->isLeaf)
			printf("Error type B at line %d: %s (Unexpected \"%s\" in this position)\n", yylineno, msg,yylval->value);
		else{
			printf("Error type B at line %d: %s (Confusing usage this line)", yylineno, msg);	
		}
		ErrorNum=ErrorNum+1;	
	}

