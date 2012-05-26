
GCC=g++
LEX=flex
BISON=bison

CFLAGS=-std=c++0x
CFLAGS+=$(CFLAGSADD)
FILES = blahsymtable\
		typechk\
		printast\
		blahlog


blahc: blahc.cpp blahparser.cpp blahlexer.cpp ${FILES:%=%.o}
	g++ $^ -o $@ ${CFLAGS} 

dbg:
	make clean
	make CFLAGSADD=-ggdb

blahparser.cpp: blahparser.y
	${BISON} -dy -o $@ $^

blahlexer.cpp: blahlexer.l
	${LEX} -o $@ $^

%.o: %.cpp
	g++ $^ -c ${CFLAGS}

clean:
		rm  blahparser.hpp blahlexer.cpp blahparser.cpp *.o
