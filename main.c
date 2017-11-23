#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "array_list.h"
#include "lex.h"
#include "parse.h"
#include "comp_unit.h"
#include "token.h"

static bool
has_suffix(const char* str, const char* suffix) {
	size_t str_len = strlen(str);
	size_t suffix_len = strlen(suffix);
	if (suffix_len > str_len) {
		return false;
	}
	return !strcmp(&str[str_len - suffix_len], suffix);
}

static void 
read_comp_unit(struct compilation_unit* unit) {
	// til i write my own pre-processor use the
	// existing preprocessor from GCC

	char pp_file_path[512];
	sprintf(pp_file_path, "%s.ppc", unit->path);

	{
		char cmd[512] = {0};
		sprintf(cmd, "cc -E %s > %s", unit->path, pp_file_path);
		system(cmd);
	}	

	FILE* file = fopen(pp_file_path, "r");
	if (file == NULL) {
		char error_msg[2048];
		sprintf(error_msg, "failed to read file '%s'\n", pp_file_path);
		perror(error_msg);
		return;
	}

	fseek(file, 0, SEEK_END);
	unit->length = ftell(file);
	rewind(file);

	printf(" - file '%s' is %zd bytes\n", unit->path, unit->length);

	unit->contents = malloc(unit->length + 1);
	if (unit->contents == NULL) {
		fprintf(stderr, "failed to allocate memory for file '%s'\n", unit->path);
		return;
	}

	fread(unit->contents, sizeof(*unit->contents), unit->length, file);
	unit->contents[unit->length] = '\0';

	fclose(file);

	// remove the generated pp file
	// once it has been read into memory.
	remove(pp_file_path);
}

int 
main(int argc, char** argv) {
	clock_t compiler_start = clock();

	if (argc <= 1) {
		fprintf(stderr, "error: no input files\n");
		return -1;
	}

	struct compilation_unit units[argc - 1];
	memset(&units, 0, argc - 1);

	printf("Loading compilation units:\n");
	printf("- ");
	int num_units = 0;
	for (int i = 1; i < argc; i++) {
		if (i > 1) printf(", ");
		if (i % 5 == 0) putchar('\n');

		if (has_suffix(argv[i], ".c")) {
			printf("'%s'", argv[i]);
			units[num_units++] = (struct compilation_unit){.path = argv[i]};
		}
	}
	putchar('\n');

	// first we run lex/parse on all files

	printf("Lexical Analysis on %d compilation unit(s)\n", num_units);
	for (int i = 0; i < num_units; i++) {
		struct compilation_unit* current_unit = &units[i];

		// we do the file reading here just before we lex
		// this is because we dont want to load all the files 
		// before we do anything, and if the lexer fails we wont
		// have loads of memory allocated and it can just exit.
		read_comp_unit(current_unit);

		struct lexer lex_inst = {0};
		struct array_list* token_stream = tokenize(&lex_inst, current_unit);

		for (int i = 0; i < token_stream->length; i++) {
			struct token* tok = token_stream->items[i];
			print_tok(tok);
		}
	}

	// cleanup stuff
	{
		for (int i = 0; i < num_units; i++) {
			struct compilation_unit* unit = &units[i];
			free(unit->contents);
		}
	}

	clock_t compiler_end = clock();
	double time_taken_sec = ((double) (compiler_end - compiler_start)) / CLOCKS_PER_SEC;
	double time_taken_ms = time_taken_sec / 1000;
	printf("compiled in %f/ms\n", time_taken_ms);

	return 0;
}
