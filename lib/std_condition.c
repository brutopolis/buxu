#include "bruter.h"


// std conditions
// std conditions
// std conditions

function(brl_std_condition_if)
{
    Int result = arg_i(0);
    
    if (args->size == 2)
    {
        if (arg(eval(vm, arg(0).string, context)).integer)
        {
            result = eval(vm, arg(1).string, context);
        }
    }
    else if (args->size == 3) // ifelse
    {
        if (arg(eval(vm, arg(0).string, context)).integer)
        {
            result = eval(vm, arg(1).string, context);
        }
        else
        {
            result = eval(vm, arg(2).string, context);
        }
    }
    unuse_var(vm, arg_i(0));
    arg(0).integer = result;
    arg_t(0) = TYPE_NUMBER;
    return -1;
}

function(brl_std_condition_equals)
{
    Int result = 0;
    for (Int i = 1; i < args->size; i++)
    {
        result = arg(i - 1).integer == arg(i).integer;
    }
    unuse_var(vm, arg_i(0));
    arg(0).integer = result;
    arg_t(0) = TYPE_NUMBER;
    return -1;
}

function(brl_std_condition_not_equals)
{
    Int result = 0;
    for (Int i = 1; i < args->size; i++)
    {
        result = arg(i - 1).integer != arg(i).integer;
    }
    unuse_var(vm, arg_i(0));
    arg(0).integer = result;
    arg_t(0) = TYPE_NUMBER;
    return -1;
}

function(brl_std_condition_greater)
{
    Int result = 0;
    for (Int i = 1; i < args->size; i++)
    {
        result = arg(i - 1).integer > arg(i).integer;
    }
    unuse_var(vm, arg_i(0));
    arg(0).integer = result;
    arg_t(0) = TYPE_NUMBER;
    return -1;
}

function(brl_std_condition_greater_equals)
{
    Int result = 0;
    for (Int i = 1; i < args->size; i++)
    {
        result = arg(i - 1).integer >= arg(i).integer;
    }
    unuse_var(vm, arg_i(0));
    arg(0).integer = result;
    arg_t(0) = TYPE_NUMBER;
    return -1;
}

function(brl_std_condition_less)
{
    Int result = 0;
    for (Int i = 1; i < args->size; i++)
    {
        result = arg(i - 1).integer < arg(i).integer;
    }
    return result;
}

function(brl_std_condition_less_equals)
{
    Int result = 0;
    for (Int i = 1; i < args->size; i++)
    {
        result = arg(i - 1).integer <= arg(i).integer;
    }
    return result;
}

function(brl_std_condition_and)
{
    Int result = 1;
    for (Int i = 0; i < args->size; i++)
    {
        if (!arg(i).integer)
        {
            result = 0;
            break;
        }
    }
    return result;
}

function(brl_std_condition_raw_or)
{
    if (arg(0).integer || arg(1).integer)
    {
        return 1;
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