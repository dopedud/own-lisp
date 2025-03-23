#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef union test_union
{
    int x;
    int y;

    struct
    {
        int x_struct;
        int y_struct;
    };
} test_union;

int main()
{
    test_union test;
    test.x = 3;
    test.y = 4;
    test.x_struct = 7;
    test.y_struct = 6;

    printf("%i\n", test.x);
    printf("%i\n", test.y);
    printf("%i\n", test.x_struct);
    printf("%i\n", test.y_struct);

    return 0;
}