#include "blahsymtable.h"
#include <algorithm>


TArray::TArray(TType* typ, Sizelist sizes):TType("Arrayof"+typ->name,typ->size*length,false,false,false,true){
	length = sizes.front();
	if(sizes.size()==1){
		type=new TArray(typ,sizes.front());
	} else{
		sizes.pop_front();
		type=new TArray(typ,sizes);
		name="Arrayof"+type->name;
	}
}




void TRegister::addField(TVar *typ, std::string name){
	int offset=size;
	if(offset%typ->type.alignment!=0){
		offset+= typ->type.alignment-(offset%typ->type.alignment);
	}
	typ->offset = offset;
	fields[name]=typ;
	size=offset+typ->type.size;
	


}

void TUnion::addField(TVar *typ, std::string name){
	fields[name]=typ;
	if(size<typ->type.size){
		size=typ->type.size;
	}
}

bool cmp(const std::pair<std::string,TVar*>  &a, const std::pair<std::string,TVar*> &b){
	return a.second->type.alignment > b.second->type.alignment;
};


TRegister::TRegister(std::string name,Fields fieldl):TStructured(name){
	std::sort(fieldl.begin(),fieldl.end(),cmp);
	alignment = fieldl.front().second->type.alignment;
	for(int i=0;i<fieldl.size();i++){
		addField((fieldl[i]).second,(fieldl[i]).first);
	}
}

TUnion::TUnion(std::string name,Fields fieldl):TStructured(name,true){
	std::sort(fieldl.begin(),fieldl.end(),cmp);
	alignment = fieldl.front().second->type.alignment;
	for(int i=0;i<fieldl.size();i++){
		addField((fieldl[i]).second,(fieldl[i]).first);
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
	vars[Tuple(v->name,scopeStack.front())]=v;
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
	std::unordered_map<Tuple,TVar*>::iterator it;
	it=vars.find(Tuple(name,scopeStack.front()));
	if (it==vars.end()) return NULL;
	return it->second;
}

TVar* Symtable::lookupVar(const std::string name){
	std::unordered_map<Tuple,TVar*>::iterator it;
	std::list<int>::iterator lit;
	for(lit=scopeStack.begin();lit!=scopeStack.end();lit++){
		it=vars.find(Tuple(name,*lit));
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


