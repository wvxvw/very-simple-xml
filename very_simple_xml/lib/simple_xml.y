%debug
%define api.pure full
%define api.push-pull push
%locations
%lex-param {void* scanner}
%parse-param {void* scanner}
%parse-param {pool p}

%{
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "pool.h"
#include "helpers.h"
#include "simple_xml.tab.h"

void yyerror(YYLTYPE *locp, void* scanner, pool p, char const *msg) {
    fprintf(stderr, "%d/%d: %s\n", locp->first_line, locp->first_column, msg);
}
%}

%union {
    size_t str;
    void* arr;
}
%start s;
%token NODE_START;
%token NODE_END;
%token NODE_CLOSE;
%token NODE_MID;
%token NODE_NAME;
%token ATT_NAME;
%token EQUALS;
%token COMMENT_START;
%token COMMENT_END;
%token CDATA_START;
%token CDATA_END;
%token QUOTED;
%token APOSED;
%token UNAPOS;
%token TEXT;
%token PROLOG;
%type <str> NODE_START NODE_END NODE_CLOSE NODE_MID NODE_NAME
     EQUALS COMMENT_START COMMENT_END CDATA_START CDATA_END
     QUOTED APOSED UNAPOS TEXT PROLOG ATT_NAME value;
%type <arr> head atts element text comment cdata attr elt
     any anys s prolog node;

%%

head    : NODE_START {
    if (p->node("element", @1.first_line, @1.first_column, get_str(p, $1 + 1), p->pyobject)) {
        YYABORT;
    }
    reset_pool(p);
 };

node    : head atts | head ;

element :       node NODE_END { p->pop(p->pyobject); reset_pool(p); }
        |       node NODE_MID anys NODE_CLOSE NODE_NAME NODE_MID {
     p->pop(p->pyobject);
     reset_pool(p);
 }
        |       node NODE_MID NODE_CLOSE NODE_NAME NODE_MID {
     p->pop(p->pyobject);
     reset_pool(p);
 };

text    : TEXT {
    if (p->node("text", @1.first_line, @1.first_column, get_str(p, $1), p->pyobject)) {
        YYABORT;
    }
    reset_pool(p);
 };

comment : COMMENT_START COMMENT_END {
    if (p->node("comment", @1.first_line, @1.first_column, get_str(p, $1 + 4), p->pyobject)) {
        YYABORT;
    }
    reset_pool(p);
 };

cdata   : CDATA_START CDATA_END {
    if (p->node("cdata", @1.first_line, @1.first_column, get_str(p, $1 + 9), p->pyobject)) {
        YYABORT;
    }
    reset_pool(p);
 };

attr    : ATT_NAME EQUALS value {
    if (p->attribute(@1.first_line, @1.first_column, get_str(p, $1), get_str(p, $3), p->pyobject)) {
        YYABORT;
    }
    reset_pool(p);
 };

value   : QUOTED | APOSED ;

atts    : attr atts | attr ;

elt     : element | comment | cdata ;

any     : elt | text ;

anys    : any anys | any ;

prolog  : PROLOG {
    char* pi = get_str(p, $1 + 2);
    size_t len = strlen(pi);
    pi[len - 2] = '\0';
    if (p->node("pi", @1.first_line, @1.first_column, pi, p->pyobject)) {
        printf("Reject from python\n");
        YYABORT;
    }
    reset_pool(p);
};

s       : any | prolog TEXT any ;

%%
