#ifndef POOL_H_
#define POOL_H_

#include <stddef.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct {
    char* mem;
    size_t occupied;
    size_t capacity;
} _pool, *pool;

void reset_pool(pool);

void extend_pool(pool, size_t);

char* alloc_str(const char*, pool);


#endif // POOL_H_
