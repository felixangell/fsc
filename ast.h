#ifndef AST_H
#define AST_H

#include "type.h"

struct type_node {
	union {
		// type
		// e.g. int, float, double, some_structure
		struct token* primitive;
		
		// type [name] (type name, type name)
		struct {
			struct type_node* return_type;
			struct named_unit* name;
			struct array_list* types;
			struct array_list* names;
		} function;

		// {type [name], type [name], type [name]}
		struct {
			struct array_list* types;
			struct array_list* names;
		} structure;
		
		// (type [name], type [name], type [name])
		struct {
			struct array_list* types;
		} tuple;

		// [type]
		struct {
			struct type_node* base;
		} array;

		// &type
		struct {
			struct type_node* base;
		} reference;

		// type*
		struct {
			struct type_node* base;
		} pointer;
	};
	u32 kind;
};

struct named_unit {
	bool32 mutable;
	struct token* name;
};

struct block_node {
	struct array_list* nodes;
};

struct ast_node {
	union {
		struct function_decl_node {
			struct named_unit* name;
			struct type_node* function_type;
			struct block_node* body;
		} function_decl;
		struct {
			struct token* name;
			struct array_list* methods;
		} impl_decl;
	} node;
	u32 type;
};

#endif
