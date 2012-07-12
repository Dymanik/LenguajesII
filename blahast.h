#ifndef BLAHAST
#define BLAHAST
#include "blahsymtable.h"
#include "blahlog.h"
#include "blahblock.h"
#include <iostream>
#include <list>

class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;
typedef std::vector<TVar*> VarList;


class Node {
	public:
		virtual TType* typeChk(Symtable t ,TType* exp=NULL){return t.lookupType("Void");}
		virtual void print(std::ostream& os,int depth=0)=0;
		virtual ~Node() {}
};

class NExpression : public Node {
	public:
		TType* type;
		bool constant;
		std::list<Inst*> truelist;
		std::list<Inst*> falselist;
		
		NExpression(TType* type=new TUndef(),bool constant=false):type(type),constant(constant){}
		virtual NExpression* constantFold(){return this;}
		virtual Operand* codeGen(IBlock* block)=0;
};

class NLRExpression : public NExpression{
	public:
		bool islexpr;
		bool isAccess;
		TVar* base;
		NLRExpression(TType* type,bool isAccess=false,bool islexpr=false):NExpression(type),islexpr(islexpr),isAccess(isAccess){}
};

class NRExpression: public NExpression{
	public:
		NLRExpression* expr;
		bool constant;
		NRExpression(NLRExpression* expr,bool constant=false):NExpression(expr->type),expr(expr),constant(constant){}

		Operand* codeGen(IBlock* block);
		TType* typeChk(Symtable t ,TType* exp=NULL);
		void print(std::ostream& os,int depth=0);

};

class NStatement : public Node {
	public:
	bool fundecl;
	bool vardecl;
	std::list<Inst*> nextlist;
	std::list<Inst*> breaklist;
	std::list<Inst*> continuelist;
	std::list<Inst*> returnlist;

	NStatement():fundecl(false),vardecl(false){};
	virtual void codeGen(IBlock* block)=0;
};

class NExpressionStatement : public NStatement {
	public:
		NExpression* expr;
		NExpressionStatement(NExpression* expr): expr(expr){}
		void codeGen(IBlock* block);
		TType* typeChk(Symtable,TType*);
		void print(std::ostream& os,int depth=0);
};

class NInteger : public NExpression {
	public:
		int value;
		NInteger(int value):NExpression(new TInteger(),true),value(value){}
		TType* typeChk(Symtable t ,TType* exp=NULL);
		
		Operand* codeGen(IBlock* block);
		NExpression* constantFold(){return this;};

		void print(std::ostream& os,int depth=0);
};

class NFloat : public NExpression {
	public:
		float value;
		NFloat(float value):NExpression(new TFloat(),true),value(value){}
		TType* typeChk(Symtable,TType* t=NULL);
		Operand* codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);
};

class NFloatToInt : public NExpression {
	public:
		NExpression* expr;
		NFloatToInt(NExpression* expr):NExpression(new TInteger(),expr->constant),expr(expr){}
		TType* typeChk(Symtable,TType* t=NULL);
		NExpression* constantFold();
		Operand* codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);
};

class NIntToFloat : public NExpression {
	public:
		NExpression* expr;
		NIntToFloat(NExpression* expr):NExpression(new TFloat(),expr->constant),expr(expr){}
		TType* typeChk(Symtable,TType* t=NULL);
		NExpression* constantFold();
		Operand* codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);
};

class NString : public NExpression {
	public:
		std::string value;
		NString(std::string value):value(value),NExpression(new TArray((new TChar()),(int)value.length())){}
		TType* typeChk(Symtable,TType* t=NULL);
		void print(std::ostream& os,int depth=0);
};

class NChar : public NExpression {
	public:
		char value;
		NChar(char value):NExpression(new TChar(),true),value(value){}
		TType* typeChk(Symtable,TType* t=NULL);
		Operand* codeGen(IBlock* block);

		void print(std::ostream& os,int depth=0);
};

class NBool : public NExpression {
	public:
		bool value;
		NBool(bool value): NExpression(new TBool(),true),value(value){}
		TType* typeChk(Symtable,TType* t=NULL);
		Operand* codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);
};

class NArray : public NExpression {
	public:
		ExpressionList values;
		NArray(ExpressionList values):NExpression(values[0]->type),values(values){}
		TType* typeChk(Symtable,TType* t=NULL);
		Operand* codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);
};

