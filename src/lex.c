#include "../util/array_list.h"
#include "lex.h"

// if this fails it will either return an empty
// array or all the tokens it managed to process.
// the array needs to be cleaned up after!
struct array_list* tokenize(struct lexer* lex, char* input) {
	// i wonder if we could estimate what capacity
	// based on the size of the source file
	// e.g. filesize_bytes / average_tok_len or something
	struct array_list* list = array_list_make(128);

	return list;
}