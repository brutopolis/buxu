#include "bruter.h" // 237 lines
//"texty", just a example to show a interpreter replacement

void* previous_interpreter;

Value get_next_arg(VirtualMachine *vm)
{
    Value result;
    char* token = strtok(NULL, " \n\r\t");

    if (token[0] == '@')
    {
        token += 1;
        result.integer = vm->stack->data[atoi(token)].integer;
    }
    else if(strchr(token,'.') > 0)
    {
        result.number = atof(token);
    }
    else
    {
        result.integer = atoi(token);
    }

    return result;
}

Int texty_interpreter(void *_vm, char* cmd)
{
    VirtualMachine* vm = (VirtualMachine*) _vm;
    char* token = strtok(cmd, " \n\r\t");
    Int a, b, c;

    if(strcmp(token, "EXIT")==0)
    {
        vm->interpret = previous_interpreter;
    } 
    else if(strcmp(token, "SET")==0)
    {
        vm->stack->data[get_next_arg(vm).integer].integer = get_next_arg(vm).integer;
    }
    else if(strcmp(token, "PRINT")==0)
    {
        
        printf("%d\n", get_next_arg(vm));
    }
    else if(strcmp(token, "PUSH")==0)
    {
        stack_push(*vm->stack, get_next_arg(vm));
        stack_push(*vm->typestack, TYPE_INTEGER);
    } 
    else if(strcmp(token, "UNSHIFT")==0)
    {
        stack_unshift(*vm->stack, get_next_arg(vm));
        stack_unshift(*vm->typestack, TYPE_INTEGER);
    } 
    else if(strcmp(token, "POP")==0)
    {
        stack_pop(*vm->stack);
        stack_pop(*vm->typestack);
    } 
    else if(strcmp(token, "SHIFT")==0)
    {
        stack_shift(*vm->stack);
        stack_shift(*vm->typestack);
    } 
    else if(strcmp(token, "INSERT")==0)
    {
        a = get_next_arg(vm).integer;
        stack_insert(*vm->stack, a, get_next_arg(vm));
        stack_insert(*vm->typestack, a, TYPE_INTEGER);
    } 
    else if(strcmp(token, "REMOVE")==0)
    {
        a = get_next_arg(vm).integer;
        stack_remove(*vm->stack, a);
        stack_remove(*vm->typestack, a);
    } 
    else if(strcmp(token, "COPY")==0)
    {
        vm->stack->data[get_next_arg(vm).integer].integer = get_next_arg(vm).integer;
    } 
    else if(strcmp(token, "SWAP")==0)
    {
        a = get_next_arg(vm).integer;
        b = get_next_arg(vm).integer;
        c = vm->stack->data[a].integer;
        vm->stack->data[a].integer = vm->stack->data[b].integer;
        vm->stack->data[b].integer = c;
    } 
    else if(strcmp(token, "REPEAT")==0)
    {
        a = get_next_arg(vm).integer;
        token = strtok(NULL, "\0");
        char* newstr = str_replace_all(token, ",", ";");

        for (Int i = 0; i < a; i++)
        {
            eval(vm, newstr);
        }

        free(newstr);
    } 
    else if(strcmp(token, "ADD")==0)
    {
        vm->stack->data[get_next_arg(vm).integer].integer += get_next_arg(vm).integer;
    } 
    else if(strcmp(token, "SUB")==0)
    {
        vm->stack->data[get_next_arg(vm).integer].integer -= get_next_arg(vm).integer;
    } 
    else if(strcmp(token, "MUL")==0)
    {
        vm->stack->data[get_next_arg(vm).integer].integer *= get_next_arg(vm).integer;
    } 
    else if(strcmp(token, "DIV")==0)
    {
        vm->stack->data[get_next_arg(vm).integer].integer /= get_next_arg(vm).integer;
    } 
    else if(strcmp(token, "MOD")==0)
    {
        vm->stack->data[get_next_arg(vm).integer].integer %= get_next_arg(vm).integer;
    }
    else if(strcmp(token, "RETURN")==0)
    {
        return get_next_arg(vm).integer;
    }
    else
    {
        printf("instruction %s not found.\n", token);
        vm->interpret = previous_interpreter;
    }
    
    return -1;
}

Int use_texty(VirtualMachine* vm, IntList* args)
{
    previous_interpreter = vm->interpret;
    vm->interpret = texty_interpreter;
    return -1;
}

void init_texty(VirtualMachine* vm)
{
    register_builtin(vm,"texty",use_texty);
}
