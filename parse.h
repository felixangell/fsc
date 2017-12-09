#ifndef PARSE_H
#define PARSE_H

#include <collectc/array.h>

#include "type.h"

struct parser {
	u64 pos;
	Array* tokens;
};

Array* 
parse(Array* tokens);

#endif
