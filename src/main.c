// this file is the utility tool for bruter

// bruter header
#include "./bruter.h"

// if arduino, do not include file functions
#ifndef ARDUINO

char* readfile(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return NULL;
    }
    char *code = (char*)malloc(1);
    code[0] = '\0';
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, file) != -1)
    {
        code = (char*)realloc(code, strlen(code) + strlen(line) + 1);
        strcat(code, line);
    }
    free(line);
    fclose(file);
    return code;
};

void writefile(char *filename, char *code)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        return;
    }
    fprintf(file, "%s", code);
    fclose(file);
}

#endif

Int std_include(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    char *code = readfile(vm->stack->data[filename].string);
    Int result = -1;
    if (code == NULL)
    {
        printf("File %s not found\n", filename);
        exit(1);
    }
    else
    {
        result = eval(vm, code);
        free(code);
    }
    return result;
}

Int std_file_read(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    char *code = readfile(vm->stack->data[filename].string);
    Int result = -1;
    if (code == NULL)
    {
    }
    else
    {
        result = eval(vm, code);
        free(code);
    }
    //freeRawVar(filename);
    return result;
}

Int std_file_write(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    Int code = stack_shift(*args);
    writefile(vm->stack->data[filename].string, vm->stack->data[code].string);
    return -1;
}

Int std_repl(VirtualMachine *vm, IntList *args)
{
    printf("bruter v%s\n", VERSION);
    char *cmd;
    Int result = -1;
    while (result == -1)
    {
        cmd = (char*)malloc(1024);
        printf("@> ");
        scanf("%[^\n]%*c", cmd);
        result = eval(vm, cmd);
        free(cmd);
    }

    printf("repl returned: @%d ", result);
    char * str = str_format("print (get %d)", result);
    eval(vm, str);
    free(str);
    return result;
}

int main(int argv, char **argc)
{
    VirtualMachine *vm = make_vm();

    preset_all(vm);

    spawn_builtin(vm, "file.read", std_file_read);
    spawn_builtin(vm, "file.write", std_file_write);
    spawn_builtin(vm, "include", std_include);

    spawn_builtin(vm, "repl", std_repl);


    // read file pointed by argv[1]
    if (argv == 1)
    {
        IntList *varlist = make_int_list();
        std_repl(vm, varlist);
        stack_free(*varlist);
    }
    else 
    {
        char *_code = readfile(argc[1]);
        if (_code == NULL)
        {
            printf("File not found\n");
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
                char * str = str_format("print (get %d)", list->data[i]);
                eval(vm, str);
                free(str);
            }
        }
        else 
        {
            char * str = str_format("print (get %d)", result);
            eval(vm, str);
            free(str);
        }
        
    }
    free_vm(vm);
}