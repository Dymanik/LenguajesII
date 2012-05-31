#ifndef BLAHAST
#define BlAHAST
#include "blahsymtable.h"
#include "blahlog.h"
#include "blahinstruction.h"
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
		std::list<int> truelist;
		std::list<int> falselist;
		NExpression(TType* type=new TUndef(),bool constant=true):type(type),constant(constant){}
		
		virtual Operand* codeGen(IBlock* block){
			std::cerr<<"NOTIMPLEMENTED"<<std::endl;	
		}
};

class NLRExpression : public NExpression{
	public:
		bool islexpr;
		NLRExpression(TType* type,bool islexpr=false):NExpression(type),islexpr(islexpr){}
};

class NStatement : public Node {
	public:
	virtual void codeGen(IBlock* block){};
};

class NExpressionStatement : public NStatement {
	public:
		NExpression* expr;
		NExpressionStatement(NExpression* expr): expr(expr){}
		void print(std::ostream& os,int depth=0);
};

class NInteger : public NExpression {
	public:
		int value;
		NInteger(int value):NExpression(new TInteger()),value(value){}
		TType* typeChk(Symtable t ,TType* exp=NULL);
		
		Operand* codeGen(IBlock* block);

		void print(std::ostream& os,int depth=0);
};

class NFloat : public NExpression {
	public:
		float value;
		NFloat(float value):NExpression(new TFloat()),value(value){}
		TType* typeChk(Symtable,TType* t=NULL);
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
		NChar(char value):NExpression(new TChar()),value(value){}
		TType* typeChk(Symtable,TType* t=NULL);
		Operand* codeGen(IBlock* block);

		void print(std::ostream& os,int depth=0);
};

class NBool : public NExpression {
	public:
		bool value;
		NBool(bool value): NExpression(new TBool()),value(value){}
		TType* typeChk(Symtable,TType* t=NULL);
		Operand* codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);
};

class NArray : public NExpression {
	public:
		ExpressionList values;
		NArray(ExpressionList values):NExpression(values[0]->type),values(values){}
		TType* typeChk(Symtable,TType* t=NULL);
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
		NArrayAccess(NLRExpression *lexpr, NExpression *index):lexpr(lexpr),index(index),NLRExpression(lexpr->type){}
		TType* typeChk(Symtable,TType* t=NULL);
		void print(std::ostream& os,int depth=0);

};

class NStructAccess : public NLRExpression{
	public:
		NLRExpression *lexpr;
		std::string name;
		NStructAccess(NLRExpression *lexpr,std::string name):lexpr(lexpr),name(name),NLRExpression(NULL){}
		TType* typeChk(Symtable,TType* t=NULL);
		void print(std::ostream& os,int depth=0);
};

class NFunctionCall : public NExpression {
	public:
		std::string name;
		ExpressionList arguments;
		NFunctionCall(std::string name, ExpressionList arguments) : name(name), arguments(arguments){}
		TType* typeChk(Symtable,TType* t=NULL);
		void print(std::ostream& os,int depth=0);
		
};

class NAritmeticBinaryOperator : public NExpression{
	public:
		std::string op;
		NExpression *lexp;
		NExpression *rexp;
		NAritmeticBinaryOperator(NExpression* lexp,std::string op,NExpression* rexp):op(op),lexp(lexp),rexp(rexp){}
		TType* typeChk(Symtable,TType* t=NULL);
		Operand* codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);

};

class NBooleanBinaryOperator :public NExpression {
	public:
		std::string op;
		NExpression *lexp;
		NExpression *rexp;
		NBooleanBinaryOperator(NExpression* lexp,std::string op,NExpression* rexp):op(op),lexp(lexp),rexp(rexp){}
		TType* typeChk(Symtable,TType* t=NULL);
//		Operand* codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);

};

class NComparison : public NExpression {
	public:
		std::string op;
		NExpression *lexp;
		NExpression *rexp;
		NComparison(NExpression* lexp,std::string op,NExpression* rexp):op(op),lexp(lexp),rexp(rexp){}
		TType* typeChk(Symtable,TType* t=NULL);
		void print(std::ostream& os,int depth=0);
};

