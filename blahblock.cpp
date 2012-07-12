#include "blahblock.h"
#include <sstream>


using namespace std;

string newlabel(){
	static int num=0;
	stringstream lab;
	lab << "L" << num++;
	return lab.str();
}

string newblocklabel(){
	static int num=0;
	stringstream lab;
	lab << "B" << num++;
	return lab.str();
}

void IBlock::addinst(Inst* i,bool lab){

	if(lab){
		nextinstr++;
		i->labels.push_back(newlabel());
	}
	instructions.push_back(i);

};

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

void IBlock::printInst(std::ostream &os){
	list<Inst*>::iterator it;
	for(it = instructions.begin();it!=instructions.end();it++){
		(*it)->print(os);
	}
}

void IBlock::print(std::ostream &os){

	if(blocks.empty()){
		cout<<"withoutblocks"<<endl;
		printInst(os);
	}else{
		for(int i=0;i<blocks.size();i++){
			if(!blocks[i]->pred.empty()){
				os<<blocks[i]->label<<":\n";
				blocks[i]->printInst(os);
			}
		}
	}

/*
	list<IBlock*> queue;
	list<IBlock*> blocks;
	list<IBlock*>::iterator it;
	queue.push_back(this);
	while(!queue.empty()){
		IBlock* b = queue.front();
		queue.pop_front();
		blocks.push_back(b);
		if(!b->flag){
			b->flag=true;
			if(b->label!=""){
				os<<b->label<<":\n";
			}
			b->printInst(os);
			for(it=b->succ.begin();it!=b->succ.end();it++){
				queue.push_back(*it);
			}
		}
	}

	for(it=blocks.begin();it!=blocks.end();it++){
		(*it)->flag=false;
	}
*/
}

void IBlock::printFlow(std::ostream &os){

	list<IBlock*> queue;
	list<IBlock*> blocks;
	list<IBlock*>::iterator it;
	queue.push_back(this);
	while(!queue.empty()){
		IBlock* b = queue.front();
		queue.pop_front();
		blocks.push_back(b);
		if(!b->flag){
			b->flag=true;
			os<<b->label<<"->{";
			for(it=b->succ.begin();it!=b->succ.end();it++){
				os<<(*it)->label;
				queue.push_back(*it);
			}
			os<<"}"<<endl;
		}
	}

	for(it=blocks.begin();it!=blocks.end();it++){
		(*it)->flag=false;
	}
}


void IBlock::basicBlocks(){

	vector<Inst*> leaders;
	list<Inst*>::iterator li;
	int n=1;
	bool nextleader=true;
	for(li=instructions.begin();li!=instructions.end();li++){
		if(nextleader){
			(*li)->bleader=true;
			nextleader=false;
		}
		if((*li)->op==Inst::CALL){
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


	unordered_map<string,IBlock*> lab2Blk;
	IBlock* newBlock = new IBlock();
	Inst* inst = instructions.front();
	inst->labels.pop_back();
	inst->labels.push_back(newblocklabel());
	newBlock->label=inst->labels.back();
	newBlock->addinst(inst,false);
	lab2Blk[newBlock->label]=newBlock;

	instructions.pop_front();

	while(!instructions.empty()){
		inst = instructions.front();
		instructions.pop_front();
		if(inst->bleader){
			blocks.push_back(newBlock);
			newBlock = new IBlock();
			newBlock->label=newblocklabel();
			inst->labels.pop_back();
			inst->labels.push_back(newBlock->label);
			lab2Blk[newBlock->label]=newBlock;
		}
		newBlock->addinst(inst,false);
	}

	blocks.push_back(newBlock);
	this->addSucc(blocks[0]);	

	for(int i=1;i<blocks.size();i++){

		switch(blocks[i-1]->instructions.back()->isJump()){
			case 2:
				blocks[i-1]->addSucc(blocks[i]);
			case 1:
				{
				Inst* dest = blocks[i-1]->instructions.back()->getJumpDest();	
				blocks[i-1]->addSucc(lab2Blk[dest->labels.back()]);
				}
				break;
			case 0:
				blocks[i]->addPred(blocks[i-1]);
				break;
		}
	}
	

}

void IBlock::addSucc(IBlock* b){
	succ.push_back(b);
	b->pred.push_back(this);
}

void IBlock::addPred(IBlock* b){
	pred.push_back(b);
	b->succ.push_back(this);
}
