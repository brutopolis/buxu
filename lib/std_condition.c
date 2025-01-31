#include "bruter.h"


// std conditions
// std conditions
// std conditions

function(brl_std_condition_if)
{
    Int result = -1;
    if (eval(vm, arg(0).string, context))
    {
        result = eval(vm, arg(1).string, context);

    }
    return result;
}

function(brl_std_condition_ifelse)
{
    Int result = -1;
    if (eval(vm, arg(0).string, context))
    {
        result = eval(vm, arg(1).string, context);
    }
    else
    {
        result = eval(vm, arg(2).string, context);
    }
    return result;
}

function(brl_std_condition_equals)
{
    return(arg(0).integer == arg(1).integer);
}

function(brl_std_condition_not_equals)
{
    return(arg(0).integer != arg(1).integer);
}

function(brl_std_condition_greater)
{
    return(arg(0).integer > arg(1).integer);
}

function(brl_std_condition_greater_equals)
{
    return(arg(0).integer >= arg(1).integer);
}

function(brl_std_condition_less)
{
    return(arg(0).integer < arg(1).integer);
}

function(brl_std_condition_less_equals)
{
    return(arg(0).integer <= arg(1).integer);
}

function(brl_std_condition_not)
{
    if (arg(0).integer)
    {
        return 0;
    }
    return 1;
}

function(brl_std_condition_and)
{
    if (arg(0).integer && arg(1).integer)
    {
        return 1;
    }
    return 0;
}

function(brl_std_condition_raw_or)
{
    if (arg(0).integer || arg(1).integer)
    {
        return 1;
    }
    return 0;
}

void init_std_condition(VirtualMachine *vm)
{   
    register_builtin(vm, "if", brl_std_condition_if);
    register_builtin(vm, "ifelse", brl_std_condition_ifelse);

    register_builtin(vm, "==", brl_std_condition_equals);
    register_builtin(vm, "!=", brl_std_condition_not_equals);
    register_builtin(vm, ">", brl_std_condition_greater);
    register_builtin(vm, ">=", brl_std_condition_greater_equals);
    register_builtin(vm, "<", brl_std_condition_less);
    register_builtin(vm, "<=", brl_std_condition_less_equals);
    register_builtin(vm, "not", brl_std_condition_not);
    register_builtin(vm, "&&", brl_std_condition_and);
    register_builtin(vm, "||", brl_std_condition_raw_or);
}