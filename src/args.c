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
    ref.variable = vm->stack->data[ref.index];
    return ref;
}

void argpush(VirtualMachine *vm, IntList *args, Int index)
{
    StackPush(*args, index);
}

void argunshift(VirtualMachine *vm, IntList *args, Int index)
{
    StackInsert(*args, 0, index);
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