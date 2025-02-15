#include "bruter.h"

// math functions
// math functions
// math functions
// math functions


function(brl_std_math_add)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).number += arg(i).number;
    }
    return -1;
}

function(brl_std_math_sub)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).number -= arg(i).number;
    }
    return -1;
}

function(brl_std_math_mul)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).number *= arg(i).number;
    }
    return -1;
}

function(brl_std_math_div)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).number /= arg(i).number;
    }
    return -1;
}

function(brl_std_math_mod)
{
    #if __SIZEOF_POINTER__ == 8
        arg(0).number = fmod(arg(0).number, arg(1).number);
    #else
        arg(0).number = fmodf(arg(0).number, arg(1).number);
    #endif
    return -1;
}

function(brl_std_math_random)
{
    arg(0).number = rand();
    return -1;
}

function(brl_std_math_seed)
{
    srand(arg(0).number);
    return -1;
}

function(brl_std_math_round)
{
    #if __SIZEOF_POINTER__ == 8
        arg(0).number = round(arg(0).number);
    #else
        arg(0).number = roundf(arg(0).number);
    #endif
    return -1;
}

function(brl_std_math_floor)
{
    #if __SIZEOF_POINTER__ == 8
        arg(0).number = floor(arg(0).number);
    #else
        arg(0).number = floorf(arg(0).number);
    #endif
    return -1;
}

function(brl_std_math_ceil)
{
    #if __SIZEOF_POINTER__ == 8
        arg(0).number = ceil(arg(0).number);
    #else
        arg(0).number = ceilf(arg(0).number);
    #endif
    return -1;
}

function(brl_std_math_sin)
{
    #if __SIZEOF_POINTER__ == 8
        arg(0).number = sin(arg(0).number);
    #else
        arg(0).number = sinf(arg(0).number);    
    #endif
    return -1;
}

function(brl_std_math_cos)
{
    #if __SIZEOF_POINTER__ == 8
        arg(0).number = cos(arg(0).number);
    #else
        arg(0).number = cosf(arg(0).number);
    #endif
    return -1;  
}

function(brl_std_math_tan)
{
    #if __SIZEOF_POINTER__ == 8
        arg(0).number = tan(arg(0).number);
    #else
        arg(0).number = tanf(arg(0).number);
    #endif
    return -1;
}

function(brl_std_min)
{
    Float min = arg(0).number;
    for (Int i = 1; i < args->size; i++)
    {
        #if __SIZEOF_POINTER__ == 8
            min = fmin(min, arg(i).number);
        #else
            min = fminf(min, arg(i).number);
        #endif
    }
    arg(0).number = min;
    return -1;
}

function(brl_std_max)
{
    Float max = arg(0).number;
    for (Int i = 1; i < args->size; i++)
    {
        #if __SIZEOF_POINTER__ == 8
            max = fmax(max, arg(i).number);
        #else
            max = fmaxf(max, arg(i).number);
        #endif
    }
    arg(0).number = max;
    return -1;
}

// destructive/inplace!!
void init_std_math(VirtualMachine *vm)
{
    // math functions are inplace(destructive), you might want to use $ to do non-destructive operations
    // e.g. $ + a 2; // a is not changed
    register_builtin(vm, "+", brl_std_math_add);
    register_builtin(vm, "-", brl_std_math_sub);
    register_builtin(vm, "*", brl_std_math_mul);
    register_builtin(vm, "/", brl_std_math_div);
    register_builtin(vm, "\%", brl_std_math_mod);

    register_builtin(vm, "random", brl_std_math_random);
    register_builtin(vm, "seed", brl_std_math_seed);

    register_builtin(vm, "round", brl_std_math_round);
    register_builtin(vm, "floor", brl_std_math_floor);
    register_builtin(vm, "ceil", brl_std_math_ceil);

    register_builtin(vm, "sin", brl_std_math_sin);
    register_builtin(vm, "cos", brl_std_math_cos);
    register_builtin(vm, "tan", brl_std_math_tan);

    register_builtin(vm, "min", brl_std_min);
    register_builtin(vm, "max", brl_std_max);
}