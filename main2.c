#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpc/mpc.h"

#define MAX_STR_LENGTH 2048

#define LISP_ASSERT(args, condition, error) \
    if (!(condition)) { lisp_val_delete(args); return lisp_val_error(error); }

#define PROGRAM_STR "program"
#define NUMBER_STR "number"
#define SYMBOL_STR "symbol"
#define EXPRESSION_STR "expression"
#define S_EXPRESSION_STR "s_expression"
#define Q_EXPRESSION_STR "q_expression"

#include "prompter.h"

// create enumeration for possible lisp value types
enum { LISP_VAL_NUM, LISP_VAL_ERR, LISP_VAL_SYM, LISP_VAL_FUN, LISP_VAL_SEXPR, LISP_VAL_QEXPR };

// declare lisp value struct and lisp environment
struct lisp_val;
struct lisp_env;
typedef struct lisp_val lisp_val;
typedef struct lisp_env lisp_env;

// declare builtin function pointer
typedef lisp_val* (*lisp_builtin)(lisp_env* env, lisp_val* v);

struct lisp_val
{
    int type;

    long number;
    char* error;
    char* symbol;
    lisp_builtin function;

    int count;
    struct lisp_val** cell;
};

struct lisp_env
{
    int count;
    char** symbols;
    lisp_val** values;
};

// function declarations for creating and deleting environments
lisp_env* lisp_env_new();
void lisp_env_delete(lisp_env* env);

// function declarations for getting and putting values into entries from the environment
lisp_val* lisp_env_get(lisp_env* env, lisp_val* v);
void lisp_env_put(lisp_env* env, lisp_val* symbol, lisp_val* v);

// function declarations for adding builtin functions in the environment
void lisp_env_add_builtin(lisp_env* env, char* name, lisp_builtin function);
void lisp_env_add_builtins(lisp_env* env);

// function declarations for creating, copying and deleting lisp values
lisp_val* lisp_val_number(long x);
lisp_val* lisp_val_error(char* e);
lisp_val* lisp_val_symbol(char* s);
lisp_val* lisp_val_function(lisp_builtin f);
lisp_val* lisp_val_sexpr();
lisp_val* lisp_val_qexpr();

lisp_val* lisp_val_copy(lisp_val* v);

void lisp_val_delete(lisp_val* v);

// function delcarations for manipulating s-expressions
lisp_val* lisp_val_pop(lisp_val* v, int i);
lisp_val* lisp_val_take(lisp_val* v, int i);
lisp_val* lisp_val_join(lisp_val* x, lisp_val* y);

// function declarations for reading lisp expressions
lisp_val* lisp_val_read(mpc_ast_t* ast);
lisp_val* lisp_val_read_num(mpc_ast_t* ast);
lisp_val* lisp_val_add(lisp_val* v, lisp_val* x);

// function declarations for evaluating lisp expressions
lisp_val* lisp_val_eval(lisp_env* env, lisp_val* v);
lisp_val* lisp_val_eval_sexpr(lisp_env* env, lisp_val* v);

lisp_val* builtin_list(lisp_env* env, lisp_val* v);
lisp_val* builtin_head(lisp_env* env, lisp_val* v);
lisp_val* builtin_tail(lisp_env* env, lisp_val* v);
lisp_val* builtin_join(lisp_env* env, lisp_val* v);
lisp_val* builtin_eval(lisp_env* env, lisp_val* v);
lisp_val* builtin_operator(lisp_env* env, lisp_val* v, char* operator);

lisp_val* builtin_add(lisp_env* env, lisp_val* v);
lisp_val* builtin_sub(lisp_env* env, lisp_val* v);
lisp_val* builtin_mul(lisp_env* env, lisp_val* v);
lisp_val* builtin_div(lisp_env* env, lisp_val* v);

// function declarations for printing lisp values
void lisp_val_print(lisp_val* v);
void lisp_val_println(lisp_val* v);
void lisp_val_expr_print(lisp_val* v, char open, char close);

static char input[MAX_STR_LENGTH];

