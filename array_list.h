#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include "type.h"

struct array_list {
	void** items;
	u64 length, capacity;
};

struct array_list* 
array_list_make(u64 capacity);

void 
array_list_push(struct array_list* list, void* item);

void 
array_list_free(struct array_list* list);

#endif
