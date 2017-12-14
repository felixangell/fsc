#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <collectc/hashset.h>
#include <assert.h>

#include "ast.h"
#include "parse.h"
#include "token.h"
#include "grammar.h"

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

static struct token*
next(struct parser* p) {
	return peek(p, 0);
}

static struct token*
consume(struct parser* p) {
	struct token* t = peek(p, 0);
	p->pos += 1;
	return t;
}

static struct token*
expect_lexeme(struct parser* p, const char* lexeme) {
	const struct token* next_tok = next(p);
	if (!strncmp(next_tok->lexeme, lexeme, next_tok->length)) {
		return consume(p);
	}
	printf("error: expected '%s'\n", lexeme);
	return NULL;
}

static bool
is_type(struct token* t) {
	static char* TYPES[] = {
		"_Alignas", "auto", "_Bool", "char",
		"_Complex", "const", "double", "enum",
		"extern", "float", "_Imaginary", "inline",
		"int", "long", "_Noreturn" ,"register", "restrict",
		"short", "signed", "static", "struct", "typedef",
		"typeof", "union", "unsigned", "void", "volatile",
	};
	
	#define array_len(x) (sizeof(x) / sizeof(x[0]))

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

// notes:
// at least one type specifier
// at most one storage-class-specifier
// 

static void
parse_decl_spec(struct parser* p, struct decl_spec* specs) {
	char keyword[512] = {0};

	for (int i = 0;;i++) {
		struct token* tok = next(p);
		memcpy(keyword, tok->lexeme, tok->length);
		keyword[tok->length + 1] = 0;

		struct decl_spec spec;
		if (is_type_qualifier(keyword)) {
			printf("%s is a type qualifier!\n", keyword);
			spec.type = DS_TYPE_QUALIFIER;
		} else if (is_type_specifier(keyword)) {
			printf("%s is a type specifier!\n", keyword);
			spec.type = DS_TYPE_SPECIFIER;
		} else if (is_storage_class_specifier(keyword)) {
			printf("%s is a storage class specifier!\n", keyword);
			spec.type = DS_STORAGE_CLASS;
		} else {
			return;
		}

		spec.t = consume(p);
		*(specs + i) = spec;
	}
}

static bool 
parse_decl(struct parser* p, struct ast_node* node) {
	assert(node != NULL);

	struct token* tok = next(p);

	// technically this assumption holds well because
	// keywords are guaranteed to be no longer than
	// 512 characters!
	// -
	// we need to convert the lexeme into a null
	// terminated string for it to be validly looked up
	// in the hash set.	
	char keyword[512] = {0};
	memcpy(keyword, tok->lexeme, tok->length);

	// FIXME, dynamically allocate this?
	// we have a limit of 32 specs at most here.
	struct decl_spec specs[32] = {0};
	parse_decl_spec(p, specs);

	for (int i = 0; i < array_len(specs); i++) {
		if (specs[i].t != NULL) {
			print_tok(specs[i].t);
		}
	}

	return false;
}

static struct ast_node*
parse_node(struct parser* p) {
	// this is kind of annoying because
	// we have to do this check multiple times
	struct ast_node* node = malloc(sizeof(*node));
	if (!parse_decl(p, node)) {
		node->kind = AST_DECL_SPEC;
		return node;
	}

	printf("what just happened?!\n");
	print_tok(next(p));
	exit(1);
	return NULL;
}

Array* 
parse(Array* tokens) {
	struct parser p = {
		.tokens = tokens,
		.pos = 0,
	};

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
