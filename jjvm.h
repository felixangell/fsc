#ifndef JJVM_H
#define JJVM_H

#include <collectc/array.h>

struct virtual_thread;

struct jjvm_inst {
	Array* stack;
	struct virtual_thread* curr_thrd;
};

void 
execute_instructions(struct jjvm_inst* instance, Array* instructions);

#endif
