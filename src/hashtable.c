#include "hashtable.h"

#include <stdlib.h>

#include "../SipHash/siphash.h"

/*
static i32 hash(char* key) {

}
*/

struct hashtable* hashtable_make(u64 size) {
	struct hashtable* table = malloc(sizeof(*table));
	table->size = size;
	table->entries = malloc(sizeof(*table->entries) * size);
	return table;
}

struct hashtable_entry hashtable_entry_make_pair(char* key, void* value) {
	struct hashtable_entry entry;
	entry.key = key;
	entry.value = value;
	return entry;
}

void hashtable_insert(struct hashtable* table, char* key, void* value) {

}

void* hashtable_get(struct hashtable* table, char* key) {

}
