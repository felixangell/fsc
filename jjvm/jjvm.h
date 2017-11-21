#ifndef JJVM_H
#define JJVM_H

struct array_list;
struct virtual_thread;

struct jjvm_inst {
	// our stack of virtual threads
	struct array_list* stack;
	struct virtual_thread* curr_thrd;
};

void 
execute_instructions(struct jjvm_inst* instance, struct array_list* instructions);

#endif
