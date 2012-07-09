#ifndef BLAHTAC
#define BLAHTAC
#include "blahinstruction.h"
#include "blahblock.h"
#include "blahsymtable.h"
#include "blahast.h"
#include <vector>

class TAC{

	public:
	std::vector<IBlock*> functionblocks;
	std::vector<TVar*> global_vars;
	TAC(){};
	TAC(NBlock* b){ASTtoTAC(b);};

	bool ASTtoTAC(NBlock*);

	void print(std::ostream&);

};

#endif
