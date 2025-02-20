#include "bruter.h"


// std conditions
// std conditions
// std conditions

function(brl_std_condition_if)
{
    Int result = -1;
    if (args->size == 2)
    {
        if (eval(vm, arg(0).string, context))
        {
            result = eval(vm, arg(1).string, context);
        }
    }
    else if (args->size == 3) // ifelse
    {
        if (eval(vm, arg(0).string, context))
        {
            result = eval(vm, arg(1).string, context);
        }
        else
        {
            result = eval(vm, arg(2).string, context);
        }
    }
    return result;
}

function(brl_std_condition_equals)
{
    for (Int i = 1; i < args->size; i++)
    {
        if (arg(i - 1).integer != arg(i).integer)
        {
            return 0;
        }
    }
    return 1;
}

function(brl_std_condition_not_equals)
{
    for (Int i = 1; i < args->size; i++)
    {
        if (arg(i - 1).integer == arg(i).integer)
        {
            return 0;
        }
    }
    return 1;
}

function(brl_std_condition_greater)
{
    for (Int i = 1; i < args->size; i++)
    {
        if (arg(i - 1).number <= arg(i).number)
        {
            return 0;
        }
    }
    return 1;
}

function(brl_std_condition_greater_equals)
{
    for (Int i = 1; i < args->size; i++)
    {
        if (arg(i - 1).number < arg(i).number)
        {
            return 0;
        }
    }
    return 1;
}

function(brl_std_condition_less)
{
    for (Int i = 1; i < args->size; i++)
    {
        if (arg(i - 1).number >= arg(i).number)
        {
            return 0;
        }
    }
    return 1;
}

function(brl_std_condition_less_equals)
{
    for (Int i = 1; i < args->size; i++)
    {
        if (arg(i - 1).number > arg(i).number)
        {
            return 0;
        }
    }
    return 1;
}

function(brl_std_condition_and)
{
    for (Int i = 0; i < args->size; i++)
    {
        if (arg_i(i) > 0)
        {
            return 0;
        }
    }
    return 1;
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

// index-based!!
void init_std_condition(VirtualMachine *vm)
{
    register_builtin(vm, "if", brl_std_condition_if);

    register_builtin(vm, "==", brl_std_condition_equals);
    register_builtin(vm, "!=", brl_std_condition_not_equals);
    register_builtin(vm, ">", brl_std_condition_greater);
    register_builtin(vm, ">=", brl_std_condition_greater_equals);
    register_builtin(vm, "<", brl_std_condition_less);
    register_builtin(vm, "<=", brl_std_condition_less_equals);
    
    register_builtin(vm, "&&", brl_std_condition_and);
    register_builtin(vm, "||", brl_std_condition_raw_or);
}