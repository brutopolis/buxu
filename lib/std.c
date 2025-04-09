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
    printf("%f\n", pun(arg_i(0), i, f));
    return -1;
}

init(std)
{
    register_function(vm, "new", new);
    register_function(vm, "print.float", print_float);
    register_function(vm, "print.int", print_int);
    register_function(vm, "print.string", print_string);
    register_function(vm, "print.index", print_index);
    register_function(vm, "print.float.index", print_float_index);
}