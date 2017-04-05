#ifndef LEX_H
#define LEX_H

#include "type.h"

struct array_list;
struct compilation_unit;

struct lexer {
	struct compilation_unit* unit;
	u64 pos;
};

struct array_list* 
tokenize(struct lexer* lex, struct compilation_unit* unit);

#endif