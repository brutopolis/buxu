#include "bruter.h"


Int brl_mem_clear(VirtualMachine *vm, IntList *args)
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

Int brl_mem_hold(VirtualMachine *vm, IntList *args)
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

Int brl_mem_keep(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    hold_var(vm, index);
    return index;
}

Int brl_mem_unhold(VirtualMachine *vm, IntList *args)
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

Int brl_mem_rebase(VirtualMachine *vm, IntList *args)
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

Int brl_mem_collect(VirtualMachine *vm, IntList *args)
{
    collect_garbage(vm);
    return -1;
}

Int brl_mem_swap(VirtualMachine *vm, IntList *args)
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

Int brl_mem_set(VirtualMachine *vm, IntList *args)
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

Int brl_mem_use(VirtualMachine *vm, IntList *args)
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

Int brl_mem_unuse(VirtualMachine *vm, IntList *args)
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

Int brl_mem_copy(VirtualMachine *vm, IntList *args)
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

Int brl_mem_get(VirtualMachine *vm, IntList *args)
{
    Int index = (Int)stack_shift(*args);
    if (index >= 0 && index < vm->stack->size)
    {
        return index;
    }
    
    return -1;
}


Int brl_mem_delete(VirtualMachine *vm, IntList *args)
{
    while (args->size > 0)
    {
        unuse_var(vm, stack_shift(*args));
    }
    return -1;
}


Int brl_mem_length(VirtualMachine *vm, IntList *args)
{
    return new_number(vm, vm->stack->size);
}

Int brl_mem_next(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    stack_unshift(*vm->unused, index);
    while (args->size > 0)
    {
        stack_unshift(*vm->unused, stack_shift(*args));
    }
    return -1;
}

Int brl_mem_sector_new(VirtualMachine *vm, IntList *args)
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

Int brl_mem_sector_copy(VirtualMachine *vm, IntList *args)
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

Int brl_mem_sector_swap(VirtualMachine *vm, IntList *args)
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
    register_builtin(vm, "sector.new", brl_mem_sector_new);
    register_builtin(vm, "sector.copy", brl_mem_sector_copy);
    register_builtin(vm, "sector.swap", brl_mem_sector_swap);
}

void init_mem(VirtualMachine *vm)
{
    register_number(vm, "mem.size", sizeof(Float));
    register_builtin(vm, "mem.use", brl_mem_use);
    register_builtin(vm, "mem.unuse", brl_mem_unuse);
    register_builtin(vm, "mem.get", brl_mem_get);
    register_builtin(vm, "mem.hold", brl_mem_hold);
    register_builtin(vm, "mem.keep", brl_mem_keep);
    register_builtin(vm, "mem.set", brl_mem_set);
    register_builtin(vm, "mem.copy", brl_mem_copy);
    register_builtin(vm, "mem.len", brl_mem_length);
    register_builtin(vm, "mem.clear", brl_mem_clear);
    register_builtin(vm, "mem.rebase", brl_mem_rebase);
    register_builtin(vm, "mem.unhold", brl_mem_unhold);
    register_builtin(vm, "mem.delete", brl_mem_delete);
    register_builtin(vm, "mem.collect", brl_mem_collect);
    register_builtin(vm, "mem.swap", brl_mem_swap);
    register_builtin(vm, "mem.next", brl_mem_next);

    init_sector(vm);
}