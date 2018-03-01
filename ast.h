#ifndef AST_H
#define AST_H

#include <stdbool.h>

struct type;

enum decl_spec_type {
	DS_STORAGE_CLASS,
	DS_TYPE_SPECIFIER,
	DS_TYPE_QUALIFIER,
	DS_FUNCTION_SPEC,
};

enum node_type {
	/*
	declaration_spec =
		storage_class_spec declaration_spec,
		type_specifier declaration_spec,
		type_qualifer declaration_spec,
		function_spec declaration_spec;
	*/
	AST_DECL_SPEC,
	AST_FUNC_DEF,
};

struct ast_node {
	int kind;
	struct decl_spec {
		struct token* t;
		enum decl_spec_type type;
	} decl_spec;
};

struct type {
	int kind;
	union {
		struct type* ptr;
		struct {
			struct array* fields;
			int offset;
			bool is_union; // false => it's a structure
		} structure;
		struct {
			int offs;
			int size;
		} bit_field;
		struct {
			struct type* ret;
			struct array* params;
			bool variadic;
			// old func style?
		} function;
	};
};

#endif
