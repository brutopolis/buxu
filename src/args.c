#include "bruter.h"

//args

Reference argget(VirtualMachine *vm, IntList *args, Int index)
{
    Reference ref;
    ref.index = StackShift(*args);
    ref.variable = vm->stack->data[ref.index];
    return ref;
}

Reference argshift(VirtualMachine *vm, IntList *args)
{
    Reference ref;
    ref.index = StackShift(*args);
    //if a string do a copy
    if (vm->stack->data[ref.index]->type == TYPE_STRING)
    {
        char * temp = toString(vm->stack->data[ref.index]->value.pointer);
        ref.variable = createString(vm, temp);
        free(temp);
    }
    else if (vm->stack->data[ref.index]->type == TYPE_LIST)
    {
        IntList *list = (IntList*)vm->stack->data[ref.index]->value.pointer;
        ref.variable = createList(vm);
        for (Int i = 0; i < list->size; i++)
        {
            StackPush(*(IntList*)ref.variable->value.pointer, list->data[i]);
        }
    }
    else
    {
        ref.variable = vm->stack->data[ref.index];
    }
    return ref;
}

void argpush(VirtualMachine *vm, IntList *args, Int index)
{
    StackPush(*args, index);
}

void arginsert(VirtualMachine *vm, IntList *args, Int index, Int value)
{
    StackInsert(*args, index, value);
}

Reference argremove(VirtualMachine *vm, IntList *args, Int index)
{
    Reference ref;
    ref.index = StackRemove(*args, index);
    ref.variable = vm->stack->data[ref.index];
    return ref;
}

Int argsize(VirtualMachine *vm, IntList *args)
{
    return args->size;
}

void argfree(VirtualMachine *vm, IntList *args)
{
    while (args->size > 0)
    {
        freeVar(vm, StackShift(*args));
    }
    StackFree(*args);
    free(args);
    args = NULL;
}