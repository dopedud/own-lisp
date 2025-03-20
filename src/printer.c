#include <stdio.h>

#include "definitions.h"

#include "printer.h"

void lispvalue_print(lispvalue* lv)
{
    switch (lv->type)
    {
        case LISPVALUE_ERROR:       printf("error: %s", lv->error); break;
        case LISPVALUE_NUMBER:      printf("%lli", lv->number); break;
        case LISPVALUE_SYMBOL:      printf("%s", lv->symbol); break;
        case LISPVALUE_SEXPRESSION: lispvalue_print_expression(lv, '(', ')'); break;
        case LISPVALUE_QEXPRESSION: lispvalue_print_expression(lv, '{', '}'); break;
        case LISPVALUE_FUNCTION:    
            if(lv->builtin) printf("<builtin>");
            
            else
            {
                printf("(\\ "); lispvalue_print(lv->formals);
                putchar(' '); lispvalue_print(lv->body); putchar(')');
            }
             
            break;
    }
}

void lispvalue_println(lispvalue* lv)
{
    lispvalue_print(lv); putchar('\n');
}

void lispvalue_print_expression(lispvalue* lv, char open, char close)
{
    putchar(open);

    // print each nested expression
    for (int i = 0; i < lv->cell_count; i++)
    {
        // recursively print each nested expression
        lispvalue_print(lv->cells[i]);

        // keep adding spaces between each nested expression until loop ends
        if (i != (lv->cell_count - 1)) putchar(' ');
    }

    putchar(close);
}