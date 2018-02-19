%debug
%define api.pure full
%locations
%lex-param {void* scanner}
%parse-param {void* scanner}
%lex-param {pool p}
%parse-param {pool p}

%{
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "pool.h"
#include "helpers.h"
#include "simple_xml.tab.h"
#include "py_callback.h"

void yyerror(YYLTYPE *locp, void* scanner, pool p, char const *msg) {
    fprintf(stderr, "%d/%d: %s\n", locp->first_line, locp->first_column, msg);
}
%}

%union {
    char* str;
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
    py_node_callback("element", @1.first_line, @1.first_column, $1 + 1);
    reset_pool(p);
 };

node    : head atts | head ;

element :       node NODE_END { py_pop_callback(); reset_pool(p); }
        |       node NODE_MID anys NODE_CLOSE NODE_NAME NODE_MID {
     py_pop_callback();
     reset_pool(p);
 }
        |       node NODE_MID NODE_CLOSE NODE_NAME NODE_MID {
     py_pop_callback();
     reset_pool(p);
 };

text    : TEXT {
    py_node_callback("text", @1.first_line, @1.first_column, $1);
    py_pop_callback();
    reset_pool(p);
 };

comment : COMMENT_START COMMENT_END {
    py_node_callback("cdata", @1.first_line, @1.first_column, $1 + 4);
    py_pop_callback();
    reset_pool(p);
 };

cdata   : CDATA_START CDATA_END {
    py_node_callback("cdata", @1.first_line, @1.first_column, $1 + 9);
    py_pop_callback();
    reset_pool(p);
 };

attr    : ATT_NAME EQUALS value {
    py_attribute_callback(@1.first_line, @1.first_column, $1, $3);
    reset_pool(p);
 };

value   : QUOTED | APOSED ;

atts    : attr atts | attr ;

elt     : element | comment | cdata ;

any     : elt | text ;

anys    : any anys | any ;

prolog  : PROLOG {
    py_node_callback("pi", @1.first_line, @1.first_column, $1);
    py_pop_callback();
    reset_pool(p);
};

s       : any | prolog TEXT any ;

%%
