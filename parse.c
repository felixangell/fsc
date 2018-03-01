#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "array.h"
#include "ast.h"
#include "parse.h"
#include "token.h"
#include "grammar.h"

static bool
tok_str_cmp(struct token *t, const char* lexeme) {
	size_t len = strlen(lexeme);
	if (len != t->length) {
		return false;
	}
	return !strncmp(t->lexeme, lexeme, len);
}

static struct token* 
peek(struct parser* p, int offs) {
	struct token* t = array_get(p->tokens, p->pos + offs);
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

	for (int i = 0; i < array_len(TYPES); i++) {
		if (tok_str_cmp(t, TYPES[i])) {
			return true;
		}
	}

	return false;
}

static void
parse_decl_spec(struct parser* p, struct decl_spec* specs) {
	char keyword[512] = {0};

	for (int i = 0;;i++) {
		struct token* tok = next(p);
		memcpy(keyword, tok->lexeme, tok->length);
		keyword[tok->length + 1] = 0;

		if (i >= 32) {
			fprintf(stderr, "tell the developer to go read about dynamic memory allocation\n");
			exit(0);
		}

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
		specs[i] = spec;
	}
}

static bool 
parse_decl(struct parser* p, struct ast_node* node) {
	assert(node != NULL);

	return false;
}

// compare a token to the given string
static bool
scmp_token(struct token* t, char* str) {
	return !strncmp(t->lexeme, str, t->length);
}

static bool
has_next(struct parser* p) {
	return p->pos < p->tokens->size;
}

static void
skip_parens(struct parser* p) {
	consume(p); // (
	for (int i = 0; has_next(p) && !scmp_token(next(p), ")"); i++) {
		consume(p);
	}
	consume(p); // )
}

static bool
is_func_def(struct parser* p) {
	uint64_t old_pos = p->pos;

	bool result = false;
	for (;;) {
		struct token* t = next(p);
		if (tok_str_cmp(t, ";")) {
			break;
		}

		if (is_type(t)) {
			consume(p);
			continue;
		}

		if (tok_str_cmp(t, "(")) {
			skip_parens(p);
			continue;
		}

		consume(p);
		skip_parens(p);

		if (tok_str_cmp(next(p), "{") || is_type(next(p))) {
			result = true;
		}
		break;
	}

	p->pos = old_pos;
	return result;
}

static struct array*
parse_types(struct parser* p) {
	struct array* types = new_array(1);

	struct type* last_type = NULL;
	for (;;) {
		struct type* type = malloc(sizeof(*type));

		struct token* t = next(p);

		// normal primitive type
		// e.g int or float, etc.
		if (is_type(t)) {
			type->iden = consume(p);
		} else if (tok_str_cmp(t, "*")) {
			consume(p);
			type->ptr = last_type;

			// pop
			types->size--;
		} else {
			break;
		}

		last_type = type;
		array_add(types, type);
	}

	return types;
}

static struct ast_node*
parse_assign_expr(struct parser* p) {
	printf("parse_assign_expr: \n");
	print_tok(next(p));
	printf("\n");
	return NULL;
}

static struct ast_node*
parse_comma_expr(struct parser* p) {
	struct ast_node* node = parse_assign_expr(p);
	if (!node) {
		fprintf(stderr, "failed to parse_assign_expr\n");
		exit(0);
	}
	return node;
}

static struct ast_node*
parse_expr(struct parser* p) {
	struct ast_node* expr = parse_comma_expr(p);
	if (!expr) {
		fprintf(stderr, "failed to parse_comma_expr\n");
		exit(0);
	}
	return expr;
}

static bool
parse_if(struct parser* p, struct stat_node* stat) {
	if (!tok_str_cmp(next(p), "if")) {
		return false;
	}
	consume(p);

	expect_lexeme(p, "(");
	struct ast_node* condition = parse_expr(p);
	expect_lexeme(p, ")");
}

static bool
parse_stat(struct parser* p, struct stat_node* stat) {
	struct token* t = next(p);
	
	if (tok_str_cmp(t, "if")) {
		return parse_if(p, stat);
	} 
	else if (tok_str_cmp(t, "else")) {
		assert(0 && "unimplemented");		
	} 
	else if (tok_str_cmp(t, "for")) {
		assert(0 && "unimplemented");		
	} 
	else if (tok_str_cmp(t, "while")) {
		assert(0 && "unimplemented");		
	} 
	else if (tok_str_cmp(t, "return")) {
		assert(0 && "unimplemented");
	} 
	else if (tok_str_cmp(t, "break")) {
		assert(0 && "unimplemented");		
	}
	else if (tok_str_cmp(t, "continue")) {
		assert(0 && "unimplemented");
	}

	printf("what is: ");
	print_tok(t);
	return false;
}

static void 
parse_func_def(struct parser* p, struct ast_node* node) {
	node->func = malloc(sizeof(*node->func));

	// parse specs.
	struct decl_spec* specs = malloc(sizeof(*specs));	
	node->func->spec = specs;
	parse_decl_spec(p, specs);

	// parse types.
	struct array* types = parse_types(p);

	struct token* func_name = consume(p);
	if (func_name->type != T_IDENTIFIER) {
		fprintf(stderr, "expected identifier for func name, found: \n");
		print_tok(func_name);
		putchar('\n');
	}

	expect_lexeme(p, "(");
	for (int i = 0; has_next(p) && !tok_str_cmp(next(p), ")"); i++) {
		if (i > 0) {
			expect_lexeme(p, ",");
		}

		struct array* t = parse_types(p);
		struct token* iden = consume(p);
	}
	expect_lexeme(p, ")");

	if (!tok_str_cmp(next(p), "{")) {
		expect_lexeme(p, ";");
		return;
	}

	struct array* stats = new_array(32);

	// parse block!
	expect_lexeme(p, "{");
	for (int i = 0; has_next(p) && !tok_str_cmp(next(p), "}"); i++) {
		struct stat_node* stat = malloc(sizeof(*stat));
		if (!parse_stat(p, stat)) {
			assert(0 && "shit!");
		}
		array_add(stats, stat);
	}
	expect_lexeme(p, "}");
}

static struct ast_node*
parse_node(struct parser* p) {
	// this is kind of annoying because
	// we have to do this check multiple times
	struct ast_node* node = malloc(sizeof(*node));
	if (is_func_def(p)) {
		parse_func_def(p, node);
		node->kind = AST_FUNC_DEF;
		printf("Parsed function: \n");
		return node;
	} else {
		parse_decl(p, node);
		node->kind = AST_DECL;
		printf("Parsed declaration: \n");
		return node;
	}

	printf("stuck on: \n");
	print_tok(next(p));
	exit(1);
	return NULL;
}

struct array* 
parse(struct array* tokens) {
	struct parser p = {
		.tokens = tokens,
		.pos = 0,
	};

	// maybe measure and make
	// this a reasonable assumption
	// but for now it'll do
	struct array* ast_nodes = new_array(32);

	while (p.pos < tokens->size) {
		struct ast_node* node = parse_node(&p);
		if (node != NULL) {
			array_add(ast_nodes, node);
		}
	}

	return ast_nodes;
}
