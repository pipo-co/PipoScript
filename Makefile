CC ?= gcc
XFLAGS ?= -ggdb -fsanitize=address -fno-omit-frame-pointer
CFLAGS = -Wall -pedantic -Wno-newline-eof -D_POSIX_C_SOURCE -std=c99 -I$(PWD) $(XFLAGS)

COMPILER_UTILS_PATH=compilerUtils/*
COMPILER_UTILS_SOURCE_FILES = $(foreach dir, $(COMPILER_UTILS_PATH), $(wildcard $(dir)/[^_]*.c))
COMPILER_UTILS_OBJECTS = $(patsubst %.c, %.o, $(COMPILER_UTILS_SOURCE_FILES))

PIPO_FUNCTIONS_PATH=$(wildcard pipoScriptFunctions/*.c)
PIPO_FUNCTIONS_OBJECTS = $(patsubst %.c, %.o, $(PIPO_FUNCTIONS_PATH))

UTILS_PATH=$(wildcard utils/*.c)
UTILS_OBJECTS = $(patsubst %.c, %.o, $(UTILS_PATH))

EXAMPLES_DIRS = $(filter-out $(wildcard examples/*.html), $(wildcard examples/*))
EXAMPLES_OUTPUTS = $(patsubst %, %.html, $(EXAMPLES_DIRS))

OBJECTS = $(COMPILER_UTILS_OBJECTS) $(PIPO_FUNCTIONS_OBJECTS) $(UTILS_OBJECTS)

LEX = lex.yy.c
YACC = y.tab.c
COMPILER ?= ./pipoCompiler

all: $(COMPILER)

$(COMPILER): $(LEX) $(YACC) $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(LEX): pipoScript.l
	lex $<

$(YACC): pipoScript.y
	yacc -v -t -d $< -Wno-yacc

errors: FORCE $(COMPILER)
	cd errors; COMPILER="../$(COMPILER) $(P_FLAGS)" ./run.sh
	
examples: FORCE $(EXAMPLES_OUTPUTS)

%.html: % $(COMPILER)
	$(COMPILER) $(P_FLAGS) -o $@ $(wildcard $</*.pipo)

clean:
	$(RM) $(LEX) y.output y.tab.h $(YACC) $(COMPILER) $(OBJECTS) $(EXAMPLES_OUTPUTS)

FORCE:

.PHONY: all clean examples FORCE