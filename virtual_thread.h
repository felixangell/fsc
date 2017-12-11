#ifndef VIRTUAL_THREAD_H
#define VIRTUAL_THREAD_H

#define KILOBYTE 1000
#define MEGABYTE (KILOBYTE * KILOBYTE)

#define STACK_SIZE (1 * MEGABYTE)
#define DATASEGMENT_SIZE (1 * MEGABYTE)

struct stack_frame;

struct virtual_thread {
	struct stack_frame* curr_frame;
	uint8_t globals[DATASEGMENT_SIZE];
	uint64_t sp;
	uint64_t pc;
};

struct stack_frame* 
push_frame(struct virtual_thread* parent_thread);

struct stack_frame* 
pop_frame(struct virtual_thread* parent_thread);

#endif
