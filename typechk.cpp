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
}

TType* NFloatToInt::typeChk(Symtable t ,TType* exp){
	return type;
}

TType* NIntToFloat::typeChk(Symtable t ,TType* exp){
	return type;
}

TType* NArray::typeChk(Symtable t,TType* exp){
	bool ok=true;
	for(int i=0; i<values.size();i++){
		ok= ok && (*((TArray*)type)->type) == *(values[i]->typeChk(t,exp));
	}

	if(!ok) {
		log.add(Msg(0,"Array elements of diferent types",2));
		return t.lookupType("error");
	}

	return type;
}

TType* NRExpression::typeChk(Symtable t,TType* exp){
	type = expr->typeChk(t,exp);
	return type;
}

TType* NExpressionStatement::typeChk(Symtable t,TType* exp){

	TType* x = expr->typeChk(t,exp);
	if(x->name=="error"){
		return x;
	}
	return t.lookupType("Void"); 
}


TType* NVar::typeChk(Symtable t,TType* exp){
	return type;
}

TType* NArrayAccess::typeChk(Symtable t,TType* exp){

	TType* ltype = lexpr->typeChk(t,exp);
	TType* itype = index->typeChk(t,exp);

	if(*ltype==TError() || *itype==TError()){
		return t.lookupType("error");
	}

	if(!ltype->isArr){
		log.add(Msg(0,"Array access of non-array type",2));
		return t.lookupType("error");
	}
	if(itype->name!="Integer"){
		log.add(Msg(0,"Array index must be an integer",2));
		return t.lookupType("error");
	}
	index=index->constantFold();
	type= ((TArray*)ltype)->type;
	return type;
}

TType* NStructAccess::typeChk(Symtable t, TType* exp){
	TType* ltype = lexpr->typeChk(t,exp);
	if(!ltype->isStruct){
		log.add(Msg(0,"Trying to access a member of a non-structured type",2));
		return t.lookupType("error");
	}
	var = ((TStructured*)ltype)->access(name);
	
	if (var==NULL){
		
		log.add(Msg(0,type->name+" doesn't have a field named "+name,2));
		return t.lookupType("error");
	}
	type = &var->type;
	return type;
	
}

TType* NFunctionCall::typeChk(Symtable t,TType* exp){
	std::vector<TType*> argTypes;
	bool ok=true;
	TType* temp;
	for(int i=0;i<arguments.size();i++){
		temp=arguments[i]->typeChk(t,exp);
		if(temp->name=="error"){
			ok=ok&&false;
		}else{
			arguments[i]=arguments[i]->constantFold();
			argTypes.push_back(temp);
		}
	}
	if(!ok){
		return t.lookupType("error");
	}
	func=t.lookupFunc(name,argTypes);

	if(func==NULL){
		std::stringstream s;
		s << "function " <<name<<"( ";
		for(int i=0;i<argTypes.size();i++){
			s << argTypes[i]->name<<",";
		}
		s << "\b ) not defined";
		log.add(Msg(0,s.str(),2));
		return t.lookupType("error");
	}
	type=&func->type;
	return type;
}

TType* NAritmeticBinaryOperator::typeChk(Symtable t, TType* exp){
	TType* ltype = lexp->typeChk(t,exp);
	TType* rtype = rexp->typeChk(t,exp);

	if(ltype->isNumeric && rtype->isNumeric){
		type = ltype;
		if(ltype->name == "Float" || rtype->name == "Float"){
			if(op==MOD){
				log.add(Msg(0,"%% operands must be integers",2));
				return t.lookupType("error");
			}

			if(ltype->name == "Integer"){
				lexp = new NIntToFloat(lexp);
			}
			if(rtype->name == "Integer"){
				rexp = new NIntToFloat(rexp);
			}
			type=lexp->type;
		}
		return type;
	}else{
		log.add(Msg(0,op+" operands must be numeric",2));
		return t.lookupType("error");
	}
}

TType* NBooleanBinaryOperator::typeChk(Symtable t ,TType* exp){
	TType* ltype = lexp->typeChk(t,exp);
	TType* rtype = rexp->typeChk(t,exp);

	if(*ltype==*rtype && ltype->name=="Bool"){
		type=ltype;
		return type;
	}else{
		log.add(Msg(0,op+" operands must be boolean",2));

		return t.lookupType("error");
	}
}


TType* NComparison::typeChk(Symtable t,TType* exp){
	TType* ltype = lexp->typeChk(t,exp);
	TType* rtype = rexp->typeChk(t,exp);


	if(ltype->isNumeric && rtype->isNumeric){
		if(ltype->name == "Float" || rtype->name == "Float"){
			if(ltype->name == "Integer"){
				lexp = new NIntToFloat(lexp);
			}
			if(rtype->name == "Integer"){
				rexp = new NIntToFloat(rexp);
			}
		}
		type=t.lookupType("Bool");
		return type;
	}else{
		log.add(Msg(0,op+" operands must be numeric in comparison",2));
		return t.lookupType("error");
	}
}

