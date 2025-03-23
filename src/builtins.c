#include <string.h>

#include "definitions.h"
#include "constdest.h"
#include "evaluator.h"

#include "builtins.h"

#define LISP_ASSERT(lv, condition, format, ...)                     \
    if (!(condition))                                               \
    {                                                               \
        lispvalue* error = lispvalue_error(format, ##__VA_ARGS__);  \
        lispvalue_delete(lv);                                       \
        return error;                                               \
    }                                                               \

lispvalue* builtin_define(lispenv* le, lispvalue* lv)
{
    LISP_ASSERT(lv, lv->cells[0]->type == LISPVALUE_QEXPRESSION,
        "function \"define\" passed in an incorrect type: "
        "expected %s, got %s",
        lv_type_to_name(LISPVALUE_QEXPRESSION), lv_type_to_name(lv->cells[0]->type)
    )

    lispvalue* symbols = lv->cells[0];

    for (int i = 0; i < symbols->cell_count; i++)
    {
        LISP_ASSERT(lv, symbols->cells[i]->type == LISPVALUE_SYMBOL,
            "function \"define\" cannot define non-symbol: "
            "non-symbol number %i", i
        )
    }

    LISP_ASSERT(lv, symbols->cell_count == lv->cell_count - 1,
        "function \"define\" received inconsistent number of values and symbols: "
        "%lli symbols, %lli values",
        symbols->cell_count, lv->cell_count - 1
    )

    for (int i = 0; i < symbols->cell_count; i++)
    lispenv_put(le, symbols->cells[i]->symbol, lv->cells[i + 1]);

    lispvalue_delete(lv);
    return lispvalue_sexpression();
}

lispvalue* builtin_len(lispenv* le, lispvalue* lv)
{
    LISP_ASSERT(lv, lv->cells[0]->cell_count != 0,
        "function \"len\" passed in no arguments")

    else LISP_ASSERT(lv, lv->cell_count == 1,
        "function \"len\" passed in too many arguments")

    else LISP_ASSERT(lv, lv->cells[0]->type == LISPVALUE_QEXPRESSION,
        "function \"len\" passed in an incorrect type: "
        "expected %s, got %s",
        lv_type_to_name(LISPVALUE_QEXPRESSION), lv_type_to_name(lv->cells[0]->type)
    )

    lispvalue* x = lispvalue_take(lv, 0);

    return lispvalue_number(x->cell_count);
}

lispvalue* builtin_list(lispenv* le, lispvalue* lv)
{
    lv->type = LISPVALUE_QEXPRESSION;
    return lv;
}

lispvalue* builtin_head(lispenv* le, lispvalue* lv)
{
    LISP_ASSERT(lv, lv->cells[0]->cell_count != 0,
        "function \"head\" passed in no arguments")

    else LISP_ASSERT(lv, lv->cell_count == 1,
        "function \"head\" passed in too many arguments")

    else LISP_ASSERT(lv, lv->cells[0]->type == LISPVALUE_QEXPRESSION,
        "function \"head\" passed in an incorrect type: "
        "expected %s, got %s",
        lv_type_to_name(LISPVALUE_QEXPRESSION), lv_type_to_name(lv->cells[0]->type)
    )

    lispvalue* x = lispvalue_take(lv, 0);

    while (x->cell_count > 1) lispvalue_delete(lispvalue_pop(x, 1));

    return x;
}

lispvalue* builtin_tail(lispenv* le, lispvalue* lv)
{
    LISP_ASSERT(lv, lv->cells[0]->cell_count != 0,
        "function \"tail\" passed in no arguments")

    else LISP_ASSERT(lv, lv->cell_count == 1,
        "function \"tail\" passed in too many arguments")

    else LISP_ASSERT(lv, lv->cells[0]->type == LISPVALUE_QEXPRESSION,
        "function \"tail\" passed in an incorrect type: "
        "expected %s, got %s",
        lv_type_to_name(LISPVALUE_QEXPRESSION), lv_type_to_name(lv->cells[0]->type)
    )

    lispvalue* x = lispvalue_take(lv, 0);

    lispvalue_delete(lispvalue_pop(x, 0));
    
    return x;
}

lispvalue* builtin_join(lispenv* le, lispvalue* lv)
{
    for (int i = 0; i < lv->cell_count; i++) 
    LISP_ASSERT(lv, lv->cells[i]->type == LISPVALUE_QEXPRESSION,
        "function \"join\" passed in an incorrect type at argument %i: "
        "expected %s, got %s", i,
        lv_type_to_name(LISPVALUE_QEXPRESSION), lv_type_to_name(lv->cells[i]->type)
    )

    lispvalue* x = lispvalue_pop(lv, 0);

    while (lv->cell_count) x = lispvalue_join(x, lispvalue_pop(lv, 0));

    lispvalue_delete(lv);
    
    return x;
}

lispvalue* builtin_reverse(lispenv* le, lispvalue* lv)
{
    LISP_ASSERT(lv, lv->cells[0]->cell_count != 0,
        "function \"reverse\" passed in no arguments")

    else LISP_ASSERT(lv, lv->cell_count == 1,
        "function \"reverse\" passed in too many arguments")

        else LISP_ASSERT(lv, lv->cells[0]->type == LISPVALUE_QEXPRESSION,
            "function \"reverse\" passed in an incorrect type: "
            "expected %s, got %s",
            lv_type_to_name(LISPVALUE_QEXPRESSION), lv_type_to_name(lv->cells[0]->type)
        )

    lispvalue* reversed = lispvalue_qexpression();

    lispvalue* x = lispvalue_take(lv, 0);

    while (x->cell_count)
    reversed = lispvalue_add(reversed, lispvalue_pop(x, x->cell_count - 1));

    lispvalue_delete(x);

    return reversed;
}

lispvalue* builtin_eval(lispenv* le, lispvalue* lv)
{
    LISP_ASSERT(lv, lv->cells[0]->cell_count != 0,
        "function \"eval\" passed in no arguments")

    else LISP_ASSERT(lv, lv->cell_count == 1, 
        "function \"eval\" passed in too many arguments")
    
    else LISP_ASSERT(lv, lv->cells[0]->type == LISPVALUE_QEXPRESSION,
        "function \"eval\" passed in an incorrect type: "
        "expected %s, got %s",
        lv_type_to_name(LISPVALUE_QEXPRESSION), lv_type_to_name(lv->cells[0]->type)
    )

    lispvalue* x = lispvalue_take(lv, 0);
    x->type = LISPVALUE_SEXPRESSION;
    return lispvalue_eval(le, x);
}

lispvalue* builtin_lambda(lispenv* le, lispvalue* lv)
{
    LISP_ASSERT(lv, lv->cells[0]->cell_count != 0,
        "user-defined function passed in no arguments")

    else LISP_ASSERT(lv, lv->cells[0]->cell_count == 2,
        "user-defined function passed in too little/too many arguments")

    else LISP_ASSERT(lv, lv->cells[0]->type == LISPVALUE_QEXPRESSION,
        "user-defined function passed in an incorrect type for argument 0: "
        "expected %s, got %s",
        lv_type_to_name(LISPVALUE_QEXPRESSION), lv_type_to_name(lv->cells[0]->type)
    )

    else LISP_ASSERT(lv, lv->cells[1]->type == LISPVALUE_QEXPRESSION,
        "user-defined function passed in an incorrect type for argument 1: "
        "expected %s, got %s",
        lv_type_to_name(LISPVALUE_QEXPRESSION), lv_type_to_name(lv->cells[0]->type)
    )

    // check first q_expression contain only symbols
    for (int i = 0; i < lv->cells[0]->cell_count; i++)
    LISP_ASSERT(lv, lv->cells[0]->cells[i]->type == LISPVALUE_SYMBOL,
        "cannot operate formals on a non-symbol: "
        "expected %s, got %s",
        lv_type_to_name(LISPVALUE_SYMBOL), lv_type_to_name(lv->cells[0]->cells[i]->type)
    )

    lispvalue* formals = lispvalue_pop(lv, 0);
    lispvalue* body = lispvalue_take(lv, 0);

    return lispvalue_lambda(formals, body);
}

lispvalue* lispvalue_join(lispvalue* lv, lispvalue* new_lv)
{
    while (new_lv->cell_count) lv = lispvalue_add(lv, lispvalue_pop(new_lv, 0));
    lispvalue_delete(new_lv);
    return lv;
}

lispvalue* builtin_add(lispenv* le, lispvalue* lv) { return builtin_operator(le, lv, ADD_SYMBOL_STR); }
lispvalue* builtin_sub(lispenv* le, lispvalue* lv) { return builtin_operator(le, lv, SUB_SYMBOL_STR); }
lispvalue* builtin_mul(lispenv* le, lispvalue* lv) { return builtin_operator(le, lv, MUL_SYMBOL_STR); }
lispvalue* builtin_div(lispenv* le, lispvalue* lv) { return builtin_operator(le, lv, DIV_SYMBOL_STR); }
lispvalue* builtin_rem(lispenv* le, lispvalue* lv) { return builtin_operator(le, lv, REM_SYMBOL_STR); }

lispvalue* builtin_operator(lispenv* le, lispvalue* lv, char* op)
{
    // ensures all elements are numbers
    for (int i = 0; i < lv->cell_count; i++)
    {
        if (lv->cells[i]->type != LISPVALUE_NUMBER) 
        {
            lispvalue_delete(lv);
            return lispvalue_error("cannot operate on non-number");
        }
    }

    lispvalue* x = lispvalue_pop(lv, 0);

    // perform unary negation
    if (!strcmp(op, SUB_SYMBOL_STR) && lv->cell_count == 0) x->number = -x->number;

    // perform operations on the rest of the elements
    while (lv->cell_count > 0)
    {
        lispvalue* y = lispvalue_pop(lv, 0);

        if (!strcmp(op, ADD_SYMBOL_STR)) x->number += y->number;
        else if (!strcmp(op, SUB_SYMBOL_STR)) x->number -= y->number;
        else if (!strcmp(op, MUL_SYMBOL_STR)) x->number *= y->number;
        else if (!strcmp(op, DIV_SYMBOL_STR))
        {
            if (!y->number)
            {
                lispvalue_delete(x); lispvalue_delete(y);
                x = lispvalue_error("division by zero"); break;
            }

            x->number /= y->number;
        }

        else if (!strcmp(op, REM_SYMBOL_STR)) x->number %= y->number;
    
        lispvalue_delete(y);
    }

    lispvalue_delete(lv);
    
    return x;
}