#include "blahinstruction.h"
#include <sstream>

using namespace std;
string Operand::toStr(){
	stringstream os;
	switch(type){
		case TEMP:
			if(value.temp==0){
				os<<"Base"<<endl;
			}else{
				os<<"T"<<value.temp;
			}
			break;
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
			os<<value.floating;
			break;
		case VAR:
			os<<value.var->name;
			break;
		case FUNC:
			os<<"FUNC: "<<value.func->name;
			break;
		case LABEL:
			os<<"LABEL:"<<value.label->labels.front();
			break;
	}
	return os.str();
}

void Quad::print(std::ostream &os){
	
	if(!labels.empty()){
		string str;
		while(labels.size()>1){
			str = labels.front();
			labels.pop_front();
			os<<str<<":"<<endl;

		}
		str = labels.front();
		labels.pop_front();
		os<<str<<":";
	}
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
			os<<"call "<< arg1->toStr()<<",",arg2->toStr();
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
