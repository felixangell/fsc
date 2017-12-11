#ifndef STACK_FRAME_H
#define STACK_FRAME_H

#include <stdint.h>

#define LOCAL_MEMORY_SIZE 1024

struct virtual_thread;

struct stack_frame {
	struct virtual_thread* parent_thread;
	struct stack_frame* parent;

	uint8_t locals[LOCAL_MEMORY_SIZE];
	uint64_t local_index;
	uint64_t return_addr;
};

#endif
