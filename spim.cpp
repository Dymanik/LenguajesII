#include "spim.h"
using namespace std;



void SPIM::print(std::ostream& os){

	os<<".data"<<endl;
	os<<"_nl: .asciiz \"\\n\""<<endl;
	for(int i = 0 ; i < global_vars.size();i++){
		TVar* v=global_vars[i];

		switch(v->type.size){
			case 1:
				os<<v->name<<": ";
				os<<".byte 0";
				break;
			case 4:
				os<<v->name<<": ";
				os<<".word 0";
				break;
			default:
				switch(v->type.alignment){
					case 2:
						os<<".align 1";
						break;
					case 4:
						os<<".align 2";
				}
				os<<endl<<v->name<<": ";
				os<<".space "<<v->type.size;
		}
		os<<endl;
	}

	os<<".text"<<endl;
	
	printfuncts(os);
	

	os<<"main:"<<endl;
	functionblocks[0]->print(os);

	for(int i =1;i<functionblocks.size();i++){
		functionblocks[i]->print(os);
	}
}


void SPIM::printfuncts(ostream& os){
	os<<"__printInteger:"<<endl;
	os<<"\tli $v0, 1"<<endl;
	os<<"\tlw $a0, 0($sp)"<<endl;
	os<<"\tsyscall"<<endl;
	os<<"\tli $v0, 4"<<endl;
	os<<"\tla $a0, _nl"<<endl;
	os<<"\tsyscall"<<endl;
	os<<"\tjr $ra"<<endl;

	os<<"__printFloat:"<<endl;
	os<<"\tl.s $f12, 0($sp)"<<endl;
	os<<"\tli $v0, 2"<<endl;
	os<<"\tsyscall"<<endl;
	os<<"\tli $v0, 4"<<endl;
	os<<"\tla $a0, _nl"<<endl;
	os<<"\tsyscall"<<endl;
	os<<"\tjr $ra"<<endl;

	os<<"__readI:"<<endl;
	os<<"\tli $v0, 5"<<endl;
	os<<"\tsyscall"<<endl;
	os<<"\tjr $ra"<<endl;

	os<<"__readF:"<<endl;
	os<<"\tli $v0, 6"<<endl;
	os<<"\tsyscall"<<endl;
	os<<"\tmfc1 $v0 , $f0"<<endl;
	os<<"\tjr $ra"<<endl;

}


