all:
	lex -v c.l
	yacc -v -t -d c.y
	gcc lex.yy.c y.tab.c -ll