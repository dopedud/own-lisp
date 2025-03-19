#include <errno.h>
#include <stdlib.h>

#include "constdest.h"

#include "reader.h"

lispvalue* lispvalue_read(mpc_ast_t* ast)
{
    // if a symbol or a number is encountered return the appropriate type
    if (strstr(ast->tag, NUMBER_STR)) return lispvalue_read_number(ast);
    else if (strstr(ast->tag, SYMBOL_STR)) return lispvalue_symbol(ast->contents);

    lispvalue* lv = NULL;

    // if root (>) or s_expression is encountered then create an empty list
    if (!strcmp(ast->tag, ">") || strstr(ast->tag, SEXPRESSION_STR))   
    lv = lispvalue_sexpression();

    else if (strstr(ast->tag, QEXPRESSION_STR)) lv = lispvalue_qexpression();

    // then fill this list with any valid expression contained within
    for (int i = 0; i < ast->children_num; i++)
    {
        if (!strcmp(ast->children[i]->contents, "(")
        || !strcmp(ast->children[i]->contents, ")")
        || !strcmp(ast->children[i]->contents, "{")
        || !strcmp(ast->children[i]->contents, "}")
        || !strcmp(ast->children[i]->tag, "regex"))
        continue;

        lv = lispvalue_add(lv, lispvalue_read(ast->children[i]));
    }

    return lv;
}

lispvalue* lispvalue_read_number(mpc_ast_t* ast)
{
    errno = 0;
    int64_t number = strtol(ast->contents, NULL, 10);
    return (errno != ERANGE) ? lispvalue_number(number) : lispvalue_error("cannot read number");
}