#include "bruter.h" // 237 lines
//"stacky", just a example to show a interpreter replacement

void* previous_interpreter;

enum {
    EXIT,
    SET,
    FSET,
    PUSH,
    POP,
    UNSHIFT,
    SHIFT,
    INSERT,
    REMOVE,
    PRINT,
    SWAP,
    COPY,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    FADD,
    FSUB,
    FMUL,
    FDIV,
    FMOD,
    FLOOR,
    CEIL,
    ROUND,
    TRUNC,
    POW,
    FLOAT,
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
    Int a, b, c;
    switch (inst)
    {
    case EXIT:
        vm->interpret = previous_interpreter;
        break;

    case SET:
        vm->stack->data[get_next_arg(vm,current).integer].integer = get_next_arg(vm,current).integer;
        break;

    case FSET:
        vm->stack->data[get_next_arg(vm,current).integer].number = get_next_arg(vm,current).number;
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

    case INSERT:
        a = get_next_arg(vm, current).integer;
        stack_insert(*vm->stack, a, get_next_arg(vm, current));
        stack_insert(*vm->typestack, a, TYPE_INTEGER);
        break;

    case REMOVE:
        a = get_next_arg(vm, current).integer;
        stack_remove(*vm->stack, a);
        stack_remove(*vm->typestack, a);
        break;

    case COPY:
        vm->stack->data[get_next_arg(vm, current).integer].integer = get_next_arg(vm, current).integer;
        break;

    case SWAP:
        a = get_next_arg(vm, current).integer;
        b = get_next_arg(vm, current).integer;
        c = vm->stack->data[a].integer;
        vm->stack->data[a].integer = vm->stack->data[b].integer;
        vm->stack->data[b].integer = c;
        break;

    case ADD:
        vm->stack->data[get_next_arg(vm, current).integer].integer += get_next_arg(vm, current).integer;
        break;

    case SUB:
        vm->stack->data[get_next_arg(vm, current).integer].integer -= get_next_arg(vm, current).integer;
        break;

    case MUL:
        vm->stack->data[get_next_arg(vm, current).integer].integer *= get_next_arg(vm, current).integer;
        break;

    case DIV:
        vm->stack->data[get_next_arg(vm, current).integer].integer /= get_next_arg(vm, current).integer;
        break;

    case MOD:
        vm->stack->data[get_next_arg(vm, current).integer].integer %= get_next_arg(vm, current).integer;
        break;

    case FADD:
        vm->stack->data[get_next_arg(vm, current).integer].number += get_next_arg(vm, current).number;
        break;

    case FSUB:
        vm->stack->data[get_next_arg(vm, current).integer].number -= get_next_arg(vm, current).number;
        break;
    
    case FMUL:
        vm->stack->data[get_next_arg(vm, current).integer].number *= get_next_arg(vm, current).number;
        break;

    case FDIV:
        vm->stack->data[get_next_arg(vm, current).integer].number /= get_next_arg(vm, current).number;
        break;

    case FMOD:
        vm->stack->data[get_next_arg(vm, current).integer].number = fmod(vm->stack->data[get_next_arg(vm, current).integer].number, get_next_arg(vm, current).number);
        break;

    case FLOOR:
        vm->stack->data[get_next_arg(vm, current).integer].integer = floor(vm->stack->data[get_next_arg(vm, current).integer].number);
        break;

    case CEIL:
        vm->stack->data[get_next_arg(vm, current).integer].integer = ceil(vm->stack->data[get_next_arg(vm, current).integer].number);
        break;

    case ROUND:
        vm->stack->data[get_next_arg(vm, current).integer].integer = round(vm->stack->data[get_next_arg(vm, current).integer].number);
        break;

    case FLOAT:
        a = get_next_arg(vm, current).integer;
        vm->stack->data[a].number = (Float)vm->stack->data[a].integer;
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
