#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lex.h"
#include "comp_unit.h"

#define JAVAJAVA_FILE_EXT ".yava"

static bool 
has_suffix(char* str, char* suffix) {
	size_t str_len = strlen(str);
	size_t suffix_len = strlen(suffix);
	if (suffix_len > str_len) {
		return false;
	}
	return !strcmp(&str[str_len - suffix_len], suffix);
}

static char* 
read_file(const char* path) {
	FILE* file = fopen(path, "r");
	if (!file) {
		fprintf(stderr, "failed to read file '%s'\n", path);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	u64 file_length = ftell(file);
	rewind(file);

	printf(" - file '%s' is %zd bytes\n", path, file_length);

	char* contents = malloc(file_length + 1);
	if (!contents) {
		fprintf(stderr, "failed to allocate memory for file '%s'\n", path);
		return NULL;
	}

	fread(contents, sizeof(*contents), file_length, file);
	contents[file_length] = '\0';
	return contents;
}

int 
main(int argc, char** argv) {
	struct compilation_unit* units[argc - 1];
	int num_units = 0;
	for (int i = 1; i < argc; i++) {
		if (has_suffix(argv[i], JAVAJAVA_FILE_EXT)) {
			printf("Loading compilation unit '%s'\n", argv[i]);
			units[num_units++] = &(struct compilation_unit){
				.path = argv[i],
			};
		}
	}	

	printf("Lexical Analysis on %d compilation unit(s)\n", num_units);
	for (int i = 0; i < num_units; i++) {
		struct compilation_unit* current_unit = units[i];

		// we do the file reading here just before we lex
		// this is because we dont want to load all the files 
		// before we do anything, and if the lexer fails we wont
		// have loads of memory allocated and it can just exit.
		current_unit->contents = read_file(current_unit->path);

		struct lexer lex_inst = {0};
		tokenize(&lex_inst, current_unit->contents);
	}

	return 0;
}
