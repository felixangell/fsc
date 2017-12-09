#include "jjvm.h"
#include "instr.h"
#include "virtual_thread.h"

static struct virtual_thread* MAIN_THREAD;

enum {
	// pushes a new stack frame
	ENTR,

	// pops the current frame and jumps
	// to the return addr specified
	RET,

	// pushes the value specified in the first
	// operand onto the stack
	PSH,
	PSHS,
	PSHI,
	PSHL,

	// 
	POP,
	POPS,
	POPI,
	POPL,

	STR,
	STRS,
	STRI,
	STRL,

	LD,
	LDS,
	LDI,
	LDL,

	ADD,
	ADDS,
	ADDI,
	ADDL,

	NCALL,
	GOTO,
	JNE,

	CMP,
	CMPS,
	CMPI,
	CMPL,
};

void 
execute_instructions(struct jjvm_inst* inst, Array* instrs) {
	struct virtual_thread* curr = inst->curr_thrd;

	while (curr->pc < array_size(instrs)) {
		struct jjvm_instruction* instr;
		array_get_at(instrs, curr->pc++, (void*) &instr);

		switch (instr->id) {
			// TODO:
		}
	}
}