int main(int argc, char** argv) 
{
    // create some parsers
    mpc_parser_t* program       = mpc_new(PROGRAM_STR);

    mpc_parser_t* number        = mpc_new(NUMBER_STR);
    mpc_parser_t* symbol        = mpc_new(SYMBOL_STR);
    mpc_parser_t* expression    = mpc_new(EXPRESSION_STR);
    mpc_parser_t* s_expression  = mpc_new(SEXPRESSION_STR);
    mpc_parser_t* q_expression  = mpc_new(QEXPRESSION_STR);

    // define them with the following language
    mpca_lang(MPCA_LANG_DEFAULT,
    "                                                                                                           \
        "NUMBER_STR"        : /-?[0-9]+/ ;                                                                      \
        "SYMBOL_STR"        : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;                                                \
        "SEXPRESSION_STR"  : '(' <"EXPRESSION_STR">* ')' ;                                                     \
        "QEXPRESSION_STR"  : '{' <"EXPRESSION_STR">* '}' ;                                                     \
        "EXPRESSION_STR"    : <"NUMBER_STR"> | <"SYMBOL_STR"> | <"SEXPRESSION_STR"> | <"QEXPRESSION_STR"> ;   \
        "PROGRAM_STR"       : /^/ <"EXPRESSION_STR">* /$/ ;                                                     \
    ",
    number, symbol, s_expression, q_expression, expression, program);

    lisp_env* env = lisp_env_new();
    lisp_add_builtins(env);

    printf("Lispy Version 0.0.1\n");
    printf("Press Ctrl+C to Exit\n");

    while (1)
    {
        // output our prompt and get input
        char* input = readline("lispy> ");
        add_history(input);

        // attempts to parse user input
        mpc_result_t result;
        
        if (mpc_parse("<stdin>", input, program, &result))
        {
            lisp_val* r = lisp_val_eval(env, lisp_val_read(result.output));
            // lisp_val* r = lisp_val_read(result.output);
            lisp_val_println(r);
            lisp_val_delete(r);

            mpc_ast_delete(result.output);
        }

        else
        {
            mpc_err_print(result.error);
            mpc_err_delete(result.error);
        }

        free(input);
    }

    // free up the parsers
    mpc_cleanup(6, number, symbol, s_expression, q_expression, expression, program);

    return 0;
}

lisp_env* lisp_env_new()
{
    lisp_env* env = malloc(sizeof(lisp_env));
    env->count = 0;
    env->symbols = NULL;
    env->values = NULL;
    
    return env;
}

void lisp_env_delete(lisp_env* env)
{
    for (int i = 0; i < env->count; i++)
    {
        free(env->symbols[i]);
        lisp_env_delete(env->values[i]);
    }

    free(env->symbols);
    free(env->values);
    free(env);
}

lisp_val* lisp_env_get(lisp_env* env, lisp_val* v)
{
    // iterate over all entries stored in the environment
    // check if the entry string matches the given string
    // if it does, return a copy of the value
    for (int i = 0; i < env->count; i++)
    if (!strcmp(env->symbols[i], v->symbol)) 
    return lisp_val_copy(env->values[i]);

    return lisp_val_error("unknown symbol!");
}

void lisp_env_put(lisp_env* env, lisp_val* symbol, lisp_val* v)
{   
    // iterate over all entries stored in the environment
    // check to see if a variable with the same name already exists
    for (int i = 0; i < env->count; i++)
    {
        // if an entry is found delete the value at that position
        // and replace with the new value supplied by user
        if (!strcmp(env->symbols, symbol->symbol))
        {
            lisp_val_delete(env->values[i]);
            env->values[i] = lisp_val_copy(v);
            return;
        }

        // if no existing entry found, allocate space for the new entry
        env->count++;
        env->values = realloc(env->values, sizeof(lisp_val*) * env->count);
        env->symbols = realloc(env->symbols, sizeof(char*) * env->count);

        // copy the contents of the entry and its value into the new location
        env->values[env->count - 1] = lisp_val_copy(v);
        env->symbols[env->count - 1] = malloc(strlen(symbol->symbol) + 1);
        strcpy(env->symbols[env->count - 1], symbol->symbol);
    }
}

