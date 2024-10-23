#include "bruter.h"

/*
    this lib is a lib to manipulate integers
*/

//#ifndef ARDUINO

Int integer_to_float(VirtualMachine *vm, IntList *args) // example function
{
    Int _ref = stack_shift(*args);
    Int result = new_number(vm, (Float)vm->stack->data[_ref].integer);
    return result;
}

Int integer_to_string(VirtualMachine *vm, IntList *args) // example function
{
    Int _ref = stack_shift(*args);
    char *str = (char*)malloc(32);
    sprintf(str, "%ld", vm->stack->data[_ref].integer);
    Int result = new_string(vm, str);
    free(str);
    return result;
}

Int integer_from_float(VirtualMachine *vm, IntList *args) // example function
{
    Int _ref = stack_shift(*args);
    Int result = new_number(vm, (Int)vm->stack->data[_ref].number);
    return result;
}

Int integer_from_string(VirtualMachine *vm, IntList *args) // example function
{
    Int _ref = stack_shift(*args);
    Int result = new_number(vm, atoi(vm->stack->data[_ref].string));
    return result;
}

void init_integer(VirtualMachine *vm) // integer lib initializer
{
    registerBuiltin(vm, "integer.to.float", integer_to_float); // register example function
    registerBuiltin(vm, "integer.to.string", integer_to_string); // register example function
    registerBuiltin(vm, "integer.from.float", integer_from_float); // register example function
    registerBuiltin(vm, "integer.from.string", integer_from_string); // register example function
}

//#endif