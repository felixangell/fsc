#include <stdlib.h>

#include "array_list.h"

struct array_list* array_list_make(u64 capacity) {
	struct array_list* list = malloc(sizeof(*list));
	list->items = malloc(sizeof(*list->items) * capacity);
	list->length = 0;
	list->capacity = capacity;
	return list;
}

void array_list_push(struct array_list* list, void* item) {
	if (list->length == list->capacity) {
		list->capacity *= 2;
		list->items = realloc(list->items, sizeof(*list->items) * list->capacity);
	}
	list->items[list->length++] = item;	
}

// todo predicate thing to free the item that is done on
// each iteration
void array_list_free(struct array_list* list) {
	free(list->items);
	free(list);
}
