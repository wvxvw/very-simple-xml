#include <stdio.h>
#include <string.h>
#include "pool.h"
#include "py_callback.h"
#include "simple_xml.tab.h"
#include "simple_xml.lex.h"

void print_usage() {
    printf("simple_xml [FILE]\n");
}

void py_node_callback(const char* type, int line, int col, const char* value) {
    fprintf(stdout, "node(%s %d/%d): %s\n", type, line, col, value);
}

void py_attribute_callback(int line, int col, const char* name, const char* value) {
    fprintf(stdout, "attribute(%s = %s %d/%d)\n", name, value, line, col);
}

void py_pop_callback() {
    fprintf(stdout, "pop\n");
}

int main(int argc, char** argv) {
    int res;
    if (argc == 2) {
        yyscan_t yyscanner;

        res = yylex_init(&yyscanner);
        if (res != 0) {
            fprintf(stderr, "Couldn't initialize scanner\n");
            return res;
        }
        FILE* h = fopen(argv[1], "rb");
        /* yydebug = 1; */
        
        if (h == NULL) {
            fprintf(stderr, "Couldn't open: %s\n", argv[1]);
            return errno;
        }
        _pool memory;
        memory.mem = malloc(sizeof(char) * 500);
        memory.occupied = 0;
        yyset_in(h, yyscanner);
        fprintf(stderr, "Scanner set\n");
        res = yyparse(yyscanner, &memory);
        fprintf(stderr, "Parsed\n");
        yylex_destroy(yyscanner);
        free(memory.mem);
        return res;
    }
    if (argc > 2) {
        fprintf(stderr, "Wrong number of arguments\n");
    }
    print_usage();
    return 1;
}
