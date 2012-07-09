#include "tac.h"
#include "blahlog.h"
extern Log log;

using namespace std;
bool TAC::ASTtoTAC(NBlock* b){
	if(b->mainBlock){
		vector<NFunctionDeclaration*> functs;
		TFunc* main=NULL;
		IBlock* global=new IBlock();
		for(int i=0;i<b->statements.size();i++){
			if(b->statements[i]->fundecl){
				NFunctionDeclaration* t = (NFunctionDeclaration*) b->statements[i];
				if(t->name=="main"){
					if(main==NULL){
						main=t->func;
					}else{
						log.add(Msg(0,"There must be only one main()",3));
						return false;
					}
				}
				functs.push_back((NFunctionDeclaration*)b->statements[i]);
			}else if(b->statements[i]->vardecl){
				b->statements[i]->codeGen(global);
				global_vars.push_back(((NVariableDeclaration*)b->statements[i])->var);
			}else{
				b->statements[i]->codeGen(global);
			}
		}
		if(main==NULL){
			log.add(Msg(0,"There must be a main()",3));
			return false;
		}

		global->addinst(new Quad(Quad::CALL,new Operand(main),new Operand(0),NULL,"start de program"));
		global->addinst(new Quad(Quad::EXIT,NULL,NULL,NULL,"EXIT"));

		functionblocks.push_back(global);

		for(int i=0;i<functs.size();i++){
			IBlock* block = new IBlock();
			functs[i]->codeGen(block);
			functionblocks.push_back(block);
		}
	}	
	return true;
}


void TAC::print(std::ostream& os){

	for(int i =0;i<functionblocks.size();i++){
		functionblocks[i]->print(os);
	}

}

