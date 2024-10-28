#include "bruter.h"


Int mem_clear(VirtualMachine *vm, IntList *args)
{
    while (vm->temp->size > 0)
    {
        unuse_var(vm, stack_shift(*vm->temp));
    }

    return -1;
}

Int mem_hold(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    if (args->size > 0)
    {
        Int as = stack_shift(*args);
        Int _type = vm->typestack->data[index];
    }
    hold_var(vm, index);
    return -1;
}

Int mem_keep(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    if (args->size > 0)
    {
        Int as = stack_shift(*args);
        Int _type = vm->typestack->data[index];
    }
    hold_var(vm, index);
    return index;
}

Int mem_unhold(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    unhold_var(vm, index);
    return -1;
}

Int mem_rebase(VirtualMachine *vm, IntList *args)
{
    Int index = vm->stack->size - 1;
    while (vm->unused->size > 0)
    {
        index = stack_pop(*vm->unused);
        // removes
        for (Int i = 0; i < vm->hashes->size; i++)
        {
            if (vm->hashes->data[i].index == index)
            {
                free(vm->hashes->data[i].key);
                stack_remove(*vm->hashes, i);
            }
        }
        for (Int i = 0; i < vm->unused->size; i++)
        {
            if (vm->unused->data[i] == index)
            {
                stack_remove(*vm->unused, i);
            }
        }
        for (Int i = 0; i < vm->temp->size; i++)
        {
            if (vm->temp->data[i] == index)
            {
                stack_remove(*vm->temp, i);
            }
        }
        for (Int i = 0; i < vm->stack->size; i++)
        {
            if (vm->typestack->data[i] == TYPE_LIST)
            {
                IntList *list = (IntList*)vm->stack->data[i].pointer;
                for (Int j = 0; j < list->size; j++)
                {
                    if (list->data[j] == index)
                    {
                        list->data[j] = -1;
                    }
                    else if (list->data[j] > index)
                    {
                        list->data[j]--;
                    }
                }
            }
        }

        // updates
        for (Int i = 0; i < vm->temp->size; i++)
        {
            if (vm->temp->data[i] == index)
            {
                vm->temp->data[i] = -1;
            }
            else if (vm->temp->data[i] > index)
            {
                vm->temp->data[i]--;
            }
        }
        for (Int i = 0; i < vm->unused->size; i++)
        {
            if (vm->unused->data[i] == index)
            {
                vm->unused->data[i] = -1;
            }
            else if (vm->unused->data[i] > index)
            {
                vm->unused->data[i]--;
            }
        }
        for (Int i = 0; i < vm->hashes->size; i++)
        {
            if (vm->hashes->data[i].index == index)
            {
                vm->hashes->data[i].index = -1;
            }
            else if (vm->hashes->data[i].index > index)
            {
                vm->hashes->data[i].index--;
            }
        }

        stack_remove(*vm->stack, index);
        stack_remove(*vm->typestack, index);
    }

    return -1;
}

Int mem_collect(VirtualMachine *vm, IntList *args)
{
    collect_garbage(vm);
    return -1;
}

Int mem_edit(VirtualMachine *vm, IntList *args)
{
    Int variable = stack_shift(*args);
    Int value = stack_shift(*args);
    
    if (variable >= 0 && variable < vm->stack->size)
    {
        if(vm->typestack->data[variable] == TYPE_STRING )
        {
            free(vm->stack->data[variable].string);
        }
        else if (vm->typestack->data[variable] == TYPE_LIST)
        {
            stack_free(*((IntList*)vm->stack->data[variable].pointer));
        }

        if (value >= 0 && value < vm->stack->size)
        {
            vm->stack->data[variable] = vm->stack->data[value];
            vm->typestack->data[variable] = vm->typestack->data[value];
        }
    }
    
    return -1;
}

Int mem_copy(VirtualMachine *vm, IntList *args)
{
    Int variable = stack_shift(*args);
    Int value = stack_shift(*args);
    Int newvar = new_var(vm);
    
    if (variable >= 0 && variable < vm->stack->size)
    {
        if (value >= 0 && value < vm->stack->size)
        {
            vm->stack->data[newvar] = value_duplicate(vm->stack->data[value], vm->typestack->data[value]);
            vm->typestack->data[newvar] = vm->typestack->data[value];
        }
    }
    
    return newvar;
}

Int mem_get(VirtualMachine *vm, IntList *args)
{
    Int index = (Int)stack_shift(*args);
    if (index >= 0 && index < vm->stack->size)
    {
        return index;
    }
    
    return -1;
}


