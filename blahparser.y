%{
#include "blahlog.h"
#include "blahast.h"
#include "blahsymtable.h"
#include <cstdarg>
extern int yylex (void);
//void yyerror (char const *a){printf("ERROR: %s\n",a);};

void yyerror(char const *s, ...);

NBlock *programAST;
bool flagerror=false;
int flagfdecl=0;
Symtable table;
Log log;
int offset;

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
	Sizelist* sizes;
	Fields* fields;
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
%type	<exprvec> fun_call_args expr_lst simple_arr_lst complx_arr_lst
%type	<block>	program stmts block decls 
%type 	<fun_decl> fun_firm
%type   <const_arr> array complx_arr simple_arr
%type	<var_decl> var_decl varr_decl
%type	<stmt>	stmt decl fun_decl ctrl_for
%type	<stmt>	ctrl_while ctrl_if var_asgn reg_decl union_decl
%type	<type> type
%type	<sizes> int_arr
%type	<fields> fields

/* Matematical operators precedence */
%left	<token>	TOR
%left	<token>	TAND
%nonassoc <token>	TEQ TNEQ TGEQ TLEQ '<' '>'	
%left	<token>	'+' '-' 
%left	<token> '*' '/' '%'
%left 	<token> NEG TNOT
%left	<token> ACCESS

%locations
%error-verbose
%start program
%%


program		: decls			{programAST=$1;}
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

var_decl	: type ID			{
								TVar *v = new TVar(*$2,*$1);
								table.insert(v);
								$$=new NVariableDeclaration(v);
								}
			| type ID '=' expr	{
								TVar *v = new TVar(*$2,*$1);
								table.insert(v);
								$$=new NVariableDeclaration(v,new NAssignment(new NVar(v),$4));
								}

			| type ID '=' array	{
								TVar *v = new TVar(*$2,*$1);
								table.insert(v);
								$$=new NVariableDeclaration(v,new NAssignment(new NVar(v),$4));
								}
			;

fun_decl	: fun_firm block	{$1->block=$2;table.endScope();}
			;


union_decl	: UNION TYPEID beg_block fields end_block	{$$= new NUnionDeclaration(*$2,*$4);table.insert(new TUnion(*$2,*$4));}
			;


reg_decl	: REGISTER TYPEID beg_block fields end_block	{$$ = new NRegisterDeclaration(*$2,*$4);table.insert(new TRegister(*$2,*$4));}
			;

fields		: type ID				{$$ = new Fields();$$->push_back(std::make_pair(*$2,new TVar(*$2,*$1)));}
			| fields ',' type ID	{$1->push_back(std::make_pair(*$4,new TVar(*$4,*$3)));}

var_decls 	: var_decl					{$$=new VariableList();$$->push_back($1);}	
			| var_decls ',' var_decl	{$1->push_back($3);}
			;


fun_firm	: type ID fun_decl_args		{$$=new NFunctionDeclaration($1,*$2,*$3);
											std::vector<TType*> argtypes;
											for(int i=0;i<$3->size();i++){
												argtypes.push_back(&((*$3)[i]->var->type));
											}
										TFunc* t = new TFunc(*$2,*$1,argtypes);
										$$->func=t;
										table.insert(t);}
			;

fun_decl_args	: fun_scope ')'				{$$ = new VariableList();}
				| fun_scope var_decls ')'	{$$=$2;}
				;

fun_scope	: '('	{table.begFuncScope();}

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
												if ($$->isArr){
												}
											}else{
												$$ = new TError();
											}
										}
			| STRING '[' INT ']'	{$$ = new TArray(table.lookupType("Char"),$3);}
			;

int_arr		: INT				{$$ = new Sizelist();$$->push_back($1);}
			| int_arr ',' INT	{$1->push_back($3);}
			;

