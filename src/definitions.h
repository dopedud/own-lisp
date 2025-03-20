#pragma once

#include <stdint.h>

// #include "evaluator.h"

// define string constants as macros
// s_expression is "symbolic expression"
// q_expression in "quoted expression"
#define PROGRAM_STR         "program"
#define EXPRESSION_STR      "expression"
#define SEXPRESSION_STR     "s_expression"
#define QEXPRESSION_STR     "q_expression"
#define SYMBOL_STR          "symbol"
#define NUMBER_STR          "number"

#define DEFINE_STR          "define"

#define LEN_STR             "len"
#define LIST_STR            "list"
#define HEAD_STR            "head"
#define TAIL_STR            "tail"
#define JOIN_STR            "join"
#define REVERSE_STR         "reverse"
#define EVAL_STR            "eval"
#define LAMBDA_STR          "\\"

#define ADD_SYMBOL_STR "+"
#define SUB_SYMBOL_STR "-"
#define MUL_SYMBOL_STR "*"
#define DIV_SYMBOL_STR "/"
#define REM_SYMBOL_STR "%"

enum OPERATION_SYMBOL
{
    ADD = 10, SUB, MUL, DIV, REM
};

enum LISPVALUE_TYPE
{
    LISPVALUE_ERROR = 0,
    LISPVALUE_NUMBER,
    LISPVALUE_SYMBOL,
    LISPVALUE_SEXPRESSION,
    LISPVALUE_QEXPRESSION,
    LISPVALUE_FUNCTION
};

struct lispenv;
struct lispvalue;
typedef struct lispenv lispenv;
typedef struct lispvalue lispvalue;

// function pointer to point to builtin functions
typedef lispvalue*(*lispbuiltin)(lispenv*, lispvalue*);

// lisp environment struct to store declared variables and builtin and declared functions
typedef struct lispenv
{
    lispenv* parent;
    int64_t symbol_count;
    char** symbols;
    lispvalue** values;
} lispenv;

// lisp value struct to store a value's type and the value itself,
// as well as its child elements (for s_expressions and q_expressions)
typedef struct lispvalue
{
    int8_t type;

    union
    {
        int64_t number;
        char* error;
        char* symbol;

        struct
        {
            lispbuiltin builtin;
            lispenv* env;
            lispvalue* formals;
            lispvalue* body;
        };
    };

    int64_t cell_count;
    struct lispvalue** cells;
} lispvalue;

char* lv_type_to_name(int8_t type);
// char* lv_function_to_name(lispbuiltin function);