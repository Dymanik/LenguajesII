#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include "blahlog.h"
#include "blahast.h"
#include "blahsymtable.h"
#include "tac.h"
#include "spim.h"

extern int yyparse();
extern Log log;
extern Symtable table;
extern NBlock* programAST;
using namespace std;
IBlock* iblock;

struct Config{
	string file;
	bool printTree;
	bool printTAC;
	bool printSym;
	string outfile;
	
	public:
		Config():outfile("out.s"),file(""),printTree(false),printTAC(false),printSym(false){}
};

void printHelp(){
	
	cout<<"./blahc [-o file] [-a] [-s] [-t] file "<<endl;

	cout<<"\t-a\tprints the AST"<<endl;
	cout<<"\t-s\tprints the symbol table"<<endl;
	cout<<"\t-t\tprints the TAC generated"<<endl;
	cout<<"\t-o file\tgenerate spim in file file.s,"<<endl;
	
	exit(EXIT_FAILURE);
}

void parseArgs(int argc ,char **argv, Config &opts){

	if (argc==1) printHelp();

	for (int i=1;i<argc;i++){
		if(argv[i][0]=='-'){
			switch(argv[i][1]){
				case 'a':
					opts.printTree=true;
					break;
				case 's':
					opts.printSym=true;
					break;
				case 't':
					opts.printTAC=true;
					break;
				case 'o':
					if(i+1<argc){
						opts.outfile=string(argv[i+1])+".s";
					} else {
						printHelp();
					}
					break;
			}
		} else{
			if(opts.file!=""){
				cout<<"Error: more than one file especified"<<endl;
				exit(EXIT_FAILURE);
			}
			opts.file=string(argv[i]);
		}
	}

}


int main(int argc, char **argv){

	Config opts;
	parseArgs(argc,argv,opts);
	

	/*Insercion de tipos basicos*/
	table.insert(new TBool());
	table.insert(new TInteger());
	table.insert(new TFloat());
	table.insert(new TUndef());
	table.insert(new TError());
	table.insert(new TChar());
	table.insert(new TVoid());

	{
	vector<TType*> t;
	t.push_back(table.lookupType("Integer"));
	table.insert(new TFunc(string("print"),*table.lookupType("Void"),t));
	t.clear();
	t.push_back(table.lookupType("Float"));
	table.insert(new TFunc(string("print"),*table.lookupType("Void"),t));
	t.clear();
	table.insert(new TFunc(string("readI"),*table.lookupType("Integer"),t));
	table.insert(new TFunc(string("readF"),*table.lookupType("Float"),t));
	}
	if(freopen(opts.file.c_str(),"r",stdin)==NULL){
		std::cerr<<"Failed to open "<<opts.file<<std::endl;
		exit(EXIT_FAILURE);	
	};

	yyparse();
	if(programAST!=NULL){
		if(programAST->typeChk(table)->name!="error"){
			if(opts.printTree){
				cout<<"===========AST========"<<endl;
				programAST->print(std::cout);
			}
			if(opts.printSym){
				cout<<"=====SymbolTable======"<<endl;
				table.print(std::cout);
			}
			TAC tac =TAC();
			if(!tac.ASTtoTAC(programAST)){
				log.print(std::cout);
				exit(EXIT_FAILURE);	
			}

			if(opts.printTAC){
				cout<<"=========TAC=========="<<endl;
				tac.print(std::cout);
			}
		
			SPIM* spim = new SPIM(&tac);
			ofstream output;
			output.open(opts.outfile);
			spim->print(output);

		}else{
			std::cout<<"Type Error"<<std::endl;
		}
	}else{
		std::cout<<"Syntax Error"<<std::endl;
	}
	log.print(std::cout);

	return 0;
}
