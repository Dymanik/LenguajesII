#include "blahast.h"

void NExpressionStatement::print(std::ostream &os,int depth){
	os<<std::string(depth,' ')<<"NExpressionStatement: expr="<<std::endl;
	expr->print(os,depth+1);
}

void NRExpression::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NRExpression: expr="<<std::endl;
	expr->print(os,depth+1);
	
}


void NInteger::print(std::ostream &os,int depth){
	os<<std::string(depth,' ')<<"NInteger: value="<<value;
	os<<" type: "<< type->name<<std::endl;
}

void NFloat::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NFloat: value="<<value;
	os<<" type: "<< type->name<<std::endl;
}

void NString::print(std::ostream &os,int depth){
	os<<std::string(depth,' ')<<"NString: value="<<value;
	os<<" type: "<< type->name<<std::endl;
}

void NChar::print(std::ostream &os,int depth){
	os<<std::string(depth,' ')<<"NChar: value="<<value;
	os<<" type: "<< type->name<<std::endl;
}

void NBool::print(std::ostream &os,int depth){
	os<<std::string(depth,' ')<<"NBool: value="<<value;
	os<<" type: "<< type->name<<std::endl;
}

void NArray::print(std::ostream &os,int depth){

	os<<std::string(depth,' ')<<"NArray: values=["<<std::endl;
	for(int i=0;i<values.size();i++){
		values[i]->print(os,depth+1);
	}
	os<<std::string(depth,' ')<<"]"<<std::endl;
}

void NVar::print(std::ostream &os,int depth){
	os<<std::string(depth,' ')<<"NVar: {name="<<var->name;
	os<<",type="<<var->type.name<<"}"<<std::endl;
}

void NIntToFloat::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NIntToFloat: {expr="<<std::endl;
	expr->print(os,depth+1);
	os<<std::string(depth,' ')<<"}"<<std::endl;
}

void NFloatToInt::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NFloatToInt: {expr="<<std::endl;
	expr->print(os,depth+1);
	os<<std::string(depth,' ')<<"}"<<std::endl;
}

void NArrayAccess::print(std::ostream &os,int depth){
	os<<std::string(depth,' ')<<"NArrayAccess: {index="<<std::endl;
	index->print(os,depth+1);
	os<<std::string(depth,' ')<<"lexpr="<<std::endl;
	lexpr->print(os,depth+1);
	os<<std::string(depth,' ')<<"}"<<std::endl;
}

void NStructAccess::print(std::ostream &os,int depth){
	os<<std::string(depth,' ')<<"NStructAccess: {name="<<name<<",lexpr="<<std::endl;
	lexpr->print(os,depth+1);
	os<<std::string(depth,' ')<<"}"<<std::endl;
}

void NFunctionCall::print(std::ostream &os,int depth){
	os<<std::string(depth,' ')<<"NFunctionCall: {name="<<name<<",args=("<<std::endl;
	for(int i=0;i<arguments.size();i++){
		arguments[i]->print(os,depth+1);
	}
	os<<std::string(depth,' ')<<")"<<std::endl;
	os<<std::string(depth,' ')<<"}"<<std::endl;
}

void NAritmeticBinaryOperator::print(std::ostream &os,int depth){
	os<<std::string(depth,' ')<<"NAritmeticBinaryOperator: {op="<<op<<std::endl;
	os<<std::string(depth,' ')<<"lexp="<<std::endl;
	lexp->print(os,depth+1);
	os<<std::string(depth,' ')<<"rexp="<<std::endl;
	rexp->print(os,depth+1);
	os<<std::string(depth,' ')<<"}"<<std::endl;

}

void NBooleanBinaryOperator::print(std::ostream &os,int depth){
	os<<std::string(depth,' ')<<"NBooleanBinaryOperator: {op="<<op<<std::endl;
	os<<std::string(depth,' ')<<"lexp="<<std::endl;
	lexp->print(os,depth+1);
	os<<std::string(depth,' ')<<"rexp="<<std::endl;
	rexp->print(os,depth+1);
	os<<std::string(depth,' ')<<"}"<<std::endl;
}

void NComparison::print(std::ostream &os,int depth){
	os<<std::string(depth,' ')<<"NComparison: {op="<<op<<std::endl;
	os<<std::string(depth,' ')<<"lexp="<<std::endl;
	lexp->print(os,depth+1);
	os<<std::string(depth,' ')<<"rexp="<<std::endl;
	rexp->print(os,depth+1);
	os<<std::string(depth,' ')<<"}"<<std::endl;
}
void NAritmeticUnaryOperator::print(std::ostream &os,int depth){
	os<<std::string(depth,' ')<<"NAritmeticUnaryOperator: {op="<<op<<std::endl;
	os<<std::string(depth,' ')<<"rexp="<<std::endl;
	rexp->print(os,depth+1);
	os<<std::string(depth,' ')<<"}"<<std::endl;
}
void NBooleanUnaryOperator::print(std::ostream &os,int depth){
	os<<std::string(depth,' ')<<"NBooleanUnaryOperator: {op="<<op<<std::endl;
	os<<std::string(depth,' ')<<"rexp="<<std::endl;
	rexp->print(os,depth+1);
	os<<std::string(depth,' ')<<"}"<<std::endl;
}

