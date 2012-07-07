
GCC=g++
LEX=flex
BISON=bison

CFLAGS=-std=c++0x -O3 
CFLAGS+=$(CFLAGSADD)
FILES = blahsymtable\
		typechk\
		printast\
		blahlog\
		printsymtable\
		genTAC\
		blahblock\
		blahinstruction\
		constantFold


blahc: blahc.cpp blahparser.cpp blahlexer.cpp ${FILES:%=%.o}
	g++ $^ -o $@ ${CFLAGS} 

dbg:
	make clean
	make CFLAGSADD=-ggdb

blahparser.cpp: blahparser.y
	${BISON} -dy -o $@ $^

blahlexer.cpp: blahlexer.l
	${LEX} -o $@ $^

%.o: %.cpp %.h
	g++ $< -c ${CFLAGS}

printast.o: 	printast.cpp 		blahast.h
	g++ $< -c ${CFLAGS}
genTAC.o: 		genTAC.cpp 			blahast.h
	g++ $< -c ${CFLAGS}
typechk.o: 		typechk.cpp 		blahast.h
	g++ $< -c ${CFLAGS}
blahblock.o:	blahblock.cpp 		blahblock.h
	g++ $< -c ${CFLAGS}
printsymtable.o:printsymtable.cpp 	blahsymtable.h
	g++ $< -c ${CFLAGS}
constantFold.o:constantFold.cpp		blahast.h
	g++ $< -c ${CFLAGS}




clean:
		rm -f blahparser.hpp blahlexer.cpp blahparser.cpp *.o blahc
