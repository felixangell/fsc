#include "array_list.h"
#include "virtual_thread.h"

static virtual_thread* MAIN_THREAD = NULL;

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
execute_instructions(struct jjvm_inst* instance, struct array_list* instructions) {
	while (instance->current->program_counter < instructions->capacity) {
		struct jjvm_instruction* instr = instructions->items[instance->current->program_counter++];
		switch (instr->id) {

		}		
	}
}