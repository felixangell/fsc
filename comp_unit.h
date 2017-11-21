#ifndef COMP_UNIT_H
#define COMP_UNIT_H

#include "type.h"

struct compilation_unit {
	const char* path;
	u64 length;
	char* contents;
};

#endif
