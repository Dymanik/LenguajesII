%{
#include "blahlog.h"
#include "blahast.h"
#include "blahsymtable.h"
#include <cstdarg>
extern int yylex (void);
//void yyerror (char const *a){printf("ERROR: %s\n",a);};

void yyerror(char const *s, ...);

NBlock *ProgramAST;
bool flagerror=false;
int flagfdecl=0;
Symtable table;
Log log;
%}

%union{
	Node *node;
	NBlock *block;
	NExpression *expr;
	NStatement *stmt;
	NVar	*ident;
	NLRExpression *lrexpr;
	NVariableDeclaration *var_decl;
	NFunctionDeclaration *fun_decl;
	NArrayAccess *arr_access;
	NRegisterDeclaration *reg_decl;
	NUnionDeclaration *union_decl;
    NArray *const_arr;
	std::vector<NVariableDeclaration*> *varvec;
	std::vector<NExpression*> *exprvec;
	std::string *string;
	sizelist* sizes;
	TType* type;
	int token;
	long long integer;
	double floating;
	char Char;
    void * error;

}

/* Terminal Symbols */

%token  <error> error
%token  <string> STR
%token	<integer> INT
%token	<floating> FLOAT
%token 	<token> IF THEN ELSE FROM TO IN NEXT STOP
%token	<token>	CHAR UNION ARRAY TRUE FALSE STRING
%token 	<token> REGISTER DO WHILE RETURN FOR STEP 
%token	<string> ID TYPEID

/* Type of node our nonterminal represent */

%type	<expr>	expr fun_call bool_expr arit_expr comparison
%type 	<lrexpr> lrexpr
%type	<ident>	ident
%type	<varvec> fun_decl_args var_decls 
%type	<exprvec> fun_call_args expr_lst simple_arr_lst
%type	<block>	program stmts block decls 
%type 	<fun_decl> fun_firm
%type   <const_arr> array complx_arr simple_arr
%type	<var_decl> var_decl varr_decl
%type	<stmt>	stmt decl fun_decl ctrl_for
%type	<stmt>	ctrl_while ctrl_if var_asgn reg_decl union_decl
%type	<type> type
%type	<sizes> int_arr

/* Matematical operators precedence */
%nonassoc <token>	EQ NEQ GEQ LEQ '<' '>'	
%left	<token>	'+' '-' AND OR
%left	<token> '*' '/'
%left 	<token> NEG NOT
%left	<token> ACCESS

%locations

%start program
%%

program		: decls
			;

decls		: decl			{$$=new NBlock();$$->statements.push_back($1);}
			| decls decl	{$1->statements.push_back($2);}
			;

decl		: varr_decl '.' {$$=$1;}
			| reg_decl
			| union_decl
			| fun_decl
			;

varr_decl	: var_decl
			;

var_decl	: type ID			{$$=new NVariableDeclaration($1,*$2);table.insert(new TVar(*$2,*$1));}
			| type ID '=' expr	{$$=new NVariableDeclaration($1,*$2,$4);table.insert(new TVar(*$2,*$1));}
			| type ID '=' array	{$$=new NVariableDeclaration($1,*$2,$4);table.insert(new TVar(*$2,*$1));}
			;

fun_decl	: fun_firm block	{$1->block=$2;}
			;


union_decl	: UNION TYPEID beg_block var_decls end_block	{$$= new NUnionDeclaration(*$2,*$4);}
			;


reg_decl	: REGISTER TYPEID beg_block var_decls end_block	{$$ = new NRegisterDeclaration(*$2,*$4);}
			;

var_decls 	: var_decl					{$$=new VariableList();$$->push_back($1);}	
			| var_decls ',' var_decl	{$1->push_back($3);}
			;


fun_firm	: type ID fun_decl_args		{$$=new NFunctionDeclaration($1,*$2,*$3);}
			;

fun_decl_args	: fun_scope ')'				{$$ = new VariableList();}
				| fun_scope var_decls ')'	{$$=$2;}
				;

fun_scope	: '('	{table.begScope();}

ident		: ID	{TVar* temp = table.lookupVar(*$1);
					if(temp==NULL){log.add(Msg(0,"Variable "+*$1+"no ha sido declarada",2));
						$$=NULL;
					}else{
					$$=new NVar(temp);
					}
					}

type		: TYPEID					{	TType* type = table.lookupType(*$1);
											if(type!=NULL){
												$$ = table.lookupType(*$1); 
											}else{
												$$ = new TError();
											}
										}
			| ARRAY '[' int_arr ']' TYPEID	{
											TType* type = table.lookupType(*$5);
											if(type!=NULL){
												$$ = new TArray(type,*$3); 
											}else{
												$$ = new TError();
											}
										}
			| STRING '[' INT ']'	{$$ = new TArray(table.lookupType("Char"),$3);}
			;

int_arr		: INT				{$$ = new sizelist();$$->push_back($1);}
			| int_arr ',' INT	{$1->push_back($3);}
			;

expr		: lrexpr		{$$ = $1;}
			| CHAR			{$$=new NChar($1);}
			| fun_call
			| bool_expr
			| arit_expr
			| '(' expr ')'	{$$=$2;}
			;

