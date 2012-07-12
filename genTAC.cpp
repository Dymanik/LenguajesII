#include "blahast.h"
#include "blahlog.h"
extern Log log;

using namespace std;
Operand* newtemp(TType* type){
	static int t=1;
	stringstream st;
	st<<"T"<<t++<<type->name;
	return new Operand(new TVar(st.str(),*type,false,true));
}

Operand* NRExpression::codeGen(IBlock* block){
	Operand* i = expr->codeGen(block);
	if(expr->isAccess){
		Operand* t = newtemp(type);
		block->temps.push_back(t->value.var);
		block->addinst(new Quad(Quad::RINDEX,new Operand(expr->base),i, t));
		if(type->name =="Bool"){
			Quad* t1= new Quad(Quad::IFEQ,t,new Operand(0),NULL);
			Quad* t2 = new Quad(Quad::GOTO,NULL,NULL,NULL);
			block->addinst(t1);
			block->addinst(t2);
			truelist.push_back(t2);
			falselist.push_back(t1);
			return NULL;
		} else {
			return t;
		}
	}else{
		truelist.splice(truelist.begin(),expr->truelist);
		falselist.splice(falselist.begin(),expr->falselist);
		return i;
	}
}

Operand* NIntToFloat::codeGen(IBlock* block){

	Operand* t = newtemp(type);
	block->temps.push_back(t->value.var);

	Operand* e = expr->codeGen(block);
	block->addinst(new Quad(Quad::INT2FLT,e,NULL,t,"convert Int to Float"));
	return t;
}

Operand* NFloatToInt::codeGen(IBlock* block){
	Operand* t = newtemp(type);
	block->temps.push_back(t->value.var);
	Operand* e = expr->codeGen(block);
	block->addinst(new Quad(Quad::FLT2INT,e,NULL,t,"convert Float to Int"));
	return t;

}

Operand* NInteger::codeGen(IBlock* block){
	return new Operand(value);
}


Operand* NFloat::codeGen(IBlock* block){
	return new Operand(value);
}

Operand* NChar::codeGen(IBlock* block){
	return new Operand(value);
}

Operand* NBool::codeGen(IBlock* block){
	Quad* t = new Quad(Quad::GOTO,NULL,NULL,NULL);
	if(value){
		truelist.push_back(t);
	}else{
		falselist.push_back(t);
	}
	block->addinst(t);
	return NULL;
}

Operand* NArray::codeGen(IBlock* block){
	
	std::cerr<<"Invalid NArray codeGen"<<std::cerr;
}

Operand* NStructAccess::codeGen(IBlock* block){
	Operand* l = lexpr->codeGen(block);
	Operand* off = new Operand(var->offset);
	base = lexpr->base;
	if(lexpr->isAccess){
		Operand* temp= newtemp(new TInteger());
		block->temps.push_back(temp->value.var);
		if(var->offset>0){
			block->addinst(new Quad(Quad::ADD,l,off,temp,"accumulacion por accesso a structura"));
			return temp;
		}else{
			return l;
		}
	}else{
		return off;
	}

}

Operand* NArrayAccess::codeGen(IBlock* block){
	Operand* l = lexpr->codeGen(block);
	Operand* i = index->codeGen(block);
	Operand* temp= newtemp(index->type);
	block->temps.push_back(temp->value.var);
	base = lexpr->base;
	block->addinst(new Quad(Quad::MUL,i,new Operand((int)type->size),temp));
	if(lexpr->isAccess){
		Operand* res = newtemp(index->type);
		block->temps.push_back(res->value.var);
		block->addinst(new Quad(Quad::ADD,l,temp,res));
		return res;
	}else{
		return temp;
	}
}


Operand* NVar::codeGen(IBlock* block){
	base = var;
	if(type->name =="Bool" && !islexpr){
		Operand* l = new Operand(var);
		Quad* t1=new Quad(Quad::IFEQ,l,new Operand(0),NULL);
		Quad* t2=new Quad(Quad::GOTO,NULL,NULL,NULL);
		block->addinst(t1);
		block->addinst(t2);
		truelist.push_back(t2);
		falselist.push_back(t1);
		return NULL;
	}else{
		return new Operand(var);	
	}
};

void NExpressionStatement::codeGen(IBlock* block){
	expr->codeGen(block);
}

void NUnionDeclaration::codeGen(IBlock* block){}
void NRegisterDeclaration::codeGen(IBlock* block){}


void NVariableDeclaration::codeGen(IBlock* block){
	if(assignment!=NULL){
		assignment->codeGen(block);
		nextlist.splice(nextlist.begin(),assignment->nextlist);
	}
}