class NVar : public NLRExpression{
	public:
		TVar* var;
		NVar(TVar* var,bool islexpr=false) : NLRExpression(&(var->type),islexpr),var(var){}
		TType* typeChk(Symtable,TType* t=NULL);
		Operand* codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);
		
};

class NArrayAccess : public NLRExpression{
	public:
		NLRExpression *lexpr;
		NExpression *index;
		NArrayAccess(NLRExpression *lexpr, NExpression *index):lexpr(lexpr),index(index),NLRExpression(lexpr->type,true){}
		TType* typeChk(Symtable,TType* t=NULL);
		Operand* codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);

};

class NStructAccess : public NLRExpression{
	public:
		NLRExpression *lexpr;
		std::string name;
		TVar* var;
		NStructAccess(NLRExpression *lexpr,std::string name):lexpr(lexpr),name(name),NLRExpression(NULL,true){}
		TType* typeChk(Symtable,TType* t=NULL);
		Operand* codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);
};

class NFunctionCall : public NExpression {
	public:
		std::string name;
		ExpressionList arguments;
		TFunc* func;
		bool inExpr;
		NFunctionCall(std::string name, ExpressionList arguments) :inExpr(false), name(name), arguments(arguments){}
		TType* typeChk(Symtable,TType* t=NULL);
		Operand* codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);
		
};

class NAritmeticBinaryOperator : public NExpression{
	public:
		enum OP{
			ADD,SUB,DIV,MOD,MUL
		};
		OP op;
		NExpression *lexp;
		NExpression *rexp;
		NAritmeticBinaryOperator(NExpression* lexp,OP op,NExpression* rexp):op(op),lexp(lexp),rexp(rexp){}
		TType* typeChk(Symtable,TType* t=NULL);
		NExpression* constantFold();
		Operand* codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);

};

class NBooleanBinaryOperator :public NExpression {
	public:
		enum OP{
			OR,AND
		};
		OP op;
		NExpression *lexp;
		NExpression *rexp;
		NBooleanBinaryOperator(NExpression* lexp,OP op,NExpression* rexp):op(op),lexp(lexp),rexp(rexp){}
		TType* typeChk(Symtable,TType* t=NULL);
		NExpression* constantFold();
		Operand* codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);

};

class NComparison : public NExpression {
	public:
		enum OP{
			LT,GT,EQ,NEQ,GEQ,LEQ
		};
		OP op;
		NExpression *lexp;
		NExpression *rexp;
		NComparison(NExpression* lexp,OP op,NExpression* rexp):op(op),lexp(lexp),rexp(rexp){}
		TType* typeChk(Symtable,TType* t=NULL);
		NExpression* constantFold();
		Operand* codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);
};

class NAritmeticUnaryOperator : public NExpression {
	public:
		enum OP{
			UMINUS
		};
		OP op;
		NExpression *rexp;
		NAritmeticUnaryOperator(OP op,NExpression* rexp):op(op),rexp(rexp){}
		TType* typeChk(Symtable,TType*t=NULL);
		Operand* codeGen(IBlock* block);
		NExpression* constantFold();
		void print(std::ostream& os,int depth=0);
		
};

class NBooleanUnaryOperator : public NExpression {
	public:
		enum OP{
			NOT
		};
		OP op;
		NExpression *rexp;
		NBooleanUnaryOperator(OP op,NExpression* rexp):op(op),rexp(rexp){}
		TType* typeChk(Symtable,TType*t=NULL);
		Operand* codeGen(IBlock* block);
		NExpression* constantFold();
		void print(std::ostream& os,int depth=0);
		
};



class NBlock : public NStatement{
	public:
		bool mainBlock;
		StatementList statements;
		NBlock():mainBlock(false) {}
		TType* typeChk(Symtable,TType*t=NULL);
		void codeGen(IBlock* );
		void print(std::ostream& os,int depth=0);

};

class NAssignment : public NStatement{
	public:
		NLRExpression* var;
		NExpression* assignment;
		NAssignment (NLRExpression* var,NExpression* assignment):var(var),assignment(assignment){var->islexpr=true;}
		TType* typeChk(Symtable,TType* t=NULL) ;
		void codeGen(IBlock*);
		void print(std::ostream& os,int depth=0);
};

