#include <iostream>
#include <cstdio>
#include <vector>
#include "blahlog.h"
#include "blahast.h"
#include "blahsymtable.h"

extern int yyparse();
using namespace std;

int main(int argc, char **argv){

	yyparse();
	
	return 0;
}
