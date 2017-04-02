#ifndef VIRTUAL_THREAD_H
#define VIRTUAL_THREAD_H

#include <stdint.h>

#define KILOBYTE 1000
#define MEGABYTE (KILOBYTE * KILOBYTE)

#define STACK_SIZE (1 * MEGABYTE)
#define DATASEGMENT_SIZE (1 * MEGABYTE)

struct virtual_thread {
	unsigned int stack_ptr;
	uint8_t globals[DATASEGMENT_SIZE];
	struct stack_frame* current_frame;
	unsigned int program_counter;
};

struct stack_frame* 
push_frame(struct virtual_thread* parent_thread);

struct stack_frame* 
pop_frame(struct virtual_thread* parent_thread);

#endif