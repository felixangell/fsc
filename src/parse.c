#include "array_list.h"
#include "parse.h"

static struct ast_node*
parse_node(struct parser* p) {

}

void 
parse(struct array_list* tokens) {
	struct parser p;
	while (p.pos < tokens->length) {
		parse_node(&p);
	}
}