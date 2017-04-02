#include <stdlib.h>

#include "stack_frame.h"
#include "virtual_thread.h"

struct stack_frame* 
push_frame(struct virtual_thread* thread) {
	struct stack_frame* new_frame = malloc(sizeof(*new_frame));
	new_frame->parent = thread->current_frame;
	thread->current_frame = new_frame;
	return new_frame;
}

struct stack_frame* 
pop_frame(struct virtual_thread* thread) {
	struct stack_frame* prev_frame = thread->current_frame;
	thread->current_frame = prev_frame->parent;
	return prev_frame;
}