class NVariableDeclaration : public NStatement {
	public:
		TVar* var;
		NAssignment *assignment;
		NVariableDeclaration(TVar* var,NAssignment *assignment=NULL): var(var),assignment(assignment){vardecl=true;};
		TType* typeChk(Symtable,TType*t=NULL);
		void codeGen(IBlock*);
		void print(std::ostream& os,int depth=0);

};

class NFunctionDeclaration : public NStatement {
	public:
		std::string name;
		TType* type;
		NBlock *block;
		VariableList arguments;
		TFunc* func;
		NFunctionDeclaration(TType* type,std::string name,VariableList arguments,NBlock* block=NULL):name(name),type(type),block(block),arguments(arguments){fundecl=true;} 
		TType* typeChk(Symtable,TType*t=NULL);
		void codeGen(IBlock*);
		void print(std::ostream& os,int depth=0);
};

class NRegisterDeclaration : public NStatement {
	public:
		std::string name;
		Fields fields;
		NRegisterDeclaration(std::string name,Fields fields):name(name),fields(fields){}
		void codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);
			
};

class NUnionDeclaration : public NStatement {
	public:
		std::string name;
		Fields fields;
		NUnionDeclaration(std::string name,Fields fields):name(name),fields(fields){}
		void codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);
			
};

class NWhileDo : public NStatement{
	public:
		NExpression* cond;
		NBlock* block;
		NWhileDo(NExpression* cond, NBlock* block):cond(cond),block(block){}
		TType* typeChk(Symtable,TType*t=NULL);
		void codeGen(IBlock*);
		void print(std::ostream& os,int depth=0);
};

class NDoWhile : public NStatement{
	public:
		NExpression* cond;
		NBlock* block;
		NDoWhile(NExpression* cond, NBlock* block):cond(cond),block(block){}
		TType* typeChk(Symtable,TType*t=NULL);
		void codeGen(IBlock*);
		void print(std::ostream& os,int depth=0);
};

class NIf : public NStatement{
	public:
		NExpression* cond;
		NStatement* block;
		NStatement* elseBlock;
		
		NIf(NExpression* cond,NStatement* block, NStatement* elseBlock=NULL):cond(cond),block(block),elseBlock(elseBlock){}
		TType* typeChk(Symtable,TType*t=NULL);
		void codeGen(IBlock*);
		void print(std::ostream& os,int depth=0);

};

class NForRange : public NStatement{
	public:
		TVar* var;
		NExpression* beg;
		NExpression* end;
		NExpression* step;
		NBlock* block;
		NForRange(TVar* var,NExpression* beg,NExpression* end, NBlock* block, NExpression* step=NULL):var(var),beg(beg),end(end),block(block),step(step){
		if (this->step==NULL){
				this->step = new NInteger(1);
			}
		}
		void codeGen(IBlock*);
		TType* typeChk(Symtable,TType*t=NULL);
		void print(std::ostream& os,int depth=0);
};

class NForArray : public NStatement{
	public:
		TVar* var;
		NArray* array;
		NBlock* block;
		NForArray(TVar* var, NArray* array,NBlock* block):var(var),array(array),block(block){}
		TType* typeChk(Symtable,TType*t=NULL);
		void print(std::ostream& os,int depth=0);
};

class NForVar : public NStatement{
	public:
		TVar* var;
		NVar* array;
		NBlock* block;
		NForVar(TVar* var,NVar* array,NBlock* block):var(var),array(array),block(block){}
		void codeGen(IBlock*);
		TType* typeChk(Symtable,TType*t=NULL);
		void print(std::ostream& os,int depth=0);
};

class NStop :public NStatement {
	public:
		NStop():NStatement(){};
		void print(std::ostream& os,int depth=0);
		void codeGen(IBlock*);

};


class NNext : public NStatement{
	public:
		void print(std::ostream& os,int depth=0);
		void codeGen(IBlock*);

};

class NReturn : public NStatement{
	public:
		NExpression* expr;
		NReturn(NExpression* expr=NULL):expr(expr){}
		TType* typeChk(Symtable,TType*t=NULL);
		void codeGen(IBlock*);
		void print(std::ostream& os,int depth=0);
};


#endif
