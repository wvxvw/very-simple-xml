#include "pool.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

size_t alloc_str(const char* s, pool p) {
    size_t len = 1 + strlen(s);
    if (p->capacity < p->occupied + len) {
        extend_pool(p, len);
    }
    memcpy(&p->mem[p->occupied], s, len - 1);
    p->occupied += len;
    p->mem[p->occupied - 1] = '\0';
    return p->occupied - len;
}


char* get_str(pool p, size_t pt) {
    return &p->mem[pt];
}

pool make_pool(
    size_t mem,
    py_node_callback node,
    py_attribute_callback attribute,
    py_pop_callback pop,
    void* pyobject) {
    pool memory = malloc(sizeof(_pool));
    memory->mem = malloc(sizeof(char) * mem);
    memory->occupied = 0;
    memory->node = node;
    memory->attribute = attribute;
    memory->pop = pop;
    memory->pyobject = pyobject;
    return memory;
}

void free_pool(pool p) {
    free(p->mem);
    free(p);
}

char* pool_mem(pool p) {
    return p->mem;
}