void lisp_env_add_builtin(lisp_env* env, char* name, lisp_builtin function)
{
    lisp_val* symbol = lisp_val_symbol(name);
    lisp_val* value = lisp_val_function(function);

    lisp_env_put(env, symbol, value);

    lisp_val_delete(symbol);
    lisp_val_delete(value);
}

void lisp_env_add_builtins(lisp_env* env)
{
    // list functions
    list_env_add_builtin(env, "list", builtin_list);
    list_env_add_builtin(env, "head", builtin_head);
    list_env_add_builtin(env, "tail", builtin_tail);
    list_env_add_builtin(env, "eval", builtin_eval);
    list_env_add_builtin(env, "join", builtin_join);

    // mathematical functions
    list_env_add_builtin(env, "+", builtin_add(env, "+"));
    list_env_add_builtin(env, "-", builtin_sub(env, "-"));
    list_env_add_builtin(env, "*", builtin_mul(env, "*"));
    list_env_add_builtin(env, "/", builtin_div(env, "/"));
}

lisp_val* lisp_val_number(long x)
{
    lisp_val* v = malloc(sizeof(lisp_val));
    v->type = LISP_VAL_NUM;
    v->number = x;
    return v;
}

lisp_val* lisp_val_error(char* e)
{
    lisp_val* v = malloc(sizeof(lisp_val));
    v->type = LISP_VAL_ERR;
    v->error = malloc(strlen(e) + 1);
    strcpy(v->error, e);
    return v;
}

lisp_val* lisp_val_symbol(char* s)
{
    lisp_val* v = malloc(sizeof(lisp_val));
    v->type = LISP_VAL_SYM;
    v->symbol = malloc(strlen(s) + 1);
    strcpy(v->symbol, s);
    return v;
}

lisp_val* lisp_val_function(lisp_builtin f)
{
    lisp_val* v = malloc(sizeof(lisp_val));
    v->type = LISP_VAL_FUN;
    v->function = f;
    return v;
}

