#include "bruter.h"


Int mem_clear(VirtualMachine *vm, IntList *args)
{
    while (args->size > 0)
    {
        Int index = stack_shift(*args);
        if (stack_find(*vm->temp, index) >= 0)
        {
            unuse_var(vm, index);
        }
    }

    return -1;
}

Int mem_hold(VirtualMachine *vm, IntList *args)
{
    Int index;
    while (args->size > 0)
    {
        index = stack_shift(*args);
        if (index >= 0 && index < vm->stack->size)
        {
            hold_var(vm, index);
        } 
    }
    
    return -1;
}

Int mem_keep(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    hold_var(vm, index);
    return index;
}

Int mem_unhold(VirtualMachine *vm, IntList *args)
{
    Int index;
    while (args->size > 0)
    {
        index = stack_shift(*args);
        if (index >= 0 && index < vm->stack->size)
        {
            unhold_var(vm, index);
        }
    }
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

Int mem_swap(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    if (a >= 0 && a < vm->stack->size && b >= 0 && b < vm->stack->size)
    {
        Value temp = vm->stack->data[a];
        char type = vm->typestack->data[a];
        vm->stack->data[a] = vm->stack->data[b];
        vm->typestack->data[a] = vm->typestack->data[b];
        vm->stack->data[b] = temp;
        vm->typestack->data[b] = type;
    }
    return -1;
}

Int mem_set(VirtualMachine *vm, IntList *args)
{
    Int variable = stack_shift(*args);
    Int value = stack_shift(*args);
    
    if (variable >= 0 && variable < vm->stack->size)
    {
        Int found = stack_find(*vm->temp, variable);
        if (found >= 0)
        {
            unuse_var(vm, variable);
        }
        vm->stack->data[variable] = vm->stack->data[value];
        vm->typestack->data[variable] = vm->typestack->data[value];
    }
    
    return -1;
}

Int mem_use(VirtualMachine *vm, IntList *args)
{
    Int index;
    while (args->size > 0)
    {
        index = stack_shift(*args);
        if (index >= 0 && index < vm->stack->size)
        {
            use_var(vm, index);
        }
    }
    return -1;
}

Int mem_unuse(VirtualMachine *vm, IntList *args)
{
    Int index;
    while (args->size > 0)
    {
        index = stack_shift(*args);
        if (index >= 0 && index < vm->stack->size)
        {
            unuse_var(vm, index);
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

Int mem_next(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    stack_unshift(*vm->unused, index);
    while (args->size > 0)
    {
        stack_unshift(*vm->unused, stack_shift(*args));
    }
    return -1;
}

Int mem_sector_new(VirtualMachine *vm, IntList *args)
{
    Int _size = stack_shift(*args);
    Int index = new_var(vm);
    while ((vm->stack->data[_size].number - 1) > 0)
    {
        new_var(vm);
        vm->stack->data[_size].number--;
    }
    return index;
}

Int mem_sector_copy(VirtualMachine *vm, IntList *args)
{
    Int origin = stack_shift(*args);
    Int destination = stack_shift(*args);
    Int _size = stack_shift(*args);
    while (vm->stack->data[_size].number > 0)
    {
        if (origin >= 0 && origin < vm->stack->size)
        {
            if (destination >= 0 && destination < vm->stack->size)
            {
                vm->stack->data[destination] = value_duplicate(vm->stack->data[origin], vm->typestack->data[origin]);
                vm->typestack->data[destination] = vm->typestack->data[origin];
            }
        }
        origin++;
        destination++;
        vm->stack->data[_size].number--;
    }
    return -1;
}

Int mem_sector_swap(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int _size = stack_shift(*args);
    while (vm->stack->data[_size].number > 0)
    {
        if (a >= 0 && a < vm->stack->size && b >= 0 && b < vm->stack->size)
        {
            Value temp = vm->stack->data[a];
            char type = vm->typestack->data[a];
            vm->stack->data[a] = vm->stack->data[b];
            vm->typestack->data[a] = vm->typestack->data[b];
            vm->stack->data[b] = temp;
            vm->typestack->data[b] = type;
        }
        a++;
        b++;
        vm->stack->data[_size].number--;
    }
    return -1;
}

void init_sector(VirtualMachine *vm)
{
    registerBuiltin(vm, "sector.new", mem_sector_new);
    registerBuiltin(vm, "sector.copy", mem_sector_copy);
    registerBuiltin(vm, "sector.swap", mem_sector_swap);
}

void init_mem(VirtualMachine *vm)
{
    registerNumber(vm, "mem.size", sizeof(Float));
    registerBuiltin(vm, "mem.use", mem_use);
    registerBuiltin(vm, "mem.unuse", mem_unuse);
    registerBuiltin(vm, "mem.get", mem_get);
    registerBuiltin(vm, "mem.hold", mem_hold);
    registerBuiltin(vm, "mem.keep", mem_keep);
    registerBuiltin(vm, "mem.set", mem_set);
    registerBuiltin(vm, "mem.copy", mem_copy);
    registerBuiltin(vm, "mem.len", mem_length);
    registerBuiltin(vm, "mem.clear", mem_clear);
    registerBuiltin(vm, "mem.rebase", mem_rebase);
    registerBuiltin(vm, "mem.unhold", mem_unhold);
    registerBuiltin(vm, "mem.delete", mem_delete);
    registerBuiltin(vm, "mem.collect", mem_collect);
    registerBuiltin(vm, "mem.swap", mem_swap);
    registerBuiltin(vm, "mem.next", mem_next);

    init_sector(vm);
}