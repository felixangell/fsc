#ifndef LEX_H
#define LEX_H

#include <collectc/array.h>

#include "type.h"

struct compilation_unit;

struct lexer {
	struct compilation_unit* unit;
	u64 row, col;
	u64 pos;
};

struct lexer_info {
	Array* token_stream;

	// diagnostic information
	u64 lines_lexed;
};

struct lexer_info
tokenize(struct lexer* lex, struct compilation_unit* unit);

#endif
