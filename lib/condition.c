#include "buxu.h"


// std conditions
// std conditions
// std conditions

function(brl_std_condition_if)
{
    Int result = -1;
    if (args->size == 2)
    {
        if (eval(vm, arg(0).s))
        {
            result = eval(vm, arg(1).s);
        }
    }
    else if (args->size == 3) // ifelse
    {
        if (eval(vm, arg(0).s))
        {
            result = eval(vm, arg(1).s);
        }
        else
        {
            result = eval(vm, arg(2).s);
        }
    }
    return result;
}

function(brl_std_condition_equals)// ==
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                break;
            }
        }

        if (result == 0)
        {
            result = 1;
        }
    }
    
    for (Int i = 1; i < args->size; i++)
    {
        if (arg_t(i - 1) != arg_t(i))
        {
            return 0;
        }
        else
        {
            switch (arg_t(i - 1))
            {
                case TYPE_STRING:
                    if (strcmp(arg(i - 1).s, arg(i).s) != 0)
                    {
                        return 0;
                    }
                    break;
                default:
                    if (arg(i - 1).i != arg(i).i)
                    {
                        return 0;
                    }
                    break;
            }
        }
    }
    return result;
}

function(brl_std_condition_not_equals)// !=
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                break;
            }
        }

        if (result == 0)
        {
            result = 1;
        }
    }

    for (Int i = 1; i < args->size; i++)
    {
        if (arg_t(i - 1) != arg_t(i))
        {
            return result;
        }
        else
        {
            switch (arg_t(i - 1))
            {
                case TYPE_STRING:
                    if (strcmp(arg(i - 1).s, arg(i).s) == 0)
                    {
                        return 0;
                    }
                    break;
                default:
                    if (arg(i - 1).i == arg(i).i)
                    {
                        return 0;
                    }
                    break;
            }
        }
    }
    return result;
}


function(brl_std_condition_greater)
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                break;
            }
        }

        if (result == 0)
        {
            result = 1;
        }
    }

    switch (arg_t(1))
    {
        case TYPE_NUMBER:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).f <= arg(i).f)
                {
                    return 0;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).i <= arg(i).i)
                {
                    return 0;
                }
            }
            break;
    }
    return result;
}

function(brl_std_condition_greater_equals)
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                break;
            }
        }

        if (result == 0)
        {
            result = 1;
        }
    }

    switch (arg_t(1))
    {
        case TYPE_NUMBER:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).f < arg(i).f)
                {
                    return 0;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).i < arg(i).i)
                {
                    return 0;
                }
            }
            break;
    }
    return result;
}

function(brl_std_condition_less)
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                break;
            }
        }

        if (result == 0)
        {
            result = 1;
        }
    }

    switch (arg_t(1))
    {
        case TYPE_NUMBER:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).f >= arg(i).f)
                {
                    return 0;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).i >= arg(i).i)
                {
                    return 0;
                }
            }
            break;
    }
    return result;
}

function(brl_std_condition_less_equals)
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                break;
            }
        }

        if (result == 0)
        {
            result = 1;
        }
    }

    switch (arg_t(1))
    {
        case TYPE_NUMBER:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).f > arg(i).f)
                {
                    return 0;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).i > arg(i).i)
                {
                    return 0;
                }
            }
            break;
    }
    return result;
}

function(brl_std_condition_and)
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                break;
            }
        }

        if (result == 0)
        {
            result = 1;
        }
    }

    for (Int i = 1; i < args->size; i++)
    {
        if (arg_i(i) == 0)
        {
            return 0;
        }
    }
    return result;
}

function(brl_std_condition_raw_or)
{
    for (Int i = 0; i < args->size; i++)
    {
        if (arg_i(i) > 0)
        {
            return arg_i(i);
        }
    }
    return 0;
}

init(condition)
{
    register_builtin(vm, "==", brl_std_condition_equals);
    register_builtin(vm, "!=", brl_std_condition_not_equals);

    register_builtin(vm, ">", brl_std_condition_greater);
    register_builtin(vm, ">=", brl_std_condition_greater_equals);
    register_builtin(vm, "<", brl_std_condition_less);
    register_builtin(vm, "<=", brl_std_condition_less_equals);
    
    register_builtin(vm, "&&", brl_std_condition_and);
    register_builtin(vm, "||", brl_std_condition_raw_or);

    register_builtin(vm, "if", brl_std_condition_if);
}