#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#include "evaluator.h"

#include "constdest.h"

lispvalue* lispvalue_error(char* format, ...)
{
    lispvalue* lv = malloc(sizeof(lispvalue));
    lv->type = LISPVALUE_ERROR;

    va_list va;
    va_start(va, format);

    lv->error = malloc(MAX_STR_LENGTH);

    vsnprintf(lv->error, MAX_STR_LENGTH - 1, format, va);

    lv->error = realloc(lv->error, strlen(lv->error) + 1);

    va_end(va);

    return lv;
}

lispvalue* lispvalue_number(int64_t x)
{
    lispvalue* lv = malloc(sizeof(lispvalue));
    lv->type = LISPVALUE_NUMBER;
    lv->number = x;

    lv->cell_count = -1;
    lv->cells = NULL;

    return lv;
}

lispvalue* lispvalue_symbol(char* s)
{
    lispvalue* lv = malloc(sizeof(lispvalue));
    lv->type = LISPVALUE_SYMBOL;

    // allocate one extra byte for the null-terminator
    lv->symbol = malloc(strlen(s) + 1);
    // the null-terminator is copied from src to dest
    strcpy(lv->symbol, s);

    lv->cell_count = -1;
    lv->cells = NULL;

    return lv;
}

lispvalue* lispvalue_sexpression()
{
    lispvalue* lv = malloc(sizeof(lispvalue));
    lv->type = LISPVALUE_SEXPRESSION;
    lv->cell_count = 0;
    lv->cells = NULL;

    return lv;
}

lispvalue* lispvalue_qexpression()
{
    lispvalue* lv = malloc(sizeof(lispvalue));
    lv->type = LISPVALUE_QEXPRESSION;
    lv->cell_count = 0;
    lv->cells = NULL;

    return lv;
}

lispvalue* lispvalue_function(lispbuiltin function)
{
    lispvalue* lv = malloc(sizeof(lispvalue));
    lv->type = LISPVALUE_FUNCTION;
    lv->builtin = function;
    
    lv->cell_count = -1;
    lv->cells = NULL;

    return lv;
}

lispvalue* lispvalue_lambda(lispvalue* formals, lispvalue* body)
{
    lispvalue* lv = malloc(sizeof(lispvalue));
    lv->type = LISPVALUE_FUNCTION;

    // setting builtin to NULL means this is a user-defined function
    lv->builtin = NULL;

    lv->env = lispenv_new();
    
    lv->formals = formals;
    lv->body = body;

    return lv;
}

lispvalue* lispvalue_add(lispvalue* lv, lispvalue* new_lv)
{
    lv->cell_count++;
    lv->cells = realloc(lv->cells, sizeof(lispvalue*) * lv->cell_count);
    lv->cells[lv->cell_count - 1] = new_lv;

    return lv;
}

lispvalue* lispvalue_copy(lispvalue* lv)
{
    lispvalue* x = malloc(sizeof(lispvalue));
    x->type = lv->type;

    switch (lv->type)
    {
        case LISPVALUE_NUMBER:      x->number = lv->number; break;
        case LISPVALUE_FUNCTION:    
            if (!lv->builtin) x->builtin = lv->builtin;
            
            else
            {
                x->env = lv->env;
                x->formals = lv->formals;
                x->body = lv->body;    
            }

            break;

        case LISPVALUE_ERROR:
            x->error = malloc(strlen(lv->error) + 1);
            strcpy(x->error, lv->error);
            break;

        case LISPVALUE_SYMBOL:
            x->symbol = malloc(strlen(lv->symbol) + 1);
            strcpy(x->symbol, lv->symbol);
            break;

        case LISPVALUE_SEXPRESSION:
        case LISPVALUE_QEXPRESSION:
            x->cell_count = lv->cell_count;
            x->cells = malloc(sizeof(lispvalue*) * x->cell_count);

            for (int i = 0; i < x->cell_count; i++) x->cells[i] = lispvalue_copy(lv->cells[i]);
            break;
    }

    return x;
}

void lispvalue_delete(lispvalue* lv)
{
    switch (lv->type)
    {
        // do nothing special for the number and function type
        case LISPVALUE_NUMBER: break;
        case LISPVALUE_FUNCTION:
            if (!lv->builtin)
            {
                lispenv_delete(lv->env);
                lispvalue_delete(lv->formals);
                lispvalue_delete(lv->body);
            }
            break;

        // free string data for error and symbol type
        case LISPVALUE_ERROR: free(lv->error); break;
        case LISPVALUE_SYMBOL: free(lv->symbol); break;

        // recursively free all elements inside s_expression and q_expression type
        // also free the s_expression and q_expression type itself
        case LISPVALUE_SEXPRESSION:
        case LISPVALUE_QEXPRESSION:
            for (int i = 0; i < lv->cell_count; i++) lispvalue_delete(lv->cells[i]);
            free(lv->cells);
            break;
    }

    free(lv);
}

