CC ?= gcc
CFLAGS ?= -Wall -pedantic -std=c99 -ggdb -fsanitize=address -fno-omit-frame-pointer -I$(PWD)

COMPILER_UTILS_PATH=compilerUtils/*
COMPILER_UTILS_SOURCE_FILES = $(foreach dir, $(COMPILER_UTILS_PATH), $(wildcard $(dir)/[^_]*.c))
COMPILER_UTILS_OBJECTS = $(patsubst %.c, %.o, $(COMPILER_UTILS_SOURCE_FILES))

PIPO_FUNCTIONS_PATH=$(wildcard pipoScriptFunctions/*.c)
PIPO_FUNCTIONS_OBJECTS = $(patsubst %.c, %.o, $(PIPO_FUNCTIONS_PATH))

LEX = lex.yy.c
YACC = y.tab.c
COMPILER=pipoCompiler

all: $(COMPILER)

$(COMPILER): $(LEX) $(YACC) $(COMPILER_UTILS_OBJECTS) $(PIPO_FUNCTIONS_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@  -ll 

$(LEX): c.l
	lex $<

$(YACC): c.y
	yacc -v -t -d $< -Wno-yacc

test: $(SOURCES) 
	$(CC) $(CCFLAGS_PICHI) example.c -o example $^ -I $(LIB_PATH)
	./example > example.html
	rm example

clean:
	rm -f $(LEX) $(YACC) $(COMPILER) $(COMPILER_UTILS_OBJECTS)

.PHONY: all cleam