#include "blahsymtable.h"

void Tuple::print(std::ostream& os,int depth)const {
	os<<std::string(2*depth,' ')<<"("<<id<<","<<scope<<")";
}

void TType::print(std::ostream& os,int depth){
	os<<std::string(2*depth,' ')<<name<<": size="<<size<<std::endl;
}

void TStructured::print(std::ostream& os,int depth){
	os<<std::string(2*depth,' ')<<name<<": size="<<size<<" fields={"<<std::endl;
	std::unordered_map<std::string,TVar*>::iterator it;
	for(it=fields.begin();it!=fields.end();it++){
		it->second->print(os,depth+1);
	}
	os<<std::string(2*depth,' ')<<"}"<<std::endl;
}

void TVar::print(std::ostream& os,int depth){
	os<<std::string(2*depth,' ')<<name<<" type="<<type.name<<" offset="<<offset<<std::endl;
}

void TFunc::print(std::ostream& os, int depth){
	os<<std::string(2*depth,' ')<<name<<" returntype="<<type.name;
	os<<" argtypes=(";
	for(int i=0;i<args.size();i++){
		os<<args[i]->name<<",";
	}
	os<<")"<<std::endl;
}


void Symtable::printVars(std::ostream& os){
	os<<"-VARIABLES-"<<std::endl;
	std::unordered_map<Tuple,TVar*>::iterator it;
	for(it=vars.begin();it!=vars.end();it++){
		it->first.print(os,1);
		it->second->print(os,1);
	}
}

void Symtable::printFuncs(std::ostream& os){
	os<<"-FUNCTIONS-"<<std::endl;
	std::unordered_map<std::string,TFunc*>::iterator it;
	for(it=functions.begin();it!=functions.end();it++){
		it->second->print(os,1);
	}
}

void Symtable::printTypes(std::ostream& os){
	os<<"-TYPES-"<<std::endl;
	std::unordered_map<std::string,TType*>::iterator it;
	for(it=types.begin();it!=types.end();it++){
		it->second->print(os,1);
	}
}

void Symtable::print(std::ostream& os){
	printTypes(os);
	printVars(os);
	printFuncs(os);

}