void SpimInst::print(ostream &os){

	if(labels.size()>1){
		list<string>::iterator it;
		it=labels.begin();
		os<<(*it)<<":\n";
		it++;
	}
	bool undef =false;
	bool jlabel =false;
	os<<"\t";


	switch(op){
		case ADD:
			{
				string op="add";
				if(onFloat){
					op+=".s";
				}
				os<<op<<" "<<result->toStr()<<" , "<< arg1->toStr()<<" , "<<arg2->toStr();
			}
			break;
		case SUB:
			{
				string op="sub";
				if(onFloat){
					op+=".s";
				}
				os<<op<<" "<<result->toStr()<<" , "<< arg1->toStr()<<" , "<<arg2->toStr();
				break;
			}
		case MUL:
			{
				string op="mul";
				if(onFloat){
					op+=".s";
				}
				os<<op<<" "<<result->toStr()<<" , "<< arg1->toStr()<<" , "<<arg2->toStr();
				break;
			}
		case DIV:
			{
				string op="div";
				if(onFloat){
					op+=".s";
				}
				os<<op<<" "<<result->toStr()<<" , "<< arg1->toStr()<<" , "<<arg2->toStr();
				break;
			}
		case MOD:

			os<<"rem "<<result->toStr()<<" , "<< arg1->toStr()<<" , "<<arg2->toStr();
			break;

		case UMINUS:
			{
				string op="neg";
				if(onFloat){
					op+=".s";
				}
				os<<op<<" "<<result->toStr()<<" , "<< arg1->toStr();
				break;
			}
		case INT2FLT:
			os<<"mtc1 "<<arg1->toStr()<<" , "<< result->toStr()<<endl;
			os<<"\tcvt.s.w "<<result->toStr()<<" , "<< result->toStr();
			break;
		case FLT2INT:
			os<<"cvt.w.s "<<arg1->toStr()<<" , "<< arg1->toStr()<<endl;
			os<<"\tmfc1 "<<result->toStr()<<" , "<< arg1->toStr();
			break;

		case COPY:
			os<<"#copy "<<result->toStr()<<" := "<< arg1->toStr();
			break;
		case LINDEX:
			os<<"add "<<result->toStr()<<" , "<<arg2->toStr() << " , " << result->toStr();
			os<<"\t#"<<result->toStr()<<"["<<arg2->toStr()<<"] = "<<arg1->toStr();
			break;
		case RINDEX:
			os<<"add "<<arg1->toStr()<<" , "<<arg2->toStr() << " , " << arg1->toStr();
			os<<"#"<<result->toStr()<<" = "<<arg1->toStr()<<"["<<arg2->toStr()<<"]";
			break;
		case GOTO:
			os<<"j ";
			if(result){
				jlabel=true;
			}else{
				undef = true;
			}
			break;
		case IFEQ:
			if (onFloat){
				os<<"c.eq.s "<<arg1->toStr()<<" , "<< arg2->toStr()<<endl;
				os<<"bc1t ";
			}else{
				os<<"beq "<<arg1->toStr()<<" , "<< arg2->toStr()<<" , ";
			}
			if(result){
				jlabel=true;
			}else{
				undef = true;
			}
			break;
		case IFNEQ:
			if (onFloat){
				os<<"c.eq.s "<<arg1->toStr()<<" , "<< arg2->toStr()<<endl;
				os<<"bc1f ";
			}else{
				os<<"bneq "<<arg1->toStr()<<" , "<< arg2->toStr()<<" , ";
			}
			if(result){
				jlabel=true;
			}else{
				undef = true;
			}
			break;
		case IFLT:
			if (onFloat){
				os<<"c.lt.s "<<arg1->toStr()<<" , "<< arg2->toStr()<<endl;
				os<<"bc1t ";
			}else{
				os<<"blt "<<arg1->toStr()<<" , "<< arg2->toStr()<<" , ";
			}
			if(result){
				jlabel=true;
			}else{
				undef = true;
			}
			break;
		case IFGT:
			if (onFloat){
				os<<"c.gt.s "<<arg1->toStr()<<" , "<< arg2->toStr()<<endl;
				os<<"bc1t ";
			}else{
				os<<"bgt "<<arg1->toStr()<<" , "<< arg2->toStr()<<" , ";
			}
			if(result){
				jlabel=true;
			}else{
				undef = true;
			}
			break;
		case IFLEQ:
			if (onFloat){
				os<<"c.le.s "<<arg1->toStr()<<" , "<< arg2->toStr()<<endl;
				os<<"bc1t ";
			}else{
				os<<"ble "<<arg1->toStr()<<" , "<< arg2->toStr()<<" , ";
			}
			if(result){
				jlabel=true;
			}else{
				undef = true;
			}
			break;
		case IFGEQ:
			if (onFloat){
				os<<"c.ge.s "<<arg1->toStr()<<" , "<< arg2->toStr()<<endl;
				os<<"bc1t ";
			}else{
				os<<"bge "<<arg1->toStr()<<" , "<< arg2->toStr()<<" , ";
			}
			if(result){
				jlabel=true;
			}else{
				undef = true;
			}
			break;
		case PARAM:
			if(onFloat){
				os<<"s.s "<<arg1->toStr()<<" , -4($sp)"<<endl;
			}else{
				os<<"sw "<<arg1->toStr()<<" , -4($sp)"<<endl;

			}
			os<<"\tla $sp , -4($sp)"<<endl;
			break;
		case RETRIEVE:
			{
				
				TVar* v = result->value.var;
				if(v->isParam){
					os<<"sw "<< arg2->toStr()<<" , "<<v->offset<<"($fp)";
				}else{
					string st="";
					if(v->type.basic){
						switch(v->type.size){
							case 4:
								st="sw ";
								break;
							case 1:
								st="sb ";
								break;
						}
						if(v->scope==0){
							os<<st<<" $v0 , "<<result->toStr();
						}else{
							os<<st<<" $v0 , "<<-(v->offset+8+(int)v->type.size)<<"($fp)";
						}
					}
				}
			}

			break;
		case CALL:
			{
				os<<"jal __"<< arg1->toStr()<<endl;
				int n = arg2->value.integer*4;
				os<<"\tla $sp , "<<n<<"($sp)";
			}
			break;
		case RETURN:
			if(arg1!=NULL){
				if(onFloat){
					os<<"mfc1 $v0 , "<<arg1->toStr();
				}else{
					os<<"move $v0 , "<<arg1->toStr();
				}
			}
			os<<" #return ";
			break;
		case EPILOGUE:
			os<<"sw $fp , -4($sp)"<<endl;
			os<<"\tsw $ra , -8($sp)"<<endl;
			os<<"\tla $fp , 0($sp)"<<endl;
			{
				int n = arg1->value.func->stacksize +8;
				os<<"\tla $sp , -"<<n<<"($sp)";
			}
			break;
		case PROLOGUE:
			os<<"la $sp , 0($fp)"<<endl;
			os<<"\tlw $ra , -8($fp)"<<endl;
			os<<"\tlw $fp , -4($fp)"<<endl;
			os<<"\tjr $ra";
			break;
		case EXIT:
			os<<"li $v0, 10"<<endl;
			os<<"\tsyscall";
			break;
		case LD:
			{
				if(onFloat){
					switch(arg2->type){
						case Operand::FLOAT:
							os<<"li.s "<<result->toStr()<<" , "<<arg2->toStr();
							break;
						case Operand::VAR:
							{
								TVar* v = arg2->value.var;
								if(v->isParam){
									os<<"l.s "<<result->toStr()<<" , "<<v->offset<<"($fp)";
								}else if(v->type.basic){
									if(v->scope==0){
										os<<"l.s "<<result->toStr()<<" , "<<v->name;
									}else{
										os<<"l.s "<<result->toStr()<<" , "<<-(v->offset+12)<<"($fp)";

									}
								}else{
									cerr<<"variable no basica en flotante";
								}
							}
							break;
						case Operand::REG:
							os<<"l.s "<<result->toStr()<<" , 0("<<arg2->toStr()<<")";
							break;
						default:
							cerr<<"Operando invalido";
							break;
					}
				} else {
					switch(arg2->type){
						case Operand::INT:
							os<<"li "<<result->toStr()<<" , "<<arg2->toStr();
							break;
						case Operand::CHAR:
							os<<"li "<<result->toStr()<<" , "<<(int)arg2->value.character;
							break;
						case Operand::VAR:
							{
								TVar* v = arg2->value.var;
								if(v->isParam){
									os<<"lw "<<result->toStr()<<" , "<<v->offset<<"($fp)";
								}else if(v->type.basic){
									string ld = "";
									switch(v->type.size){
										case 4:
											ld="lw ";
											break;
										case 1:
											ld="lb ";
											break;
									}

									if(v->scope==0){
										os<<ld<<" "<<result->toStr()<<" , "<<v->name;
									}else{
										os<<ld<<" "<<result->toStr()<<" , "<<-(((int)v->type.size+v->offset)+8)<<"($fp)";
									}
								}else{
									if(v->scope==0){
										os<<"la "<<result->toStr()<<" , "<<v->name;
									}else{
										os<<"la "<<result->toStr()<<" , "<<-(v->offset+8+(int)v->type.size)<<"($fp)";
									}
								}
							}
							break;
						case Operand::REG:
							os<<"lw "<<result->toStr()<<" , 0("<<arg2->toStr()<<")";
							break;
						default:
							cerr<<"Invalid load operand"<<endl;
					}
				}
			}
			break;
		case ST:
			{
				string st = "";
				if(onFloat){
					switch(result->type){
						case Operand::VAR:
							{
								TVar* v = result->value.var;
								if(v->isParam){
									os<<"s.s " << arg2->toStr()<<" , "<<v->offset<<"($fp)";
								}else{
									if(v->scope==0){
										os<<"s.s "<<arg2->toStr()<<" , "<<v->name;
									}else{
										os<<"s.s "<<arg2->toStr()<<" , "<<-(v->offset+8+(int)v->type.size)<<"($fp)";
									}
								}
								break;
							}
						case Operand::REG:
							os<<"s.s "<< arg2->toStr()<<" , 0("<<result->toStr()<<")";
							break;
						default:
							cerr<<"Storing outside a variable";
					}
				} else{
					switch(result->type){
						case Operand::VAR:
							{
								TVar* v = result->value.var;
								if(v->isParam){
									os<<"sw "<< arg2->toStr()<<" , "<<v->offset<<"($fp)";
								}else{
									string st="";
									if(v->type.basic){
										switch(v->type.size){
											case 4:
												st="sw ";
												break;
											case 1:
												st="sb ";
												break;
										}
										if(v->scope==0){
											os<<st<<" "<<arg2->toStr()<<" , "<<v->name;
										}else{
											os<<st<<" "<<arg2->toStr()<<" , "<<-(v->offset+8+(int)v->type.size)<<"($fp)";
										}
									}
								}
								break;
							}
						case Operand::REG:
							os<<"sw "<< arg2->toStr()<<" , 0("<<result->toStr()<<")";
							break;
						default:
							cerr<<"Storing outside a variable";
					}
				}
				break;
			}
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