void NBlock::print(std::ostream &os,int depth){
	os<<std::string(depth,' ')<<"NBlock: statements={"<<std::endl;
	for(int i=0;i<statements.size();i++){
		statements[i]->print(os,depth+1);
		os<<std::string(depth,' ')<<";"<<std::endl;
	}
	os<<std::string(depth,' ')<<"}"<<std::endl;
}


void NVariableDeclaration::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NVariableDeclaration: {type="<<var->type.name<<" name="<<var->name<<" ";
	if(assignment!=NULL){
		os<<"assignment="<<std::endl;
		assignment->print(os,depth+1);
	}
	os<<std::string(depth,' ')<<"}"<<std::endl;
}


void NFunctionDeclaration::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NFunctionDeclaration: {type="<<type->name<<" name="<<name<<" ";
	os<<",arguments={"<<std::endl;
	for(int i=0;i<arguments.size();i++){
		arguments[i]->print(os,depth+1);
		os<<std::string(depth,' ')<<","<<std::endl;
	}
	os<<std::string(depth,' ')<<"},block="<<std::endl;
	block->print(os,depth+1);
}

void NRegisterDeclaration::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NRegisterDeclaration: {name="<<name;
	os<<",fields={"<<std::endl;
	for(int i=0;i<fields.size();i++){
		fields[i].second->print(os,depth+1);
		os<<std::string(depth,' ')<<","<<std::endl;
	}
	os<<std::string(depth,' ')<<"}"<<std::endl;
	os<<std::string(depth,' ')<<"}";
}

void NUnionDeclaration::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NUnionDeclaration: {name="<<name;
	os<<",fields={"<<std::endl;
	for(int i=0;i<fields.size();i++){
		fields[i].second->print(os,depth+1);
		os<<std::string(depth,' ')<<","<<std::endl;
	}
	os<<std::string(depth,' ')<<"}"<<std::endl;
	os<<std::string(depth,' ')<<"}";
}

void NWhileDo::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NWhileDo: {cond="<<std::endl;
	cond->print(os,depth+1);
	os<<std::string(depth,' ')<<",block="<<std::endl;
	block->print(os,depth+1);
	os<<std::string(depth,' ')<<"}";
}

void NDoWhile::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NDoWhile: {cond="<<std::endl;
	cond->print(os,depth+1);
	os<<std::string(depth,' ')<<",block="<<std::endl;
	block->print(os,depth+1);
	os<<std::string(depth,' ')<<"}";
}

void NIf::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NIf: {cond="<<std::endl;
	cond->print(os,depth+1);
	os<<std::string(depth,' ')<<",block="<<std::endl;
	block->print(os,depth+1);
	if(elseBlock!=NULL){
		os<<std::string(depth,' ')<<",elseblock="<<std::endl;
		block->print(os,depth+1);
	}
	os<<std::string(depth,' ')<<"}";
}

void NForRange::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NForRange: {var="<<var<<std::endl;
	os<<std::string(depth,' ')<<",beg="<<std::endl;
	beg->print(os,depth+1);
	os<<std::string(depth,' ')<<",end="<<std::endl;
	end->print(os,depth+1);
	os<<std::string(depth,' ')<<",step="<<std::endl;
	step->print(os,depth+1);
	os<<std::string(depth,' ')<<",block="<<std::endl;
	block->print(os,depth+1);
	os<<std::string(depth,' ')<<"}";
}

void NForArray::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NForArray: {var="<<var<<std::endl;
	os<<std::string(depth,' ')<<",array="<<std::endl;
	array->print(os,depth+1);
	os<<std::string(depth,' ')<<",block="<<std::endl;
	block->print(os,depth+1);
	os<<std::string(depth,' ')<<"}";
}

void NForVar::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NForVar: {var="<<var<<std::endl;
	os<<std::string(depth,' ')<<",array="<<std::endl;
	array->print(os,depth+1);
	os<<std::string(depth,' ')<<",block="<<std::endl;
	block->print(os,depth+1);
	os<<std::string(depth,' ')<<"}";
}

void NNext::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NNext"<<std::endl;
}

void NStop::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NStop"<<std::endl;
}


void NReturn::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NReturn:";
	if(expr==NULL){
		os<<" void"<<std::endl;
	}else{
		os<<" val="<<std::endl;
	}
	expr->print(os,depth+1);
}


void NAssignment::print(std::ostream &os, int depth){
	os<<std::string(depth,' ')<<"NAssignment:{lvalue="<<std::endl;
	var->print(os,depth+1);
	os<<std::string(depth,' ')<<",rvalue="<<std::endl;
	assignment->print(os,depth+1);
	os<<std::string(depth,' ')<<"}"<<std::endl;
}

