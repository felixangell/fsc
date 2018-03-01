#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "array.h"
#include "token.h"
#include "lex.h"
#include "comp_unit.h"

#define array_len(x) (sizeof(x) / sizeof(x[0]))

#include "grammar.h"

// GRAMMAR STUFF

bool is_reserved_keyword(const char* keyword) {
	static char* RESERVED_KEYWORDS[] = {
		"auto","break","case","char","const","continue","default","do","double","else","enum","extern","float",
		"for","goto","if","inline","int","long","register","restrict","return","short","signed","sizeof","static",
		"struct","switch","typedef","union","unsigned","void","volatile","while","_Bool","_Complex","_Imaginary"
	};

	for (size_t i = 0; i < array_len(RESERVED_KEYWORDS); i++) {
		if (!strncmp(keyword, RESERVED_KEYWORDS[i], strlen(keyword))) {
			return true;
		}
	}

	return false;
}

bool is_type_qualifier(const char* str) {
	char fst = str[0];
	if (fst == 'c') {
		return !strcmp(str, "const");
	} else if (fst == 'v') {
		return !strcmp(str, "volatile");
	}

	return false;
}

bool is_storage_class_specifier(const char* str) {
	static char* STORAGE_CLASS_SPECIFIER[] = {
		"auto", "register", "static", "extern", "typedef"
	};

	for (size_t i = 0; i < array_len(STORAGE_CLASS_SPECIFIER); i++) {
		if (!strncmp(str, STORAGE_CLASS_SPECIFIER[i], strlen(str))) {
			return true;
		}
	}

	return false;
}

bool is_type_specifier(const char* str) {
	static char* TYPE_SPECIFIERS[] = {
		"void", "char", "short", "int", "long", "float",
		"double", "signed", "unsigned",
	};

	for (size_t i = 0; i < array_len(TYPE_SPECIFIERS); i++) {
		if (!strncmp(str, TYPE_SPECIFIERS[i], strlen(str))) {
			return true;
		}
	}

	return false;
}

static bool
has_prefix(const char* str, const char* what) {
	return !strncmp(str, what, strlen(what));
}

bool 
cmp_lexeme(struct token* tok, struct token* other_tok) {
	return !strncmp(tok->lexeme, other_tok->lexeme, tok->length);
}

bool 
cmp_type(struct token* tok, int type) {
	return tok->type == type;	
}

bool 
cmp_tok(struct token* tok, struct token* other) {
	return cmp_lexeme(tok, other) && cmp_type(tok, other->type);
}

void
print_tok(struct token* tok) {
	/// NOTE:
	/// make sure this is up to date with
	/// token.h!
	static const char* token_types[] = {
		"identifier", "keyword", "integer", "float",
		"string", "character", "punctuator",
	};

	#define TAB "    "
	printf(TAB "'%.*s', %s\n", (int) tok->length, tok->lexeme, token_types[tok->type]);
}

static char 
peek(struct lexer* lex) {
	return lex->unit->contents[lex->pos];
}

static char 
peek_at(struct lexer* lex, int offs) {
	return lex->unit->contents[lex->pos + offs];
}

static char 
consume(struct lexer* lex) {
	char curr = lex->unit->contents[lex->pos];
	if (curr == '\n') {
		lex->row++;
		lex->col = 1;
	}
	lex->col++;
	lex->pos++;
	return curr;
}

static bool 
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

	fprintf(stderr, "error: unexpected symbol '%c'\n", c);
	assert(0);
}

static bool 
is_identifier(char c) {
	return isalnum(c) || c == '_';
}

static struct token* 
recognize_identifier(struct lexer* lex) {
	struct token* tok = consume_while(lex, is_identifier);

	{
		char val[512] = {0};
		memcpy(&val, tok->lexeme, tok->length);
		tok->type = T_IDENTIFIER;
		if (is_reserved_keyword(val)) {
			tok->type = T_KEYWORD;			
		}
	}

	return tok;
}

static bool 
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

static bool 
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

static int 
get_symbol_size(struct lexer* lex) {
	static const char* keywords[] = {
		"<<=", ">>=", "...",
		"<<", ">>", "<=", ">=", "==", "!=", "&&", "||",
		"*=", "/=", "%%=", "+=", "-=", "&=", "^=", "|=",
		"--", "++", "->",
	};

	char symbol[8] = {0};
	memcpy(&symbol, &lex->unit->contents[lex->pos], 3);

	// TODO: we could use a hash set here but for now
	// ill leave it as is.
	for (int i = 0; i < array_len(keywords); i++) {
		if (has_prefix(symbol, keywords[i])) {
			return strlen(keywords[i]);
		}
	}

	return 1;
}

