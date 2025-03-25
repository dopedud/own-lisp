#include <stdlib.h>
#include <string.h>

#include "constdest.h"
#include "builtins.h"

#include "evaluator.h"

lispvalue* lispvalue_call(lispenv* le, lispvalue* function, lispvalue* lv)
{
    if (function->builtin) return function->builtin(le, lv);

    int given = lv->cell_count;
    int total = function->formals->cell_count;

    // while arguments remain to be processed
    while (lv->cell_count)
    {
        // if we ran out of formal arguments to bind, throw an error
        if (function->formals->cell_count == 0)
        {
            lispvalue_delete(lv);
            return lispvalue_error("function passed in too many arguments: "
            "expected %i, got %i", total, given);
        }

        // pop the first symbol from the formals
        lispvalue* symbol = lispvalue_pop(function->formals, 0);

        // pop next argument from the list
        lispvalue* value = lispvalue_pop(lv, 0);

        // bind a copy into the function's environment
        lispenv_put(function->env, symbol->symbol, value);

        lispvalue_delete(symbol); lispvalue_delete(value);
    }

    // argument list has now been cleared up so delete
    lispvalue_delete(lv);

    // if all formals have been bound, evaluate function
    if (function->formals->cell_count == 0)
    {
        function->env->parent = le;

        return builtin_eval(function->env,
        lispvalue_add(lispvalue_sexpression(),
        lispvalue_copy(function->body)));
    }
    
    // else return partially evaluated function
    else return lispvalue_copy(function); 
}


lispvalue* lispvalue_eval(lispenv* le, lispvalue* lv)
{
    // evaluate symbols, do symbol lookup on the environment
    if (lv->type == LISPVALUE_SYMBOL)
    {
        lispvalue* x = lispenv_get(le, lv->symbol);
        lispvalue_delete(lv);
        return x;
    }

    // evaluate s_expressions
    if (lv->type == LISPVALUE_SEXPRESSION)
    return lispvalue_eval_sexpression(le, lv);

    // otherwise remain the same
    return lv;
}

lispvalue* lispvalue_eval_sexpression(lispenv* le, lispvalue* lv)
{
    // evaluate children
    for (int i = 0; i < lv->cell_count; i++)
    lv->cells[i] = lispvalue_eval(le, lv->cells[i]);
    
    // error checking
    for (int i = 0; i < lv->cell_count; i++)
    if (lv->cells[i]->type == LISPVALUE_ERROR) return lispvalue_take(lv, i);

    if (lv->cell_count == 0) return lv;

    if (lv->cell_count == 1) return lispvalue_take(lv, 0);

    lispvalue* function = lispvalue_pop(lv, 0);
    
    // verify that the first element is a function
    if (function->type != LISPVALUE_FUNCTION)
    {
        lispvalue_delete(function); lispvalue_delete(lv);
        return lispvalue_error("first element is not a function");
    }

    // call builtin with operator
    lispvalue* result = lispvalue_call(le, function, lv);
    lispvalue_delete(function);
    return result;
}