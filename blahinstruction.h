#ifndef BLAHINST
#define BLAHINST
#include <list>
#include "blahsymtable.h"
#include "regdescriptor.h"
#include <sstream>
#include <iostream>

struct Operand;

class Inst{
	public:
		enum OP {
			ADD,SUB,MUL,DIV,MOD,UMINUS,CALL,PARAM,
			FLT2INT,INT2FLT,
			LINDEX,RINDEX,RETURN,COPY,GOTO,IFEQ,IFNEQ,IFLT,
			IFLEQ,IFGT,IFGEQ,PROLOGUE,EPILOGUE,RETRIEVE,EXIT,
			LD,ST
		};

		std::string comment;
		Operand* result;
		Operand* arg1;
		Operand* arg2;
		OP op;
		bool bleader;
		std::list<std::string> labels;
		virtual void print(std::ostream&)=0;
		virtual int isJump();
		virtual Inst* getJumpDest();
		virtual bool operator==(const Inst& b) const;
		Inst(OP op, Operand* arg1,Operand* arg2,Operand* result,std::string comment=""):bleader(false),comment(comment),result(result),arg1(arg1),arg2(arg2),op(op){};

};


struct Operand{
	enum TYPE{
		INT,BOOL,CHAR,FLOAT,VAR,FUNC,LABEL,REG
	}type;
	union VAL{
		int integer;
		char character;
		bool boolean;
		float floating;
		Inst* label;
		TVar *var;
		TFunc *func;
		RegDescriptor* reg;
	} value;
	
	bool onFloat;

	Operand(char val):type(CHAR),onFloat(false){value.character=val;}
	Operand(bool val):type(BOOL),onFloat(false){value.boolean=val;}
	Operand(TVar* val):type(VAR){value.var=val;onFloat=val->type.name=="Float";}
	Operand(TFunc* val):type(FUNC){value.func=val;onFloat=val->type.name=="Float";}
	Operand(float val):type(FLOAT),onFloat(true){value.floating=val;}
	Operand(Inst* i):type(LABEL),onFloat(false){value.label=i;}
	Operand(int val):type(INT),onFloat(false){value.integer = val;}
	Operand(RegDescriptor* val):type(REG),onFloat(val->isfloat){value.reg = val;}

	public:
	std::string toStr();
};


class Quad:public Inst{
	public:
	Quad(Inst::OP op, Operand* arg1,Operand* arg2,Operand* result,std::string comment=""):Inst(op,arg1,arg2,result,comment){};
	void print(std::ostream &os);

};


#endif
