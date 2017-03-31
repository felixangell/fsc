#ifndef STACK_FRAME_H
#define STACK_FRAME_H

#define LOCAL_MEMORY_SIZE 1024

struct virtual_thread;

struct stack_frame {
	struct virtual_thread* parent;
	struct stack_frame* frame;

	uint8_t locals[LOCAL_MEMORY_SIZE];
	int local_index;

	int return_addr;
};

#endif