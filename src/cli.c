
// this is the buxu's interpreter frontend when not using as a lib
// there are some tweaks that try be compatible with mac and windows, totally untested;
// mingw, tcc and even other non-*cc compilers like cl.exe are expected to work;
// this is a non-essential tool.

// beside having compatibility with windows, some features are only available on linux
// the dynamic library loading is only available on linux, mac and mingw

// buxu header
#include "buxu.h"

// linux, macos and mingw
#if defined(__unix__) || defined(__MINGW32__) || defined(__MINGW64__)
    #include <dlfcn.h> // dynamic library loading
#endif

StringList *args;
IntList* libs;
StringList* libs_names;
VirtualMachine* vm; // global vm
char *_code = NULL;

StringList* str_split_char(char *str, char delim)
{
    StringList *splited = list_init(StringList);

    Int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == delim)
        {
            i++;
        }
        else
        {
            Int j = i;
            while (str[j] != delim && str[j] != '\0')
            {
                j++;
            }
            list_push(*splited, strndup(str + i, j - i));
            i = j;
        }
    }

    return splited;
}

// file functions
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

bool file_exists(char* filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return false;
    }
    fclose(file);
    return true;
}

Int repl(VirtualMachine *vm)
{
    buxu_print(EMOTICON_DEFAULT, "BRUTER v%s", VERSION);
    buxu_print(EMOTICON_DEFAULT, "buxu v%s", BUXU_VERSION);
    char *cmd;
    Int result = -1;
    int junk = 0;
    while (result == -1)
    {
        cmd = (char*)malloc(1024); // 1kb, should be enough for a repl
        printf(EMOTICON_IDLE ": ");
        junk = scanf("%[^\n]%*c", cmd);
        if (junk == 0)
        {
            free(cmd);
            break;
        }
        result = eval(vm, cmd);
        free(cmd);
    }

    printf("%s: ", EMOTICON_DEFAULT);
    switch (data_t(result))
    {
        case TYPE_ALLOC:
            printf("%s", data(result).s);
            break;
        default:
            printf("%ld", (Int)data(result).i);
            break;
    }
    
    printf("\n");
    return result;
}

#if defined(__unix__) || defined(__MINGW32__) || defined(__MINGW64__) // only available on unix and mingw

void buxu_dl_open(char* libpath)
{
    bool usingLibName = false;
    char* name_backup = libpath;

    #if defined(__MINGW32__) || defined(__MINGW64__)
        const char* extension = ".dll";
    #else
        const char* extension = ".so";
    #endif

    // check if the library is already loaded
    for (Int i = 0; i < libs_names->size; i++)
    {
        if (strcmp(libpath, libs_names->data[i]) == 0)
        {
            // already loaded, it wouldnt crash, neither reload the lib, but would duplicate variables to the same pointers, a waste of memory
            buxu_error("library %s already loaded", libpath);
            return;
        }
    }

    if (!file_exists(libpath))
    {
        // not in a direct path, lets try the lib paths
        StringList *splited = str_split_char(data(hash_find(vm, "file.path")).s, ';');
        for (Int i = 0; i < splited->size; i++)
        {
            char* _libpath = str_format("%s/%s%s", splited->data[i], libpath, extension);
            if (file_exists(_libpath))
            {
                usingLibName = true;
                libpath = _libpath;
                break;
            }
            free(_libpath);
        }
        list_free(*splited);
        if (!usingLibName)
        {
            buxu_error("library %s not found", libpath);
            return;
        }
    }

    void *handle = dlopen(libpath, RTLD_LAZY | RTLD_GLOBAL);

    if (handle != NULL)
    {
        list_push(*libs, (Int)handle);
        if (usingLibName)
        {
            list_push(*libs_names, strdup(libpath));
        }
        else
        {
            list_push(*libs_names, strdup(name_backup));
        }
    }
    else 
    {
        buxu_error("%s", dlerror());
        return;
    }

    StringList *splited = str_split_char(libpath, '/');
    StringList *splited2 = str_split_char(splited->data[splited->size - 1], '.');
    char *_libpath = splited2->data[0];

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
        buxu_error("init_%s not found", libpath);
        // then lets close the library
        dlclose(handle);
        list_pop(*libs);
        list_pop(*libs_names);
        return;
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
}

