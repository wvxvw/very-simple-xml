#ifndef POOL_H_
#define POOL_H_

#include <stddef.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef int (*py_node_callback)(const char*, int, int, const char*, void*);

typedef int (*py_attribute_callback)(int, int, const char*, const char*, void*);

typedef void (*py_pop_callback)(void*);

typedef struct {
    py_node_callback node;
    py_attribute_callback attribute;
    py_pop_callback pop;
    char* mem;
    size_t occupied;
    size_t capacity;
    void* pyobject;
} _pool, *pool;

void reset_pool(pool);

void extend_pool(pool, size_t);

size_t alloc_str(const char*, pool);

pool make_pool(size_t, py_node_callback, py_attribute_callback, py_pop_callback, void*);

void free_pool(pool);

char* pool_mem(pool);

char* get_str(pool, size_t);


#endif // POOL_H_
