#pragma once

#include "definitions.h"

// function to call user-defined functions
lispvalue* lispvalue_call(lispenv* le, lispvalue* function, lispvalue* lv);

lispvalue* lispvalue_eval(lispenv* le, lispvalue* lv);
lispvalue* lispvalue_eval_sexpression(lispenv* le, lispvalue* lv);