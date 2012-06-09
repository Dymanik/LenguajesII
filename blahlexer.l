%{
#include <string>
#include <limits>
#include "blahlog.h"
#include "blahast.h"
#include "blahparser.hpp"
#define SAVE_STRING yylval.string = new std::string(string_buf)
#define MAX_STR_CONST 2048
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;\
    yylloc.first_column = yycolumn; yylloc.last_column = yycolumn+yyleng-1;\
    yycolumn +=yyleng;

char string_buf[MAX_STR_CONST];
char *string_buf_ptr;
extern Log log;


int yycolumn=1;
bool lexerror;
%}
%option noyywrap

%x COMMENTB
%x COMMENTL
%x STRESC
%x STRLIT
%x CHR

DOUBLE [0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?

%%

<INITIAL>{

	"/*"				BEGIN(COMMENTB);
	"//"				BEGIN(COMMENTL);
	\"					{string_buf_ptr=string_buf;BEGIN(STRESC);}
	\'					{string_buf_ptr=string_buf;BEGIN(STRLIT);}
	"#"					BEGIN(CHR);

	[1-9][0-9]*			{	yylval.integer=strtol(yytext,NULL,10);
							if(yylval.integer > std::numeric_limits<int>::max()){
								log.add(Msg(yylineno,"Integer overflow",1));
							}
							return INT;
						}

	0[0-7]*				{	yylval.integer=strtol(yytext,NULL,8);
							if(yylval.integer < std::numeric_limits<int>::min()){
								log.add(Msg(yylineno,"Integer underflow",1));
							} else if(yylval.integer > std::numeric_limits<int>::max()){
								log.add(Msg(yylineno,"Integer overflow",1));
							}
							return INT;
						}
	
	0x[0-9A-Fa-f]*		{	yylval.integer=strtol(yytext,NULL,16);
							if(yylval.integer < std::numeric_limits<int>::min()){
								log.add(Msg(yylineno,"Integer underflow",1));
							} else if(yylval.integer > std::numeric_limits<int>::max()){
								log.add(Msg(yylineno,"Integer overflow",1));
							}
							return INT;
						}

	{DOUBLE}			{
							yylval.floating=strtod(yytext,NULL);
							if(yylval.floating < std::numeric_limits<float>::min()){
								log.add(Msg(yylineno,"Float underflow",1));
							} else if(yylval.floating > std::numeric_limits<float>::max()){
								log.add(Msg(yylineno,"Float overflow",1));
							}
							
							return FLOAT;
						}

	[(){}\[\],.!=<>%]	{return yytext[0];}

	"is"				{return '=';}

	"=="				{return TEQ;}
	"<="				{return TLEQ;}
	">="				{return TGEQ;}
	"!="				{return TNEQ;}
	"->"				{return ACCESS;}

	"&&"				|
	"and"				{return TAND;}
	"||"				|
	"or"				{return TOR;}
	"^"					|
	"if"				{return IF;}
	"then"				{return THEN;}
	"else"				{return ELSE;}
	"true"				{return TRUE;}
	"false"				{return FALSE;}

	"while"				{return WHILE;}
	"do"				{return DO;}
	"stop"				{return STOP;}
	"next"				{return NEXT;}
	
	"for"				{return FOR;}
	"from"				{return FROM;}
	"in"				{return IN;}
	"to"				{return TO;}
	"return"			{return RETURN;}
	"step"				{return STEP;}
	

	"+"					|
	"plus"				{return '+';}
	"-"					|
	"minus"				{return '-';}
	"/"					|
	"dividedby"			{return '/';}
	"*"					|
	"times"				{return '*';}

	"not"				{return '!';}

	"string"			{return STRING;}
	"register"			{return REGISTER;}
	"union"				{return UNION;}
	"Arrayof"			{return ARRAY;}

	[A-Z][A-Za-z0-9_]*	{yylval.string = new std::string(yytext, yyleng); return TYPEID;}
	[a-z][A-Za-z0-9_]* 	{yylval.string = new std::string(yytext, yyleng); return ID;}

	\n				    {yylineno++;yycolumn=1;}

	[ \t]                /* Ignore whitespace */
}

<COMMENTB>{
   "*/"      BEGIN(INITIAL);
    [^*\n]+   // eat comment in chunks
	"*"       // eat the lone star
	\n        yylineno++;
}

<COMMENTL>{
	[^\n]+	
	\n		{yylineno++;BEGIN(INITIAL);}
}

<STRESC>{
	\"			{	BEGIN(INITIAL);
					*string_buf_ptr='\0';
					SAVE_STRING;
					return STR;
				}

	\n			{
					log.add(Msg(yylineno,"Unterminated string", 2));
					lexerror=true;
					yylineno++;yycolumn=1;
				}

	\\n			*string_buf_ptr++ = '\n';
	\\v			*string_buf_ptr++ = '\v';
	\\t			*string_buf_ptr++ = '\t';
	\\r			*string_buf_ptr++ = '\r';
	\\b			*string_buf_ptr++ = '\b';
	\\f			*string_buf_ptr++ = '\f';
	\\"			*string_buf_ptr++ = '"';

	[^\\\n\"]+	{	char *yptr = yytext;
					while ( *yptr )	
						*string_buf_ptr++ = *yptr++;
				}
}	 

<STRLIT>{
	\'			{	BEGIN(INITIAL);
					*string_buf_ptr='\0';
					SAVE_STRING;
					return STR;
				}

	\n			{
					log.add(Msg(yylineno,"Unterminated string", 2));
					lexerror=true;
					yylineno++;yycolumn=1;
				}

	[^\\\n\"]+	{	char *yptr = yytext;
					while ( *yptr )	
						*string_buf_ptr++ = *yptr++;
				}
}

<CHR>{
    \n                  {log.add(Msg(yylineno,"Unexpected newline",2));
							lexerror=true;
                            yycolumn = 1;}

    \\n					{yylval.Char = '\n';
                            BEGIN(INITIAL);
                            return CHAR;}
	\\v					{yylval.Char = '\v';
                            BEGIN(INITIAL);
                            return CHAR;}
	\\t					{yylval.Char = '\t';
                            BEGIN(INITIAL);
                            return CHAR;}
	\\r					{yylval.Char = '\r';
                            BEGIN(INITIAL);
                            return CHAR;}
	\\b					{yylval.Char = '\b';
                            BEGIN(INITIAL);
                            return CHAR;}
	\\f					{yylval.Char = '\f';
                            BEGIN(INITIAL);
                            return CHAR;}
    \\s                 {yylval.Char = ' ';
                            BEGIN(INITIAL);
                            return CHAR;}

	.                   {yylval.Char = yytext[0];
                            BEGIN(INITIAL);
                            return CHAR;}

}
