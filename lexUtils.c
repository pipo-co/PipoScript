#include "lexUtils.h"

void multiLineComment(int (*input)(void)) {
	char c, prev = 0;

	while ((c = input()) != 0)      /* (EOF maps to 0) */
	{
		if (c == '/' && prev == '*')
			return;

		prev = c;

	}
	fprintf(stderr, "Error in line %d : unterminated comment\n", yylineno);
	exit(2);
}

char * lex_copy_string(char* string, int len) {
    char *ans = (char*)emalloc(sizeof(*ans)*len + 1);

    strncpy(ans, string, len + 1);

    return ans;
}