#include "blahast.h"

NExpression* NAritmeticBinaryOperator::constantFold(){

	lexp = lexp->constantFold();
	rexp = rexp->constantFold();

	if(lexp->constant && rexp->constant){
		if(lexp->type->name == "Float"){
			float lf,rf;
			lf = ((NFloat*) lexp)->value;
			rf = ((NFloat*) rexp)->value;
			delete lexp;
			delete rexp;
			switch(op){
				case ADD:
					return new NFloat(lf+rf);
				case SUB:
					return new NFloat(lf-rf);
				case DIV:
					return new NFloat(lf/rf);
				case MUL:
					return new NFloat(lf*rf);
			}
		} else{
			int lf,rf;
			lf = ((NInteger*) lexp)->value;
			rf = ((NInteger*) rexp)->value;
			delete lexp;
			delete rexp;
			switch(op){
				case ADD:
					return new NInteger(lf+rf);
				case SUB:
					return new NInteger(lf-rf);
				case DIV:
					return new NInteger(lf/rf);
				case MUL:
					return new NInteger(lf*rf);
				case MOD:
					return new NInteger(lf%rf);
			}
		}
	}
	return this;
}


NExpression* NAritmeticUnaryOperator::constantFold(){
	rexp = rexp->constantFold();

	if(rexp->constant){
		if(rexp->type->name == "Float"){
			float rf;
			rf = ((NFloat*) rexp)->value;
			delete rexp;
			delete this;
			switch(op){
				case UMINUS:
					return new NFloat(-rf);
			}
		} else{
			int rf;
			rf = ((NInteger*) rexp)->value;
			delete rexp;
			delete this;
			switch(op){
				case UMINUS:
					return new NInteger(-rf);
			}
		}
	}
	return this;
}

NExpression* NBooleanBinaryOperator::constantFold(){

	lexp = lexp->constantFold();
	rexp = rexp->constantFold();

	if(lexp->constant){
		switch(op){
			case OR:
				if(((NBool*)lexp)->value){
					return new NBool(true);
				}else{
					return rexp;
				}
			case AND:
				if(((NBool*)lexp)->value){
					return rexp;
				}else{
					return new NBool(false);
				}
				
		}
	} 
	
	if(rexp->constant){
		switch(op){
			case OR:
				if(((NBool*)rexp)->value){
					return new NBool(true);
				}else{
					return lexp;
				}
			case AND:
				if(((NBool*)rexp)->value){
					return lexp;
				}else{
					return new NBool(false);
				}
				
		}
	} 

	return this;
}



NExpression* NBooleanUnaryOperator::constantFold(){
	
	rexp = rexp->constantFold();
	if(rexp->constant){
		switch(op){
			case NOT:
				if(((NBool*)rexp)->value){
					return new NBool(true);
				}else{
					return new NBool(false);
				}
		} 
	}
}

NExpression* NComparison::constantFold(){

	lexp = lexp->constantFold();
	rexp = lexp->constantFold();

	if(lexp->constant && rexp->constant){
		if(lexp->type->name == "Float"){
			float lf,rf;
			lf = ((NFloat*) lexp)->value;
			rf = ((NFloat*) rexp)->value;
			delete lexp;
			delete rexp;
			switch(op){
				case LT:
					return new NBool(lf<rf);
				case GT:
					return new NBool(lf>rf);
				case EQ:
					return new NBool(lf==rf);
				case NEQ:
					return new NBool(lf!=rf);
				case GEQ:
					return new NBool(lf>=rf);
				case LEQ:
					return new NBool(lf<=rf);
			}
		} else{
			int lf,rf;
			lf = ((NFloat*) lexp)->value;
			rf = ((NFloat*) rexp)->value;
			delete lexp;
			delete rexp;
			switch(op){
				case LT:
					return new NBool(lf<rf);
				case GT:
					return new NBool(lf>rf);
				case EQ:
					return new NBool(lf==rf);
				case NEQ:
					return new NBool(lf!=rf);
				case GEQ:
					return new NBool(lf>=rf);
				case LEQ:
					return new NBool(lf<=rf);
			}
		}
	}
	return this;

}

NExpression* NIntToFloat::constantFold(){
	expr=expr->constantFold();

	if(expr->constant){
		return new NFloat(((NInteger*)expr)->value);
	}
	return this;
}

NExpression* NFloatToInt::constantFold(){
	expr=expr->constantFold();

	if(expr->constant){
		return new NInteger(((NFloat*)expr)->value);
	}
	return this;
}
