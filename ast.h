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

struct decl_spec {
	struct token* t;
	enum decl_spec_type type;
} decl_spec;

enum node_type {
	AST_FUNC_DEF,
	AST_DECL,
	AST_EXPR,
};

struct function_node {
	struct decl_spec* spec;
};

struct stat_node {
	int kind;
};

struct expr_node {
	int kind;
};

struct ast_node {
	int kind;
	struct function_node* func;
	struct expr_node* expr;
};

struct type {
	int kind;
	union {
		// *T
		struct type* ptr;
		
		// int, fooba, etc.
		struct token* iden;

		// struct { T name .. }
		struct {
			struct array* fields;
			int offset;
			bool is_union; // false => it's a structure
		} structure;


		struct {
			int offs;
			int size;
		} bit_field;

		// 
		struct {
			struct type* ret;
			struct array* params;
			bool variadic;
			// old func style?
		} function;
	};
};

#endif
