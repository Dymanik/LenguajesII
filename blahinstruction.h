#ifndef BLAHINST
#define BLAHINST
#include <list>
#include "blahsymtable.h"
#include <iostream>


struct Operand{
	enum TYPE{
		TEMP,INT,BOOL,CHAR,FLOAT,VAR,FUNC,LABEL
	}type;
	union VAL{
		int temp;
		int integer;
		char character;
		bool boolean;
		float floating;
		int label;
		TVar *var;
		TFunc *func;
	} value;

	Operand(char val):type(CHAR){value.character=val;}
	Operand(int val):type(INT){value.integer=val;}
	Operand(bool val):type(BOOL){value.boolean=val;}
	Operand(TVar* val):type(VAR){value.var=val;}
	Operand(TFunc* val):type(FUNC){value.func=val;}
	Operand(float val):type(FLOAT){value.floating=val;}
	Operand(int val,bool lab){
		if(lab){
			type=LABEL;
			value.label = val;
		}else{
			type=TEMP;
			value.temp=val;
		}
	}

	public:
	std::string toStr();
};


class Inst{
	public:
	unsigned label;
	virtual void print(std::ostream&)=0;
	Inst():label(-1){};

};

class Quad:public Inst{
	public:
	enum OP {ADD,SUB,MUL,DIV,MOD,UMINUS,CALL,PARAM,
		RETURN,COPY,GOTO,IFEQ,IFNEQ,IFLT,IFLEQ,IFGT,IFGEQ 
	};

	std::string comment;
	Operand* result;
	Operand* arg1;
	Operand* arg2;
	OP op;
	Quad(OP op, Operand* arg1,Operand* arg2,Operand* result,std::string comment=""):Inst(),comment(comment),result(result),arg1(arg1),arg2(arg2),op(op){};

	void print(std::ostream &os);

};


#endif
