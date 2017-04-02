#ifndef HASH_MAP_H
#define HASH_MAP_H

struct hash_map;
struct bucket;

struct bucket {
    const char* key;
    void* value;
};

struct hash_map {
    struct bucket* buckets;

};

hash_map* hash_map_make();

#endif
