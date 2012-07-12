#ifndef BLAHSPIM
#define BLAHSPIM
#include "instructions.h"


class Spim:public Inst{
	public:
	Quad(OP op, Operand* arg1,Operand* arg2,Operand* result,std::string comment=""):Inst(op,arg1,arg2,result,comment){};
	int isJump();
	Inst* getJumpDest();
	bool operator==(const Inst &b) const;
	void print(std::ostream &os);

};
