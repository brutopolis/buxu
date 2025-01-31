#include "bruter.h"

function(inplace_add)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).number += arg(i).number;
    }
    return -1;
}

function(inplace_sub)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).number -= arg(i).number;
    }
    return -1;
}

function(inplace_mul)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).number *= arg(i).number;
    }
    return -1;
}

function(inplace_div)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).number /= arg(i).number;
    }
    return -1;
}

function(inplace_mod)
{
    arg(0).number = fmod(arg(0).number, arg(1).number);
    return -1;
}

function(inplace_pow)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).number = pow(arg(0).number, arg(i).number);
    }
    return -1;
}

function(inplace_and)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).integer = arg(0).integer && arg(i).integer;
    }
    return -1;
}

function(inplace_or)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).integer = arg(0).integer || arg(i).integer;
    }
    return -1;
}

function(inplace_sin)
{
    arg(0).number = sin(arg(0).number);
    return -1;
}

function(inplace_cos)
{
    arg(0).number = cos(arg(0).number);
    return -1;
}

function(inplace_tan)
{
    arg(0).number = tan(arg(0).number);
    return -1;
}

function(inplace_round)
{
    arg(0).number = round(arg(0).number);
    return -1;
}

function(inplace_floor)
{
    arg(0).number = floor(arg(0).number);
    return -1;
}

function(inplace_ceil)
{
    arg(0).number = ceil(arg(0).number);
    return -1;
}

init(std_inplace)
{
    register_builtin(vm, "add", inplace_add);
    register_builtin(vm, "sub", inplace_sub);
    register_builtin(vm, "mul", inplace_mul);
    register_builtin(vm, "div", inplace_div);
    register_builtin(vm, "mod", inplace_mod);
    register_builtin(vm, "pow", inplace_pow);
    register_builtin(vm, "and", inplace_and);
    register_builtin(vm, "or", inplace_or);
    register_builtin(vm, "sin", inplace_sin);
    register_builtin(vm, "cos", inplace_cos);
    register_builtin(vm, "tan", inplace_tan);
    register_builtin(vm, "round", inplace_round);
    register_builtin(vm, "floor", inplace_floor);
    register_builtin(vm, "ceil", inplace_ceil);
}