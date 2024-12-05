#include "bruter.h"
//"bruter bytecode", just a example to show a interpreter replacement

void* previous_interpreter;

Int bc_interpreter(void *_vm, char* cmd)
{
    VirtualMachine* vm = (VirtualMachine*) _vm;
    /*IntList *args = vm->parse(vm, cmd);
    Int func = stack_shift(*args);
    Int result = -1;

    if (func > -1 && vm->typestack->data[func] == TYPE_BUILTIN)
    {
        Function function = vm->stack->data[func].pointer;
        result = function(vm, args);
    }
    else 
    {
        printf("Error: %d is not a function\n", func);
    }

    stack_free(*args);*/
    return -1;
}

void init_brbc(VirtualMachine* vm)
{
    previous_interpreter = vm->interpret;
    vm->interpret = bc_interpreter;
}
