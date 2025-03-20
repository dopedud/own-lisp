#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "mpc/mpc.h"

#define MAX_STR_LENGTH 2048

#include "prompter.h"

#include "definitions.h"
#include "constdest.h"
#include "printer.h"
#include "reader.h"
#include "evaluator.h"

// function to print the outcome of the parsed program
void print(int outcome, mpc_result_t* result, lispenv* le);

int main(int argc, char** argv)
{
    printf("Lispy Version 0.0.1\n");
    printf("type in \"q\" or \"quit\" to exit\n");

    // create some parsers
    mpc_parser_t* program       = mpc_new(PROGRAM_STR);
    mpc_parser_t* expression    = mpc_new(EXPRESSION_STR);
    mpc_parser_t* s_expression  = mpc_new(SEXPRESSION_STR);
    mpc_parser_t* q_expression  = mpc_new(QEXPRESSION_STR);
    mpc_parser_t* symbol        = mpc_new(SYMBOL_STR);
    mpc_parser_t* number        = mpc_new(NUMBER_STR);

    // define the grammar of the language
    mpca_lang(MPCA_LANG_DEFAULT,
    "                                                               \
        "PROGRAM_STR"       : /^/ <"EXPRESSION_STR">* /$/ ;         \
        "EXPRESSION_STR"    : <"NUMBER_STR"> | <"SYMBOL_STR">       \
                            | <"SEXPRESSION_STR">                   \
                            | <"QEXPRESSION_STR"> ;                 \
        "SEXPRESSION_STR"   : '(' <"EXPRESSION_STR">* ')' ;         \
        "QEXPRESSION_STR"   : '{' <"EXPRESSION_STR">* '}' ;         \
        "SYMBOL_STR"        : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;    \
        "NUMBER_STR"        : /-?[0-9]+/ ;                          \
    ",
    program, expression, s_expression, q_expression, symbol, number);

    mpc_result_t result;

    lispenv* le = lispenv_new();
    lispenv_add_builtins(le);

    // arguments provided are names to a file, so parse them
    if (argc > 1) print(mpc_parse_contents(argv[1], program, &result), &result, le);

    // else runs as an interpreter
    else
    {
        // interpreter runs in a loop forever until an exit condition is met
        // which terminates the program
        while (1)
        {
            char* input = readline("lispy> ");
            add_history(input);

            if (!strcmp(input, "quit") || !strcmp(input, "q")) break;

            print(mpc_parse("lispy> ", input, program, &result), &result, le);

            free(input);
        }
    }

    lispenv_delete(le);

    // clean up the parsers
    mpc_cleanup(6, number, symbol, q_expression, s_expression, expression, program);

    return 0;
}

void print(int outcome, mpc_result_t* result, lispenv* le)
{
    if (outcome)
    {
        mpc_ast_print(result->output);

        printf("\n");

        lispvalue* lv = lispvalue_read(result->output);

        printf("read as: ");
        lispvalue_println(lv);

        lv = lispvalue_eval(le, lv);

        printf("evaluate as: ");
        lispvalue_println(lv);

        lispvalue_delete(lv);
        mpc_ast_delete(result->output);
    }

    else
    {
        mpc_err_print(result->error);
        mpc_err_delete(result->error);
    }
}