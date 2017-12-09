#include <stdlib.h>
#include <assert.h>

#include "pool.h"

static Array* ALLOCATED_POOLS;

void cleanup_pools() {
    for (int i = 0; i < array_size(ALLOCATED_POOLS); i++) {
        struct data_pool* pool;
        array_get_at(ALLOCATED_POOLS, i, (void*) &pool);
        destroy_pool(pool);
    }
    array_destroy(ALLOCATED_POOLS);
}

static void push_block(struct data_pool* pool) {
    struct block* b = malloc(sizeof(*b));
    assert(b != NULL);

    b->len = 0;
    b->data = malloc(pool->page_size);
    if (b->data == NULL) {
        cleanup_pools();
        assert(0 && "oh fuck");        
    }
    assert(b->data != NULL);

    array_add(pool->blocks, b);
    pool->current = b;
}

struct data_pool* 
make_pool(uint64_t page_size) {
    printf("creating pool with page size of %zd\n", page_size);

    // we only need to allocate this once
    // this is for keeping track of all pools
    // that are created. they are freed at the
    // end of the compilers runtime.
    if (ALLOCATED_POOLS == NULL) {
        array_new((void*) &ALLOCATED_POOLS);
    }

    struct data_pool* pool = malloc(sizeof(*pool));
    array_add(ALLOCATED_POOLS, pool);
    array_new(&pool->blocks);
    push_block(pool);
    pool->page_size = page_size;
    return pool;
}

static inline int align(int n, int m) {
    int rem = n % m;
    return (rem == 0) ? n : n - rem + m;
}

void* 
alloc(struct data_pool* pool, size_t len) {
    len = align(len, 16);
    assert(pool != NULL);

    if (pool->current->len + len >= pool->page_size) {
        push_block(pool);   
    }

    return &pool->current->data[pool->current->len += len];
}

void 
destroy_pool(struct data_pool* pool) {
    for (uint64_t i = 0; i < array_size(pool->blocks); i++) {
        struct block* block;
        array_get_at(pool->blocks, i, (void*) &block);
        free(block->data);
        free(block);
    }
    array_destroy(pool->blocks);
    free(pool);
}