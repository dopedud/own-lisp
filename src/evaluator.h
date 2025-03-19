#pragma once

#include "definitions.h"

lispvalue* lispvalue_eval(lispenv* le, lispvalue* lv);
lispvalue* lispvalue_eval_sexpression(lispenv* le, lispvalue* lv);

lispvalue* builtin_define(lispenv* le, lispvalue* lv);

lispvalue* builtin_len(lispenv* le, lispvalue* lv);
lispvalue* builtin_list(lispenv* le, lispvalue* lv);
lispvalue* builtin_head(lispenv* le, lispvalue* lv);
lispvalue* builtin_tail(lispenv* le, lispvalue* lv);
lispvalue* builtin_join(lispenv* le, lispvalue* lv);
lispvalue* builtin_reverse(lispenv* le, lispvalue* lv);
lispvalue* builtin_eval(lispenv* le, lispvalue* lv);

lispvalue* lispvalue_join(lispvalue* lv, lispvalue* new_lv);

lispvalue* builtin_add(lispenv* le, lispvalue* lv);
lispvalue* builtin_sub(lispenv* le, lispvalue* lv);
lispvalue* builtin_mul(lispenv* le, lispvalue* lv);
lispvalue* builtin_div(lispenv* le, lispvalue* lv);
lispvalue* builtin_rem(lispenv* le, lispvalue* lv);

lispvalue* builtin_operator(lispenv* le, lispvalue* lv, char* op);

lispvalue* builtin_environment(lispenv* le, lispvalue* lv);