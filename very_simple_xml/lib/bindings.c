#include <stdio.h>
#include <string.h>
#include "pool.h"
#include "simple_xml.tab.h"
#include "simple_xml.lex.h"
#include "bindings.h"

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

parser init_parser(const char* xml_file, int debug, pool memory) {
    FILE* h = NULL;
    YYLTYPE* location = NULL;
    YYSTYPE* value = NULL;
    yypstate* ps = NULL;

    yyscan_t yyscanner = malloc(sizeof(yyscan_t));
    if (yyscanner == NULL) {
        fprintf(stderr, "Couldn't allocate scanner, errno: %d\n", ENOMEM);
        goto cleanup;
    }

    int res = yylex_init(&yyscanner);
    if (res != 0) {
        fprintf(stderr, "Couldn't initialize scanner, errno: %d\n", res);
        goto cleanup;
    }
    h = fopen(xml_file, "rb");
    yydebug = debug;
        
    if (h == NULL) {
        fprintf(stderr, "Couldn't open: %s, errno: %d\n", xml_file, errno);
        goto cleanup;
    }
    yyset_in(h, yyscanner);

    location = malloc(sizeof(YYLTYPE));
    if (location == NULL) {
        fprintf(stderr, "Couldn't allocate location, errno: %d\n", ENOMEM);
        goto cleanup;
    }
    location->first_line = 0;
    location->last_line = 0;
    location->first_column = 0;
    location->last_column = 0;
    
    value = malloc(sizeof(YYSTYPE));
    if (location == NULL) {
        fprintf(stderr, "Couldn't allocate value, errno: %d\n", ENOMEM);
        goto cleanup;
    }
    
    ps = yypstate_new();
    if (ps == NULL) {
        fprintf(stderr, "Couldn't allocate ps, errno: %d\n", ENOMEM);
        goto cleanup;
    }
    parser result = malloc(sizeof(_parser));
    result->yyscanner = yyscanner;
    result->pool = memory;
    result->location = location;
    result->value = value;
    result->status = 0;
    result->ps = ps;
    result->h = h;
    return result;
cleanup:
    if (yyscanner != NULL) {
        free(yyscanner);
    }
    if (h != NULL) {
        fclose(h);
    }
    if (location != NULL) {
        free(location);
    }
    if (value != NULL) {
        free(value);
    }
    return NULL;
}

void free_parser(parser p) {
    yypstate_delete(p->ps);
    yylex_destroy(p->yyscanner);
    free_pool(p->pool);
    free(p->location);
    free(p->value);
    fclose(p->h);
}

void parse_chunk(parser p) {
    p->status = yypush_parse(
        p->ps,
        yylex(p->value, p->location, p->yyscanner, p->pool),
        p->value,
        p->location,
        p->yyscanner,
        p->pool);
}
