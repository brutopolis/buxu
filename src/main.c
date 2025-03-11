
// bruter header
#include "../include/bruter.h"

Int repl(VirtualMachine *vm)
{
    printf("bruter v%s\n", VERSION);
    char *cmd;
    Int result = -1;
    int junk = 0;
    while (result == -1)
    {
        cmd = (char*)malloc(1024);
        printf("@> ");
        junk = scanf("%[^\n]%*c", cmd);
        if (junk == 0)
        {
            free(cmd);
            break;
        }
        result = eval(vm, cmd);
        free(cmd);
    }

    printf("repl returned: [@%ld]", result);
    print_element(vm, result);
    printf("\n");
    return result;
}

IntList* libs;
StringList* libs_names;

function(brl_main_dl_open)
{
    
    void *handle = dlopen(arg(0).string, RTLD_LAZY);

    if (handle != NULL)
    {
        list_push(*libs, (Int)handle);
        list_push(*libs_names, str_duplicate(arg(0).string));
    }
    else 
    {
        printf("error: %s\n", dlerror());
    }

    StringList *splited = str_split_char(arg(0).string, '/');
    StringList *splited2 = str_split_char(splited->data[splited->size - 1], '.');
    char *libname = splited2->data[0];

    // now lets get the init_name function
    char* tmp = str_format("init_%s", libname);
    InitFunction _init = dlsym(handle, tmp);
    free(tmp);
    if (_init != NULL)
    {
        _init(vm);
    }
    else 
    {
        printf("error: %s\n", dlerror());
        printf("error: init_%s not found\n", libname);
        // then lets close the library
        dlclose(handle);
        list_pop(*libs);
        list_pop(*libs_names);
        return -1;
    }

    for (Int i = 0; i < splited->size; i++)
    {
        free(splited->data[i]);
    }

    for (Int i = 0; i < splited2->size; i++)
    {
        free(splited2->data[i]);
    }

    list_free(*splited);
    list_free(*splited2);

    return -1;
}

function(brl_main_dl_close)
{
    char* libname = arg(0).string;
    for (Int i = 0; i < libs_names->size; i++)
    {
        if (strcmp(libname, libs_names->data[i]) == 0)
        {
            dlclose(data(libs->data[i]).pointer);
            list_fast_remove(*libs, i);
            free(libs_names->data[i]);
            list_fast_remove(*libs_names, i);
        }
    }
    return -1;
}

int main(int argc, char **argv)
{
    Int result = 0;
    StringList *args = list_init(StringList);
    char* code = NULL;
    char* path = NULL;

    libs = list_init(IntList);
    libs_names = list_init(StringList);

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            printf("bruter v%s\n", VERSION);
            return 0;
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printf("bruter v%s\n", VERSION);
            printf("usage: %s [file]\n", argv[0]);
            printf("  -v, --version\t\tprint version\n");
            printf("  -h, --help\t\tprint this help\n");
            return 0;
        }
        else
        {
            list_push(*args, argv[i]);
        }
    }
    
    VirtualMachine *vm = make_vm();
    

    register_builtin(vm, "load", brl_main_dl_open);
    register_builtin(vm, "unload", brl_main_dl_close);

    if (args->size == 0)
    {
        repl(vm);
    }
    else if (args->size > 0) 
    {
        char *_code = readfile(argv[1]);
        if (_code == NULL)
        {
            printf("file %s not found\n", argv[1]);
            return 1;
        }
        Int filepathindex = new_var(vm);
        // path without file name
        vm->typestack->data[filepathindex] = TYPE_STRING;
        // remove file name
        char *path = list_shift(*args);
        char *last = strrchr(path, '/');
        if (last == NULL)
        {
            vm->stack->data[filepathindex].string = str_duplicate("");
        }
        else
        {
            vm->stack->data[filepathindex].string = str_nduplicate(path, last - path + 1);
        }
        hash_set(vm, "file.path", filepathindex);
        register_list(vm, "file.args");
        IntList *fileargs = (IntList*)data(hash_find(vm, "file.args")).pointer;
        while (args->size > 0)
        {
            list_push(*fileargs, new_string(vm, list_shift(*args)));
        }
        result = eval(vm, _code);
        free(_code);
    }
    
    if (libs->size > 0)
    {
        for (Int i = 0; i < libs->size; i++)
        {
            dlclose((void*)libs->data[i]);
            list_pop(*libs);
            free(list_pop(*libs_names));
        }
    }
    list_free(*args);
    list_free(*libs);
    list_free(*libs_names);
    free_vm(vm);
    return result;
}
