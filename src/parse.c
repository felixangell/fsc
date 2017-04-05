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

static struct ast_node*
parse_node(struct parser* p) {
	struct token* fst = next(p);
	if (cmp_lexeme(fst, "class")) {
		
	}
	return NULL;
}

void 
parse(struct array_list* tokens) {
	struct parser p = {0};
	p.tokens = tokens;


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