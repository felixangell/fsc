#ifndef AST_H
#define AST_H

#include "type.h"

struct ast_node {
	union {
		struct class_node {
			
		};
		struct function_node {

		};
	} node;
	u32 type;
};

#endif