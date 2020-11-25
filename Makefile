CC ?= gcc
CFLAGS ?= -Wall -pedantic -std=c99 -ggdb -fsanitize=address -fno-omit-frame-pointer -I$(PWD)

COMPILER_UTILS_PATH=compilerUtils/*
COMPILER_UTILS_SOURCE_FILES = $(foreach dir, $(COMPILER_UTILS_PATH), $(wildcard $(dir)/[^_]*.c))
COMPILER_UTILS_OBJECTS = $(patsubst %.c, %.o, $(COMPILER_UTILS_SOURCE_FILES))

PIPO_FUNCTIONS_PATH=$(wildcard pipoScriptFunctions/*.c)
PIPO_FUNCTIONS_OBJECTS = $(patsubst %.c, %.o, $(PIPO_FUNCTIONS_PATH))

EXAMPLES_DIRS = $(filter-out $(wildcard examples/*.html), $(wildcard examples/*))
EXAMPLES_OUTPUTS = $(patsubst %, %.html, $(EXAMPLES_DIRS))

LEX = lex.yy.c
YACC = y.tab.c
COMPILER=./pipoCompiler

all: $(COMPILER)

$(COMPILER): $(LEX) $(YACC) $(COMPILER_UTILS_OBJECTS) $(PIPO_FUNCTIONS_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(LEX): c.l
	lex $<

$(YACC): c.y
	yacc -v -t -d $< -Wno-yacc

examples: FORCE $(EXAMPLES_OUTPUTS)

errors: FORCE $(COMPILER)
	cd errors; COMPILER=../$(COMPILER) ./run.sh
	

%.html: % $(COMPILER)
	$(COMPILER) -o $@ $(wildcard $</*.pipo)

clean:
	rm -f $(LEX) y.output y.tab.h $(YACC) $(COMPILER) $(COMPILER_UTILS_OBJECTS) $(PIPO_FUNCTIONS_OBJECTS) $(EXAMPLES_OUTPUTS)

FORCE:

.PHONY: all clean examples FORCE