lisp_val* lisp_val_sexpr() 
{
    lisp_val* v = malloc(sizeof(lisp_val));
    v->type = LISP_VAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

lisp_val* lisp_val_qexpr()
{
    lisp_val* v = malloc(sizeof(lisp_val));
    v->type = LISP_VAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

lisp_val* lisp_val_copy(lisp_val* v)
{
    lisp_val* x = malloc(sizeof(lisp_val));
    x->type = v->type;

    switch(v->type)
    {
        // copy functions and Numbers Directly
        case LISP_VAL_FUN: x->function = v->function; break;
        case LISP_VAL_NUM: x->number = v->number; break;

        // copy strings using malloc and strcpy
        case LISP_VAL_ERR:
            x->error = malloc(strlen(v->error) + 1);
            strcpy(x->error, v->error); 
            break;

        case LISP_VAL_SYM:
            x->symbol = malloc(strlen(v->symbol) + 1);
            strcpy(x->symbol, v->symbol); 
            break;

        // copy lists by copying each sub-expression
        case LISP_VAL_SEXPR:
        case LISP_VAL_QEXPR:
            x->count = v->count;
            x->cell = malloc(sizeof(lisp_val*) * x->count);
            for (int i = 0; i < x->count; i++) x->cell[i] = lisp_val_copy(v->cell[i]);
            break;
    }

    return x;
}

void lisp_val_delete(lisp_val* v)
{
    switch (v->type) 
    {
        case LISP_VAL_NUM: 
        case LISP_VAL_FUN:
            break;
    
        case LISP_VAL_ERR: free(v->error); break;
        case LISP_VAL_SYM: free(v->symbol); break;
    
        case LISP_VAL_SEXPR:
        case LISP_VAL_QEXPR:
            for (int i = 0; i < v->count; i++) lisp_val_delete(v->cell[i]);
            free(v->cell);
            break;
    }
    
    free(v);
}

lisp_val* lisp_val_pop(lisp_val* v, int i)
{
    // find the item at 'i'
    lisp_val* x = v->cell[i];
  
    // shift memory after the item at 'i' over the top
    memmove(&v->cell[i], &v->cell[i + 1], sizeof(lisp_val*) * (v->count - i - 1));
  
    v->count--;

    v->cell = realloc(v->cell, sizeof(lisp_val*) * v->count);
    
    return x;
}

lisp_val* lisp_val_take(lisp_val* v, int i) 
{
    lisp_val* x = lisp_val_pop(v, i);
    lisp_val_delete(v);
    return x;
}

lisp_val* lisp_val_join(lisp_val* x, lisp_val* y)
{
    // for each cell in 'y' add it to 'x'
    while (y->count) x = lisp_val_add(x, lisp_val_pop(y, 0));

    lisp_val_delete(y);

    return x;
}

lisp_val* lisp_val_read(mpc_ast_t* ast)
{
    if (strstr(ast->tag, NUMBER_STR)) return lisp_val_read_num(ast);
    if (strstr(ast->tag, SYMBOL_STR)) return lisp_val_symbol(ast->contents);
  
    // If root (>) or sexpr then create empty list
    lisp_val* x = NULL;
    if (!strcmp(ast->tag, ">")) x = lisp_val_sexpr();
    if (strstr(ast->tag, SEXPRESSION_STR)) x = lisp_val_sexpr();
    if (strstr(ast->tag, QEXPRESSION_STR)) x = lisp_val_qexpr();
  
    // fill this list with any valid expression contained within
    for (int i = 0; i < ast->children_num; i++) 
    {
        if (!strcmp(ast->children[i]->contents, "(")) continue;
        if (!strcmp(ast->children[i]->contents, ")")) continue;
        if (!strcmp(ast->children[i]->contents, "{")) continue;
        if (!strcmp(ast->children[i]->contents, "}")) continue;
        if (!strcmp(ast->children[i]->tag, "regex")) continue;

        x = lisp_val_add(x, lisp_val_read(ast->children[i]));
    }
  
    return x;
}

lisp_val* lisp_val_read_num(mpc_ast_t* ast)
{
    errno = 0;
    long x = strtol(ast->contents, NULL, 10);
    return errno != ERANGE ? lisp_val_number(x) : lisp_val_error("invalid number");
}

lisp_val* lisp_val_add(lisp_val* v, lisp_val* x)
{
    v->count++;
    v->cell = realloc(v->cell, sizeof(lisp_val*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

lisp_val* builtin_list(lisp_env* env, lisp_val* v)
{
    v->type = LISP_VAL_QEXPR;

    return v;
}

lisp_val* builtin_head(lisp_env* env, lisp_val* v)
{
    LISP_ASSERT(v, v->count == 1, "function 'head' passed too many arguments!");
    LISP_ASSERT(v, v->cell[0]->type == LISP_VAL_QEXPR, "function 'head' passed an incorrect type!");
    LISP_ASSERT(v, v->cell[0]->count, "function 'head' passed '{}'!");

    lisp_val* x = lisp_val_take(v, 0);

    while (x->count > 1) lisp_val_delete(lisp_val_pop(x, 1));

    return x;
}

lisp_val* builtin_tail(lisp_env* env, lisp_val* v)
{
    LISP_ASSERT(v, v->count == 1, "function 'head' passed too many arguments!");
    LISP_ASSERT(v, v->cell[0]->type == LISP_VAL_QEXPR, "function 'head' passed an incorrect type!");
    LISP_ASSERT(v, v->cell[0]->count, "function 'head' passed '{}'!");

    lisp_val* x = lisp_val_take(v, 0);

    lisp_val_delete(lisp_val_pop(x, 0));

    return x;
}

lisp_val* builtin_join(lisp_env* env, lisp_val* v)
{
    for (int i = 0; i < v->count; i++)
    LISP_ASSERT(v, v->cell[0]->type == LISP_VAL_QEXPR, "function 'join' passed an incorrect type!");

    lisp_val* x = lisp_val_pop(v, 0);

    while (v->count) x = lisp_val_join(x, lisp_val_pop(v, 0));

    lisp_val_delete(v);
    
    return x;
}

lisp_val* builtin_eval(lisp_env* env, lisp_val* v)
{
    LISP_ASSERT(v, v->count == 1, "function 'eval' passed too many arguments!");
    LISP_ASSERT(v, v->cell[0]->type == LISP_VAL_QEXPR, "function 'eval' passed an incorrect type!");

    lisp_val* x = lisp_val_take(v, 0);
    x->type = LISP_VAL_SEXPR;

    return lisp_val_eval(env, x);
}

lisp_val* builtin_operator(lisp_env* env, lisp_val* v, char* operator)
{
    for (int i = 0; i < v->count; i++)
    {
        if (v->cell[i]->type != LISP_VAL_NUM) 
        {
            lisp_val_delete(v);
            return lisp_val_error("Cannot operate on non-number!");
        }
    }

    // pop the first element
    lisp_val* x = lisp_val_pop(v, 0);

    // if no arguments and there's a minus sign then perform unary negation
    if (!strcmp(operator, "-") && !v->count) x->number = -x->number;
    
    while (v->count > 0) 
    {
        // pop the next element
        lisp_val* y = lisp_val_pop(v, 0);

        if (!strcmp(operator, "+")) x->number += y->number;
        if (!strcmp(operator, "-")) x->number -= y->number;
        if (!strcmp(operator, "*")) x->number *= y->number;
        if (!strcmp(operator, "/"))
        {
            if (!y->number) 
            {
                lisp_val_delete(x); lisp_val_delete(y);
                x = lisp_val_error("Division By Zero!"); 
                break;
            }

            x->number /= y->number;
        }

        lisp_val_delete(y);
    }

    lisp_val_delete(v);

    return x;
}

lisp_val* builtin_add(lisp_env* env, lisp_val* v) { return builtin_operator(env, v, "+"); }
lisp_val* builtin_sub(lisp_env* env, lisp_val* v) { return builtin_operator(env, v, "-"); }
lisp_val* builtin_mul(lisp_env* env, lisp_val* v) { return builtin_operator(env, v, "*"); }
lisp_val* builtin_div(lisp_env* env, lisp_val* v) { return builtin_operator(env, v, "/"); }

lisp_val* lisp_val_eval(lisp_env* env, lisp_val* v)
{
    if (v->type == LISP_VAL_SYM)
    {
        lisp_val* x = lisp_env_get(env, v);
        lisp_val_delete(v);
        
        return x;
    }

    if (v->type == LISP_VAL_SEXPR) return lisp_val_eval_sexpr(env, v);

    // all other lval types remain the same
    return v;
}

lisp_val* lisp_val_eval_sexpr(lisp_env* env, lisp_val* v)
{
    // evaluate children
    for (int i = 0; i < v->count; i++)
    v->cell[i] = lisp_val_eval(env, v->cell[i]);
  
    // error checking
    for (int i = 0; i < v->count; i++)
    if (v->cell[i]->type == LISP_VAL_ERR) return lisp_val_take(v, i);
  
    // empty expression
    if (!v->count) return v;
  
    // single expression
    if (v->count == 1) return lisp_val_take(v, 0);
  
    // ensure the first element is a function after evaluation
    lisp_val* f = lisp_val_pop(v, 0);
    if (f->type != LISP_VAL_FUN) 
    {
        lisp_val_delete(f); lisp_val_delete(v);
        return lisp_val_error("first element is not a function!");
    }
  
    // call function to get result
    lisp_val* result = f->function(env, v);
    lisp_val_delete(f);
    
    return result;
}

void lisp_val_print(lisp_val* v)
{
    switch (v->type) 
    {
        case LISP_VAL_NUM:      printf("%li", v->number); break;
        case LISP_VAL_ERR:      printf("Error: %s", v->error); break;
        case LISP_VAL_SYM:      printf("%s", v->symbol); break;
        case LISP_VAL_FUN:      printf("<function>"); break;
        case LISP_VAL_SEXPR:    lisp_val_expr_print(v, '(', ')'); break;
        case LISP_VAL_QEXPR:    lisp_val_expr_print(v, '{', '}'); break;
    }
}
  
void lisp_val_println(lisp_val* v)
{ 
    lisp_val_print(v); putchar('\n');
}

void lisp_val_expr_print(lisp_val* v, char open, char close)
{
    putchar(open);
    
    for (int i = 0; i < v->count; i++) 
    {
        lisp_val_print(v->cell[i]);
    
        // don't print trailing space if last element
        if (i != (v->count - 1)) putchar(' ');
    }

    putchar(close);
}