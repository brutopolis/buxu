#include "bruter.h"

void listpush(VirtualMachine *vm, Int list, Int value)
{
    StackPush(*((IntList*)vm->stack->data[list]->value.pointer), value);
}

void listunshift(VirtualMachine *vm, Int list, Int value)
{
    StackInsert(*((IntList*)vm->stack->data[list]->value.pointer), 0, value);
}

Int listpop(VirtualMachine *vm, Int list)
{
    return StackPop(*((IntList*)vm->stack->data[list]->value.pointer));
}

Int listshift(VirtualMachine *vm, Int list)
{
    return StackShift(*((IntList*)vm->stack->data[list]->value.pointer));
}

void listinsert(VirtualMachine *vm, Int list, Int index, Int value)
{
    StackInsert(*((IntList*)vm->stack->data[list]->value.pointer), index, value);
}

Int listremove(VirtualMachine *vm, Int list, Int index)
{
    return StackRemove(*((IntList*)vm->stack->data[list]->value.pointer), index);
}

Int listsize(VirtualMachine *vm, Int list)
{
    return ((IntList*)vm->stack->data[list]->value.pointer)->size;
}

