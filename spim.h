#ifndef BLAHSPIM
#define BLAHSPIM
#include "blahinstruction.h"
#include "blahblock.h"
#include "blahsymtable.h"
#include "blahast.h"
#include "tac.h"
#include <vector>

class SPIM{

	public:
	std::vector<IBlock*> functionblocks;
	std::vector<TVar*> global_vars;
	SPIM(){};
	SPIM(TAC* b){
		functionblocks=b->functionblocks;
		global_vars=b->global_vars;
	};

	void TAC2SPIM(TAC*);

	void printfuncts(std::ostream&);
	void print(std::ostream&);

};

class SpimInst:public Inst{
	public:
	SpimInst(OP op, Operand* arg1,Operand* arg2,Operand* result,std::string comment=""):Inst(op,arg1,arg2,result,comment){};
	void print(std::ostream &os);

};


#endif
