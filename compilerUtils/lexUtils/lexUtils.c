#include "lexUtils.h"

#include "libraries/khash.h"
#include "utils/errorHandling.h"
#include "compilerUtils/argsHandler/args.h"

KHASH_MAP_INIT_STR(StrMap, char *)

typedef khash_t(StrMap) StringsMap;

StringsMap * stringsMap;

extern int yylex_destroy(void);

static void init_static_string_handler();
static void finalize_static_string_handler();

void initializeLex(){
	init_static_string_handler();
}

void finalizeLex(){
	finalize_static_string_handler();

	// Since Flex 2.5.9
	yylex_destroy();
}

void multiLineComment(int (*input)(void)) {
	char c, prev = 0;

	while ((c = input()) != 0)      /* (EOF maps to 0) */
	{
		if (c == '/' && prev == '*')
			return;

		prev = c;

	}

	print_lineno_and_abort(args.inputFiles.currentFilename, yylineno, 3, "Unterminated comment");
}

char * lex_copy_string(char* string, int len) {
	
	string[len] = 0;
	
	khiter_t k = kh_get(StrMap, stringsMap, string);
	int ret;

	if(k != kh_end(stringsMap)) {
		return kh_value(stringsMap, k);
	}

	char *newStr = (char*)emalloc(sizeof(*newStr)*len + 1);
    strncpy(newStr, string, len);
	newStr[len] = 0;

	k = kh_put(StrMap, stringsMap, newStr, &ret);

	if(ret == -1){
		print_and_abort(3, "Error storing string %s in stringMap", string);
	}

    return kh_value(stringsMap, k) = newStr;
}

static void init_static_string_handler() {
	stringsMap = kh_init(StrMap);
}

static void finalize_static_string_handler() {

	char *val;

	kh_foreach_value(stringsMap, val, free(val));

	kh_destroy(StrMap, stringsMap);
}