void NFunctionDeclaration::codeGen(IBlock* blok){

	Quad* epi = new Quad(Quad::EPILOGUE,new Operand(func),NULL,NULL);
	epi->labels.push_front("__"+func->toStr());
	blok->addinst(epi);
	
	block->codeGen(blok);
	int end = blok->nextinstr;
	blok->addinst(new Quad(Quad::PROLOGUE,new Operand(func),NULL,NULL));
	blok->backpatch(block->nextlist,end);
	blok->backpatch(block->returnlist,end);

	for(int i=0;i<blok->temps.size();i++){
		blok->temps[i]->offset=func->stacksize;
		func->stacksize+=4;
	}
}


Operand* NFunctionCall::codeGen(IBlock* block){
	list<Operand*> list;
	for(int i=0;i<arguments.size();i++){
		list.push_front(arguments[i]->codeGen(block));
	}
	int len = arguments.size();
	Operand* n = new Operand(len);
	for(int i=0;i<arguments.size();i++){
		Operand* t = list.front();
		list.pop_front();
		block->addinst(new Quad(Quad::PARAM,t,NULL,NULL));
	}
	block->addinst(new Quad(Quad::CALL,new Operand(func),n,NULL));
	if(inExpr){
		Operand* temp = newtemp(type);
		block->temps.push_back(temp->value.var);
		block->addinst(new Quad(Quad::RETRIEVE,NULL,NULL,temp));
		if(type->name =="Bool"){
			Quad* t1=new Quad(Quad::IFEQ,temp,new Operand(0),NULL);
			Quad* t2=new Quad(Quad::GOTO,NULL,NULL,NULL);
			block->addinst(t1);
			block->addinst(t2);
			truelist.push_back(t2);
			falselist.push_back(t1);
			return NULL;
		}else{
			return temp;
		}
	}
}

Operand* NAritmeticBinaryOperator::codeGen(IBlock* block){
	
	Operand* l=lexp->codeGen(block);
	Operand* r=rexp->codeGen(block);

	Operand* res = newtemp(type);
	block->temps.push_back(res->value.var);
	Quad* t;
	switch(op){
		case ADD:
			t = new Quad(Quad::ADD,l,r,res,"Sum");
			break;
		case MUL:
			t = new Quad(Quad::MUL,l,r,res,"multiplication");
			break;
		case DIV:
			t = new Quad(Quad::DIV,l,r,res,"division");
			break;
		case SUB:
			t = new Quad(Quad::SUB,l,r,res,"subtraction");
			break;
		case MOD:
			t = new Quad(Quad::MOD,l,r,res,"modulo");
			break;
	}
	block->addinst(t);

	return res;
}

Operand* NBooleanBinaryOperator::codeGen(IBlock* block){
	Operand* l = lexp->codeGen(block);
	int t = block->nextinstr;
	Operand* r = rexp->codeGen(block);
	switch(op){
		case OR:
			block->backpatch(lexp->falselist,t);
			truelist.splice(truelist.begin(),lexp->truelist);
			truelist.splice(truelist.begin(),rexp->truelist);
			falselist.splice(falselist.begin(),rexp->falselist);
			break;
		case AND:
			block->backpatch(lexp->truelist,t);
			falselist.splice(falselist.begin(),lexp->falselist);
			falselist.splice(falselist.begin(),rexp->falselist);
			truelist.splice(truelist.begin(),rexp->truelist);
			break;
	}
	return NULL;
	
}

Operand* NAritmeticUnaryOperator::codeGen(IBlock* block){

	Operand* r=rexp->codeGen(block);

	Operand* res = newtemp(type);
	block->temps.push_back(res->value.var);
	Quad* t;
	switch(op){
		case UMINUS:
			t = new Quad(Quad::UMINUS,r,NULL,res);
			break;
	}
	block->addinst(t);

	return res;
}

