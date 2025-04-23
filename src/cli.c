
// this is the buxu's interpreter frontend
// there are some tweaks that try be compatible with mac and windows, totally untested;

// buxu header
#include "buxu.h"

#include <dlfcn.h> // dynamic library loading

List *args;
List* libs;
List* libs_names;
VirtualMachine* vm; // global vm
char *_code = NULL;

Int repl(VirtualMachine *vm)
{
    buxu_print(EMOTICON_DEFAULT, "BRUTER v%s", VERSION);
    buxu_print(EMOTICON_DEFAULT, "buxu v%s", BUXU_VERSION);
    char cmd[1024];
    Int result = -1;
    int junk = 0;
    while (result == -1)
    {
        printf(EMOTICON_IDLE ": ");
        junk = scanf("%[^\n]%*c", cmd);
        if (junk == 0)
        {
            break;
        }
        result = eval(vm, cmd);
    }

    printf("%s: ", EMOTICON_DEFAULT);
    Int nullindex = -1;
    for (Int i = 0; i < vm->values->size; i++)
    {
        if (vm->hashes->data[i].p != NULL && strcmp(vm->hashes->data[i].s, "NULL") == 0)
        {
            nullindex = i;
            break;
        }
    }

    if (result < nullindex)
    {
        printf("%p", vm->values->data[result].s);
    }
    else if (result == nullindex)
    {
        printf("NULL");
    }
    else
    {
        printf("%ld", vm->values->data[result].i);
    }
    
    printf("\n");
    return result;
}

void buxu_dl_open(char* libpath)
{
    // check if the library is already loaded
    for (Int i = 0; i < libs_names->size; i++)
    {
        if (strcmp(libpath, libs_names->data[i].s) == 0)
        {
            // already loaded, it wouldnt crash, neither reload the lib, but would duplicate variables to the same pointers, a waste of memory
            buxu_error("library %s already loaded", libpath);
            return;
        }
    }

    void *handle = dlopen(libpath, RTLD_LAZY | RTLD_GLOBAL);

    if (handle != NULL)
    {
        list_push(libs, (Value){.p = handle});
        list_push(libs_names, (Value){.s = str_duplicate(libpath)});
    }
    else 
    {
        buxu_error("%s", dlerror());
        return;
    }

    List *splited = special_split(libpath, '/');
    List *splited2 = special_split(splited->data[splited->size - 1].s, '.');
    char *_libpath = splited2->data[0].s;

    // now lets get the init_name function
    char* tmp = str_format("init_%s", _libpath);
    InitFunction _init = dlsym(handle, tmp);
    free(tmp);
    if (_init != NULL)
    {
        _init(vm);
    }
    else 
    {
        buxu_error("%s", dlerror());
        buxu_error("init_%s not found", _libpath);
        // then lets close the library
        dlclose(handle);
        list_pop(libs);
        list_pop(libs_names);
        return;
    }

    for (Int i = 0; i < splited->size; i++)
    {
        free(splited->data[i].s);
    }

    for (Int i = 0; i < splited2->size; i++)
    {
        free(splited2->data[i].s);
    }

    list_free(splited);
    list_free(splited2);
}

void buxu_dl_close(char* libpath)
{
    for (Int i = 0; i < libs_names->size; i++)
    {
        if (strcmp(libpath, libs_names->data[i].s) == 0)
        {
            dlclose(data(libs->data[i].i).p);
            list_fast_remove(libs, i);
            free(libs_names->data[i].s);
            list_fast_remove(libs_names, i);
            return;
        }
    }
    buxu_error("library %s is not loaded.", libpath);
}

function(brl_main_dl_open)
{
    char* str = arg_s(0);
    if (strstr(str, ".brl") != NULL)
    {
        buxu_dl_open(str);
    }
    else 
    {
        char* path = str_format(".buxu/bpm/%s/%s.brl", str, str);
        buxu_dl_open(path);
        free(path);
    }
    return -1;
}

function(brl_main_dl_close)
{
    char* str = arg_s(0);
    if (strstr(str, ".brl") != NULL)
    {
        buxu_dl_close(str);
    }
    else 
    {
        char* path = str_format("./.buxu/bpm/%s/%s.brl", str, str);
        buxu_dl_close(path);
        free(path);
    }
    return -1;
}

void _free_at_exit()
{
    if (libs->size > 0)
    {
        while (libs->size > 0)
        {
            dlclose(list_pop(libs).p);
            free(list_pop(libs_names).p);
        }
    }
    list_free(libs);
    list_free(libs_names);

    for (Int i = 0; i < args->size; i++)
    {
        free(args->data[i].s);
    }
    list_free(args);
    free_vm(vm);

    if (_code != NULL)
    {
        free(_code);
    }
}

int main(int argc, char **argv)
{
    // free all at exit
    if (atexit(_free_at_exit) != 0)
    {
        buxu_error("could not register the atexit function");
        return 1;
    }

    // result
    Int result = -2; // -2 because no valid buxu program will ever return -2

    // virtual machine startup
    vm = make_vm(16); // starts with capacity of 16 vars, to avoid reallocations, it will grow as needed

    // lib search paths
    char* backup;



    // arguments startup
    args = list_init(0);
    
    // dynamic library functions
    add_function(vm, "load", brl_main_dl_open);
    add_function(vm, "unload", brl_main_dl_close);

    // dynamic libraries lists startup
    libs = list_init(0);
    libs_names = list_init(0);


    // arguments parsing
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) // version
        {
            buxu_print(EMOTICON_DEFAULT, "BRUTER v%s", VERSION);
            buxu_print(EMOTICON_DEFAULT, "buxu v%s", BUXU_VERSION);
            return 0;
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) // help
        {
            buxu_print(EMOTICON_DEFAULT, "v%s", VERSION);
            buxu_print(EMOTICON_DEFAULT, "usage: %s [file]", argv[0]);
            printf("  -v, --version\t\tprint version\n");
            printf("  -h, --help\t\tprint this help\n");
            printf("  -l, --load\t\tload a library\n");
            printf("  -p, --path\t\tadd a path to the library search path\n");
            printf("  -e, --eval\t\teval a string\n");
            return 0;
        }
        else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--load") == 0) // preload libs
        {
            char *libname = str_format("load (@%s)", argv[i+1]);
            eval(vm, libname);
            free(libname);

            i+=1; // skip to the next argument
        }
        else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--eval") == 0) // eval
        {
            result = eval(vm, argv[i+1]);
            i+=1;// skip to the next argument
        }
        else // push to args
        {
            list_push(args, (Value){.s = str_duplicate(argv[i])});
        }
    }

    if (args->size == 0 && result == -2) // repl, only if no arguments and no eval rant
    {
        repl(vm);
    }
    else if (args->size > 0) // run files
    {
        char* ___file = list_shift(args).s;
    
        _code = readfile(___file);

        if (_code == NULL)
        {
            buxu_error("file %s not found", ___file);
            return 1;
        }
    
        result = eval(vm, _code);
        free(___file);
    }
    return result;
}
