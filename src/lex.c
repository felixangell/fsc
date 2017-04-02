#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../util/array_list.h"
#include "lex.h"

static inline char
next(struct lexer* lex) {
	return lex->input[lex->position];
}

static inline void
consume(struct lexer* lex) {
	printf("consumed '%c'\n", lex->input[lex->position]);
	lex->position++;
}

static void 
tokenize_identifier(struct lexer* lex) {
	// an identifier is something like
	// { letter } ( { letter | digit | '_' } )
	// it must start with a letter, but can be followed
	// by a digit, another letter, or an underscore.
	// 9oo, _foo, are some examples of non-identifiers

	// letter part
	while (isalpha(next(lex))) {
		consume(lex);
	}

	// letters, digits, underscores.
	while (isdigit(next(lex)) || isalpha(next(lex)) || next(lex) == '_') {
		consume(lex);
	}
}

// if this fails it will either return an empty
// array or all the tokens it managed to process.
// the array needs to be cleaned up after!
struct array_list* 
tokenize(struct lexer* lex, char* input) {
	// i wonder if we could estimate what capacity
	// based on the size of the source file
	// e.g. filesize_bytes / average_tok_len or something
	struct array_list* list = array_list_make(128);
	lex->input = input;

	printf("tokenizing '%s'\n", input);

	// TODO: store this in the compilation unit
	// rather than doing strlen which is O(n)!
	size_t input_len = strlen(input);
	while (lex->position < input_len) {
		u64 initial_pos = lex->position;

		// skip the junk stuff, spaces, etc.
		// between tokens
		while (next(lex) >= ' ') {
			consume(lex);
		}

		switch (next(lex)) {
		case 'a' ... 'z':
		case 'A' ... 'Z':
			tokenize_identifier(lex);
			break;
		case '0' ... '9':
			
			break;
		case '+': case '-': case '*': case '/': case '&':
		case '!': case '%': case '(': case ')': case '_':
		case '.': case ',': case '{': case '}': case '[':
		case ']': case '<': case '>': case '~': case '=':
		case '\\': case '?': case ';': case ':': case '#':
			
			break;	
		}

		// we've finished tokenizing here
		// so we'll cut out the lexeme from the source file
		
		int lexeme_len = lex->position - initial_pos;
		char lexeme[lexeme_len];
		memcpy(&lexeme, &input[initial_pos], lexeme_len);
		printf("%.*s\n", lexeme_len, lexeme);
		exit(1);
	}

	return list;
}