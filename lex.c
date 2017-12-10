#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <collectc/array.h>
#include <collectc/hashset.h>

#include "pool.h"
#include "token.h"
#include "lex.h"
#include "comp_unit.h"

#define arr_len(x) (int) (sizeof((x)) / sizeof((x)[0]))

static bool
has_prefix(const char* str, const char* what) {
	for (size_t i = 0; i < strlen(what); i++) {
		if (str[i] != what[i]) {
			return false;
		}
	}
	return true;
}

bool 
cmp_lexeme(struct token* tok, const char* val) {
	for (u64 i = 0; i < tok->length; i++) {
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

void
print_tok(struct token* tok) {
	/// NOTE:
	/// make sure this is up to date with
	/// token.h!
	static const char* token_types[] = {
		"identifier", "keyword", "integer", "float",
		"string", "character", "punctuator",
	};

	char lexeme[tok->length + 1];
	memset(lexeme, 0, tok->length + 1);
	memcpy(lexeme, tok->lexeme, tok->length);
	printf(TAB "'%s', %s\n", lexeme, token_types[tok->type]);
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
	char curr = lex->unit->contents[lex->pos];
	if (curr == '\n') {
		lex->row++;
		lex->col = 1;
	}
	lex->col++;
	lex->pos++;
	return curr;
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

	fprintf(stderr, "error: unexpected symbol '%c'\n", c);
	assert(0);
}

static inline bool 
is_identifier(char c) {
	return isalnum(c) || c == '_';
}

char* RESERVED_KEYWORDS[] = {
	"auto","break","case","char","const","continue","default","do","double","else","enum","extern","float",
	"for","goto","if","inline","int","long","register","restrict","return","short","signed","sizeof","static",
	"struct","switch","typedef","union","unsigned","void","volatile","while","_Bool","_Complex","_Imaginary"
};
static inline bool is_reserved_keyword(char* keyword) {
	static HashSet* keyword_set = NULL;

	// we should only do this once
	// TODO: we should probably free these all
	// but it really isnt necessary as the OS
	// should clean up most of it anyways
	if (keyword_set == NULL) {
		hashset_new(&keyword_set);
		for (int i = 0; i < array_len(RESERVED_KEYWORDS); i++) {
			hashset_add(keyword_set, RESERVED_KEYWORDS[i]);
		}		
	}

	return hashset_contains(keyword_set, keyword);
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

static inline int 
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
	for (int i = 0; i < arr_len(keywords); i++) {
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

static inline int align(int n, int m) {
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

Array* 
tokenize(struct lexer* lex, struct compilation_unit* unit) {
	Array* tokens;
	array_new(&tokens);
	lex->unit = unit;
	lex->row = lex->col = 1;

	u64 last_line = 0, pad = 0;

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
		}
		else if (current == '_' || islower(current) || isupper(current)) {
			recognized_token = recognize_identifier(lex);
		}
		else if (isdigit(current)) {
			recognized_token = recognize_number(lex);
		}
		else {
			switch (current) {
				case '/': {
					if (peek_at(lex, 1) == '/')
						skip_line(lex);
					else	
						recognized_token = recognize_symbol(lex);
					break;
				}
				case '"': {
					recognized_token = recognize_string(lex);
					break;
				}
				case '\'': {
					recognized_token = recognize_character(lex);
					break;
				}
				case '[': case ']': case '(': case ')': case '{': case '}':
				case '.': case '&': case '*': case '+': case '-': case '~':
				case '!': case '%': case '<': case '>': case '^':
				case '|': case '?': case ':': case ';': case '=':
				case ',': {
					recognized_token = recognize_symbol(lex);
					break;
				}
				default: {
					if (is_eof(lex)) 
						break;

					char sample[128];
					memcpy(&sample, &lex->unit->contents[lex->pos], 96);

					// NOTE: these are wrong when we take the pre-processed source
					// code into account HM!
					fprintf(stderr, "error: %s %d:%d, illegal character '%c', '%d':\n-> %s\n", 
					lex->unit->path, (int) lex->col, (int) lex->row, current, (int) current, sample);
					exit(1);  
					break;
				}

				if (recognized_token != NULL) {
					start_loc.col -= pad;
					struct token_location end_loc = capture_location(lex);
					end_loc.col -= pad;

					recognized_token->pos = (struct token_span) {
						.start = start_loc,
						.end = end_loc,
					};

					array_add(tokens, recognized_token);
				}
			}
		}
	}
	return tokens;
}
