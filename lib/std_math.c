#include "bruter.h"

// math functions
// math functions
// math functions
// math functions


function(brl_std_math_add)
{
    Float result = 0;
    for (Int i = 0; i < args->size; i++)
    {
        result += arg(i).number;
    }
    return new_number(vm, result);
}

function(brl_std_math_sub)
{
    Float result = arg(0).number;
    for (Int i = 1; i < args->size; i++)
    {
        result -= arg(i).number;
    }
    return new_number(vm, result);
}

function(brl_std_math_mul)
{
    Float result = 1;
    for (Int i = 0; i < args->size; i++)
    {
        result *= arg(i).number;
    }
    return new_number(vm, result);
}

function(brl_std_math_div)
{
    Float result = arg(0).number;
    for (Int i = 1; i < args->size; i++)
    {
        result /= arg(i).number;
    }
    return new_number(vm, result);
}

function(brl_std_math_mod)
{
    return new_number(vm, (Int)arg(0).number % (Int)arg(1).number);
}

function(brl_std_math_random)
{
    return new_number(vm, rand());
}

function(brl_std_math_seed)
{
    srand(arg(0).number);
    return -1;
}

function(brl_std_math_increment)
{
    for (Int i = 0; i < args->size; i++)
    {
        arg(i).number++;
    }
    return -1;
}

function(brl_std_math_decrement)
{
    for (Int i = 0; i < args->size; i++)
    {
        arg(i).number--;
    }
    return -1;
}

function(brl_std_math_round)
{
    return new_number(vm, round(arg(0).number));
}

function(brl_std_math_floor)
{
    return new_number(vm, floor(arg(0).number));
}

function(brl_std_math_ceil)
{
    return new_number(vm, ceil(arg(0).number));
}

function(brl_std_math_sin)
{
    return new_number(vm, sin(arg(0).number));
}

function(brl_std_math_cos)
{
    return new_number(vm, cos(arg(0).number));
}

function(brl_std_math_tan)
{
    return new_number(vm, tan(arg(0).number));
}

void init_std_math(VirtualMachine *vm)
{
    register_builtin(vm, "+", brl_std_math_add);
    register_builtin(vm, "-", brl_std_math_sub);
    register_builtin(vm, "*", brl_std_math_mul);
    register_builtin(vm, "/", brl_std_math_div);
    register_builtin(vm, "\%", brl_std_math_mod);
    register_builtin(vm, "++", brl_std_math_increment);
    register_builtin(vm, "--", brl_std_math_decrement);

    register_builtin(vm, "random", brl_std_math_random);
    register_builtin(vm, "seed", brl_std_math_seed);

    register_builtin(vm, "round", brl_std_math_round);
    register_builtin(vm, "floor", brl_std_math_floor);
    register_builtin(vm, "ceil", brl_std_math_ceil);

    register_builtin(vm, "sin", brl_std_math_sin);
    register_builtin(vm, "cos", brl_std_math_cos);
    register_builtin(vm, "tan", brl_std_math_tan);
}