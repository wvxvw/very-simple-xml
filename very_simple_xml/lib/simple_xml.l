%{
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include "helpers.h"
#include "pool.h"
#include "simple_xml.tab.h"

#define YY_USER_ACTION                          \
    yylval->str = alloc_str(yytext, p);         \
    yylloc->first_line = yylloc->last_line;     \
    yylloc->first_column = yylloc->last_column; \
    for (int i = 0; yytext[i] != '\0'; i++) {   \
        if (yytext[i] == '\n') {                \
            yylloc->last_line++;                \
            yylloc->last_column = 0;            \
        }                                       \
        else {                                  \
            yylloc->last_column++;              \
        }                                       \
    }
%}

%option yylineno
%option bison-bridge
%option bison-locations
%option reentrant

%%

[<][A-Za-z]([[:alnum:]]|\.)*            { return NODE_START; }
[>]                                     { return NODE_MID; }
[/][>]                                  { return NODE_END; }
[<][/]                                  { return NODE_CLOSE; }
[A-Za-z][[:alnum:]:.-]*/[ \n\t\r]*=     { return ATT_NAME; }
[A-Za-z][[:alnum:]:.-]*/[ \n\t\r]*[>]   { return NODE_NAME; }
=                                       { return EQUALS; }
[<]!--.*/--[>]                          { return COMMENT_START; }
--[>]                                   { return COMMENT_END; }
[<]!\[CDATA\[.*/\]\][>]                 { return CDATA_START; }
\]\][>]                                 { return CDATA_END; }
\"(\\.|[^"\\])*\"                       { return QUOTED; }
'(\\.|[^'\\])*'                         { return APOSED; }
[^<>]+/[<]                              { return TEXT; }
[<][?]xml[^>]+[>]                       { return PROLOG; }
[ \n\t\r]+

%%

int yywrap(yyscan_t yyscanner)
{
    return 1;
}
