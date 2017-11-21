#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "array_list.h"
#include "parse.h"
#include "token.h"

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
