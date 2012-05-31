#ifndef BLAHBLOCK
#define BLAHBLOCK
#include "blahinstruction.h"
#include <list>

class IBlock{
	
	public:
		int nextinstr;
		std::list <Inst*> instructions;
		IBlock():nextinstr(0){}
		int nextInstruction();
		void addinst(Inst* i);

		void print(std::ostream &os);
};




#endif
