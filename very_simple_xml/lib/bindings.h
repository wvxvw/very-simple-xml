#ifndef BINDINGS_H_
#define BINDINGS_H_

#include <stdio.h>
#include "helpers.h"
#include "simple_xml.tab.h"
#include "simple_xml.lex.h"


typedef struct {
    yyscan_t yyscanner;
    pool pool;
    YYLTYPE* location;
    YYSTYPE* value;
    int status;
    yypstate* ps;
    FILE* h;
} _parser, *parser;


parser make_parser(const char*, int, pool);

void free_parser(parser);

void parse_chunk(parser);

#endif // BINDINGS_H_
