#pragma once

#include "definitions.h"

#ifndef MAX_STR_LENGTH
#define MAX_STR_LENGTH 2048u
#endif

// ///////////////////////////////////////////
// // FUNCTION DECLERATIONS FOR LISP VALUES //
// ///////////////////////////////////////////

// functions to construct and destruct lisp values
lispvalue* lispvalue_error(char* format, ...);
lispvalue* lispvalue_number(int64_t x);
lispvalue* lispvalue_symbol(char* s);
lispvalue* lispvalue_sexpression();
lispvalue* lispvalue_qexpression();
lispvalue* lispvalue_function(lispbuiltin function);
lispvalue* lispvalue_lambda(lispvalue* formals, lispvalue* body);

// function to add child elements into a lispvalue's cells
// (for s_expressions and q_expressions)
lispvalue* lispvalue_add(lispvalue* lv, lispvalue* new_lv);

// copy and delete lisp values
lispvalue* lispvalue_copy(lispvalue* lv);
void lispvalue_delete(lispvalue* lv);

// function to "pop" a child element of a lispvalue given index "i"
lispvalue* lispvalue_pop(lispvalue* lv, int i);

// like "lispvalue_pop" but deletes the child element after popping
lispvalue* lispvalue_take(lispvalue* lv, int i);

// /////////////////////////////////////////////////
// // FUNCTION DECLERATIONS FOR LISP ENVIRONMENTS //
// /////////////////////////////////////////////////

// functions to construct and destruct lisp environments
lispenv* lispenv_new();
lispenv* lispenv_copy(lispenv* le);
void lispenv_delete(lispenv* le);

// functions to put and get a symbol to its corresponding values into an environment
void lispenv_put(lispenv* le, char* symbol, lispvalue* lv);
lispvalue* lispenv_get(lispenv* le, char* symbol);
void lispenv_define(lispenv* le, char* symbol, lispvalue* lv);

// functions to add builtin functions into an environment
void lispenv_add_builtin(lispenv* le, char* name, lispbuiltin function);
void lispenv_add_builtins(lispenv* le);