#include "definitions.h"

char* lv_type_to_name(int8_t type)
{
    switch (type)
    {
        case LISPVALUE_ERROR:       return "error";
        case LISPVALUE_NUMBER:      return "number";
        case LISPVALUE_SYMBOL:      return "symbol";
        case LISPVALUE_SEXPRESSION: return "s_expression";
        case LISPVALUE_QEXPRESSION: return "q_expression";
        case LISPVALUE_FUNCTION:    return "function";
        default:                    return "unknown";
    }
}