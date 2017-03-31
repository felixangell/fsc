#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "comp_unit.h"

#define JAVAJAVA_FILE_EXT ".yava"

static bool has_suffix(char* str, char* suffix) {
	size_t str_len = strlen(str);
	size_t suffix_len = strlen(suffix);
	if (suffix_len > str_len) {
		return false;
	}
	return !strcmp(&str[str_len - suffix_len], suffix);
}

int main(int argc, char** argv) {
	struct compilation_unit* units[argc - 1];
	int num_units = 0;
	for (int i = 1; i < argc; i++) {
		if (has_suffix(argv[i], JAVAJAVA_FILE_EXT)) {
			printf("Loading compilation unit '%s'\n", argv[i]);
			units[num_units] = &(struct compilation_unit){
				.path = argv[i],
			};
		}
	}	

	for (int i = 0; i < num_units; i++) {
		
	}

	return 0;
}
