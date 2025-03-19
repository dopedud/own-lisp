#pragma once

#include "definitions.h"

// functions to pretty print parsed tree
void lispvalue_print(lispvalue* lv);
void lispvalue_println(lispvalue* lv);
void lispvalue_print_expression(lispvalue* lv, char open, char close);