arit_expr	: INT					{$$ = new NInteger($1);}
			| FLOAT					{$$ = new NFloat($1);}
			| expr '+' expr			{$$ = new NAritmeticBinaryOperator($1,"+",$3);}
			| expr '-' expr			{$$ = new NAritmeticBinaryOperator($1,"-",$3);}
			| expr '*' expr			{$$ = new NAritmeticBinaryOperator($1,"*",$3);}
			| expr '/' expr			{$$ = new NAritmeticBinaryOperator($1,"/",$3);}
			| '-' expr %prec NEG	{$$ = new NAritmeticUnaryOperator("-",$2);}
			;

bool_expr	: TRUE					{$$ = new NBool(true);}
			| FALSE					{$$ = new NBool(false);}
			| expr AND expr			{$$ = new NBooleanBinaryOperator($1,"AND",$3);}
			| expr OR expr			{$$ = new NBooleanBinaryOperator($1,"OR",$3);}
			| '!' expr %prec NOT	{$$= new NBooleanUnaryOperator("!",$2);}
			| comparison
			;

comparison	: expr '<' expr	{$$ = new NComparison($1,"LT",$3);}
			| expr '>' expr	{$$ = new NComparison($1,"GT",$3);}
			| expr GEQ expr	{$$ = new NComparison($1,"GEQ",$3);}
			| expr LEQ expr	{$$ = new NComparison($1,"LEQ",$3);}
			| expr NEQ expr	{$$ = new NComparison($1,"NEQ",$3);}
			| expr EQ expr	{$$ = new NComparison($1,"EQ",$3);}
			;

lrexpr		: ident					{$$=$1;}
			| lrexpr '[' expr ']'	{$$ = new NArrayAccess($1,$3);}
			| lrexpr ACCESS ID		{$$ = new NStructAccess($1,*$3);}
			;

fun_call_args : '(' ')'			{$$ = new ExpressionList();}
			| '(' expr_lst ')'	{$$ = $2;}
			;

expr_lst	: expr				{$$= new ExpressionList(); $$->push_back($1);}
			| expr_lst ',' expr	{$1->push_back($3);}
			;

simple_arr	: '[' expr_lst ']'	{$$ = new NArray(*$2);}
			;

complx_arr	: '[' simple_arr_lst ']'	{}
			| '[' complx_arr_lst ']'		{}
			;

array		: simple_arr
			| complx_arr
			;

simple_arr_lst	: simple_arr					{$$= new ExpressionList(); $$->push_back($1);}
				| simple_arr_lst ',' simple_arr {$1->push_back($3);}
				;

complx_arr_lst	: complx_arr
				| complx_arr_lst ',' complx_arr
				;

block		: beg_block end_block {$$ = new NBlock();}
			| beg_block stmts end_block {$$ = $2;}
			;

beg_block	: '{'
			;

end_block	: '}'
			;

stmts		: stmt			{$$ = new NBlock(); $$->statements.push_back($1);} 
			| stmts stmt	{ $1 -> statements.push_back($2);}
			;

stmt		: ctrl_if
			| ctrl_while
			| ctrl_for
			| block				{$$ = $1;}
			| var_asgn '.'
			| varr_decl '.'		{$$ = $1;}
			| fun_call '.'		{$$ = new NExpressionStatement($1);}	
			| RETURN '.'		{$$ = new NReturn();}
			| RETURN expr '.'	{$$ = new NReturn($2);}
			| STOP '.'			{$$ = new NStop();}
			| NEXT '.'			{$$ = new NNext();}
			;

fun_call	: ID fun_call_args	{$$ = new NFunctionCall(*$1,*$2);}
			;

ctrl_if		: IF expr THEN block				{$$=new NIf($2,$4);}
			| IF expr THEN block ELSE block		{$$=new NIf($2,$4,$6);}	
			| IF expr THEN block ELSE ctrl_if	{$$=new NIf($2,$4,$6);}
			;

ctrl_while	: WHILE expr DO block		{$$=new NWhileDo($2,$4);}
			| DO block WHILE expr '.'	{$$=new NDoWhile($4,$2);}
			;

ctrl_for	: FOR ID FROM expr TO expr block			{$$=new NForRange(*$2,$4,$6,$7);}
			| FOR ID FROM expr TO expr STEP expr block	{$$=new NForRange(*$2,$4,$6,$9,$8);}
			| FOR ID IN ident block						{$$=new NForVar(*$2,$4,$5);}
			| FOR ID IN  array block					{$$=new NForArray(*$2,$4,$5);}
			;


var_asgn	: lrexpr '=' expr	{$$= new NAssignment($1,$3);}
			;

%%


void yyerror(char const *s, ...){
  va_list ap;
  va_start(ap, s);

  if(yylloc.first_line)
    fprintf(stderr, "\nSyntax error in line %d", yylloc.first_line, yylloc.first_column,
        yylloc.last_line, yylloc.last_column);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
  flagerror = 1;

}

void lyyerror(YYLTYPE t, char const *s, ...){
  va_list ap;
  va_start(ap, s);

  if(t.first_line)
    fprintf(stderr, "\n%d.%d-%d.%d: error: ", t.first_line, t.first_column,
        t.last_line, t.last_column);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}
