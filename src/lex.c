#include <stdio.h>
#include <string.h>

#include "../util/array_list.h"
#include "lex.h"

static inline char
next(struct lexer* lex) {
	return lex->input[lex->position];
}

static void 
tokenize_identifier(struct lexer* lex) {
	// an identifier is something like
	// letter ( { letter | digit | '_' } )
	// it must start with a letter, but can be followed
	// by a digit, another letter, or an underscore.

	// 9oo, _foo, are some examples of non-identifiers
			
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
		char current = input[lex->position];

		u64 initial_pos = lex->position;

		switch (current) {
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

		// we've finished tokenizing here
		// so we'll cut out the lexeme from the source file
		char lexeme[lex->position - initial_pos];
		memmove(&lexeme, &input[initial_pos], lex->position - initial_pos);
		printf("lexeme is '%s'\n", lexeme);
		}
	}

	return list;
}