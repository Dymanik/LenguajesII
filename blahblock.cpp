#include "blahblock.h"
#include <sstream>

using namespace std;

string newlabel(){
	static int num;
	stringstream lab;
	lab << "L" << num++;
	return lab.str();
}

void IBlock::addinst(Inst* i){

	nextinstr++;
	i->labels.push_back(newlabel());
	instructions.push_back(i);

};
/*
void IBlock::backpatch(list<int> l,int v){
	list<int>::iterator it;
	Quad* q = NULL;
	Operand* o= NULL;
	for(it=l.begin();it!=l.end();it++){
		list<Inst*>::iterator t;
		t = instructions.begin();
		advance(t,*it);
		q = (Quad*) *t;
		o = new Operand(v,true);
		q->result = o;
	}
}
*/
void IBlock::backpatch(list<Inst*> l,int v){
	list<Inst*>::iterator it;
	it=instructions.begin();
	advance(it,v);
	Operand* o= new Operand(*it);
	Quad* q = NULL;
	for(it=l.begin();it!=l.end();it++){
		q = (Quad*) *it;
		q->result = o;
	}

}

void IBlock::print(std::ostream &os){

	os<<"TACblock"<<endl;
	list<Inst*>::iterator it;
	for(it=instructions.begin();it!=instructions.end();it++){
		(*it)->print(os);
	}

}




IBlock* IBlock::flowgraph(){

	vector<Inst*> leaders;
	list<Inst*>::iterator li;
	int n=1;
	bool nextleader=true;
	for(li=instructions.begin();li!=instructions.end();li++){
		if(nextleader){
			(*li)->bleader=true;
			nextleader=false;
		}
		if((*li)->op==Inst::EPILOGUE){
			(*li)->bleader=true;
		}

		switch((*li)->isJump()){
			case 2:
				nextleader=true;
			case 1:
				(*li)->getJumpDest()->bleader=true;
				break;
			default:
				break;
		}
	}
	
	unordered_map<string,IBlock*> labBlock;
	vector<IBlock*> blocks;

}


