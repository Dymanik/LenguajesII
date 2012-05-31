#include <iostream>
#include <cstdio>
#include <vector>
#include "blahlog.h"
#include "blahast.h"
#include "blahsymtable.h"

extern int yyparse();
extern Log log;
extern Symtable table;
extern NBlock* programAST;
using namespace std;
IBlock* iblock;

int main(int argc, char **argv){
	
	table.insert(new TBool());
	table.insert(new TInteger());
	table.insert(new TFloat());
	table.insert(new TUndef());
	table.insert(new TError());
	table.insert(new TChar());

	freopen(argv[1],"r",stdin);


	yyparse();
	void* t;
		programAST->print(std::cout);
	if(programAST!=NULL){
		t=programAST->typeChk(table);
		cout<<"===========AST========"<<endl;
		programAST->print(std::cout);
		cout<<"=====SymbolTable======"<<endl;
		table.print(std::cout);
		iblock = new IBlock();
		programAST->codeGen(iblock);
		cout<<"=========TAC=========="<<endl;
		iblock->print(std::cout);
	}else{
		cout<<"parse error"<<endl;
	}
	log.print(std::cout);
	
	return 0;
}
