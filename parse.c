#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "array_list.h"
#include "parse.h"
#include "token.h"

/*
	primary_expr = 
		identifier
		constant
		string-literal
		( expr );

	postfix_expr =
		primary_expr
		postfix_expr [ expr ]
		postfix_expr ( arg_expr_list )
		postfix_expr . identifier
		postfix_expr -> identifier
		postfix_expr ++
		postfix_expr --
		( type_name ) { initializer_list }
		( type_name ) { initializer_list , };

	arg_expr_list =
		assignment_expr
		arg_expr_list , assignment_expr;

	unary_operator = & * - ~ !

	multiplicative_expr =
		cast_expr
		multiplicative_expr * cast_expr
		multiplicative_expr / cast_expr
		multiplicative_expr % cast_expr;

	additive_expr =
		multiplicative_expr
		additive_expr + multiplicative_expr
		additive_expr - multiplicative_expr;

	shift_expr =
		additive_expr
		shift_expr << additive_expr
		shift_expr >> additive_expr;

	relational_expr = 
		shift_expr
		relation_expr < shift_expr
		relation_expr > shift_expr
		relation_expr <= shift_expr
		relation_expr >= shift_expr;

	equality_expr =
		relational_expr
		equality_expr == relational_expr
		equality_expr != relational_expr;

	AND_expr =
		equality_expr
		AND_expr & equality_expr;

	exclusive_OR_expr =
		AND_expr
		exclusive_OR_expr ^ AND_expr;

	inclusive_OR_expr =
		exclusive_OR_expr
		inclusive_OR_expr | exclusive_OR_expr;

	logical_AND_expr =
		inclusive_OR_expr
		logical_AND_expr && inclusive_OR_expr;

	logical_OR_expr =
		logical_AND_expr
		logical_OR_expr || logical_AND_expr;

	conditional_expr =
		logical_OR_expr
		logical_OR_expr ? expr : conditional_expr;

	expr =
		assignment_expr
		expr , assignment_expr;

	constant_expr = 
		conditional_expr;

	unary_expr = 
		postfix_expr
		++ unary_expr
		-- unary_expr
		unary_operator cast_expr
		sizeof unary_expr
		sizeof ( type_name );

	declaration =
		declaration_spec init_declarator_list;

	storage_class_spec = 
		typedef
		extern
		static
		auto
		register;

	type_specifier =
		void
		char
		short
		int
		long
		float
		double
		signed
		unsigned
		_Bool
		_Complex
		struct_or_union_spec
		enum_spec
		typedef_name;

	struct_or_union =
		struct
		union;

	declaration_spec =
		storage_class_spec declaration_spec,
		type_specifier declaration_spec,
		type_qualifer declaration_spec,
		function_spec declaration_spec;

	init_decl_list =
		init_decl
		init_decl_list , init_decl;

	init_decl =
		declarator
		declarator = initializer;
*/

static struct token* 
peek(struct parser* p, int offs) {
	return p->tokens->items[p->pos + offs];
}

static inline struct token*
next(struct parser* p) {
	return peek(p, 0);
}

static struct token*
consume(struct parser* p) {
	struct token* t = peek(p, 0);
	p->pos += 1;
	return t;
}

static inline struct token*
expect_lexeme(struct parser* p, const char* lexeme) {
	if (cmp_lexeme(next(p), lexeme)) {
		return consume(p);
	}
	printf("error: expected '%s'\n", lexeme);
	return NULL;
}

static struct ast_node*
parse_struct(struct parser* p) {
	expect_lexeme(p, "struct");
	return NULL;
}

static struct ast_node*
parse_enum(struct parser* p) {
	expect_lexeme(p, "enum");	
	return NULL;
}

static struct ast_node*
parse_class(struct parser* p) {
	expect_lexeme(p, "class");
	return NULL;	
}

static struct ast_node*
parse_impl(struct parser* p) {
	expect_lexeme(p, "impl");
	return NULL;
}

static struct ast_node*
parse_node(struct parser* p) {
	struct token* fst = next(p);
	if (cmp_lexeme(fst, "class")) {
		return parse_class(p);
	}
	else if (cmp_lexeme(fst, "struct")) {
		return parse_struct(p);
	}
	else if (cmp_lexeme(fst, "enum")) {
		return parse_enum(p);
	}
	else if (cmp_lexeme(fst, "impl")) {
		return parse_impl(p);
	}

	printf("what is this...?\n");
	print_tok(fst);
	return NULL;
}

void 
parse(struct array_list* tokens) {
	struct parser p = {
		.tokens = tokens,
		.pos = 0,
	};

	// maybe measure and make
	// this a reasonable assumption
	// but for now it'll do
	struct array_list* ast_nodes = array_list_make(tokens->length / 4);
	while (p.pos < tokens->length) {
		struct ast_node* node = parse_node(&p);
		if (node != NULL) {
			array_list_push(ast_nodes, node);
		}
	}
}
