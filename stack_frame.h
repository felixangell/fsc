#ifndef STACK_FRAME_H
#define STACK_FRAME_H

#include "type.h"

#define LOCAL_MEMORY_SIZE 1024

struct virtual_thread;

struct stack_frame {
	struct virtual_thread* parent_thread;
	struct stack_frame* parent;

	u8 locals[LOCAL_MEMORY_SIZE];
	u64 local_index;
	u64 return_addr;
};

#endif
