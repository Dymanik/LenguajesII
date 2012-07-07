#ifndef BLAHINST
#define BLAHINST
#include <list>
#include "blahsymtable.h"
#include <sstream>
#include <iostream>

struct Operand;

class Inst{
	public:
		enum OP {ADD,SUB,MUL,DIV,MOD,UMINUS,CALL,PARAM,
			FLT2INT,INT2FLT,
			LINDEX,RINDEX,RETURN,COPY,GOTO,IFEQ,IFNEQ,IFLT,
			IFLEQ,IFGT,IFGEQ,PROLOGUE,EPILOGUE,RETRIEVE
		};

		std::string comment;
		Operand* result;
		Operand* arg1;
		Operand* arg2;
		OP op;
		bool bleader;
		std::list<std::string> labels;
		virtual void print(std::ostream&)=0;
		virtual int isJump()=0;
		virtual Inst* getJumpDest()=0;
		virtual bool operator==(const Inst& b) const=0;
		Inst(OP op, Operand* arg1,Operand* arg2,Operand* result,std::string comment=""):bleader(false),comment(comment),result(result),arg1(arg1),arg2(arg2),op(op){};

};


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
		Inst* label;
		TVar *var;
		TFunc *func;
	} value;

	Operand(char val):type(CHAR){value.character=val;}
	Operand(bool val):type(BOOL){value.boolean=val;}
	Operand(TVar* val):type(VAR){value.var=val;}
	Operand(TFunc* val):type(FUNC){value.func=val;}
	Operand(float val):type(FLOAT){value.floating=val;}
	Operand(Inst* i):type(LABEL){value.label=i;}
	Operand(int val,bool integer=true){
		if(integer){
			type=INT;
			value.integer = val;
		}else{
			type=TEMP;
			value.temp=val;
		}
	}

	public:
	std::string toStr();
};


class Quad:public Inst{
	public:
	Quad(OP op, Operand* arg1,Operand* arg2,Operand* result,std::string comment=""):Inst(op,arg1,arg2,result,comment){};
	int isJump();
	Inst* getJumpDest();
	bool operator==(const Inst &b) const;
	void print(std::ostream &os);

};


#endif