Operand* NComparison::codeGen(IBlock* block){
/*
	Operand* l = lexp->codeGen(block);
	Operand* r = rexp->codeGen(block);
	Quad* t1=NULL;
	switch(op){
		case LT:
			t1 = new Quad(Quad::IFGEQ,l,r,NULL);
			break;
		case GT:
			t1 = new Quad(Quad::IFLEQ,l,r,NULL);
			break;
		case GEQ:
			t1 = new Quad(Quad::IFLT,l,r,NULL);
			break;
		case LEQ:
			t1 = new Quad(Quad::IFGT,l,r,NULL);
			break;
		case NEQ:
			t1 = new Quad(Quad::IFEQ,l,r,NULL);
			break;
		case EQ:
			t1 = new Quad(Quad::IFNEQ,l,r,NULL);
			break;

	}
	falselist.push_back(t1);
	block->addinst(t1);
	return NULL;
*/
	Operand* l = lexp->codeGen(block);
	Operand* r = rexp->codeGen(block);
	Quad* t1=NULL;
	Quad* t2=new Quad(Quad::GOTO,NULL,NULL,NULL);
	switch(op){
		case LT:
			t1 = new Quad(Quad::IFLT,l,r,NULL);
			break;
		case GT:
			t1 = new Quad(Quad::IFGT,l,r,NULL);
			break;
		case GEQ:
			t1 = new Quad(Quad::IFGEQ,l,r,NULL);
			break;
		case LEQ:
			t1 = new Quad(Quad::IFLEQ,l,r,NULL);
			break;
		case NEQ:
			t1 = new Quad(Quad::IFNEQ,l,r,NULL);
			break;
		case EQ:
			t1 = new Quad(Quad::IFEQ,l,r,NULL);
			break;

	}
	truelist.push_back(t1);
	falselist.push_back(t2);
	block->addinst(t1);
	block->addinst(t2);
	return NULL;
}

Operand* NBooleanUnaryOperator::codeGen(IBlock* block){
	switch(op){
		case NOT:
			truelist.splice(truelist.begin(),rexp->falselist);
			falselist.splice(falselist.begin(),rexp->truelist);
			break;
	}
	return NULL;
}


void NBlock::codeGen(IBlock * block){
	for(int i=0;i<statements.size();i++){
		int t=block->nextinstr;
		statements[i]->codeGen(block);
		if(i>0 && t!= block->nextinstr){
			block->backpatch(nextlist,t);
			nextlist.clear();
		}
		nextlist.splice(nextlist.begin(), statements[i]->nextlist);
		breaklist.splice(breaklist.begin(), statements[i]->breaklist);
		continuelist.splice(continuelist.begin(), statements[i]->continuelist);
		returnlist.splice(returnlist.begin(), statements[i]->returnlist);
	}
}

void NIf::codeGen(IBlock * blok){

	int next;
	cond->codeGen(blok);
	next = blok->nextinstr;
	block->codeGen(blok);
	blok->backpatch(cond->truelist,next);
	nextlist.splice(nextlist.begin(),block->nextlist);
	breaklist.splice(breaklist.begin(),block->breaklist);
	continuelist.splice(continuelist.begin(),block->continuelist);
	returnlist.splice(returnlist.begin(), block->returnlist);
	if(elseBlock==NULL){
		nextlist.splice(nextlist.begin(),cond->falselist);
	}else{
		Quad* t = new Quad(Quad::GOTO,NULL,NULL,NULL);
		nextlist.push_back(t);
		blok->addinst(t);
		next = blok->nextinstr;
		elseBlock->codeGen(blok);
		blok->backpatch(cond->falselist,next);
		nextlist.splice(nextlist.begin(),elseBlock->nextlist);
		breaklist.splice(breaklist.begin(),elseBlock->breaklist);
		continuelist.splice(continuelist.begin(),elseBlock->continuelist);
		returnlist.splice(returnlist.begin(), elseBlock->returnlist);
	}

}


void NWhileDo::codeGen(IBlock * blok){
	int t1,t2;
	t1 = blok->nextinstr;
	cond->codeGen(blok);
	t2 = blok->nextinstr;
	block->codeGen(blok);
	Quad* inst = new Quad(Quad::GOTO,NULL,NULL,NULL,"Restart Loop"); 
	blok->addinst(inst);
	block->continuelist.push_back(inst);
	blok->backpatch(block->continuelist,t1);
	blok->backpatch(cond->truelist,t2);
	blok->backpatch(block->nextlist,t1);
	nextlist.splice(nextlist.begin(),cond->falselist);
	nextlist.splice(nextlist.begin(),block->breaklist);
	returnlist.splice(returnlist.begin(), block->returnlist);
}

void NDoWhile::codeGen(IBlock * blok){
	int t1,t2,t3;
	t1 = blok->nextinstr;
	block->codeGen(blok);
	t2 = blok->nextinstr;
	cond->codeGen(blok);
	blok->backpatch(block->nextlist,t2);
	blok->backpatch(block->continuelist,t2);
	blok->backpatch(cond->truelist,t1);
	nextlist.splice(nextlist.begin(),cond->falselist);
	nextlist.splice(nextlist.begin(),block->breaklist);
	returnlist.splice(returnlist.begin(), block->returnlist);
}

