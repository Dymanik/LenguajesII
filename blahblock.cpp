#include "blahblock.h"
#include <sstream>

using namespace std;
void IBlock::addinst(Inst* i){
	
	stringstream lab;
	lab << nextinstr++;
	i->labels.push_back(lab.str());
	instructions.push_back(i);

};

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

void IBlock::print(std::ostream &os){

	os<<"TACblock"<<endl;
	list<Inst*>::iterator it;
	for(it=instructions.begin();it!=instructions.end();it++){
		(*it)->print(os);
	}

}


