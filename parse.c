#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "array.h"
#include "ast.h"
#include "parse.h"
#include "token.h"
#include "grammar.h"

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

	char token_lexeme[t->length];
	memset(&token_lexeme, 0, t->length);
	memcpy(&token_lexeme, t->lexeme, t->length);

	#define array_len(x) (sizeof(x) / sizeof(x[0]))

	for (int i = 0; i < array_len(TYPES); i++) {
		if (!strncmp(token_lexeme, TYPES[i], t->length)) {
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
	uint64_t starting_pos = p->pos;

	bool func_def = false;
	for (;;) {
		struct token* tok = next(p);

		if (scmp_token(tok, ";")) {
			break;
		}

		if (is_type(tok)) {
			consume(p);
			continue;
		}

		if (scmp_token(tok, "(")) {
			skip_parens(p);
			continue;
		}

		if (tok->type != T_IDENTIFIER) continue;

		if (scmp_token(tok, "(")) {
			continue;
		}
		skip_parens(p);

		func_def = scmp_token(tok, "{") || is_type(next(p));
		break;
	}

	p->pos = starting_pos;
	return func_def;
}

static void 
parse_func_def(struct parser* p, struct ast_node* node) {
	printf("parsing func def!\n");
}

static struct ast_node*
parse_node(struct parser* p) {
	// this is kind of annoying because
	// we have to do this check multiple times
	struct ast_node* node = malloc(sizeof(*node));
	if (is_func_def(p)) {
		parse_func_def(p, node);
		node->kind = AST_FUNC_DEF;
		return node;
	} else {
		parse_decl(p, node);
		node->kind = AST_DECL_SPEC;
		return node;
	}

	printf("what just happened?!\n");
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
		exit(0);
	}

	return ast_nodes;
}
