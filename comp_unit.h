#ifndef COMP_UNIT_H
#define COMP_UNIT_H

#include <stdint.h>

struct compilation_unit {
	const char* path;
	uint64_t length;
	char* contents;
};

#endif
