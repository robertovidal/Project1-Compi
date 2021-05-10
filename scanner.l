%{
	#include "scanner.h"
	#include <stdio.h>
	#include <string.h>
	int int_val;
	float float_val;
	token Get_Token(void);	
	void change_input(char *file);
	void finish(void);
%}

intvalue     [0-9]+
floatvalue	[0-9]+\.[0-9]+
stringvalue \".*\"
charvalue	\'.\'
identifier       ([a-zA-Z]|_)([a-zA-Z0-9]|_)*
doublequotationmark	\"
simplequotationmark \'
backslash	\\
comment	\/\/.*
multilinecomment	[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]

%%

\t {return TAB;}
\n	{return NEWLINE;}
{comment} {}
{multilinecomment} {}
{intvalue}	{int_val = atoi(yytext) ;return INTVALUE;}
{floatvalue}	{float_val = atof(yytext) ;return FLOATVALUE;}
{stringvalue}	{return STRINGVALUE;}
{charvalue}	{return CHARVALUE;}
"auto"	{ return AUTO;}
"break"	{return BREAK;}
"case"	{return CASE;}
"char"	{return CHAR;}
"const"	{return CONST;}
"continue"	{return CONTINUE;}
"default"	{return DEFAULT;}
"do"	{return DO;}
"double"	{return DOUBLE;}
"else"	{return ELSE;}
"enum" {return ENUM;}
"extern"	{return EXTERN;}
"float"	{return FLOAT;}
"for"	{return FOR;}
"goto"	{return GOTO;}
"if"	{return IF;}
"int"	{return INT;}
"long" {return LONG;}
"register"	{return REGISTER;}
"return"	{return RETURN;}
"short"	{return SHORT;}
"signed"	{return SIGNED;}
"sizeof"	{return SIZEOF;}
"static"	{return STATIC;}
"struct"	{return STRUCT;}
"switch"	{return SWITCH;}
"typedef"	{return TYPEDEF;}
"union"	{return UNION;}
"unsigned"	{return UNSIGNED;}
"void"	{return VOID;}
"volatile"	{return VOLATILE;}
"while"	{return WHILE;}
"++"	{return INCREMENTOP;}
"--"	{return DECREMENTOP;}
"=="	{return EQUALOP;}
"!="	{return NOTEQUALOP;}
">="	{return GREATEROREQUALTHANOP;}
"<="	{return LESSOREQUALTHANOP;}
"&&"	{return ANDOP;}
"||"	{return OROP;}
"<<"	{return LEFTSHIFTOP;}
">>"	{return RIGHTSHIFTOP;}
"+="	{return ADDASSIGNMENTOP;}
"-="	{return SUBSTRACTASSIGNMENTOP;}
"*="	{return MULTASSIGNMENTOP;}
"/="	{return DIVASSIGNMENTOP;}
"%="	{return MODASSIGNMENTOP;}
"<<="	{return LEFTSHIFTASSIGNMENTOP;}
">>="	{return RIGHTSHIFTASSIGNMENTOP;}
"&="	{return BITWISEASSIGNMENTOP;}
"^="	{return BITWISEEXCLUSIVEASSIGNMENTOP;}
"|="	{return BITWISEINCLUSIVEASSIGNMENTOP;}
"~" {return VIRGULILLA;}
"!" {return EXCLAMATIONMARK;}
"#" {return NUMBERSIGN;}
"$"	{return DOLLARSIGN	;}
"%"	{return PERCENTSIGN;}
"^"	{return CARET;}
"&"	{return AMPERSAND;}
"*"	{return ASTERISK;}
"("	{return LEFTPARENTHESIS;}
")"	{return RIGHTPARENTHESIS;}
"_"	{return UNDERSCORE;}
"+"	{return PLUSSIGN;}
"|"	{return VERTICALSIGN;}
{backslash}	{return BACKSLASH;}
"`"	{return APOSTROPHE;}
"-"	{return MINUSSIGN;}
"="	{return EQUALSIGN;}
"{"	{return LEFTBRACE;}
"}"	{return RIGHTBRACE;}
"["	{return LEFTBRACKET;}
"]"	{return RIGHTBRACKET;}
":"	{return COLON;}
{doublequotationmark}	{return DOUBLEQUOTATIONMARK;}
{simplequotationmark}	{return SIMPLEQUOTATIONMARK;}	
";"	{return SEMICOLON;}
"<"	{return OPENINGANGLEBRACKET;}
">"	{return CLOSINGANGLEBRACKET;}
"?"	{return QUESTIONMARK;}
","	{return COMMA;}	
"."	{return PERIOD;}
"/"	{return SLASH;}
{identifier} {return IDENTIFIER;}
<<EOF>>	{return END_OF_FILE;}
" "	{}
.	{return OTHER;}

%%

void change_input(char *file){
	yyin = fopen(file, "r");
}

token Get_Token(){
	struct tokens theToken; 
	token_code code = yylex();
	if(code == INTVALUE){
		theToken.valueint = int_val;
	} else if(code != END_OF_FILE){
		theToken.valuechar = yytext;
	}
	theToken.code = code;
	return theToken;
}

void finish(void){
	fclose(yyin);
}