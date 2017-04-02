#ifndef LEX_H
#define LEX_H

#include "type.h"

struct token {
	const char* lexeme;
	u64 length;
	enum {
		T_IDENTIFIER,
		T_INTEGER,
		T_FLOAT,
		T_STRING,
		T_CHAR,
		T_SYMBOL,
	} type;
};

bool 
cmp_lexeme(struct token* tok, const char* val);

bool 
cmp_type(struct token* tok, int type);

bool 
cmp_tok(struct token* tok, struct token* other);

struct array_list;
struct compilation_unit;

struct lexer {
	struct compilation_unit* unit;
	u64 pos;
};

struct array_list* 
tokenize(struct lexer* lex, struct compilation_unit* unit);

#endif