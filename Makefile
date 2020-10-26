CC=gcc
CCFLAGS=-Wall -pedantic -std=c99 -ggdb -fsanitize=address -fno-omit-frame-pointer
CCFLAGS_PICHI=-Wall -pedantic -std=c99

LIB_PATH=./libraries
SOURCES=$(wildcard $(LIB_PATH)/*.c)

all:
	lex -v c.l
	yacc -v -t -d c.y
	gcc lex.yy.c y.tab.c -ll

test: $(SOURCES) 
	$(CC) $(CCFLAGS) example.c -o example $^ -I $(LIB_PATH)
	./example > example.html
	rm example