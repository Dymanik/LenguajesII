#ifndef BLAHBLOCK
#define BLAHBLOCK
#include "blahinstruction.h"
#include <list>

class IBlock{
	public:
		std::string label;
		bool flag;
		std::vector <TVar*> temps;
		int nextinstr;
		std::list <Inst*> instructions;
		std::vector<IBlock*> blocks;
		std::list <IBlock*> pred;
		std::list <IBlock*> succ;
		IBlock():nextinstr(0),flag(false),label(""){}
		IBlock(std::list<Inst*> instructions):nextinstr(0),flag(false),label(""){}
		void basicBlocks();
		void backpatch(std::list<Inst*> ,int);
		void addinst(Inst* i,bool s=true);
		void print(std::ostream &os);
		void printInst(std::ostream &os);
		void printFlow(std::ostream &os);
		void addSucc(IBlock*);	
		void addPred(IBlock*);	
};




#endif
