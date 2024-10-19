// this file is the utility tool for bruter

// bruter header
#include "./bruter.h"



int main(int argc, char **argv)
{
    StringList *args = make_string_list();
    for (int i = 1; i < argc; i++)
    {
        stack_push(*args, argv[i]);
    }
    VirtualMachine *vm = make_vm();

    // <libraries init>

    // read file pointed by argv[1]
    if (args->size == 0)
    {
        interpret(vm, "repl");
    }
    else if (args->size == 1)
    {
        char *_code = readfile(argv[1]);
        if (_code == NULL)
        {
            printf("file not found\n");
            return 1;
        }

        Int result = eval(vm, _code);
        free(_code);
        
        printf("returned: [%d]", result);
        switch (vm->typestack->data[result])
        {
            case TYPE_NUMBER:
                printf("{number}");
                break;
            case TYPE_STRING:
                printf("{string}");
                break;
            case TYPE_LIST:
                printf("{list}");
                break;
            case TYPE_BUILTIN:
                printf("{builtin}");
                break;
            case TYPE_FUNCTION:
                printf("{function}");
                break;
            default:
                printf("{unknown}");
                break;
        }

        if (result>=0)
        {
            printf(": ");
        }
        else 
        {
            printf("\n");
        }
        
        if (vm->typestack->data[result] == TYPE_LIST)
        {
            IntList *list = (IntList*)vm->stack->data[result].pointer;
            for (Int i = 0; i < list->size; i++)
            {
                char * str = str_format("print @%d", list->data[i]);
                eval(vm, str);
                free(str);
            }
        }
        else 
        {
            char * str = str_format("print @%d", result);
            eval(vm, str);
            free(str);
        }
    }
    stack_free(*args);
    free_vm(vm);
}