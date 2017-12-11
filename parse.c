#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <collectc/hashset.h>

#include "ast.h"
#include "parse.h"
#include "token.h"

/*
	https://cs.wmich.edu/~gupta/teaching/cs4850/sumII06/The%20syntax%20of%20C%20in%20Backus-Naur%20form.htm
	http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf

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

	type_qualifier = 
		const
		atomic;

	declaration_spec =
		storage_class_spec declaration_spec,
		type_specifier declaration_spec,
		type_qualifer declaration_spec,
		function_spec declaration_spec;

	struct_or_union =
		struct
		union;

	init_decl_list =
		init_decl
		init_decl_list , init_decl;

	init_decl =
		declarator
		declarator = initializer;
*/

static struct token* 
peek(struct parser* p, int offs) {
	struct token* t;
	array_get_at(p->tokens, p->pos + offs, (void*) &t);
	return t;
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

static inline bool
is_type(struct token* t) {
	static char* TYPES[] = {
		"_Alignas", "auto", "_Bool", "char",
		"_Complex", "const", "double", "enum",
		"extern", "float", "_Imaginary", "inline",
		"int", "long", "_Noreturn" ,"register", "restrict",
		"short", "signed", "static", "struct", "typedef",
		"typeof", "union", "unsigned", "void", "volatile",
	};
	
	static HashSet* types_table;
	if (types_table == NULL) {
		hashset_new(&types_table);
		for (int i = 0; i < array_len(TYPES); i++) {
			hashset_add(types_table, TYPES[i]);
		}
	}

	char token_lexeme[t->length];
	memcpy(&token_lexeme, t->lexeme, t->length);
	return hashset_contains(types_table, token_lexeme);
}

static void parse_decl_spec(struct type* base_type) {

}

/*
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

	type_qualifier = 
		const
		atomic;

	declaration_spec =
		storage_class_spec declaration_spec,
		type_specifier declaration_spec,
		type_qualifer declaration_spec,
		function_spec declaration_spec;

*/
static void parse_decl() {
	struct type base_type;
	parse_decl_spec(&base_type);

	for (;;) {
		
	}
}

static struct ast_node*
parse_node(struct parser* p) {
	for (;;) {

	}
	return NULL;
}

Array* 
parse(Array* tokens) {
	struct parser p = {
		.tokens = tokens,
		.pos = 0,
	};

	if ((void*)1 == (void*)1) {
		return NULL;
	}

	// maybe measure and make
	// this a reasonable assumption
	// but for now it'll do
	Array* ast_nodes;
	array_new(&ast_nodes);
	while (p.pos < array_size(tokens)) {
		struct ast_node* node = parse_node(&p);
		if (node != NULL) {
			array_add(ast_nodes, node);
		}
	}

	return ast_nodes;
}
