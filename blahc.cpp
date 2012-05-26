#include <iostream>
#include <cstdio>
#include <vector>
#include "blahlog.h"
#include "blahast.h"
#include "blahsymtable.h"

extern int yyparse();
extern Log log;
extern Symtable table;
using namespace std;

int main(int argc, char **argv){

	freopen(argv[1],"r",stdin);

	table.insert(new TBool());
	table.insert(new TInteger());
	table.insert(new TFloat());
	table.insert(new TUndef());
	table.insert(new TError());
	table.insert(new TChar());


	yyparse();
	log.print(std::cout);
	
	return 0;
}
