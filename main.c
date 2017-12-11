#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <unistd.h>
#include <collectc/array.h>

#include "lex.h"
#include "parse.h"
#include "pool.h"
#include "comp_unit.h"
#include "token.h"

static bool DUMP_TOK_STREAM = false;

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
	if (access(unit->path, F_OK) == -1) {
		fprintf(stderr, "error: no such file '%s'\n", unit->path);
		return;
	}

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
		sprintf(error_msg, "error: failed to read file '%s'\n", pp_file_path);
		perror(error_msg);
		return;
	}

	fseek(file, 0, SEEK_END);
	unit->length = ftell(file);
	rewind(file);

	printf(" - file '%s' is %zd bytes\n", unit->path, unit->length);

	unit->contents = malloc(unit->length + 1);
	if (unit->contents == NULL) {
		fprintf(stderr, "error: failed to allocate memory for file '%s'\n", unit->path);
		return;
	}

	fread(unit->contents, sizeof(*unit->contents), unit->length, file);
	unit->contents[unit->length] = '\0';

	fclose(file);

	// remove the generated pp file
	// once it has been read into memory.
	remove(pp_file_path);
}

long long
curr_time_ms() {
	struct timeval timer;
	gettimeofday(&timer, NULL);
	long long time = timer.tv_sec * 1000LL + timer.tv_usec / 1000;
	return time;
}

int 
main(int argc, char** argv) {
	long long start_time = curr_time_ms();

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

	Array* TOKEN_STREAMS[num_units];
	u64 program_loc = 0;

	printf("Lexical Analysis on %d compilation unit(s)\n", num_units);
	for (int i = 0; i < num_units; i++) {
		struct compilation_unit* current_unit = &units[i];

		// we do the file reading here just before we lex
		// this is because we dont want to load all the files 
		// before we do anything, and if the lexer fails we wont
		// have loads of memory allocated and it can just exit.
		read_comp_unit(current_unit);

		struct lexer lex_inst = {0};
		struct lexer_info lex = tokenize(&lex_inst, current_unit);;
		TOKEN_STREAMS[i] = lex.token_stream;
		program_loc += lex.lines_lexed;

		if (DUMP_TOK_STREAM) {
			for (int i = 0; i < array_size(TOKEN_STREAMS[i]); i++) {
				struct token* tok;
				array_get_at(TOKEN_STREAMS[i], i, (void*) &tok);
				print_tok(tok);
			}
		}

		parse(TOKEN_STREAMS[i]);
	}

	// cleanup stuff
	{
		for (int i = 0; i < num_units; i++) {
			struct compilation_unit* unit = &units[i];
			array_destroy(TOKEN_STREAMS[i]);
			free(unit->contents);
		}
	}

	cleanup_pools();

	long long end_time = curr_time_ms();
	int time_taken_ms = end_time - start_time;

	printf("compiled %zd/LOC in %d/ms\n", program_loc, time_taken_ms);

	return 0;
}
