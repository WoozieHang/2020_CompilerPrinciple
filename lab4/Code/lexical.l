
%{
	#include "syntax.tab.h"
	#include"tree.h"
	int line=1;
	extern int ErrorNum;
%}
%option yylineno
INT 0|[1-9][0-9]*
FLOAT ([0-9]+[.][0-9]+)|((([0-9]*.[0-9]+)|([0-9]+.[0-9]*))(E|e)([+-][0-9]+|[0-9]+))
SEMI ";"
COMMA ","
ASSIGNOP "="
RELOP ">"|"<"|">="|"<="|"=="|"!="
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
DOT "."
NOT "!"
TYPE "int"|"float"
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
STRUCT "struct"
RETURN "return"
IF "if"
ELSE "else"
WHILE "while"
ID [_A-Za-z][_A-Za-z0-9]*
BLANK (\t|\ )
ENTER (\r|\n)
%%
{ENTER} {eofLine=eofLine+1;}
{BLANK} {} 
{INT} {chaNum=chaNum+1;yylval=createNode(INT,yytext,yylineno,NULL,0,true);return INT;}
{FLOAT} {chaNum=chaNum+1;yylval=createNode(FLOAT,yytext,yylineno,NULL,0,true);return FLOAT;}
{SEMI} {chaNum=chaNum+1;yylval=createNode(SEMI,yytext,yylineno,NULL,0,true);return SEMI;}
{COMMA} {chaNum=chaNum+1;yylval=createNode(COMMA,yytext,yylineno,NULL,0,true);return COMMA;}
{ASSIGNOP} {chaNum=chaNum+1;yylval=createNode(ASSIGNOP,yytext,yylineno,NULL,0,true);return ASSIGNOP;}
{RELOP} {chaNum=chaNum+1;yylval=createNode(RELOP,yytext,yylineno,NULL,0,true);return RELOP;}
{PLUS} {chaNum=chaNum+1;yylval=createNode(PLUS,yytext,yylineno,NULL,0,true);return PLUS;}
{MINUS} {chaNum=chaNum+1;yylval=createNode(MINUS,yytext,yylineno,NULL,0,true);return MINUS;}
{STAR} {chaNum=chaNum+1;yylval=createNode(STAR,yytext,yylineno,NULL,0,true);return STAR;}
{DIV} {chaNum=chaNum+1;yylval=createNode(DIV,yytext,yylineno,NULL,0,true);return DIV;}
{AND} {chaNum=chaNum+1;yylval=createNode(AND,yytext,yylineno,NULL,0,true);return AND;}
{OR} {chaNum=chaNum+1;yylval=createNode(OR,yytext,yylineno,NULL,0,true);return OR;}
{DOT} {chaNum=chaNum+1;yylval=createNode(DOT,yytext,yylineno,NULL,0,true);return DOT;}
{NOT} {chaNum=chaNum+1;yylval=createNode(NOT,yytext,yylineno,NULL,0,true);return NOT;}
{TYPE} {chaNum=chaNum+1;yylval=createNode(TYPE,yytext,yylineno,NULL,0,true);return TYPE;}
{LP} {chaNum=chaNum+1;yylval=createNode(LP,yytext,yylineno,NULL,0,true);return LP;}
{RP} {chaNum=chaNum+1;yylval=createNode(RP,yytext,yylineno,NULL,0,true);return RP;}
{LB} {chaNum=chaNum+1;yylval=createNode(LB,yytext,yylineno,NULL,0,true);return LB;}
{RB} {chaNum=chaNum+1;yylval=createNode(RB,yytext,yylineno,NULL,0,true);return RB;}
{LC} {chaNum=chaNum+1;yylval=createNode(LC,yytext,yylineno,NULL,0,true);return LC;}
{RC} {chaNum=chaNum+1;yylval=createNode(RC,yytext,yylineno,NULL,0,true);return RC;}
{STRUCT} {chaNum=chaNum+1;yylval=createNode(STRUCT,yytext,yylineno,NULL,0,true);return STRUCT;}
{RETURN} {chaNum=chaNum+1;yylval=createNode(RETURN,yytext,yylineno,NULL,0,true);return RETURN;}
{IF} {chaNum=chaNum+1;yylval=createNode(IF,yytext,yylineno,NULL,0,true);return IF;}
{ELSE} {chaNum=chaNum+1;yylval=createNode(ELSE,yytext,yylineno,NULL,0,true);return ELSE;}
{WHILE} {chaNum=chaNum+1;yylval=createNode(WHILE,yytext,yylineno,NULL,0,true);return WHILE;}
{ID} {chaNum=chaNum+1;yylval=createNode(ID,yytext,yylineno,NULL,0,true);return ID;}
. {chaNum=chaNum+1;printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno,yytext);ErrorNum=ErrorNum+1;}
%%
