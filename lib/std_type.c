#include "bruter.h"

function(brl_std_type_get)
{
    arg(0).integer = arg_t(0);
    return -1;
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
        Int index;
        case TYPE_NUMBER:
            switch ((Int)arg(1).number)
            {
                case TYPE_STRING:
                    _str =  str_format("%ld", (Int)arg(0).number);
                    arg(0).string = _str;
                    arg_t(0) = TYPE_STRING;
                    return -1;
                    break;
                case TYPE_ANY:
                    arg(0).integer = arg(0).number;
                    arg_t(0) = TYPE_ANY;
                    return -1;
                    break;
            }
            break;
        case TYPE_STRING:
            _str = arg(0).string;
            switch ((Int)arg(1).number)
            {
                case TYPE_NUMBER:
                    arg(0).number = atof(arg(0).string);
                    arg_t(0) = TYPE_NUMBER;
                    free(_str);
                    return -1;
                    break;
                case TYPE_ANY:
                    arg(0).integer = atoi(arg(0).string);
                    arg_t(0) = TYPE_ANY;
                    free(_str);
                    return -1;
                    break;
            }
            break;
        case TYPE_LIST:
            switch ((Int)arg(1).number)
            {
                case TYPE_STRING:
                    IntList* backup = (IntList*)arg(0).pointer;
                    char* _str = list_stringify(vm, (IntList*)arg(0).pointer);
                    arg(0).string = _str;
                    arg_t(0) = TYPE_STRING;
                    list_free(*backup);
                    return -1;
                    break;
            }
            break;
        case TYPE_ANY:
            switch ((Int)arg(1).number)
            {
                case TYPE_STRING:
                    _str = str_format("%ld", arg(0).integer);
                    arg(0).string = _str;
                    arg_t(0) = TYPE_STRING;
                    return -1;
                    break;
                case TYPE_NUMBER:
                    arg(0).number = arg(0).integer;
                    arg_t(0) = TYPE_NUMBER;
                    return -1;
                    break;
            }
            break;
    }
}

// destructive/inplace!!
void init_std_type(VirtualMachine *vm)
{
    // type size(4 or 8 bytes)
    register_number(vm, "type.size", sizeof(Value));

    // types
    register_number(vm, "type.any", TYPE_ANY);
    register_number(vm, "type.number", TYPE_NUMBER);
    register_number(vm, "type.string", TYPE_STRING);
    register_number(vm, "type.list", TYPE_LIST);

    // type functions are inplace(destructive), you might want to use $ to do non-destructive operations
    // e.g. $ type a; // returns the type of a instead turning a into a number of its type
    // type functions
    register_builtin(vm, "type", brl_std_type_get);
    register_builtin(vm, "pun", brl_std_type_set);
    register_builtin(vm, "cast", brl_std_type_cast);
}