LEX    = flex
BISON  = bison
BFLAGS = --output=parser.cpp

comp:	parser.cpp lexer.cpp main.cpp ast.o symtable.o
		g++ $^ -o $@ -lm -ly -std=c++0x

deb: 
		bison -o parser.cpp -vtdy parser.y
		flex -o lexer.cpp  lexer.l parser.hpp
		g++ parser.cpp lexer.cpp main.cpp ast.h symtable.h -o comp -lm -ly -Wno-deprecated -ggdb -DDEBUG

parser.cpp:	parser.y
		bison -o $@ -dy $^

parser.hpp : parser.cpp

symtable.o: symtable.cpp
	g++ $^ -c 
ast.o: ast.cpp
	g++ $^ -c 

lexer.cpp:	lexer.l parser.hpp
		flex -o $@  $^ 

clean:
		rm comp parser.hpp lexer.cpp parser.cpp
