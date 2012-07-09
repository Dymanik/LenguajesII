#include "spim.h"
using namespace std;



void SPIM::print(std::ostream& os){

	os<<".data"<<endl;

	for(int i = 0 ; i < global_vars.size();i++){
		TVar* v=global_vars[i];

		os<<v->name<<": ";
		switch(v->type.size){
			case 1:
				os<<".byte 0";
				break;
			case 4:
				os<<".word 0";
				break;
			default:
				os<<".space "<<v->type.size;
		}
		os<<endl;
	}

	os<<".text"<<endl;
	
	printfuncts(os);
	

	os<<"main:"<<endl;
	functionblocks[0]->print(os);
	os<<"\tli $v0, 10"<<endl;
	os<<"\tsyscall"<<endl;

	for(int i =1;i<functionblocks.size();i++){
		functionblocks[i]->print(os);
	}
}


void TAC2SPIM(TAC* tac){


}


void SPIM::printfuncts(ostream& os){
	os<<"__printInteger:"<<endl;
	os<<"\tli $v0, 1"<<endl;
	os<<"\tlw $a0, 0(sp)"<<endl;
	os<<"\tsyscall"<<endl;
	os<<"\tjr $ra"<<endl;

	os<<"__printFloat:"<<endl;
	os<<"\tl.s $f12, 0(sp)"<<endl;
	os<<"\tli $v0, 2"<<endl;
	os<<"\tsyscall"<<endl;
	os<<"\tjr $ra"<<endl;
}


