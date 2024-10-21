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
        char * str = str_format("print 'returned:' @%d", result);
        eval(vm, str);
        free(str);
        //}
    }
    stack_free(*args);
    free_vm(vm);
}