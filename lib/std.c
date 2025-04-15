#include "buxu.h"

function(new)
{
    hash_set(vm, arg(0).s, arg_i(1));
    return -1;
}

function(ls)
{
    for (Int i = 0; i < vm->values->size; i++)
    {
        if (vm->hashes->data[i].p != NULL)
        {
            printf("[%ld](\"%s\"):\t\t", i, vm->hashes->data[i].s);
        }
        else
        {
            printf("[%ld](\"\"):\t\t", i);
        }

        printf(" %i\n", vm->values->data[i].s);
    }
    return -1;
}

function(_return)
{
    if (args->size < 1)
    {
        return -1;
    }
    else
    {
        return arg_i(0);
    }
}

function(_add)
{
    arg(0).i += arg(1).i;
    return -1;
}

function(_repeat)
{
    Int times = arg(0).i;
    char* code = arg(1).s;
    Int result = -1;
    for (Int i = 0; i < times; i++)
    {
        result = eval(vm, code);
        if (result != -1)
        {
            break;
        }
    }
    return result;
}

function(print_float)
{
    printf("%f\n", arg(0).f);
    return -1;
}

function(print_int)
{
    printf("%ld\n", arg(0).i);
    return -1;
}

init(std)
{
    new_function(vm, "new", new);
    new_function(vm, "ls", ls);
    new_function(vm, "return", _return);
    new_function(vm, "add", _add);
    new_function(vm, "repeat", _repeat);
    new_function(vm, "print.float", print_float);
    new_function(vm, "print.int", print_int);
}