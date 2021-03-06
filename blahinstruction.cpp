#include "blahinstruction.h"
#include <sstream>

using namespace std;
string Operand::toStr(){
	stringstream os;
	switch(type){
		case INT:
			os<<value.integer;
			break;
		case CHAR:
			os<<value.character;
			break;
		case BOOL:
			if(value.boolean){
				os<<"true";
			}else{
				os<<"false";
			}
			break;
		case FLOAT:
			os<<fixed<<value.floating;
			break;
		case VAR:
			os<<value.var->name<<value.var->scope;
			break;
		case FUNC:
			os<<value.func->toStr();
			break;
		case LABEL:
			os<<value.label->labels.front();
			break;
		case REG:
			os<<value.reg->name;
			break;
		default:
			os<<INT<<BOOL<<CHAR<<FLOAT<<VAR<<FUNC<<LABEL;
			os<<">"<<type<<"<";
			os<<"unkown type";
			break;
	}
	return os.str();
}

void Quad::print(std::ostream &os){


	if(labels.size()>1){
		list<string>::iterator it;
		it=labels.begin();
		os<<(*it)<<":";
		it++;
/*		while(it!=labels.end()){
			os<<"\n"<<(*it)<<":";
			it++;
		}
*/	}
	bool undef =false;
	bool jlabel =false;
	os<<"\t";
	switch(op){
		case ADD:
			os<<result->toStr()<<" := "<< arg1->toStr()<<" + "<<arg2->toStr();
			break;
		case SUB:
			os<<result->toStr()<<" := "<< arg1->toStr()<<" - "<<arg2->toStr();
			break;
		case MUL:
			os<<result->toStr()<<" := "<< arg1->toStr()<<" * "<<arg2->toStr();
			break;
		case DIV:
			os<<result->toStr()<<" := "<< arg1->toStr()<<" / "<<arg2->toStr();
			break;
		case MOD:
			os<<result->toStr()<<" := "<< arg1->toStr()<<" % "<<arg2->toStr();
			break;

		case UMINUS:
			os<<result->toStr()<<" := -"<< arg1->toStr();
			break;
		case INT2FLT:
			os<<result->toStr()<<" := INT2FLT(" << arg1->toStr()<<")";
			break;
		case FLT2INT:
			os<<result->toStr()<<" := FLT2INT(" << arg1->toStr()<<")";
			break;

		case COPY:
			os<<result->toStr()<<" := "<< arg1->toStr();
			break;
		case LINDEX:
			os<<result->toStr()<<"["<<arg2->toStr()<<"] = "<<arg1->toStr();
			break;
		case RINDEX:
			os<<result->toStr()<<" = "<<arg1->toStr()<<"["<<arg2->toStr()<<"]";
			break;
		case GOTO:
			os<<"goto ";
			if(result){
				jlabel=true;
			}else{
				undef = true;
			}
			break;
		case IFEQ:
			os<<"if "<<arg1->toStr()<<" == "<< arg2->toStr()<<" goto ";
			if(result){
				jlabel=true;
			}else{
				undef = true;
			}
			break;
		case IFNEQ:
			os<<"if "<<arg1->toStr()<<" != "<< arg2->toStr()<<" goto ";
			if(result){
				jlabel=true;
			}else{
				undef = true;
			}
			break;
		case IFLT:
			os<<"if "<<arg1->toStr()<<" < "<< arg2->toStr()<<" goto ";
			if(result){
				jlabel=true;
			}else{
				undef = true;
			}
			break;
		case IFGT:
			os<<"if "<<arg1->toStr()<<" > "<< arg2->toStr()<<" goto ";
			if(result){
				jlabel=true;
			}else{
				undef = true;
			}
			break;
		case IFLEQ:
			os<<"if "<<arg1->toStr()<<" <= "<< arg2->toStr()<<" goto ";
			if(result){
				jlabel=true;
			}else{
				undef = true;
			}
			break;
		case IFGEQ:
			os<<"if "<<arg1->toStr()<<" >= "<< arg2->toStr()<<" goto ";
			if(result){
				jlabel=true;
			}else{
				undef = true;
			}
			break;
		case PARAM:
			os<<"param "<<arg1->toStr();
			break;
		case RETRIEVE:
			os<<"retrieve "<<result->toStr();
			break;
		case CALL:
			os<<"call "<< arg1->toStr()<<","<<arg2->toStr();
			break;
		case RETURN:
			os<<"return ";
			if(arg1!=NULL){
				os<<arg1->toStr();
			}
			break;
		case EPILOGUE:
			os<<"epilogue "<<arg1->toStr();
			break;
		case PROLOGUE:
			os<<"prologue "<<arg1->toStr();
			break;
		case EXIT:
			os<<"exit";
			break;
		default:
			os<<"print no definido";
			break;

	}
	if(undef){
		os<<"undef";
	}else if(jlabel){
		os<<result->toStr();
	}
	if(comment.size()>0) os<<"\t#"<<comment;
	os<<endl;
}



int Inst::isJump(){
	switch(op){
		case GOTO:
			return 1;
		case IFLEQ:
		case IFGT:
		case IFEQ:
		case IFNEQ:
		case IFLT:
		case IFGEQ:
			return 2;
		default:
			return 0;
	}
}

Inst* Inst::getJumpDest(){
	switch(op){
		case GOTO:
		case IFLEQ:
		case IFGT:
		case IFEQ:
		case IFNEQ:
		case IFLT:
		case IFGEQ:
			return result->value.label;
		default:
			return NULL;
	}

}

bool Inst::operator==(const Inst &b) const{
	return op == b.op && arg1==b.arg1 && arg2==b.arg2 && result==b.result;
}
