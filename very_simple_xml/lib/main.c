#include <stdio.h>
#include <string.h>
#include "pool.h"
#include "helpers.h"
#include "simple_xml.tab.h"
#include "simple_xml.lex.h"

void print_usage() {
    printf("simple_xml [FILE]\n");
}

int node_callback(const char* type, int line, int col, const char* value, void* pyobject) {
    fprintf(stdout, "node(%s %d/%d): %s\n", type, line, col, value);
    return 0;
}

int attribute_callback(int line, int col, const char* name, const char* value, void* pyobject) {
    fprintf(stdout, "attribute(%s = %s %d/%d)\n", name, value, line, col);
    return 0;
}

void pop_callback(void* pyobject) {
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
        YYLTYPE location;
        location.first_line = 0;
        location.last_line = 0;
        location.first_column = 0;
        location.last_column = 0;
        YYSTYPE value;
        _pool memory;
        memory.mem = malloc(sizeof(char) * 500);
        memory.occupied = 0;
        memory.node = &node_callback;
        memory.attribute = &attribute_callback;
        memory.pop = &pop_callback;
        yyset_in(h, yyscanner);
        fprintf(stderr, "Scanner set\n");
        /* res = yyparse(yyscanner, &memory); */
        int status;
        yypstate *ps = yypstate_new();
        do {
            status = yypush_parse(
                ps,
                yylex(&value, &location, yyscanner, &memory),
                &value,
                &location,
                yyscanner,
                &memory);
        } while (status == YYPUSH_MORE);
        yypstate_delete(ps);

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
