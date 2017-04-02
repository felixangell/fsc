#ifndef PARSE_H
#define PARSE_H

#include "type.h"

struct array_list;

struct parser {
	u64 pos;
	struct array_list* tokens;
};

void 
parse(struct array_list* tokens);

#endif