void buxu_dl_close(char* libpath)
{
    for (Int i = 0; i < libs_names->size; i++)
    {
        if (strcmp(libpath, libs_names->data[i]) == 0)
        {
            dlclose(data(libs->data[i]).p);
            list_fast_remove(*libs, i);
            free(libs_names->data[i]);
            list_fast_remove(*libs_names, i);
            return;
        }
    }
    buxu_error("library %s is not loaded.", libpath);
}

function(brl_main_dl_open)
{
    buxu_dl_open(arg(0).s);
    return -1;
}

function(brl_main_dl_close)
{
    buxu_dl_close(arg(0).s);
    return -1;
}

#endif

void _free_at_exit()
{
    #if defined(__unix__) || defined(__MINGW32__) || defined(__MINGW64__) // only available on unix and mingw
    if (libs->size > 0)
    {
        for (Int i = 0; i < libs->size+1; i++)
        {
            dlclose((void*)libs->data[i]);
            list_pop(*libs);
            free(list_pop(*libs_names));
        }
    }
    list_free(*libs);
    list_free(*libs_names);
    #endif

    list_free(*args);
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
    vm = make_vm();

    // lib search paths
    char* backup;

    Int index = new_var(vm, TYPE_ALLOC, pun(strdup("./lib/"), s, i));
    hash_set(vm, "file.path", index);

    // arguments startup
    args = list_init(StringList);
    
    #if defined(__unix__) || defined(__MINGW32__) || defined(__MINGW64__) // only available on unix and mingw

        // dynamic library functions
        register_function(vm, "load", brl_main_dl_open);
        register_function(vm, "unload", brl_main_dl_close);

        // dynamic libraries lists startup
        libs = list_init(IntList);
        libs_names = list_init(StringList);
    #endif


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
            // if contains space then it is a list of libraries
            if (strchr(argv[i+1], ' ') != NULL)
            {
                StringList *splited = special_space_split(argv[i+1]);
                for (Int j = 0; j < splited->size; j++)
                {
                    buxu_dl_open(splited->data[j]);
                }
                list_free(*splited);
            }
            else
            {
                buxu_dl_open(argv[i+1]);
            }

            i+=1; // skip to the next argument
        }
        else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--eval") == 0) // eval
        {
            result = eval(vm, argv[i+1]);
            i+=1;// skip to the next argument
        }
        else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--path") == 0) // add path
        {
            Int index = hash_find(vm, "file.path");
            backup = data(index).s;
            data(index).s = str_format("%s;%s", data(index).s, argv[i+1]);
            free(backup);
            i+=1; // skip to the next argument
        }
        else // push to args
        {
            list_push(*args, argv[i]);
        }
    }

    if (args->size == 0 && result == -2) // repl, only if no arguments and no eval rant
    {
        repl(vm);
    }
    else if (args->size > 0) // run files
    {
        char* ___file = list_shift(*args);
    
        _code = readfile(___file);

        if (_code == NULL)
        {
            buxu_error("file %s not found", ___file);
            return 1;
        }

        // this does not seems right, review it if some memory leak or crash occurs
        Int filepathindex = new_var(vm, TYPE_ALLOC, pun(___file, s, i));

        // remove file name
        char *path = list_shift(*args);
        char *last = strrchr(path, '/');

        if (last == NULL)
        {
            vm->stack->data[filepathindex].s = strdup("");
        }
        else
        {
            vm->stack->data[filepathindex].s = strndup(path, last - path + 1);
        }

        hash_set(vm, "file.path", filepathindex);
        
        
        
    
        result = eval(vm, _code);
    }
    return result;
}
