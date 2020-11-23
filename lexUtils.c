#include "lexUtils.h"
#include "khash.h"

KHASH_MAP_INIT_STR(StrMap, char *)

typedef khash_t(StrMap) StringsMap;

StringsMap * stringsMap;

static void init_static_string_handler();
static void finalize_static_string_handler();

void initializeLex(){
	init_static_string_handler();
}

void finalizeLex(){
	finalize_static_string_handler();
}

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
	
	string[len]=0;
	
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
		print_and_abort("Error storing string in stringMap");
	}

    return kh_value(stringsMap, k) = newStr;
}

static void init_static_string_handler() {
	stringsMap = kh_init(StrMap);
}

static void finalize_static_string_handler() {
	
	for (khiter_t k = kh_begin(stringsMap); k != kh_end(stringsMap); ++k){
        if (kh_exist(stringsMap, k)){
            free(kh_value(stringsMap, k));
        }
    }

	kh_destroy(StrMap, stringsMap);
}