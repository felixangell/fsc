#ifndef POOL_H
#define POOL_H

#include <stdint.h>
#include <collectc/array.h>

struct block {
    void* data;
    uint64_t len;
};

struct data_pool {
    Array* blocks;
    struct block* current;
    uint64_t page_size;
};

void cleanup_pools();

struct data_pool* 
make_pool(uint64_t capacity);

void* 
alloc(struct data_pool* pool, size_t len);

void 
destroy_pool(struct data_pool* pool);

#endif