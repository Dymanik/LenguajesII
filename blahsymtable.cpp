#include "blahsymtable.h"

void TRegister::addField(TVar *typ, std::string name){
	fields[name]=typ;
	size+=typ->type.size;
}

void TUnion::addField(TVar *typ, std::string name){
	fields[name]=typ;
	if(size<typ->type.size){
		size=typ->type.size;
	}
}

TType* TStructured::accessType(std::string name){
	std::unordered_map<std::string,TVar*>::iterator it;
	it=fields.find(name);
	if(it==fields.end()){
		return NULL;
	}
	return &it->second->type;


}

/**insert functions,vars and types*/
void Symtable::insert(TType* t){
	types[t->name]=t;
}

void Symtable::insert(TVar* v){
	vars[std::pair<std::string,int>(v->name,scopeStack.front())]=v;
}

void Symtable::insert(TFunc* f){
	std::string key= f->name;
	for(int i=0; i<f->args.size(); i++){
		key += f->args[i]->name;
	}
	functions[key]=f;
}

/** lookups types, vars, and functions*/

TVar* Symtable::lookupLocalVar(const std::string name){
	std::unordered_map<std::pair<std::string,int>,TVar*>::iterator it;
	it=vars.find(make_pair(name,scopeStack.front()));
	if (it==vars.end()) return NULL;
	return it->second;
}

TVar* Symtable::lookupVar(const std::string name){
	std::unordered_map<std::pair<std::string,int>,TVar*>::iterator it;
	std::list<int>::iterator lit;
	for(lit=scopeStack.begin();lit!=scopeStack.end();lit++){
		it=vars.find(make_pair(name,*lit));
		if (it!=vars.end()) {
			return it->second;
		}
	}
	return NULL;
	
}

TType* Symtable::lookupType(const std::string name){
	std::unordered_map<std::string,TType*>::iterator it;
	it=types.find(name);
	if (it==types.end()) return NULL;
	return it->second;
}

TFunc* Symtable::lookupFunc(const std::string name, const std::vector<TType*> args){
	std::unordered_map<std::string,TFunc*>::iterator it;
	std::string key = name;
	for(int i=0; i<args.size(); i++){
		key += args[i]->name;
	}
	it=functions.find(key);
	if(it==functions.end())return NULL;	
	return it->second;
}


