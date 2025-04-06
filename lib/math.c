#include "buxu.h"

// math functions
// math functions
// math functions
// math functions

function(brl_std_math_add)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            for (Int i = 1; i < args->size; i++)
            {
                switch (arg_t(i))
                {
                    case TYPE_NUMBER:
                        arg(0).f += arg(i).f;
                        break;
                    default:
                        arg(0).f += (Float)arg(i).i;
                        break;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                switch (arg_t(i))
                {
                    case TYPE_NUMBER:
                        arg(0).i += (Int)arg(i).f;
                        break;
                    default:
                        arg(0).i += arg(i).i;
                        break;
                }
            }
            break;
    }
    return -1;
}

function(brl_std_math_sub)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            for (Int i = 1; i < args->size; i++)
            {
                switch (arg_t(i))
                {
                    case TYPE_NUMBER:
                        arg(0).f -= arg(i).f;
                        break;
                    default:
                        arg(0).f -= (Float)arg(i).i;
                        break;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                switch (arg_t(i))
                {
                    case TYPE_NUMBER:
                        arg(0).i -= (Int)arg(i).f;
                        break;
                    default:
                        arg(0).i -= arg(i).i;
                        break;
                }
            }
            break;
    }
    return -1;
}

function(brl_std_math_mul)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            for (Int i = 1; i < args->size; i++)
            {
                switch (arg_t(i))
                {
                    case TYPE_NUMBER:
                        arg(0).f *= arg(i).f;
                        break;
                    default:
                        arg(0).f *= (Float)arg(i).i;
                        break;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                switch (arg_t(i))
                {
                    case TYPE_NUMBER:
                        arg(0).i *= (Int)arg(i).f;
                        break;
                    default:
                        arg(0).i *= arg(i).i;
                        break;
                }
            }
            break;
    }
    return -1;
}

function(brl_std_math_div)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            for (Int i = 1; i < args->size; i++)
            {
                switch (arg_t(i))
                {
                    case TYPE_NUMBER:
                        arg(0).f /= arg(i).f;
                        break;
                    default:
                        arg(0).f /= (Float)arg(i).i;
                        break;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                switch (arg_t(i))
                {
                    case TYPE_NUMBER:
                        arg(0).i /= (Int)arg(i).f;
                        break;
                    default:
                        arg(0).i /= arg(i).i;
                        break;
                }
            }
            break;
    }
    return -1;
}

function(brl_std_math_mod)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            #if __SIZEOF_POINTER__ == 8
                arg(0).f = fmod(arg(0).f, arg(1).f);
            #else
                arg(0).f = fmodf(arg(0).f, arg(1).f);
            #endif
            break;
        default:
            arg(0).i %= arg(1).i;
            break;
    }
    
    return -1;
}

function(brl_std_math_random)
{
    switch (args->size)
    {
        case 2: // (min, max)
            arg(0).f = rand() % (Int)arg(1).f + (Int)arg(0).f;
            break;
        
        default:
            arg(0).f = rand();
            break;
    }
    return -1;
}

function(brl_std_math_seed)
{
    srand(arg(0).i);
    return -1;
}

function(brl_std_math_round)
{
    #if __SIZEOF_POINTER__ == 8
        arg(0).f = round(arg(0).f);
    #else
        arg(0).f = roundf(arg(0).f);
    #endif
    return -1;
}

function(brl_std_math_floor)
{
    #if __SIZEOF_POINTER__ == 8
        arg(0).f = floor(arg(0).f);
    #else
        arg(0).f = floorf(arg(0).f);
    #endif
    return -1;
}

function(brl_std_math_ceil)
{
    #if __SIZEOF_POINTER__ == 8
        arg(0).f = ceil(arg(0).f);
    #else
        arg(0).f = ceilf(arg(0).f);
    #endif
    return -1;
}

function(brl_std_math_sin)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            #if __SIZEOF_POINTER__ == 8
                arg(0).f = sin(arg(0).f);
            #else
                arg(0).f = sinf(arg(0).f);
            #endif
            break;
        default:
            arg(0).f = sin((Float)arg(0).i);
            arg_t(0) = TYPE_NUMBER;
            break;
    }
    return -1;
}

function(brl_std_math_cos)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            #if __SIZEOF_POINTER__ == 8
                arg(0).f = cos(arg(0).f);
            #else
                arg(0).f = cosf(arg(0).f);
            #endif
            break;
        default:
            arg(0).f = cos((Float)arg(0).i);
            arg_t(0) = TYPE_NUMBER;
            break;
    }
    return -1;  
}

function(brl_std_math_tan)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            #if __SIZEOF_POINTER__ == 8
                arg(0).f = tan(arg(0).f);
            #else
                arg(0).f = tanf(arg(0).f);
            #endif
            break;
        default:
            arg(0).f = tan((Float)arg(0).i);
            arg_t(0) = TYPE_NUMBER;
            break;
    }
    return -1;
}

function(brl_std_math_bitwise_and)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).i &= arg(i).i;
    }
    return -1;
}

function(brl_std_math_bitwise_or)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).i |= arg(i).i;
    }
    return -1;
}

function(brl_std_math_bitwise_xor)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).i ^= arg(i).i;
    }
    return -1;
}

function(brl_std_math_bitwise_not)
{
    arg(0).i = ~arg(0).i;
    return -1;
}

function(brl_std_math_bitwise_shift_left)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).i <<= arg(i).i;
    }
    return -1;
}

function(brl_std_math_bitwise_shift_right)
{
    for (Int i = 1; i < args->size; i++)
    {
        arg(0).i >>= arg(i).i;
    }
    return -1;
}

function(brl_std_min)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
        {
            Float min = arg(0).f;
            for (Int i = 1; i < args->size; i++)
            {
                #if __SIZEOF_POINTER__ == 8
                    min = fmin(min, arg(i).f);
                #else
                    min = fminf(min, arg(i).f);
                #endif
            }
            arg(0).f = min;
        }
        default:
        {
            Int min = arg(0).i;
            for (Int i = 1; i < args->size; i++)
            {
                min = min < arg(i).i ? min : arg(i).i;
            }
            arg(0).i = min;
            break;
        }
    }
    return -1;
}

function(brl_std_max)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
        {
            Float max = arg(0).f;
            for (Int i = 1; i < args->size; i++)
            {
                #if __SIZEOF_POINTER__ == 8
                    max = fmax(max, arg(i).f);
                #else
                    max = fmaxf(max, arg(i).f);
                #endif
            }
            arg(0).f = max;
        }
        default:
        {
            Int max = arg(0).i;
            for (Int i = 1; i < args->size; i++)
            {
                max = max > arg(i).i ? max : arg(i).i;
            }
            arg(0).i = max;
            break;
        }
    }
    return -1;
}

init(math)
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

    // bitwise
    register_builtin(vm, "&", brl_std_math_bitwise_and);
    register_builtin(vm, "|", brl_std_math_bitwise_or);
    register_builtin(vm, "^", brl_std_math_bitwise_xor);
    register_builtin(vm, "~", brl_std_math_bitwise_not);
    register_builtin(vm, "<<", brl_std_math_bitwise_shift_left);
    register_builtin(vm, ">>", brl_std_math_bitwise_shift_right);
}