class NAritmeticUnaryOperator : public NExpression {
	public:
		std::string op;
		NExpression *rexp;
		NAritmeticUnaryOperator(std::string op,NExpression* rexp):op(op),rexp(rexp){}
		TType* typeChk(Symtable,TType*t=NULL);
		Operand* codeGen(IBlock* block);
		void print(std::ostream& os,int depth=0);
		
};

class NBooleanUnaryOperator : public NExpression {
	public:
		std::string op;
		NExpression *rexp;
		NBooleanUnaryOperator(std::string op,NExpression* rexp):op(op),rexp(rexp){}
		TType* typeChk(Symtable,TType*t=NULL);
		void print(std::ostream& os,int depth=0);
		
};

class NBlock : public NStatement{
	public:
		StatementList statements;
		NBlock() {}
		TType* typeChk(Symtable,TType*t=NULL);
		void codeGen(IBlock* );
		void print(std::ostream& os,int depth=0);

};

class NVariableDeclaration : public NStatement {
	public:
		TVar* var;
		NExpression *assignment;
		NVariableDeclaration(TVar* var,NExpression *assignment=NULL): var(var),assignment(assignment){};
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
		NFunctionDeclaration(TType* type,std::string name,VariableList arguments,NBlock* block=NULL):name(name),type(type),block(block),arguments(arguments){} 
		TType* typeChk(Symtable,TType*t=NULL);
		void print(std::ostream& os,int depth=0);
};

class NRegisterDeclaration : public NStatement {
	public:
		std::string name;
		Fields fields;
		NRegisterDeclaration(std::string name,Fields fields):name(name),fields(fields){}
		void print(std::ostream& os,int depth=0);
			
};

class NUnionDeclaration : public NStatement {
	public:
		std::string name;
		Fields fields;
		NUnionDeclaration(std::string name,Fields fields):name(name),fields(fields){}
		void print(std::ostream& os,int depth=0);
			
};

class NWhileDo : public NStatement{
	public:
		NExpression* cond;
		NBlock* block;
		NWhileDo(NExpression* cond, NBlock* block):cond(cond),block(block){}
		TType* typeChk(Symtable,TType*t=NULL);
		void print(std::ostream& os,int depth=0);
};

class NDoWhile : public NStatement{
	public:
		NExpression* cond;
		NBlock* block;
		NDoWhile(NExpression* cond, NBlock* block):cond(cond),block(block){}
		TType* typeChk(Symtable,TType*t=NULL);
		void print(std::ostream& os,int depth=0);
};

class NIf : public NStatement{
	public:
		NExpression* cond;
		NStatement* block;
		NStatement* elseBlock;
		
		NIf(NExpression* cond,NStatement* block, NStatement* elseBlock=NULL):cond(cond),block(block),elseBlock(elseBlock){}
		TType* typeChk(Symtable,TType*t=NULL);
		void print(std::ostream& os,int depth=0);

};

class NForRange : public NStatement{
	public:
		TVar* var;
		NExpression* beg;
		NExpression* end;
		NExpression* step;
		NBlock* block;
		NForRange(TVar* var,NExpression* beg,NExpression* end, NBlock* block, NExpression* step=NULL):var(var),beg(beg),end(end),block(block){
			if (this->step==NULL){
				this->step = new NInteger(1);
			}
		}
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
		TType* typeChk(Symtable,TType*t=NULL);
		void print(std::ostream& os,int depth=0);
};

class NStop :public NStatement {
	public:
		NStop():NStatement(){};
		void print(std::ostream& os,int depth=0);

};


class NNext : public NStatement{
	public:
		void print(std::ostream& os,int depth=0);

};

class NReturn : public NStatement{
	public:
		NExpression* expr;
		NReturn(NExpression* expr=NULL):expr(expr){}
		TType* typeChk(Symtable,TType*t=NULL);
		void print(std::ostream& os,int depth=0);
};

class NAssignment : public NStatement{
	public:
		NLRExpression* var;
		NExpression* assignment;
		NAssignment (NLRExpression* var,NExpression* assignment):var(var),assignment(assignment){}
		TType* typeChk(Symtable,TType* t=NULL) ;
		void codeGen(IBlock*);
		void print(std::ostream& os,int depth=0);
};

#endif
