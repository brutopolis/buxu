#include "bruter.h" // 237 lines
//"stacky", just a example to show a interpreter replacement

void* previous_interpreter;

enum {
    EXIT,
    PUSH,
    POP,
    UNSHIFT,
    SHIFT,
    INSERT,
    REMOVE,
    PRINT
};

// PUSH 4

Int next_int(char** _str)
{
    Int result = atoi(*_str);
    char* _next = strchr(*_str, ' ')+1;
    
    if ((Int)_next == -1)
    {
        _next = strchr(*_str, 0);
    }
    
    *_str = _next;
    return result;
}

Float next_float(char** _str)
{
    Float result = atof(*_str);
    char* _next = strchr(*_str, ' ')+1;
    
    if ((Int)_next == -1)
    {
        _next = strchr(*_str, 0);
    }
    
    *_str = _next;
    return result;
}

Int solve_ref(VirtualMachine *vm, char** _str)
{
    Int result = atoi(*_str+1);
    result = vm->stack->data[result].integer;

    char* _next = strchr(*_str, ' ')+1;
    
    if ((Int)_next == -1)
    {
        _next = strchr(*_str, 0);
    }
    
    *_str = _next;
    return result;
}

Value get_next_arg(VirtualMachine *vm, char** _str)
{
    Value result;
    while (is_space(*_str[0]))
    {
        *_str += 1;
    }

    if (*_str[0] == '@')
    {
        result.integer = solve_ref(vm, _str);
    }
    else if(strchr(*_str,'.') > 0)
    {
        result.number = next_float(_str);
    }
    else 
    {
        result.integer = next_int(_str);
    }

    return result;
}

Int stacky_interpreter(void *_vm, char* cmd)
{
    VirtualMachine* vm = (VirtualMachine*) _vm;
    char** current = &cmd;
    // teste
    // teste
    Int inst = get_next_arg(vm,current).integer;
    switch (inst)
    {
    case EXIT:
        vm->interpret = previous_interpreter;
        break;

    case PRINT:
        printf("%d\n", get_next_arg(vm,current));
        break;
    
    case PUSH:
        stack_push(*vm->stack, get_next_arg(vm, current));
        stack_push(*vm->typestack, TYPE_INTEGER);
        break;

    case UNSHIFT:
        stack_unshift(*vm->stack, get_next_arg(vm, current));
        stack_unshift(*vm->typestack, TYPE_INTEGER);
        break;

    case POP:
        stack_pop(*vm->stack);
        stack_pop(*vm->typestack);
        break;

    case SHIFT:
        stack_shift(*vm->stack);
        stack_shift(*vm->typestack);
        break;

    

    default:
        printf("instruction %d not found.\n", inst);
        vm->interpret = previous_interpreter;
        break;
    }
    
    return -1;
}

Int use_stacky(VirtualMachine* vm, IntList* args)
{
    previous_interpreter = vm->interpret;
    vm->interpret = stacky_interpreter;
    return -1;
}

void init_stacky(VirtualMachine* vm)
{
    register_builtin(vm,"stacky",use_stacky);

}
