#include "pool.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void reset_pool(pool p) {
    p->occupied = 0;
}

void extend_pool(pool p, size_t len) {
    char* mem = malloc(sizeof(char*) * MAX(p->capacity * 2, p->occupied + len));
    memcpy(mem, p->mem, p->occupied);
    free(p->mem);
    p->capacity *= 2;
    p->mem = mem;
}

char* alloc_str(const char* s, pool p) {
    size_t len = 1 + strlen(s);
    if (p->capacity < p->occupied + len) {
        extend_pool(p, len);
    }
    memcpy(&p->mem[p->occupied], s, len - 1);
    p->occupied += len;
    p->mem[p->occupied - 1] = '\0';
    return &p->mem[p->occupied - len];
}
