#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>

struct token_location {
	uint64_t index;
	uint64_t row, col;
};

struct token {
	const char* lexeme;
	struct token_span {
		struct token_location start;
		struct token_location end;
	} pos;
	uint64_t length;
	enum {
		T_IDENTIFIER,
		T_KEYWORD,
		T_INTEGER,
		T_FLOAT,
		T_STRING,
		T_CHAR,
		T_PUNCTUATOR,
	} type;
};

bool 
cmp_lexeme(struct token* tok, struct token* other_tok);

void
print_tok(struct token* tok);

bool 
cmp_type(struct token* tok, int type);

bool 
cmp_tok(struct token* tok, struct token* other);

#endif