void NForRange::codeGen(IBlock* blok){

	Operand* b = beg->codeGen(blok);
	Operand* e = end->codeGen(blok);
	Operand* st = step->codeGen(blok);
	Operand* v = new Operand(var);


	blok->addinst(new Quad(Quad::COPY,b,0,v,"For initialization"));
	int start = blok->nextinstr;

	Quad* t1 = new Quad(Quad::IFLT,st,new Operand(0),NULL,"Check if we are going up or down");
	Quad* t2 = new Quad(Quad::IFGT,v,e,NULL,"(positive step) check if we passed the upper bound");
	Quad* t3 = new Quad(Quad::GOTO,NULL,NULL,NULL,"Jump into the for loop");
	Quad* t4 = new Quad(Quad::IFLT,v,e,NULL,"(negative step) check if we passed the lower bound");

	blok->addinst(t1);
	blok->addinst(t2);
	blok->addinst(t3);
	int lbchk = blok->nextinstr;
	blok->addinst(t4);

	nextlist.push_back(t1);
	blok->backpatch(nextlist,lbchk);
	nextlist.clear();
	nextlist.push_back(t3);
	
	int stblk = blok->nextinstr;
	block->codeGen(blok);
	blok->backpatch(nextlist,stblk);
	nextlist.clear();

	int endblk = blok->nextinstr;
	blok->addinst(new Quad(Quad::ADD,v,st,v,"sum the step"));

	Quad* t5=new Quad(Quad::GOTO,NULL,NULL,new Operand(t1),"Restart Loop");
	blok->addinst(t5);
	blok->backpatch(block->continuelist,endblk);
	blok->backpatch(block->nextlist,endblk);
	
	nextlist.push_back(t2);
	nextlist.push_back(t4);
	nextlist.splice(nextlist.begin(),block->breaklist);
	returnlist.splice(returnlist.begin(), block->returnlist);

}


void NForVar::codeGen(IBlock* blok){
	cerr<<"For for Variables TAc Gen not implemented"<<endl;
}

void NStop::codeGen(IBlock* block){
	Quad* t = new Quad(Quad::GOTO,NULL,NULL,NULL,"Stop loop");
	breaklist.push_back(t);
	block->addinst(t);
}

void NNext::codeGen(IBlock* block){
	Quad* t = new Quad(Quad::GOTO,NULL,NULL,NULL,"Continue loop");
	continuelist.push_back(t);
	block->addinst(t);
}

void NReturn::codeGen(IBlock* block){
	Operand* ret = NULL;
	if(expr!=NULL){
		ret = expr->codeGen(block);
		int t= block->nextinstr;
		Quad* inst = new Quad(Quad::RETURN,NULL,NULL,NULL,"Function return");
		block->addinst(inst);
		if(expr->type->name=="Bool"){			
			if(!expr->truelist.empty()){
				block->backpatch(expr->truelist,t);
				ret =new Operand(1);
			}
			if(!expr->falselist.empty()){
				block->backpatch(expr->falselist,t);
				ret =new Operand(0);
			}
		}
		inst->arg1=ret;
	}
	Quad* t = new Quad(Quad::GOTO,NULL,NULL,NULL,"return: jump to epilogue");
	block->addinst(t);
	returnlist.push_back(t);
}


void NAssignment::codeGen(IBlock* block){
	Operand* l = var->codeGen(block);
	Operand* r = assignment->codeGen(block);

	if(assignment->type->name=="Bool"){
		bool tru=false;
		if(!assignment->truelist.empty()){
			int t = block->nextinstr;
			if(!var->isAccess){
				block->addinst(new Quad(Quad::COPY,new Operand(1),NULL,l,"True assignment"));
			} else {
				block->addinst(new Quad(Quad::LINDEX,new Operand(1),l,new Operand(var->base),"True Assignment"));
			}
			block->backpatch(assignment->truelist,t);
			tru=true;
		}
		if(!assignment->falselist.empty()){
			if(tru){
				Quad* inst= new Quad(Quad::GOTO,NULL,NULL,NULL);
				block->addinst(inst);
				nextlist.push_back(inst);
			}
			int t = block->nextinstr;
			if(!var->isAccess){
			block->addinst(new Quad(Quad::COPY,new Operand(0),NULL,l,"False assignment"));
			} else {
				block->addinst(new Quad(Quad::LINDEX,new Operand(1),l,new Operand(var->base),"False Assignment"));
			}
			block->backpatch(assignment->falselist,t);
		}

	}else{
		if(!var->isAccess){
			block->addinst(new Quad(Quad::COPY,r,NULL,l, "Assignment"));
		}else{
			block->addinst(new Quad(Quad::LINDEX,r,l,new Operand(var->base)));
		}
	}
}
