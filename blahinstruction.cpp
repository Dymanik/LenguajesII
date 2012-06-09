#include "blahinstruction.h"
#include <sstream>

using namespace std;
string Operand::toStr(){
	stringstream os;
	switch(type){
		case TEMP:
			os<<"T"<<value.temp;
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
		case LABEL:
			os<<"LABEL:"<<value.label;
			break;
	}
	return os.str();
}

void Quad::print(std::ostream &cout){
	
	unsigned n=-1;
	if(n!=label){
		cout<<label<<":";
	}
	cout<<"\t";
	switch(op){
		case ADD:
			cout<<result->toStr()<<" := "<< arg1->toStr()<<" + "<<arg2->toStr();
			break;
		case SUB:
			cout<<result->toStr()<<" := "<< arg1->toStr()<<" - "<<arg2->toStr();
			break;
		case MUL:
			cout<<result->toStr()<<" := "<< arg1->toStr()<<" * "<<arg2->toStr();
			break;
		case DIV:
			cout<<result->toStr()<<" := "<< arg1->toStr()<<" / "<<arg2->toStr();
			break;
		case MOD:
			cout<<result->toStr()<<" := "<< arg1->toStr()<<" % "<<arg2->toStr();
			break;
		
		case UMINUS:
			cout<<result->toStr()<<" := -"<< arg1->toStr();
			break;
		case COPY:
			cout<<result->toStr()<<" := "<< arg1->toStr();
			break;
		case GOTO:
			cout<<"goto "<<result->toStr();
			break;
		case IFEQ:
			cout<<"if "<<arg1->toStr()<<" == "<< arg2->toStr()<<" goto "<< result->toStr();
			break;
		case IFNEQ:
			cout<<"if "<<arg1->toStr()<<" != "<< arg2->toStr()<<" goto "<< result->toStr();
			break;
		case IFLT:
			cout<<"if "<<arg1->toStr()<<" < "<< arg2->toStr()<<" goto "<< result->toStr();
			break;
		case IFGT:
			cout<<"if "<<arg1->toStr()<<" > "<< arg2->toStr()<<" goto "<< result->toStr();
			break;
		case IFLEQ:
			cout<<"if "<<arg1->toStr()<<" <= "<< arg2->toStr()<<" goto "<< result->toStr();
			break;
		case IFGEQ:
			cout<<"if "<<arg1->toStr()<<" >= "<< arg2->toStr()<<" goto "<< result->toStr();
			break;
		case CALL:
			cout<<"call "<< result->toStr();
			break;
		case RETURN:
			cout<<"return";
			if(arg1!=NULL){
				cout<<arg1->toStr();
			}
			break;
	}
	if(comment.size()>0)cout<<"\t#"<<comment;
	cout<<endl;
}
