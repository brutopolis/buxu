#include "bruter.h"

#define TYPE_ARRAY 16

Int std_array_new(VirtualMachine *vm, IntList *args)
{
    Int _size = stack_shift(*args);
    Int result = new_var(vm);
    hold_var(vm, result);
    
    vm->typestack->data[result] = TYPE_ARRAY;
    vm->stack->data[result].number = vm->stack->data[_size].number;
    while (_size > 0)
    {
        hold_var(vm, new_var(vm));
        _size--;
    }
    return result;
}

Int std_array_get(VirtualMachine *vm, IntList *args)
{
    Int _array = stack_shift(*args);
    Int _index = stack_shift(*args);
    return (_array + (Int)vm->stack->data[_index].number);
}

Int std_array_set(VirtualMachine *vm, IntList *args)
{
    Int _array = stack_shift(*args);
    Int _index = stack_shift(*args);
    Int _value = stack_shift(*args);
    vm->stack->data[(_array + (Int)vm->stack->data[_index].number)].number = vm->stack->data[_value].number;
    return -1;
}

Int std_array_length(VirtualMachine *vm, IntList *args)
{
    Int _array = stack_shift(*args);
    return new_number(vm, vm->stack->data[_array].number);
}

//#ifndef ARDUINO
void init_array(VirtualMachine *vm) // example lib initializer
{
    registerBuiltin(vm, "array.new", std_array_new); // register example function
    registerBuiltin(vm, "array.get", std_array_get); // register example function
    registerBuiltin(vm, "array.set", std_array_set); // register example function
    registerBuiltin(vm, "array.length", std_array_length); // register example function
}

//#endif