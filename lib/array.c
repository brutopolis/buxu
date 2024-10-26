#include "bruter.h"

#define TYPE_ARRAY 16

Int array_new(VirtualMachine *vm, IntList *args)
{
    Int _size = stack_shift(*args);
    
    // manual push to avoid gettin unused slots as array, check new_var in src/bruter.c to understand
    Value value;
    value.pointer = NULL;
    stack_push(*vm->stack, value);
    stack_push(*vm->typestack, TYPE_NIL);
    
    Int result = vm->stack->size-1;
    hold_var(vm, result);
    
    vm->typestack->data[result] = TYPE_ARRAY;
    _size = vm->stack->data[_size].number;
    vm->stack->data[result].number = _size;
    for (Int i = 1; i <= _size; i++)
    {
        Value value;
        value.pointer = NULL;
        stack_push(*vm->stack, value);
        stack_push(*vm->typestack, TYPE_NIL);
        
        Int result = vm->stack->size-1;
        hold_var(vm, result);
    }
    return result;
}

Int array_get(VirtualMachine *vm, IntList *args)
{
    Int _array = stack_shift(*args);
    Int _index = stack_shift(*args);
    return (_array + (Int)vm->stack->data[_index].number);
}

Int array_set(VirtualMachine *vm, IntList *args)
{
    Int _array = stack_shift(*args);
    Int _index = stack_shift(*args);
    Int _value = stack_shift(*args);
    vm->stack->data[_array + (Int)vm->stack->data[_index].number] = vm->stack->data[_value];
    return -1;
}

Int array_fill(VirtualMachine *vm, IntList *args)
{
    Int _array = stack_shift(*args);
    Int _value = stack_shift(*args);
    Int _size = vm->stack->data[_array].number;
    for (Int i = 1; i <= _size; i++)
    {
        vm->stack->data[_array + i] = vm->stack->data[_value];
        vm->typestack->data[_array + i] = vm->typestack->data[_value];
    }
    return -1;
}

Int array_length(VirtualMachine *vm, IntList *args)
{
    Int _array = stack_shift(*args);
    return new_number(vm, vm->stack->data[_array].number);
}

Int array_resize(VirtualMachine *vm, IntList* args)
{
    Int _array = stack_shift(*args);
    Int _size = stack_shift(*args);
    Int _oldsize = vm->stack->data[_array].number;
    if (vm->stack->data[_size].number > _oldsize)
    {
        // update size, them insert, them update the references
        vm->stack->data[_array].number = vm->stack->data[_size].number;
        Int _diff = (Int)vm->stack->data[_size].number - _oldsize;
        for (Int i = 1; i <= _diff; i++)
        {
            // manual insert, check new_var in src/bruter.c to understand
            Value value;
            value.pointer = NULL;
            stack_insert(*vm->stack, _array + _oldsize + i, value);
            stack_insert(*vm->typestack, _array + _oldsize + i, TYPE_NIL);
        }

        // update temp references
        for (Int i = 0; i < vm->temp->size; i++)
        {
            if (vm->temp->data[i] >= _array + _oldsize)
            {
                vm->temp->data[i] += _diff;
            }
        }

        // update unused references
        for (Int i = 0; i < vm->unused->size; i++)
        {
            if (vm->unused->data[i] >= _array + _oldsize)
            {
                vm->unused->data[i] += _diff;
            }
        }

        // update hash references
        for (Int i = 0; i < vm->hashes->size; i++)
        {
            if (vm->hashes->data[i].index >= _array + _oldsize)
            {
                vm->hashes->data[i].index += _diff;
            }
        }
    }
    else if (vm->stack->data[_size].number < _oldsize)
    {
        for (Int i = _oldsize; i > vm->stack->data[_size].number; i--)
        {
            unuse_var(vm, _array + i);
        }
        vm->stack->data[_array].number = vm->stack->data[_size].number;
    }
    return -1;
}

Int array_copy(VirtualMachine *vm, IntList *args)
{
    Int _array = stack_shift(*args);
    Int _newarray;
    Value value = value_duplicate(vm->stack->data[_array], vm->typestack->data[_array]);
    stack_push(*vm->stack, value);
    stack_push(*vm->typestack, TYPE_ARRAY);
    hold_var(vm, vm->stack->size);
    _newarray = vm->stack->size;
    printf("new array: %d\n", _newarray);

    vm->stack->data[_newarray].number = vm->stack->data[_array].number;
    vm->typestack->data[_newarray] = TYPE_ARRAY;
    for (Int i = 1; i <= vm->stack->data[_array].number; i++)
    {
        Value value = value_duplicate(vm->stack->data[_array + i], vm->typestack->data[_array + i]);
        stack_push(*vm->stack, value);
        stack_push(*vm->typestack, vm->typestack->data[_array + i]);
        hold_var(vm, vm->stack->size);
    }
    return _newarray;
}

