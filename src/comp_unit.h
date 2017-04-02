#ifndef COMP_UNIT_H
#define COMP_UNIT_H

#include "../util/type.h"

struct compilation_unit {
	const char* path;
	u64 length;
	char* contents;
};

#endif
