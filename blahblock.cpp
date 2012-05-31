#include "blahblock.h"

void IBlock::addinst(Inst* i){
	
	i->label=nextInstruction();
	instructions.push_back(i);


};

int IBlock::nextInstruction(){
	    return nextinstr++;
}


void IBlock::print(std::ostream &os){

	std::cout<<"TACblock"<<std::endl;
	std::list<Inst*>::iterator it;
	for(it=instructions.begin();it!=instructions.end();it++){
		(*it)->print(std::cout);
	}

}
