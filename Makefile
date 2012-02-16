LEX    = flex
BISON  = bison
BFLAGS = --output=parser.cpp

deb: 
		bison -o parser.cpp -vtdy parser.y
		flex -o lexer.cpp  lexer.l parser.hpp
		g++ parser.cpp lexer.cpp main.cpp ast.h symtable.h -o comp -lm -ly -Wno-deprecated

comp:	parser.cpp lexer.cpp main.cpp ast.h symtable.h
		g++ $^ -o $@ -lm -ly -Wno-deprecated

parser.cpp:	parser.y
		bison -o $@ -dy $^

parser.hpp : parser.cpp

lexer.cpp:	lexer.l parser.hpp
		flex -o $@  $^ 

clean:
		rm comp parser.hpp lexer.cpp parser.cpp
