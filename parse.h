#ifndef PARSE_H
#define PARSE_H

#include <stdint.h>

struct array;

struct parser {
	uint64_t pos;
	struct array* tokens;
};

struct array* 
parse(struct array* tokens);

#endif
