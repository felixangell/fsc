#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

typedef void* array_list_item;

struct array_list {
	array_list_item* items;
	size_t index, capacity;
};

struct array_list* array_list_make(size_t capacity);
void array_list_push(struct array_list* list, array_list_item item);
void array_list_free(struct array_list* list);

#endif