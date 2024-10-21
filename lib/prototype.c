#include "bruter.h"

//#ifndef ARDUINO


// std prototype
// std prototype
// std prototype

Int prototype_copy(VirtualMachine *vm, IntList *args)
{
    Int prototype_name = stack_shift(*args);
    Int new_name = stack_shift(*args);
    for (Int i = vm->hashes->size-1; i >= 0; i--)
    {
        if (str_find(vm->hashes->data[i].key, vm->stack->data[prototype_name].string) == 0)
        {
            Int index = new_var(vm);
            vm->stack->data[index] = value_duplicate(vm->stack->data[vm->hashes->data[i].index], vm->typestack->data[vm->hashes->data[i].index]);
            vm->typestack->data[index] = vm->typestack->data[vm->hashes->data[i].index];
            char* tmpstr = str_replace_all(vm->hashes->data[i].key, vm->stack->data[prototype_name].string, vm->stack->data[new_name].string);
            hash_set(vm, tmpstr, index);
            free(tmpstr);
        }
    }
    return -1;
}

Int prototype_compare(VirtualMachine *vm, IntList *args)// compare two prototypes structure and type, return 0 if they are the same, -1 if they are different
{
    Int prototype1 = stack_shift(*args);
    Int prototype2 = stack_shift(*args);

    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (str_find(vm->hashes->data[i].key, vm->stack->data[prototype1].string) == 0)
        {
            char * tmpstr = str_replace_all(vm->hashes->data[i].key, vm->stack->data[prototype1].string, vm->stack->data[prototype2].string);
            Int index = hash_find(vm, tmpstr);
            free(tmpstr);
            if (index == -1)
            {
                return new_number(vm, 0);
            }
        }
        else if (str_find(vm->hashes->data[i].key, vm->stack->data[prototype2].string) == 0)
        {
            char * tmpstr = str_replace_all(vm->hashes->data[i].key, vm->stack->data[prototype2].string, vm->stack->data[prototype1].string);
            Int index = hash_find(vm, tmpstr);
            free(tmpstr);
            if (index == -1)
            {
                return new_number(vm, 0);
            }
        }
    }

    return new_number(vm, 1);
}

Int prototype_hold(VirtualMachine *vm, IntList *args)
{
    Int prototype = stack_shift(*args);
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (str_find(vm->hashes->data[i].key, vm->stack->data[prototype].string) == 0)
        {
            hold_var(vm, vm->hashes->data[i].index);
        }
    }
    return -1;
}

Int prototype_unhold(VirtualMachine *vm, IntList *args)
{
    Int prototype = stack_shift(*args);
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (str_find(vm->hashes->data[i].key, vm->stack->data[prototype].string) == 0)
        {
            unhold_var(vm, vm->hashes->data[i].index);
        }
    }
    return -1;
}

Int prototype_equals(VirtualMachine *vm, IntList *args)
{
    Int prototype1 = stack_shift(*args);
    Int prototype2 = stack_shift(*args);
    Int result = -1;
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (str_find(vm->hashes->data[i].key, vm->stack->data[prototype1].string) == 0)
        {
            char * tmpstr = str_replace_all(vm->hashes->data[i].key, vm->stack->data[prototype1].string, vm->stack->data[prototype2].string);
            Int index = hash_find(vm, tmpstr);
            free(tmpstr);
            if (index == -1)
            {
                return new_number(vm, 0);
            }
        }
    }
    return result;
}

void init_prototype(VirtualMachine *vm)
{
    registerBuiltin(vm, "prototype.copy", prototype_copy);
    registerBuiltin(vm, "prototype.hold", prototype_hold);
    registerBuiltin(vm, "prototype.unhold", prototype_unhold);
    registerBuiltin(vm, "prototype.equals", prototype_equals);
    registerBuiltin(vm, "prototype.compare", prototype_compare);
}

//#endif