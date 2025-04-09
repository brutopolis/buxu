#include "buxu.h"

function(new)
{
    register_var(vm, arg(0).s , TYPE_DATA, arg(1).i);
    return -1;
}

function(print_float)
{
    printf("%f\n", arg(0).f);
    return -1;
}

function(print_int)
{
    printf("%d\n", arg(0).i);
    return -1;
}

function(print_string)
{
    printf("%s\n", arg(0).s);
    return -1;
}

function(print_index)
{
    printf("%d\n", arg_i(0));
    return -1;
}

function(print_float_index)
{
    printf("%f\n", arg_f(0));
    return -1;
}

init(std)
{
    register_function(vm, "new", TYPE_DATA, new);
    register_function(vm, "print.float", TYPE_DATA, print_float);
    register_function(vm, "print.int", TYPE_DATA, print_int);
    register_function(vm, "print.string", TYPE_DATA, print_string);
    register_function(vm, "print.index", TYPE_DATA, print_index);
    register_function(vm, "print.float.index", TYPE_DATA, print_float_index);
}