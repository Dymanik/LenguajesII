#include "tac.h"
#include "spim.h"
#include "regdescriptor.h"

using namespace std;

SPIM::SPIM(TAC* tac){
	global_vars=tac->global_vars;
	functionblocks = tac->functionblocks;
	for(int i=0;i<tac->functionblocks.size();i++){
		tac2spim(functionblocks[i]);
	}
	
};


class SPIMRegisters{

	public:
	vector<RegDescriptor*> regs;
	vector<RegDescriptor*> coregs;

	SPIMRegisters(){
		for(int i =2; i<23;i++){
			stringstream name;
			name<<"$"<<i;
			regs.push_back(new RegDescriptor(name.str(),false));
		}
		for(int i =0; i<31;i=i+2){
			stringstream name;
			name<<"$f"<<i;
			coregs.push_back(new RegDescriptor(name.str(),true));
		}
	}


};

void instructions2spim(IBlock*,SPIMRegisters&);

void SPIM::tac2spim(IBlock* block){

	SPIMRegisters regs;
	for(int i =0; i<block->blocks.size();i++){
		instructions2spim(block->blocks[i],regs);
	}
}

void instructions2spim(IBlock* b,SPIMRegisters& regs){

	list<Inst*> spimlist;

	Inst* inst;
	while(!b->instructions.empty()){
		inst = b->instructions.front();
		b->instructions.pop_front();
		Operand* a1=NULL,*a2=NULL,*res=NULL;
		bool onFloat=false;
		bool store=true;
//		cout<<"<<<";
//		inst->print(cout);


//		cout<<">>>";
		switch(inst->op){

			case Inst::CALL:
			case Inst::GOTO:
			case Inst::EXIT:
			case Inst::PROLOGUE:
			case Inst::EPILOGUE:
			case Inst::RETRIEVE:
			{

				SpimInst *i = new SpimInst(false,inst->op,inst->arg1,inst->arg2,inst->result,inst->comment);
				i->labels = inst->labels;
				spimlist.push_back(i);
//				spimlist.back()->print(cout);
				break;
			}
			case Inst::RINDEX:
				a1 = new Operand(regs.regs[1]);
				a2 = new Operand(regs.regs[2]);
				res = new Operand(regs.regs[0]);
				spimlist.push_back(new SpimInst(false,SpimInst::LD,NULL,inst->arg2,a2));
				spimlist.push_back(new SpimInst(false,SpimInst::LD,NULL,inst->arg1,a1));
				spimlist.push_back(new SpimInst(false,SpimInst::ADD,a1,a2,a1));
				spimlist.push_back(new SpimInst(false,SpimInst::LD,NULL,a1,a1));
				spimlist.push_back(new SpimInst(false,SpimInst::ST,NULL,a1,inst->result));
					
				break;
			case Inst::LINDEX:
				a1 = new Operand(regs.regs[1]);
				a2 = new Operand(regs.regs[2]);
				res = new Operand(regs.regs[0]);
				spimlist.push_back(new SpimInst(false,SpimInst::LD,NULL,inst->arg1,res));
				spimlist.push_back(new SpimInst(false,SpimInst::LD,NULL,inst->arg2,a2));
				spimlist.push_back(new SpimInst(false,SpimInst::LD,NULL,inst->result,a1));
				spimlist.push_back(new SpimInst(false,SpimInst::ADD,a1,a2,a1));
				spimlist.push_back(new SpimInst(false,SpimInst::ST,NULL,res,a1));

				break;
			case Inst::IFLEQ:
			case Inst::IFGEQ:
			case Inst::IFEQ:
			case Inst::IFNEQ:
			case Inst::IFGT:
			case Inst::IFLT:
			case Inst::RETURN:
			case Inst::PARAM:
				store=false;
			default:
				if(inst->arg1!=NULL){
					onFloat= onFloat||inst->arg1->onFloat;
					if(inst->arg1->type==Operand::LABEL ||inst->arg1->type==Operand::FUNC ){
						a1= inst->arg1;
					}else{
						if(inst->arg1->onFloat){
							a1 = new Operand(regs.coregs[1]);
							spimlist.push_back(new SpimInst(true,SpimInst::LD,NULL,inst->arg1,a1));
						}else{
							a1 = new Operand(regs.regs[1]);
							spimlist.push_back(new SpimInst(false,SpimInst::LD,NULL,inst->arg1,a1));
						}
//						spimlist.back()->print(cout);
					}
				}
				if(inst->arg2!=NULL){
					onFloat= onFloat||inst->arg2->onFloat;
					if(inst->arg2->type==Operand::LABEL ||inst->arg2->type==Operand::FUNC ){
						a2= inst->arg2;
					}else{
						if(inst->arg2->onFloat){
							a2 = new Operand(regs.coregs[2]);
							spimlist.push_back(new SpimInst(true,SpimInst::LD,NULL,inst->arg2,a2));
						}else{
							a2 = new Operand(regs.regs[2]);
							spimlist.push_back(new SpimInst(false,SpimInst::LD,NULL,inst->arg2,a2));
						}
//						spimlist.back()->print(cout);
					}
				}

				
				if(inst->result!=NULL){
					onFloat= onFloat||inst->result->onFloat;
					if(inst->result->type==Operand::LABEL ||inst->result->type==Operand::FUNC ){
						res= inst->result;
					}else{
						int i =0;
						if(inst->op==Inst::COPY){
							i = 1;
						}
						if(inst->result->onFloat){
							
							res = new Operand(regs.coregs[i]);

						}else{
							res = new Operand(regs.regs[i]);
						}
					}
				}

				spimlist.push_back(new SpimInst(onFloat,inst->op,a1,a2,res,inst->comment));
//				spimlist.back()->print(cout);
				if(store){
					spimlist.push_back(new SpimInst(onFloat,SpimInst::ST,NULL,res,inst->result));
//					spimlist.back()->print(cout);
				}
		}
	}
	b->instructions=spimlist;
}
