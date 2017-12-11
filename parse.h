#ifndef PARSE_H
#define PARSE_H

#include <collectc/array.h>
#include <stdint.h>

struct parser {
	uint64_t pos;
	Array* tokens;
};

Array* 
parse(Array* tokens);

#endif