Int array_concat(VirtualMachine *vm, IntList *args)
{
    Int _array1 = stack_shift(*args);
    Int _array2 = stack_shift(*args);
    Int _newarray;
    Value value = value_duplicate(vm->stack->data[_array1], vm->typestack->data[_array1]);
    stack_push(*vm->stack, value);
    stack_push(*vm->typestack, TYPE_ARRAY);
    hold_var(vm, vm->stack->size);
    _newarray = vm->stack->size;
    printf("new array: %d\n", _newarray);

    vm->stack->data[_newarray].number = vm->stack->data[_array1].number + vm->stack->data[_array2].number;
    vm->typestack->data[_newarray] = TYPE_ARRAY;
    for (Int i = 1; i <= vm->stack->data[_array1].number; i++)
    {
        Value value = value_duplicate(vm->stack->data[_array1 + i], vm->typestack->data[_array1 + i]);
        stack_push(*vm->stack, value);
        stack_push(*vm->typestack, vm->typestack->data[_array1 + i]);
        hold_var(vm, vm->stack->size);
    }
    for (Int i = 1; i <= vm->stack->data[_array2].number; i++)
    {
        Value value = value_duplicate(vm->stack->data[_array2 + i], vm->typestack->data[_array2 + i]);
        stack_push(*vm->stack, value);
        stack_push(*vm->typestack, vm->typestack->data[_array2 + i]);
        hold_var(vm, vm->stack->size);
    }
    return _newarray;
}

Int array_push(VirtualMachine *vm, IntList *args)
{
    Int _array = stack_shift(*args);
    Int _value = stack_shift(*args);
    Int _size = vm->stack->data[_array].number;
    vm->stack->data[_array].number = _size + 1;
    Value value = value_duplicate(vm->stack->data[_value], vm->typestack->data[_value]);
    stack_push(*vm->stack, value);
    stack_push(*vm->typestack, vm->typestack->data[_value]);
    hold_var(vm, vm->stack->size);
    return -1;
}

// everything below this line is not tested at all

Int array_pop(VirtualMachine *vm, IntList *args)
{
    Int _array = stack_shift(*args);
    Int _size = vm->stack->data[_array].number;
    if (_size > 0)
    {
        unuse_var(vm, _array + _size);
        vm->stack->data[_array].number = _size - 1;
    }
    return -1;
}

Int array_shift(VirtualMachine *vm, IntList *args)
{
    Int _array = stack_shift(*args);
    Int _size = vm->stack->data[_array].number;
    if (_size > 0)
    {
        unuse_var(vm, _array + 1);
        for (Int i = 1; i < _size; i++)
        {
            vm->stack->data[_array + i] = vm->stack->data[_array + i + 1];
            vm->typestack->data[_array + i] = vm->typestack->data[_array + i + 1];
        }
        vm->stack->data[_array].number = _size - 1;
    }
    return -1;
}

Int array_unshift(VirtualMachine *vm, IntList *args)
{
    Int _array = stack_shift(*args);
    Int _value = stack_shift(*args);
    Int _size = vm->stack->data[_array].number;
    vm->stack->data[_array].number = _size + 1;
    for (Int i = _size; i > 0; i--)
    {
        vm->stack->data[_array + i] = vm->stack->data[_array + i - 1];
        vm->typestack->data[_array + i] = vm->typestack->data[_array + i - 1];
    }
    Value value = value_duplicate(vm->stack->data[_value], vm->typestack->data[_value]);
    vm->stack->data[_array + 1] = value;
    vm->typestack->data[_array + 1] = vm->typestack->data[_value];
    hold_var(vm, _array + 1);
    return -1;
}

void init_array(VirtualMachine *vm) // example lib initializer
{
    registerNumber(vm, "array.type", TYPE_ARRAY);

    registerBuiltin(vm, "array.new", array_new);
    registerBuiltin(vm, "array.get", array_get);
    registerBuiltin(vm, "array.set", array_set);
    registerBuiltin(vm, "array.copy", array_copy);
    registerBuiltin(vm, "array.fill", array_fill);
    registerBuiltin(vm, "array.resize", array_resize);
    registerBuiltin(vm, "array.length", array_length);
    registerBuiltin(vm, "array.concat", array_concat);
    registerBuiltin(vm, "array.push", array_push);
    registerBuiltin(vm, "array.pop", array_pop);
    registerBuiltin(vm, "array.shift", array_shift);
    registerBuiltin(vm, "array.unshift", array_unshift);
    
}
