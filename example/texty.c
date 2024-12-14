#include "bruter.h" // 237 lines
//"texty", just a example to show a interpreter replacement

void* previous_interpreter;

Value get_next_arg(VirtualMachine *vm)
{
    Value result;
    char* token = strtok(NULL, " \n\r\t");

    if (token[0] == '@')
    {
        result.number = vm->stack->data[atoi(token+1)].number;
    }
    else
    {
        result.number = atof(token);
    }

    return result;
}

Int texty_interpreter(void *_vm, char* cmd)
{
    VirtualMachine* vm = (VirtualMachine*) _vm;
    char* token = strtok(cmd, " \n\r\t");

    if(strcmp(token, "EXIT")==0)
    {
        vm->interpret = previous_interpreter;
    } 
    else if(strcmp(token, "SET")==0)
    {
        vm->stack->data[(Int)get_next_arg(vm).number].number = get_next_arg(vm).number;
    }
    else if(strcmp(token, "PRINT")==0)
    {
        printf("%f\n", get_next_arg(vm).number);
    }
    else if(strcmp(token, "PUSH")==0)
    {
        stack_push(*vm->stack, get_next_arg(vm));
        stack_push(*vm->typestack, TYPE_NUMBER);
    } 
    else if(strcmp(token, "UNSHIFT")==0)
    {
        stack_unshift(*vm->stack, get_next_arg(vm));
        stack_unshift(*vm->typestack, TYPE_NUMBER);
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
        Int index = get_next_arg(vm).number;
        stack_insert(*vm->stack, index, get_next_arg(vm));
        stack_insert(*vm->typestack, index, TYPE_NUMBER);
    } 
    else if(strcmp(token, "REMOVE")==0)
    {
        Int index = get_next_arg(vm).number;
        stack_remove(*vm->stack, index);
        stack_remove(*vm->typestack, index);
    } 
    else if(strcmp(token, "COPY")==0)
    {
        vm->stack->data[(Int)get_next_arg(vm).number].number = get_next_arg(vm).number;
    } 
    else if(strcmp(token, "SWAP")==0)
    {
        Int a = get_next_arg(vm).number;
        Int b = get_next_arg(vm).number;
        Int c = vm->stack->data[a].number;
        vm->stack->data[a].number = vm->stack->data[b].number;
        vm->stack->data[b].number = c;
    } 
    else if(strcmp(token, "REPEAT")==0)
    {
        Int index = (Int)get_next_arg(vm).number;
        token = strtok(NULL, "\0");
        char* newstr = str_replace_all(token, ",", ";");

        for (Int i = 0; i < index; i++)
        {
            eval(vm, newstr);
        }

        free(newstr);
    } 
    else if(strcmp(token, "ADD")==0)
    {
        vm->stack->data[(Int)get_next_arg(vm).number].number += get_next_arg(vm).number;
    } 
    else if(strcmp(token, "SUB")==0)
    {
        vm->stack->data[(Int)get_next_arg(vm).number].number -= get_next_arg(vm).number;
    } 
    else if(strcmp(token, "MUL")==0)
    {
        vm->stack->data[(Int)get_next_arg(vm).number].number *= get_next_arg(vm).number;
    } 
    else if(strcmp(token, "DIV")==0)
    {
        vm->stack->data[(Int)get_next_arg(vm).number].number /= get_next_arg(vm).number;
    } 
    else if(strcmp(token, "MOD")==0)
    {
        Int index = (Int)get_next_arg(vm).number;
        vm->stack->data[index].number = fmod(vm->stack->data[index].number, get_next_arg(vm).number);
    }
    else if(strcmp(token, "RETURN")==0)
    {
        return get_next_arg(vm).number;
    }
    else if(strcmp(token, "REGISTER")==0)
    {
        hash_set(vm, strtok(NULL, " \n\r\t"), (Int)get_next_arg(vm).number);
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
