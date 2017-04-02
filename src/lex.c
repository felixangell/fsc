#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "array_list.h"
#include "lex.h"
#include "comp_unit.h"

#define arr_len(x) (int) (sizeof((x)) / sizeof((x)[0]))

bool 
cmp_lexeme(struct token* tok, const char* val) {
	for (uint64_t i = 0; i < tok->length; i++) {
		if (tok->lexeme[i] != val[i]) {
			return false;
		}
	}
	return true;
}

bool 
cmp_type(struct token* tok, int type) {
	return tok->type == type;	
}

bool 
cmp_tok(struct token* tok, struct token* other) {
	return cmp_lexeme(tok, other->lexeme) && cmp_type(tok, other->type);
}

static inline char 
peek(struct lexer* lex) {
	return lex->unit->contents[lex->pos];
}

static inline char 
peek_at(struct lexer* lex, int offs) {
	return lex->unit->contents[lex->pos + offs];
}

static char 
consume(struct lexer* lex) {
	return lex->unit->contents[lex->pos++];
}

static inline bool 
is_eof(struct lexer* lex) {
	return lex->pos >= lex->unit->length;
}

static struct token* 
consume_while(struct lexer* lex, bool (*predicate)(char)) {
	uint64_t initial = lex->pos;
	while (!is_eof(lex) && predicate(peek(lex))) {
		consume(lex);
	}
	struct token* tok = malloc(sizeof(*tok));
	tok->lexeme = &lex->unit->contents[initial];
	tok->length = lex->pos - initial;
	return tok;
}

static char 
expect(struct lexer* lex, char c) {
	if (peek(lex) == c) {
		return consume(lex);
	}
	fprintf(stderr, "error: unexpecteed symbol '%c'\n", c);
	exit(1);
}

static inline bool 
is_identifier(char c) {
	return isalnum(c) || c == '_';
}

static struct token* 
recognize_identifier(struct lexer* lex) {
	struct token* tok = consume_while(lex, is_identifier);
	tok->type = T_IDENTIFIER;
	return tok;
}

static inline bool 
is_number(char c) {
	return (c >= '0' && c <= '9');
}

static struct token* 
recognize_number(struct lexer* lex) {
	struct token* number = consume_while(lex, is_number);
	number->type = T_INTEGER;
	if (peek(lex) == '.') {
		expect(lex, '.');
		struct token* part = consume_while(lex, is_number);
		number->length += part->length + 1; // + 1 is the dot we consumed.
		number->type = T_FLOAT;
	}
	return number;
}

static inline bool 
is_layout(char c) {
	return (c <= ' ');
}

static void 
skip_layout(struct lexer* lex) {
	consume_while(lex, is_layout);
}

static struct token* 
consume_amount(struct lexer* lex, int offs) {
	uint64_t initial = lex->pos;
	for (int i = 0; i < offs; i++) {
		consume(lex);
	}
	struct token* tok = malloc(sizeof(*tok));
	tok->lexeme = &lex->unit->contents[initial];
	tok->length = lex->pos - initial;
	return tok;
}

bool 
has_prefix(const char* str, const char* what) {
	for (size_t i = 0; i < strlen(what); i++) {
		if (str[i] != what[i]) {
			return false;
		}
	}
	return true;
}

static inline int 
get_symbol_size(struct lexer* lex) {
	static char* keywords[] = {
	"<<=", ">>=", "...",
	"<<", ">>", "<=", ">=", "==", "!=", "&&", "||",
	"*=", "/=", "%%=", "+=", "-=", "&=", "^=", "|="
	};

	char* three = &lex->unit->contents[lex->pos];
	for (int i = 0; i < arr_len(keywords); i++) {
		if (has_prefix(three, keywords[i])) {
			return strlen(keywords[i]);
		}
	}
	return 1;
}

// NOTE: when lexing strings or characters
// the surrounding quotes are discarded.
static struct token* 
recognize_symbol(struct lexer* lex) {
	return consume_amount(lex, get_symbol_size(lex));
}

static inline bool 
is_not_string_closer(char c) {
	return (c != '"');
}

static struct token* 
recognize_string(struct lexer* lex) {
	expect(lex, '"');
	struct token* str_tok = consume_while(lex, is_not_string_closer);
	expect(lex, '"');
	str_tok->type = T_STRING;
	return str_tok;
}

static inline bool 
is_not_char_closer(char c) {
	return (c != '\'');
}

static struct token* 
recognize_character(struct lexer* lex) {
	expect(lex, '\'');
	// FIXME: this is done for now by just eating everything
	// inbetween single quotes.
	struct token* char_tok = consume_while(lex, is_not_char_closer);
	expect(lex, '\'');
	char_tok->type = T_CHAR;
	return char_tok;
}

void 
skip_line(struct lexer* lex) {
	while (peek(lex) != '\n') {
		consume(lex);
	}
	expect(lex, '\n');
}

struct array_list* 
tokenize(struct lexer* lex, struct compilation_unit* unit) {
	struct array_list* tokens = array_list_make(8);
	lex->unit = unit;

	while (lex->pos < unit->length) {
		skip_layout(lex);
		char current = peek(lex);
		
		switch (current) {
		case 'a' ... 'z':
		case 'A' ... 'Z':
			array_list_push(tokens, recognize_identifier(lex));
			break;
		case '/':
			if (peek_at(lex, 1) == '/')
				skip_line(lex);
			else
				array_list_push(tokens, recognize_symbol(lex));
			break;
		case '#': // FIXME
			skip_line(lex);
			break; 
		case '"':
			array_list_push(tokens, recognize_string(lex));
			break;
		case '\'':
			array_list_push(tokens, recognize_character(lex));
			break;
		case '0' ... '9':
			array_list_push(tokens, recognize_number(lex));
			break;
		case '[': case ']': case '(': case ')': case '{': case '}':
		case '.': case '&': case '*': case '+': case '-': case '~':
		case '!': case '%': case '<': case '>': case '^':
		case '|': case '?': case ':': case ';': case '=':
		case ',':
			array_list_push(tokens, recognize_symbol(lex));
			break;
		default:
			if (is_eof(lex)) break;
			fprintf(stderr, "error: %s:%d/%d, illegal character '%c':\n", 
			lex->unit->path, (int) lex->pos, (int) lex->unit->length, current);
			exit(1);  
			break;
		}
	}
	return tokens;
}