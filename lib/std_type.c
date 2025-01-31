#include "bruter.h"

function(brl_std_type_get)
{
    Int result = new_number(vm, arg_t(0));
    return result;
}

function(brl_std_type_set)
{
    arg_t(0) = (Int)arg(1).number;
    return -1;
}


function(brl_std_type_cast)
{
    switch (arg_t(0))
    {
        char* _str;
        case TYPE_NUMBER:
            switch ((Int)arg(1).number)
            {
                case TYPE_STRING:
                    _str =  str_format("%ld", (Int)arg(0).number);
                    Int result = new_string(vm, _str);
                    free(_str);
                    return result;
                    break;
                case TYPE_ANY:
                    Int index = new_var(vm);
                    data(index).integer = arg(0).number;
                    return index;
                    break;
            }
            break;
        case TYPE_STRING:
            switch ((Int)arg(1).number)
            {
                case TYPE_NUMBER:
                    return new_number(vm, atof(arg(0).string));
                    break;
                case TYPE_ANY:
                    Int index = new_var(vm);
                    data(index).integer = atoi(arg(0).string);
                    break;
            }
            break;
        case TYPE_LIST:
            switch ((Int)arg(1).number)
            {
                case TYPE_STRING:
                    return new_string(vm, list_stringify(vm, (IntList*)arg(0).pointer));
                    break;
                case TYPE_ANY:
                    return new_number(vm, (Int)arg(0).pointer);
                    break;
            }
            break;
        case TYPE_ANY:
            switch ((Int)arg(1).number)
            {
                case TYPE_STRING:
                    return new_string(vm, str_format("%ld", arg(0).integer));
                    break;
                case TYPE_NUMBER:
                    return new_number(vm, (Float)arg(0).integer);
                    break;
            }
            break;
        case TYPE_FUNCTION:
            if ((Int)arg(1).number == TYPE_STRING)
            {
                return new_string(vm, function_stringify(vm, (InternalFunction*)arg(0).pointer));
            }
            break;
        case TYPE_BUILTIN:
            break;
    }
}

void init_std_type(VirtualMachine *vm)
{
    // type size(4 or 8 bytes)
    register_number(vm, "type.size", sizeof(Value));

    // types
    register_number(vm, "type.any", TYPE_ANY);
    register_number(vm, "type.number", TYPE_NUMBER);
    register_number(vm, "type.string", TYPE_STRING);
    register_number(vm, "type.builtin", TYPE_BUILTIN);
    register_number(vm, "type.list", TYPE_LIST);
    register_number(vm, "type.function", TYPE_FUNCTION);

    // type functions
    register_builtin(vm, "type", brl_std_type_get);
    register_builtin(vm, "pun", brl_std_type_set);
    register_builtin(vm, "cast", brl_std_type_cast);
}