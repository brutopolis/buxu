#include "buxu.h"

function(new)
{
    register_var(vm, arg(0).s , TYPE_DATA, arg(1).i);
    return -1;
}

function(print)
{
    switch (arg_t(0).string)
    {
        case 1:
            switch (arg_t(0).alloc)
            {
                case 1:
                    printf("%s\n", arg(0).s);
                    break;
                case 0:
                    printf("%s\n", arg(0).i8);
                    break;
        }
        break;
        case 0:
            switch (arg_t(0).floating_point)
            {
                case 1:
                    printf("%f\n", arg(0).f);
                    break;
                case 0:
                    switch (arg_t(0).alloc)
                    {
                        case 1:
                            printf("%p\n", arg(0).p);
                            break;
                        case 0:
                            printf("%ld\n", (Int)arg(0).i);
                            break;
                    }
                    break;
            }
        break;
    }
    return -1;
}

function(print_index)
{
    printf("%d\n", arg_i(0));
    return -1;
}

function(print_float_index)
{
    printf("%f\n", pun(arg_i(0), i, f));
    return -1;
}

function(std_return)
{
    if (args->size == 0)
    {
        return -1;
    }
    return arg(0).i;
}

function(std_ls)
{
    for (Int i = 0; i < vm->stack->size; i++)
    {
        switch (data_t(i).string)
        {
            case 1:
                switch (data_t(i).alloc)
                {
                    case 1:
                        printf("(%d)\t\t[%s]\n", i, data(i).s);
                        break;
                    case 0:
                        printf("(%d)\t\t[%s]\n", i, data(i).i8);
                        break;
                }
                break;
            case 0:
                switch (data_t(i).floating_point)
                {
                    case 1:
                        printf("(%d)\t\t[%f]\n", i, data(i).f);
                        break;
                    case 0:
                        switch (data_t(i).alloc)
                        {
                            case 1:
                                printf("(%d)\t\t[%p]\n", i, data(i).p);
                                break;
                            case 0:
                                printf("(%d)\t\t[%ld]\n", i, (Int)data(i).i);
                                break;
                        }
                        break;
                }
                break;
                
        }
    }
    return -1;
}

function(std_ls_hash)
{
    for (Int i = 0; i < vm->hash_names->size; i++)
    {
        switch (data_t(i).string)
        {
            case 1:
                switch (data_t(i).alloc)
                {
                    case 1:
                        printf("(%d)\t%s\t[%s]\n", i, vm->hash_names->data[i], data(i).s);
                        break;
                    case 0:
                        printf("(%d)\t%s\t[%s]\n", i, vm->hash_names->data[i], data(i).i8);
                        break;
                }
                break;
            case 0:
                switch (data_t(i).floating_point)
                {
                    case 1:
                        printf("(%d)\t%s\t[%f]\n", i, vm->hash_names->data[i], data(i).f);
                        break;
                    case 0:
                        switch (data_t(i).alloc)
                        {
                            case 1:
                                printf("(%d)\t%s\t[%p]\n", i, vm->hash_names->data[i], data(i).p);
                                break;
                            case 0:
                                printf("(%d)\t%s\t[%ld]\n", i, vm->hash_names->data[i], (Int)data(i).i);
                                break;
                        }
                        break;
                }
                break;
                
        }
    }
    return -1;
}

function(brl_std_loop_repeat)
{
    Int result = -1;
    
    for (int i = 0; i < (Int)arg(0).i; i++)
    {
        result = eval(vm,arg(1).s);
        if (result > -1)
        {
            break;
        }
    }
    
    return result;
}

function(brl_std_add)
{
    arg(0).f += arg(1).f;
    return -1;
}



init(std)
{
    register_function(vm, "new", new);
    register_function(vm, "print", print);
    register_function(vm, "print.index", print_index);
    register_function(vm, "print.float.index", print_float_index);
    register_function(vm, "return", std_return);
    register_function(vm, "ls", std_ls);
    register_function(vm, "ls.hash", std_ls_hash);

    register_function(vm, "repeat", brl_std_loop_repeat);
    register_function(vm, "+", brl_std_add);
}