expr		: lrexpr		{$$ = new NRExpression($1);}
			| CHAR			{$$=new NChar($1);}
			| fun_call		{$$=$1; ((NFunctionCall*) $$)->inExpr = true;} 
			| bool_expr
			| arit_expr
			| '(' expr ')'	{$$=$2;}
			;

arit_expr	: INT					{$$ = new NInteger($1);}
			| FLOAT					{$$ = new NFloat($1);}
			| expr '+' expr			{$$ = new NAritmeticBinaryOperator($1,NAritmeticBinaryOperator::ADD,$3);}
			| expr '-' expr			{$$ = new NAritmeticBinaryOperator($1,NAritmeticBinaryOperator::SUB,$3);}
			| expr '*' expr			{$$ = new NAritmeticBinaryOperator($1,NAritmeticBinaryOperator::MUL,$3);}
			| expr '/' expr			{$$ = new NAritmeticBinaryOperator($1,NAritmeticBinaryOperator::DIV,$3);}
			| expr '%' expr			{$$ = new NAritmeticBinaryOperator($1,NAritmeticBinaryOperator::MOD,$3);}
			| '-' expr %prec NEG	{$$ = new NAritmeticUnaryOperator(NAritmeticUnaryOperator::UMINUS,$2);}
			;

bool_expr	: TRUE					{$$ = new NBool(true);}
			| FALSE					{$$ = new NBool(false);}
			| expr TAND expr			{$$ = new NBooleanBinaryOperator($1,NBooleanBinaryOperator::AND,$3);}
			| expr TOR expr			{$$ = new NBooleanBinaryOperator($1,NBooleanBinaryOperator::OR,$3);}
			| '!' expr %prec TNOT	{$$= new NBooleanUnaryOperator(NBooleanUnaryOperator::NOT,$2);}
			| comparison
			;

comparison	: expr '<' expr	{$$ = new NComparison($1,NComparison::LT,$3);}
			| expr '>' expr	{$$ = new NComparison($1,NComparison::GT,$3);}
			| expr TGEQ expr	{$$ = new NComparison($1,NComparison::GEQ,$3);}
			| expr TLEQ expr	{$$ = new NComparison($1,NComparison::LEQ,$3);}
			| expr TNEQ expr	{$$ = new NComparison($1,NComparison::NEQ,$3);}
			| expr TEQ expr	{$$ = new NComparison($1,NComparison::EQ,$3);}
			;

lrexpr		: ident 				{$$ = $1;}
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

complx_arr	: '[' simple_arr_lst ']'	{$$ = new NArray(*$2);}
			| '[' complx_arr_lst ']'	{$$ = new NArray(*$2);}
			;

array		: simple_arr
			| complx_arr
			;

simple_arr_lst	: simple_arr					{$$= new ExpressionList(); $$->push_back($1);}
				| simple_arr_lst ',' simple_arr {$1->push_back($3);}
				;

complx_arr_lst	: complx_arr					{$$= new ExpressionList(); $$->push_back($1);}
				| complx_arr_lst ',' complx_arr {$1->push_back($3);}
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
			| RETURN expr '.'	{$$ = new NReturn($2);}
			| RETURN '.'		{$$ = new NReturn();}
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

for			: FOR {table.begScope();}

ctrl_for	: for ID FROM expr TO expr block			{
														TVar *v = new TVar(*$2,*(new TInteger()));
														table.insert(v);
														$$=new NForRange(v,$4,$6,$7);
														table.endScope();
														}
			| for ID FROM expr TO expr STEP expr block	{
														TVar *v = new TVar(*$2,*(new TInteger()));
														table.insert(v);
														$$=new NForRange(v,$4,$6,$9,$8);
														table.endScope();
														}
			| for ID IN ident block						{
														if($4->type->isArr){
															TArray* t = (TArray*)$4->type;
															TVar *v = new TVar(*$2,*t->type);
															table.insert(v);
															$$=new NForVar(v,$4,$5);
															table.endScope();
														}else{
															flagerror=true;
														}
														}
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
