CC=gcc
CCFLAGS=-Wall -pedantic -std=c99 -ggdb -fsanitize=address -fno-omit-frame-pointer
CCFLAGS_PICHI=-Wall -pedantic -std=c99

LIB_PATH=./libraries
SOURCES=$(wildcard $(LIB_PATH)/*.c)

LIBS_TOBI=pipoUtils/pipoUtils.c symbolTable/symbolTable.c lexUtils.c yaccUtils.c AST/astNodes.c

all:
	lex -v c.l
	yacc -v -t -d c.y
	gcc lex.yy.c y.tab.c $(LIBS_TOBI) -ll $(CCFLAGS) 

test: $(SOURCES) 
	$(CC) $(CCFLAGS) example.c -o example $^ -I $(LIB_PATH)
	./example > example.html
	rm example