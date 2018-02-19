#ifndef PY_CALLBACK_H_
#define PY_CALLBACK_H_

#include <string.h>
#include <stddef.h>
#include "slice.h"

void py_node_callback(const char*, int, int, const char*);

void py_attribute_callback(int, int, const char*, const char*);

void py_pop_callback(void);

#endif // PY_CALLBACK_H_