TType* NAritmeticUnaryOperator::typeChk(Symtable t, TType* exp){
	TType* rtype = rexp->typeChk(t,exp);

	if(rtype->isNumeric){
		type=rtype;
		return rtype;
	}else{
		log.add(Msg(0,op+" operand must be numeric",2));
		return t.lookupType("error");
	}
	
}

TType* NBooleanUnaryOperator::typeChk(Symtable t, TType* exp){
	TType* rtype = rexp->typeChk(t,exp);

	if(rtype->name=="Bool"){
		type=rtype;
		return rtype;
	}else{
		log.add(Msg(0,op+" operand must be boolean",2));
		return t.lookupType("error");
	}
}

TType* NBlock::typeChk(Symtable t, TType* exp){
	bool ok=true;
	for(int i=0;i<statements.size();i++){
		ok=ok && statements[i]->typeChk(t,exp)->name!="error";
	}
	if(!ok)return t.lookupType("error");
	return t.lookupType("Void");

}

TType* NVariableDeclaration::typeChk(Symtable t, TType* exp){
	bool ok=true;
	if(assignment!=NULL){
		TType* temp=assignment->typeChk(t,exp);
		if(temp->name =="error"){
			return t.lookupType("error");
		}
	}
	return t.lookupType("Void");
}


TType* NFunctionDeclaration::typeChk(Symtable t,TType* exp){
	bool ok=true;
	return block->typeChk(t,type);
}

TType* NWhileDo::typeChk(Symtable t,TType* exp){
	
	TType* temp = cond->typeChk(t,exp);
	if(*(block->typeChk(t,exp))==TError()){
		return t.lookupType("error");
	}

	if(temp->name!="Bool"){
		log.add(Msg(0,"While condition must be a boolean expression",2));
		return t.lookupType("error");
	}

	cond=cond->constantFold();
	return t.lookupType("Void");
}

TType* NDoWhile::typeChk(Symtable t,TType* exp){
	
	TType* temp = cond->typeChk(t,exp);
	if(*(block->typeChk(t,exp))==TError()){
		return t.lookupType("error");
	}

	if(temp->name!="Bool"){
		log.add(Msg(0,"While condition must be a boolean expression",2));
		return t.lookupType("error");
	}
	cond=cond->constantFold();
	return t.lookupType("Void");
}

TType* NIf::typeChk(Symtable t, TType* exp){
	
	TType* temp = cond->typeChk(t,exp);
	if(*(block->typeChk(t,exp))==TError()){
		return t.lookupType("error");
	}

	if(elseBlock != NULL && elseBlock->typeChk(t,exp)->name=="error"){
		return t.lookupType("error");
	}

	if(temp->name!="Bool"){
		log.add(Msg(0,"If condition must be a boolean expression",2));
		return t.lookupType("error");
	}

	cond=cond->constantFold();
	return t.lookupType("Void");

}


TType* NForRange::typeChk(Symtable t, TType* exp){
	TType* Tb = beg->typeChk(t,exp);
	TType* Te = end->typeChk(t,exp);
	TType* Ts = step->typeChk(t,exp);
	bool ok=true;
	if(*Tb!= *Te || *Te != *Ts || Ts->name != "Integer"){
		log.add(Msg(0,"range expressions must be integers",2));
		ok=false;
	}

	if(block->typeChk(t,exp)->name == "error") {
		ok = false;
	}

	if (!ok){
		return t.lookupType("error");
	}
	beg=beg->constantFold();
	end=end->constantFold();
	step=step->constantFold();
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
		texp = expr->typeChk(t,exp);
	}

	if(!(texp->name == exp->name)){

		log.add(Msg(0,"Return type doesn't match",2));
		return t.lookupType("error");
	}

	if(expr!=NULL){
		expr=expr->constantFold();
	}
	return t.lookupType("Void");

}

TType* NAssignment::typeChk(Symtable t, TType* exp){
	TType* ltype= var->typeChk(t,exp);
	TType* rtype=assignment->typeChk(t,exp);

	if(*ltype==*rtype){
		assignment=assignment->constantFold();
		return t.lookupType("Void");
	}else if(ltype->isNumeric && rtype->isNumeric){
		if(ltype->name=="Integer" && rtype->name == "Float"){
			assignment = new NFloatToInt(assignment);
		}else if(ltype->name=="Float" && rtype->name=="Integer"){
			assignment = new NIntToFloat(assignment);
		}
		assignment=assignment->constantFold();
		return t.lookupType("Void");
	}else{
		log.add(Msg(0,"Asignment type doesn't match "+ltype->name+"!="+rtype->name,2));
		return t.lookupType("error");
	}

}


