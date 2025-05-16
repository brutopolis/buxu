
// this is the buxu's interpreter frontend
// there are some tweaks that try be compatible with mac and windows, totally untested;

// buxu header
#include "buxu.h"

#include <dlfcn.h> // dynamic library loading

List *args;
List* libs;
List* context; // global context
char *_code = NULL;

// file stuff
char* file_read(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return NULL;
    }

    char *code = (char*)malloc(1);
    if (code == NULL)
    {
        printf("BRUTER_ERROR: could not allocate memory for file\n");
        fclose(file);
        return NULL;
    }

    code[0] = '\0';

    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, file) != -1)
    {
        size_t new_size = strlen(code) + strlen(line) + 1;
        char *temp = realloc(code, new_size);
        if (temp == NULL)
        {
            printf("BRUTER_ERROR: could not reallocate memory while reading file\n");
            free(code);
            free(line);
            fclose(file);
            return NULL;
        }
        code = temp;
        strcat(code, line);
    }

    free(line);
    fclose(file);
    return code;
}

void file_write(char *filename, char *code)
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

Int repl(List *context)
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
        result = eval(context, cmd);
    }

    printf("%s: ", EMOTICON_DEFAULT);

    printf("%ld", data(result).i);
    
    printf("\n");
    return result;
}

void buxu_dl_open(char* libpath)
{
    // check if the library is already loaded
    Int found = table_find(libs, libpath);
    if (found != -1)
    {
        buxu_error("library %s already loaded", libpath);
        return;
    }

    void *handle = dlopen(libpath, RTLD_LAZY | RTLD_GLOBAL);

    if (handle != NULL)
    {
        table_push(libs, (Value){.p = handle}, libpath);
    }
    else 
    {
        buxu_error("%s", dlerror());
        return;
    }

    List *splited = str_split(libpath, '/');
    List *splited2 = str_split(splited->data[splited->size - 1].s, '.');
    char *_libpath = splited2->data[0].s;

    // now lets get the init_name function
    char* tmp = str_format("init_%s", _libpath);
    void (*_init)(List*) = dlsym(handle, tmp);
    free(tmp);
    if (_init != NULL)
    {
        _init(context);
    }
    else 
    {
        buxu_error("%s", dlerror());
        buxu_error("init_%s not found", _libpath);
        // then lets close the library
        dlclose(handle);
        list_pop(libs);
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
    for (Int i = 0; i < libs->size; i++)
    {
        if (strcmp(libpath, libs->keys[i]) == 0)
        {
            dlclose(data(libs->data[i].i).p);
            list_fast_remove(libs, i);
            return;
        }
    }
    buxu_error("library %s is not loaded.", libpath);
}

list_function(brl_main_dl_open)
{
    char* str = arg_s(0);
    if (strstr(str, ".brl") != NULL)
    {
        buxu_dl_open(str);
    }
    else 
    {
        char* path = str_format(".bupm/%s/%s.brl", str, str);
        buxu_dl_open(path);
        free(path);
    }
    return -1;
}

list_function(brl_main_dl_close)
{
    char* str = arg_s(0);
    if (strstr(str, ".brl") != NULL)
    {
        buxu_dl_close(str);
    }
    else 
    {
        char* path = str_format("./.bupm/%s/%s.brl", str, str);
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
        }
    }
    list_free(libs);

    for (Int i = 0; i < args->size; i++)
    {
        free(args->data[i].s);
    }
    list_free(args);

    list_free(context);

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
    context = table_init(48); // starts with capacity of 48 vars, to avoid reallocations, it will grow as needed

    // lib search paths
    char* backup;

    // arguments startup
    args = list_init(sizeof(void*));
    
    // dynamic library functions
    add_function(context, "load", brl_main_dl_open);
    add_function(context, "unload", brl_main_dl_close);

    // dynamic libraries lists startup
    libs = table_init(sizeof(void*));


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
            printf("  -l, \t\t\tload a library\n");
            printf("  -p, --path\t\tadd a path to the library search path\n");
            printf("  -e, --eval\t\teval a string\n");
            return 0;
        }
        else if (argv[i][0] == '-' && argv[i][1] == 'l') // load
        {
            char *libname = str_format("load {%s}", argv[i] + 2);
            eval(context, libname);
            free(libname);

            i+=1; // skip to the next argument
        }
        else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--eval") == 0) // eval
        {
            result = eval(context, argv[i+1]);
            i+=1;// skip to the next argument
        }
        else // push to args
        {
            list_push(args, (Value){.s = str_duplicate(argv[i])});
        }
    }

    if (args->size == 0 && result == -2) // repl, only if no arguments and no eval rant
    {
        repl(context);
    }
    else if (args->size > 0) // run files
    {
        char* ___file = list_shift(args).s;
    
        _code = file_read(___file);

        if (_code == NULL)
        {
            buxu_error("file %s not found", ___file);
            return 1;
        }
    
        result = eval(context, _code);
        free(___file);
    }
    return result;
}