// NOTE: when lexing strings or characters
// the surrounding quotes are discarded.
static struct token* 
recognize_symbol(struct lexer* lex) {
	struct token* tok = consume_amount(lex, get_symbol_size(lex));
	tok->type = T_PUNCTUATOR;
	return tok;
}

static struct token* 
recognize_string(struct lexer* lex) {
	expect(lex, '"');

	uint64_t initial = lex->pos;

	{
		while (!is_eof(lex)) {
			if (peek(lex) == '"') {
				break;
			}

			// FIXME properly handle escape characters.
			if (peek(lex) == '\\') {
				consume(lex);
				consume(lex);
			} else {
				consume(lex);
			}
		}
	}

	struct token* str_tok = malloc(sizeof(*str_tok));
	str_tok->lexeme = &lex->unit->contents[initial];
	str_tok->length = lex->pos - initial;

	expect(lex, '"');
	str_tok->type = T_STRING;
	return str_tok;
}

static bool 
is_not_char_closer(char c) {
	return c != '\'';
}

static struct token* 
recognize_character(struct lexer* lex) {
	expect(lex, '\'');

	uint64_t initial = lex->pos;

	{
		while (!is_eof(lex)) {
			if (peek(lex) == '\'') {
				break;
			}

			// FIXME properly handle escape characters.
			if (peek(lex) == '\\') {
				consume(lex);
				consume(lex);
			} else {
				consume(lex);
			}
		}
	}

	struct token* char_tok = malloc(sizeof(*char_tok));
	char_tok->lexeme = &lex->unit->contents[initial];
	char_tok->length = lex->pos - initial;

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

static int align(int n, int m) {
    int rem = n % m;
    return (rem == 0) ? n : n - rem + m;
}

struct token_location capture_location(struct lexer* lex) {
	return (struct token_location) {
		.index = lex->pos,
		.row = lex->row,
		.col = lex->col,
	};
}

static struct token* 
recognized_other(struct lexer* lex) {
	char current = peek(lex);
	switch (current) {
		case '/': {
			if (peek_at(lex, 1) == '/') {
				skip_line(lex);
				return NULL;
			}
			return recognize_symbol(lex);
		}
		case '"': {
			return recognize_string(lex);
		}
		case '\'': {
			return recognize_character(lex);
		}
		case '[': case ']': case '(': case ')': case '{': case '}':
		case '.': case '&': case '*': case '+': case '-': case '~':
		case '!': case '%': case '<': case '>': case '^':
		case '|': case '?': case ':': case ';': case '=':
		case ',': {
			return recognize_symbol(lex);
		}
		default: {
			if (is_eof(lex)) 
				return NULL;

			// peeks ahead a 128 byte
			// sample which is dumped as output
			char sample[128];
			memcpy(&sample, &lex->unit->contents[lex->pos], 96);

			// NOTE: these are wrong when we take the pre-processed source
			// code into account HM!
			fprintf(stderr, "error: %s %d:%d, illegal character '%c', '%d':\n-> %s\n", 
			lex->unit->path, (int) lex->row, (int) lex->col, current, (int) current, sample);

			// how can we recover from this error?
			exit(1);
		}
	}
}

struct lexer_info
tokenize(struct lexer* lex, struct compilation_unit* unit) {
	struct array* tokens = new_array(32);
	lex->unit = unit;
	lex->row = lex->col = 1;

	uint64_t last_line = 0, pad = 0;

	while (!is_eof(lex)) {
		last_line = lex->row;
		skip_layout(lex);

		if (lex->row == last_line) {
			pad = pad + 0;
		} 
		else {
			pad = 0;
		}

		char current = peek(lex);
		struct token* recognized_token = NULL;
		struct token_location start_loc = capture_location(lex);

		if (current == '#') {
			skip_line(lex);
			continue;
		}
		else if (current == '_' || islower(current) || isupper(current)) {
			recognized_token = recognize_identifier(lex);
		}
		else if (isdigit(current)) {
			recognized_token = recognize_number(lex);
		}
		else {
			recognized_token = recognized_other(lex);
		}

		// this is a bit weird.
		// recognized_other will return NULL
		// when we hit EOF, so we have to handle this here.
		if (is_eof(lex)) {
			break;
		}

		start_loc.col -= pad;
		struct token_location end_loc = capture_location(lex);
		end_loc.col -= pad;

		recognized_token->pos = (struct token_span) {
			.start = start_loc,
			.end = end_loc,
		};

		array_add(tokens, recognized_token);
	}

	return (struct lexer_info) {
		.token_stream = tokens,
		.lines_lexed = lex->row,
	};
}
