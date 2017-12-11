#ifndef POOL_H
#define POOL_H

#include <stdint.h>
#include <collectc/array.h>

#include "type.h"

struct block {
    void* data;
    u64 len;
};

struct data_pool {
    Array* blocks;
    struct block* current;
    u64 page_size;
};

void cleanup_pools();

struct data_pool* 
make_pool(u64 capacity);

void* 
alloc(struct data_pool* pool, size_t len);

void 
destroy_pool(struct data_pool* pool);

#endif