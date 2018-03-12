# -*- coding: utf-8 -*-

from libc.stdlib cimport abort, malloc, free
from libc.stdio cimport FILE
import asyncio

cdef extern from "lib/pool.h":
    ctypedef int (*py_node_callback)(const char*, int, int, const char*, void*)
    ctypedef int (*py_attribute_callback)(int, int, const char*, const char*, void*)
    ctypedef void (*py_pop_callback)(void*)

    ctypedef struct _pool:
        py_node_callback node
        py_attribute_callback attribute
        py_pop_callback pop
        char* mem
        size_t occupied
        size_t capacity
        void* pyobject

    ctypedef _pool* pool

cdef extern from "lib/simple_xml.tab.h":
    ctypedef struct YYLTYPE
    ctypedef union YYSTYPE
    ctypedef struct yypstate


cdef extern from "lib/simple_xml.lex.h":
    ctypedef void* yyscan_t


cdef extern from "lib/bindings.h":

    ctypedef struct _parser:
        yyscan_t yyscanner
        pool pool
        YYLTYPE* location
        YYSTYPE* value
        int status
        yypstate* ps
        FILE* h

    ctypedef _parser* parser

    parser make_parser(const char* xml_file, int debug, pool p)
    void free_parser(parser p)
    int parse_chunk(parser p)

    pool make_pool(
        size_t mem,
        py_node_callback node,
        py_attribute_callback attribute,
        py_pop_callback pop,
        void* pyobject,
    )
    void free_pool(pool)
    char* pool_mem(pool)

    cdef int YYPUSH_MORE


cdef int node_wrapper(
    const char* node_type,
    int line,
    int col,
    const char* text,
    void* pyobject,
):
    cdef bytes ptype = node_type
    cdef bytes ptext = text
    res = (<object>pyobject).on_node(ptype, ptext, line, col)
    return <int>(not res)


cdef int attribute_wrapper(
    int line,
    int col,
    const char* name,
    const char* value,
    void* pyobject,
):
    cdef bytes pname = name
    cdef bytes pvalue = value
    res = (<object>pyobject).on_attribute(pname, pvalue, line, col)
    return <int>(not res)


cdef void pop_wrapper(void* pyobject):
    (<object>pyobject).on_pop()


cdef class ParserIterator:
    cdef parser parser
    cdef int status
    cdef int first_time

    @staticmethod
    cdef ParserIterator create(parser p):
        self = <ParserIterator>ParserIterator.__new__(ParserIterator)
        self.parser = p
        self.status = 0
        self.first_time = 1
        return self

    def __aiter__(self):
        return self

    async def __anext__(self):
        if not self.first_time and self.status != YYPUSH_MORE:
            raise StopAsyncIteration
        self.first_time = 0
        self.status = parse_chunk(self.parser)


async def parse_impl(object callback):
    cdef pool mem = make_pool(
        500,
        node_wrapper,
        attribute_wrapper,
        pop_wrapper,
        <void*>callback,
    )
    cdef parser p = make_parser(
        callback.xml_file().encode('utf-8'),
        callback.debug(),
        mem,
    )
    if p == NULL:
        free_pool(mem)
        raise ValueError("could not initialize parser")
    try:
        async for status in ParserIterator.create(p):
            if status == YYPUSH_MORE:
                break
    finally:
        free_parser(p)
