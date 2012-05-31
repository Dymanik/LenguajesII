#include "blahast.h"
extern Log log;

TType* NInteger::typeChk(Symtable t,TType* exp){
	return type;
}

TType* NFloat::typeChk(Symtable t,TType* exp){
	return type;
}

TType* NString::typeChk(Symtable t,TType* exp){
	return type;
}

TType* NChar::typeChk(Symtable t,TType* exp){
	return type;
}

TType* NBool::typeChk(Symtable t,TType* exp){
	return type;
;
}

TType* NArray::typeChk(Symtable t,TType* exp){
	bool ok=true;
	for(int i=0; i<values.size();i++){
		ok= ok && (*((TArray*)type)->type) == *(values[i]->typeChk(t,exp));
	}

	if(!ok) {
		log.add(Msg(0,"Array elements of diferent types",2));
		return NULL;
	}

	return type;
}

TType* NVar::typeChk(Symtable t,TType* exp){
	type=&(var->type);
	return type;
}

TType* NArrayAccess::typeChk(Symtable t,TType* exp){

	TType* ltype = lexpr->typeChk(t,exp);
	TType* itype = index->typeChk(t,exp);
	if(!ltype->isArr){
		log.add(Msg(0,"Array access of non-array type",2));
		return NULL;
	}
	if(ltype->name!="Integer"){
		log.add(Msg(0,"Array index must be an integer",2));
		return NULL;
	}
	type= ((TArray*)ltype)->type;
	return type;
}

TType* NStructAccess::typeChk(Symtable t, TType* exp){
	TType* ltype = lexpr->typeChk(t,exp);
	if(!ltype->isStruct){
		log.add(Msg(0,"Trying to access a member of a non-structured type",2));
		return NULL;
	}
	type = ((TStructured*)ltype)->accessType(name);
	if (type==NULL){
		
		log.add(Msg(0,type->name+" doesn't have a field named "+name,2));
		return NULL;
	}
	return type;
	
}

TType* NFunctionCall::typeChk(Symtable t,TType* exp){
	std::vector<TType*> argTypes;
	bool ok=true;
	TType* temp;
	for(int i=0;i<arguments.size();i++){
		temp=arguments[i]->typeChk(t,exp);
		if(temp==NULL){
			ok=ok&&false;
		}else{
			argTypes.push_back(temp);
		}
	}
	if(!ok){
		log.add(Msg(0,"typeerror on arguments",2));
		return NULL;
	}
	func=t.lookupFunc(name,argTypes);
	type=&func->type;

	if(type==NULL){
		log.add(Msg(0,"function not defined",2));
		return NULL;
	}
	return type;
}

TType* NAritmeticBinaryOperator::typeChk(Symtable t, TType* exp){
	TType* ltype = lexp->typeChk(t,exp);
	TType* rtype = rexp->typeChk(t,exp);

	if(ltype==NULL || rtype==NULL){
		return NULL;
	}

	if(ltype->isNumeric && rtype->isNumeric){
		type=ltype;
		if(rtype->name=="Double"){
			type=rtype;
		}
		return type;
	}else{
		return NULL;
	}
}

TType* NBooleanBinaryOperator::typeChk(Symtable t ,TType* exp){
	TType* ltype = lexp->typeChk(t,exp);
	TType* rtype = rexp->typeChk(t,exp);

	if(ltype==NULL || rtype==NULL){
		return NULL;
	}
	
	if(*ltype==*rtype && ltype->name=="Bool"){
		type=ltype;
		return type;
	}
	return NULL;
}


TType* NComparison::typeChk(Symtable t,TType* exp){
	TType* ltype = lexp->typeChk(t,exp);
	TType* rtype = rexp->typeChk(t,exp);

	if(ltype==NULL || rtype==NULL){
		return NULL;
	}

	if(ltype->isNumeric && rtype->isNumeric){
		return t.lookupType("Bool");
	}
	return NULL;
}

TType* NAritmeticUnaryOperator::typeChk(Symtable t, TType* exp){
	TType* rtype = rexp->typeChk(t,exp);

	if(rtype==NULL){
		return NULL;
	}

	if(rtype->isNumeric){
		type=rtype;
		return rtype;
	}
	return NULL;
	
}

TType* NBooleanUnaryOperator::typeChk(Symtable t, TType* exp){
	TType* rtype = rexp->typeChk(t,exp);

	if(rtype==NULL){
		return NULL;
	}

	if(rtype->name=="Bool"){
		type=rtype;
		return rtype;
	}
	return NULL;
}

TType* NBlock::typeChk(Symtable t, TType* exp){
	bool ok=true;
	for(int i=0;i<statements.size();i++){
		ok=ok && statements[i]->typeChk(t,exp)!=NULL;
	}
	if(!ok)return NULL;
	return t.lookupType("Void");

}

TType* NVariableDeclaration::typeChk(Symtable t, TType* exp){
	bool ok=true;
	if(assignment!=NULL){
		TType* temp=assignment->typeChk(t,exp);
		if(!(var->type==*temp)){
			ok=false;
		}
	}
	if(!ok){
		return false;
	}
	return t.lookupType("Void");
}


TType* NFunctionDeclaration::typeChk(Symtable t,TType* exp){
	bool ok=true;
	return block->typeChk(t,type);
}

TType* NWhileDo::typeChk(Symtable t,TType* exp){
	
	TType* temp = cond->typeChk(t,exp);
	if(block->typeChk(t,exp)==NULL || temp == NULL){
		return NULL;
	}

	if(temp->name!="Bool"){

		return NULL;
	}

	return t.lookupType("Void");
}

TType* NDoWhile::typeChk(Symtable t,TType* exp){
	
	TType* temp = cond->typeChk(t,exp);
	if(block->typeChk(t,exp)==NULL || temp == NULL){
		return NULL;
	}

	if(temp->name!="Bool"){

		return NULL;
	}

	return t.lookupType("Void");
}

TType* NIf::typeChk(Symtable t, TType* exp){
	
	TType* temp = cond->typeChk(t,exp);
	if(block->typeChk(t,exp)==NULL || temp == NULL){
		return NULL;
	}

	if(elseBlock != NULL && elseBlock->typeChk(t,exp)==NULL){
		return NULL;
	}

	if(temp->name!="Bool"){
		return NULL;
	}
	
	return t.lookupType("Void");

}


//FATLALTALTLA
TType* NForRange::typeChk(Symtable t, TType* exp){
	return t.lookupType("Void");
	
}
TType* NForVar::typeChk(Symtable t, TType* exp){
	return t.lookupType("Void");
}
TType* NReturn::typeChk(Symtable t,TType* exp){

	TType* texp;
	if(expr==NULL){
		texp = t.lookupType("Void");
	}else{
		TType* texp = expr->typeChk(t,exp);
	}

	if(!(*texp == *exp)){
		log.add(Msg(0,"Return type doesn't match",2));
		return NULL;
	}

	return t.lookupType("Void");

}

TType* NAssignment::typeChk(Symtable t, TType* exp){
	TType* ltype= var->typeChk(t,exp);
	TType* rtype=assignment->typeChk(t,exp);

	if(*ltype==*rtype){
		return t.lookupType("Void");
	}

	return NULL;
}