Int mem_delete(VirtualMachine *vm, IntList *args)
{
    while (args->size > 0)
    {
        unuse_var(vm, stack_shift(*args));
    }
    return -1;
}


Int mem_length(VirtualMachine *vm, IntList *args)
{
    return new_number(vm, vm->stack->size);
}

Int mem_segment_new(VirtualMachine *vm, IntList *args)
{
    Int size = stack_shift(*args);
    Int index = new_var(vm);
    while ((vm->stack->data[size].number - 1) > 0)
    {
        new_var(vm);
        size--;
    }
    return index;
}

Int mem_segment_fill(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    Int _size = stack_shift(*args);
    Int value = stack_shift(*args);
    while (_size > 0)
    {
        unuse_var(vm, index);
        vm->stack->data[index] = value_duplicate(vm->stack->data[value], vm->typestack->data[value]);
        vm->typestack->data[index] = vm->typestack->data[value];
        index++;
        _size--;
    }
    return -1;
}

Int mem_segment_hold(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    Int _size = stack_shift(*args);
    while (_size > 0)
    {
        hold_var(vm, index);
        index++;
        _size--;
    }
    return index;
}

Int mem_segment_unhold(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    Int _size = stack_shift(*args);
    while (_size > 0)
    {
        unhold_var(vm, index);
        index++;
        _size--;
    }
    return index;
}

Int mem_segment_delete(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    Int _size = stack_shift(*args);
    while (_size > 0)
    {
        unuse_var(vm, index);
        index++;
        _size--;
    }
    return -1;
}

Int mem_segment_clear(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    Int _size = stack_shift(*args);
    while (_size > 0)
    {
        unuse_var(vm, index);
        index++;
        _size--;
    }
    return -1;
}

Int mem_segment_copy(VirtualMachine *vm, IntList *args)
{
    Int origin = stack_shift(*args);
    Int destination = stack_shift(*args);
    Int _size = stack_shift(*args);
    while (_size > 0)
    {
        vm->stack->data[destination] = value_duplicate(vm->stack->data[origin], vm->typestack->data[origin]);
        vm->typestack->data[destination] = vm->typestack->data[origin];
        origin++;
        destination++;
        _size--;
    }
    return -1;
}

Int mem_byte_set(VirtualMachine *vm, IntList *args)
{
    Int value = stack_shift(*args);
    Int index = 0;
    while (args->size > 0 && index < sizeof(Float))
    {
        vm->stack->data[value].bytes[index] = stack_shift(*args);
    }
    return -1;
}

Int mem_byte_get(VirtualMachine *vm, IntList *args)
{
    Int value = stack_shift(*args);
    Int index = stack_shift(*args);
    if (index >= 0 && index < sizeof(Float))
    {
        return vm->stack->data[value].bytes[index];
    }
    return -1;
}

void init_byte(VirtualMachine *vm)
{
    registerBuiltin(vm, "byte.set", mem_byte_set);
    registerBuiltin(vm, "byte.get", mem_byte_get);
}

void init_segment(VirtualMachine *vm)
{
    registerBuiltin(vm, "segment.new", mem_segment_new);
    registerBuiltin(vm, "segment.fill", mem_segment_fill);
    registerBuiltin(vm, "segment.hold", mem_segment_hold);
    registerBuiltin(vm, "segment.unhold", mem_segment_unhold);
    registerBuiltin(vm, "segment.delete", mem_segment_delete);
    registerBuiltin(vm, "segment.clear", mem_segment_clear);
    registerBuiltin(vm, "segment.copy", mem_segment_copy);
}

void init_mem(VirtualMachine *vm)
{
    registerNumber(vm, "mem.size", sizeof(Float));
    registerBuiltin(vm, "mem.get", mem_get);
    registerBuiltin(vm, "mem.hold", mem_hold);
    registerBuiltin(vm, "mem.keep", mem_keep);
    registerBuiltin(vm, "mem.edit", mem_edit);
    registerBuiltin(vm, "mem.copy", mem_copy);
    registerBuiltin(vm, "mem.len", mem_length);
    registerBuiltin(vm, "mem.clear", mem_clear);
    registerBuiltin(vm, "mem.rebase", mem_rebase);
    registerBuiltin(vm, "mem.unhold", mem_unhold);
    registerBuiltin(vm, "mem.delete", mem_delete);
    registerBuiltin(vm, "mem.collect", mem_collect);

    init_byte(vm);
    init_segment(vm);
}