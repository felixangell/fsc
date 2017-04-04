#include "hashtable.h"

struct hashtable* hashtable_make(u64 size) {
	struct hashtable* table = malloc(sizeof(*table));
	table->size = size;
	table->entries = malloc(sizeof(*table->entries) * size);
	return table;
}

struct hashtable_entry* hashtable_entry_make_pair(char* key, void* value) {

}

void hashtable_insert(struct hashtable* table, char* key, void* value) {

}

void* hashtable_get(struct hashtable* table, char* key) {

}