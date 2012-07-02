#ifndef BLAHBLOCK
#define BLAHBLOCK
#include "blahinstruction.h"
#include <list>

class IBlock{
	
	public:
		int nextinstr;
		std::list <Inst*> instructions;
		std::list <IBlock*> pred;
		std::list <IBlock*> succ;
		IBlock():nextinstr(0){}
		void backpatch(std::list<Inst*> ,int);
		void addinst(Inst* i);
		void print(std::ostream &os);
};




#endif
