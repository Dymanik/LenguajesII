#ifndef BLAHBLOCK
#define BLAHBLOCK
#include "blahinstruction.h"
#include <list>

class IBlock{
	
	public:
		int blocknum;
		int nextinstr;
		std::list <Inst*> instructions;
		std::list <IBlock*> pred;
		std::list <IBlock*> succ;
		IBlock():nextinstr(0){}
		IBlock(std::list<Inst*> instructions):nextinstr(0){}
		IBlock* flowgraph();
		void backpatch(std::list<Inst*> ,int);
		void addinst(Inst* i);
		void print(std::ostream &os);
};




#endif
