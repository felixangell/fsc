#include "array_list.h"

struct array_list* array_list_make(u64 capacity) {
	struct array_list* list = malloc(sizeof(*list));
	list->items = malloc(sizeof(*list->items) * capacity);
	list->index = 0;
	list->capacity = capacity;
	return list;
}

void array_list_push(struct array_list* list, array_list_item item) {
	if (list->index == list->capacity) {
		list->capacity *= 2;
		list->items = realloc(list->items, sizeof(*list->items) * list->capacity);
	}
	list->items[list->index++] = item;	
}

// todo predicate thing to free the item that is done on
// each iteration
void array_list_free(struct array_list* list) {
	free(list->items);
	free(list);
}
