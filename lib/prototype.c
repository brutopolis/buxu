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

// prototype array

Int prototype_push(VirtualMachine *vm, IntList *args)
{
    Int prototype = stack_shift(*args);
    Int value = stack_shift(*args);

    char *name;
    char *resultname;
    Int size;

    //if last char is . 
    if (vm->stack->data[prototype].string[strlen(vm->stack->data[prototype].string) - 1] == '.')
    {
        name = str_format("%ssize", vm->stack->data[prototype].string);
        size = hash_find(vm, name);
        if (size == -1)
        {
            size = new_var(vm);
            vm->typestack->data[size] = TYPE_NUMBER;
            vm->stack->data[size].number = 1;
            hash_set(vm, name, size);
        }
        resultname = str_format("%s%d", vm->stack->data[prototype].string, (Int)vm->stack->data[size].number - 1);
    }
    else
    {
        name = str_format("%s.size", vm->stack->data[prototype].string);
        size = hash_find(vm, name);
        if (size == -1)
        {
            size = new_var(vm);
            vm->typestack->data[size] = TYPE_NUMBER;
            vm->stack->data[size].number = 1;
            hash_set(vm, name, size);
        }
        resultname = str_format("%s.%d", vm->stack->data[prototype].string, (Int)vm->stack->data[size].number - 1);
    }


    vm->stack->data[size].number++;
    hash_set(vm, resultname, value);
    free(resultname);
    free(name);
    return -1;
}


Int prototype_pop(VirtualMachine *vm, IntList *args)
{
    Int prototype = stack_shift(*args);
    char *name;
    char *resultname;
    Int size;

    //if last char is . 
    if (vm->stack->data[prototype].string[strlen(vm->stack->data[prototype].string) - 1] == '.')
    {
        name = str_format("%ssize", vm->stack->data[prototype].string);
        size = hash_find(vm, name);
        if (size == -1)
        {
            return -1;
        }
        resultname = str_format("%s%d", vm->stack->data[prototype].string, (Int)vm->stack->data[size].number - 2);
    }
    else
    {
        name = str_format("%s.size", vm->stack->data[prototype].string);
        size = hash_find(vm, name);
        if (size == -1)
        {
            return -1;
        }
        resultname = str_format("%s.%d", vm->stack->data[prototype].string, (Int)vm->stack->data[size].number - 2);
    }
    Int result = hash_find(vm, resultname);
    vm->stack->data[size].number--;
    hash_unset(vm, resultname);
    free(resultname);
    free(name);
    return result;
}

void init_prototype(VirtualMachine *vm)
{
    registerBuiltin(vm, "proto.copy", prototype_copy);
    registerBuiltin(vm, "proto.hold", prototype_hold);
    registerBuiltin(vm, "proto.unhold", prototype_unhold);
    registerBuiltin(vm, "proto.equals", prototype_equals);
    registerBuiltin(vm, "proto.compare", prototype_compare);

    registerBuiltin(vm, "proto.push", prototype_push);
    registerBuiltin(vm, "proto.pop", prototype_pop);
}

//#endif