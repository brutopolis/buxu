#include "buxu.h"

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

function(print_float)
{
    printf("%f\n", arg(0).f);
    return -1;
}

function(print_bool)
{
    printf("%s\n", arg(0).i ? "true" : "false");
    return -1;
}

init(io)
{
    add_function(vm, "print.int", print_int);
    add_function(vm, "print.string", print_string);
    add_function(vm, "print.float", print_float);
    add_function(vm, "print.bool", print_bool);
}