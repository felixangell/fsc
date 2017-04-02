#include <string.h>

#include "type.h"
#include "str.h"

bool
has_prefix(const char* str, const char* what) {
	for (size_t i = 0; i < strlen(what); i++) {
		if (str[i] != what[i]) {
			return false;
		}
	}
	return true;
}

bool
has_suffix(char* str, char* suffix) {
	size_t str_len = strlen(str);
	size_t suffix_len = strlen(suffix);
	if (suffix_len > str_len) {
		return false;
	}
	return !strcmp(&str[str_len - suffix_len], suffix);
}