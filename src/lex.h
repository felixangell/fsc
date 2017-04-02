#ifndef LEX_H
#define LEX_H

#include "../util/type.h"

enum token_type {
	T_IDENTIFIER,
	T_WHOLE,
	T_FLOATING,
	T_STRING,
	T_CHARACTER,
	T_SYMBOL,
};

struct token {
	char* lexeme;
	enum token_type type;
};

struct array_list;

struct lexer {
	u64 position;
};

struct array_list* tokenize(struct lexer* lex, char* input);

#endif