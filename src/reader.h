#pragma once

#include "mpc/mpc.h"

#include "definitions.h"

lispvalue* lispvalue_read(mpc_ast_t* ast);
lispvalue* lispvalue_read_number(mpc_ast_t* ast);