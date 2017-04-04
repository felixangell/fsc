#ifndef HASHTABLE_H
#define HASHTABLE_H

struct hashtable_entry {
	char* key;
	void* value;
};

struct hashtable {
	u64 size;
	struct hashtable_entry** entries;
};

struct hashtable* hashtable_make(u64 size);
struct hashtable_entry* hashtable_entry_make_pair(char* key, void* value);
void hashtable_insert(struct hashtable* table, char* key, void* value);
void* hashtable_get(struct hashtable* table, char* key);

#endif