lispvalue* lispvalue_pop(lispvalue* lv, size_t i)
{
    // get lispvalue at index "i"
    lispvalue* x = lv->cells[i];

    // in memory, move lispvalues from index "i + 1" to index "i"
    // memory at index "i" will be overwritten by memory at index "i + 1",
    // essentially deleting it
    memmove(&lv->cells[i], &lv->cells[i + 1], sizeof(lispvalue*) * (lv->cell_count - i - 1));

    lv->cell_count--;

    // reallocate memory to accommodate the change in size
    lv->cells = realloc(lv->cells, sizeof(lispvalue*) * lv->cell_count);

    return x;
}

lispvalue* lispvalue_take(lispvalue* lv, size_t i)
{
    lispvalue* x = lispvalue_pop(lv, i);
    lispvalue_delete(lv);
    return x;
}

lispenv* lispenv_new()
{
    lispenv* le = malloc(sizeof(lispenv));
    le->parent = NULL;
    le->symbol_count = 0;
    le->symbols = NULL;
    le->values = NULL;

    return le;
}

lispenv* lispenv_copy(lispenv* le)
{
    lispenv* n = malloc(sizeof(lispenv));
    n->parent = le->parent;
    n->symbol_count = le->symbol_count;
    n->symbols = malloc(sizeof(char*) * le->symbol_count);
    n->values = malloc(sizeof(lispvalue*) * le->symbol_count);

    for (int i = 0; i < le->symbol_count; i++)
    {
        n->symbols[i] = malloc(strlen(le->symbols[i]) + 1);
        strcpy(n->symbols[i], le->symbols[i]);
        n->values[i] = lispvalue_copy(le->values[i]);
    }

    return n;
}

void lispenv_delete(lispenv* le)
{
    for (int i = 0; i < le->symbol_count; i++)
    {
        free(le->symbols[i]);
        lispvalue_delete(le->values[i]);
    }

    free(le->symbols);
    free(le->values);
    free(le);
}

void lispenv_put(lispenv* le, char* symbol, lispvalue* lv)
{
    // iterate over stored symbols, check for existence
    // if one already exists (given symbol has been defined), replace value 
    for (int i = 0; i < le->symbol_count; i++)
    {
        if (!strcmp(le->symbols[i], symbol))
        {
            lispvalue_delete(le->values[i]);
            le->values[i] = lispvalue_copy(lv);
            return;
        }
    }
    
    // otherwise allocate memory for new symbol
    le->symbol_count++;
    le->symbols = realloc(le->symbols, sizeof(char*) * le->symbol_count);
    le->values = realloc(le->values, sizeof(lispvalue*) * le->symbol_count);

    // copy given symbol and value into environment's memory
    le->values[le->symbol_count - 1] = lispvalue_copy(lv);
    le->symbols[le->symbol_count - 1] = malloc(strlen(symbol) + 1);
    strcpy(le->symbols[le->symbol_count - 1], symbol);
}

lispvalue* lispenv_get(lispenv* le, char* symbol)
{
    for (int i = 0; i < le->symbol_count; i++)
    if (!strcmp(le->symbols[i], symbol)) 
    return lispvalue_copy(le->values[i]);

    if (le->parent) return lispenv_get(le->parent, symbol);
    else return lispvalue_error("unbound symbol \"%s\"", symbol);
}

void lispenv_define(lispenv* le, char* symbol, lispvalue* lv)
{
    while (le->parent) le = le->parent;

    return lispenv_put(le, symbol, lv);
}

void lispenv_add_builtin(lispenv* le, char* name, lispbuiltin function)
{
    lispvalue* symbol = lispvalue_symbol(name);
    lispvalue* value = lispvalue_function(function);
    lispenv_put(le, symbol->symbol, value);
    lispvalue_delete(symbol); lispvalue_delete(value);
}

void lispenv_add_builtins(lispenv* le)
{
    // list functions
    lispenv_add_builtin(le, DEFINE_STR, builtin_define);
    lispenv_add_builtin(le, LEN_STR, builtin_len);
    lispenv_add_builtin(le, LIST_STR, builtin_list);
    lispenv_add_builtin(le, HEAD_STR, builtin_head);
    lispenv_add_builtin(le, TAIL_STR, builtin_tail);
    lispenv_add_builtin(le, JOIN_STR, builtin_join);
    lispenv_add_builtin(le, REVERSE_STR, builtin_reverse);
    lispenv_add_builtin(le, EVAL_STR, builtin_eval);
    lispenv_add_builtin(le, LAMBDA_STR, builtin_lambda);

    // mathematical functions
    lispenv_add_builtin(le, ADD_SYMBOL_STR, builtin_add);
    lispenv_add_builtin(le, SUB_SYMBOL_STR, builtin_sub);
    lispenv_add_builtin(le, MUL_SYMBOL_STR, builtin_mul);
    lispenv_add_builtin(le, DIV_SYMBOL_STR, builtin_div);
    lispenv_add_builtin(le, REM_SYMBOL_STR, builtin_rem);
}