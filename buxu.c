
// this is the buxu's interpreter frontend
// there are some tweaks that try be compatible with mac and windows, totally untested;

// buxu header
#include "bruter-representation.h"

// dynamic library loading
#include <dlfcn.h> 

#define BUXU_VERSION "0.1.9"

#define BUXU_EMOTICON "[=º-º=]"

static BruterList* args = NULL;
static BruterList* libs = NULL;
static BruterList* context = NULL; // global context
static char *buxu_run_code = NULL;

// file stuff
static char* file_read(char *filename)
{
    long length = 0;
    char *buffer = NULL;
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("%s: could not open file %s\n", BUXU_EMOTICON, filename);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);
    buffer = (char*)calloc(1, (size_t)length + 1);
    if (buffer == NULL)
    {
        printf("%s: could not allocate memory for file %s\n", BUXU_EMOTICON, filename);
        fclose(file);
        return NULL;
    }
    fread(buffer, 1, (size_t)length, file);
    fclose(file);
    return buffer;
}

static BruterInt repl(BruterList *received_context, BruterList* parser)
{
    char cmd[1024];
    int junk = 0;
    BruterInt result = -1;
    printf("bruter v%s\n", BRUTER_VERSION);
    printf("bruter-representation v%s\n", BR_VERSION);
    printf("buxu v%s\n", BUXU_VERSION);
    while (result == -1)
    {
        printf("%s: ", BUXU_EMOTICON);
        junk = scanf("%[^\n]%*c", cmd);
        if (junk == 0)
        {
            break;
        }
        result = br_eval(received_context, parser, cmd);
    }

    printf("%s: ", BUXU_EMOTICON);

    printf("%ld", bruter_get_int(received_context, result));
    
    printf("\n");
    return result;
}

static void buxu_dl_open(char* libpath)
{
    // check if the library is already loaded
    char *libpath_local = NULL;
    void *tmp_func = NULL;
    char *tmp = NULL;
    void (*init_func)(BruterList*) = NULL;

    BruterList *splited = NULL;
    BruterList *splited2 = NULL;
    BruterInt found = bruter_find_key(libs, libpath);
    void *handle = dlopen(libpath, RTLD_LAZY | RTLD_GLOBAL);
    if (found != -1)
    {
        printf("%s: library %s is already loaded.\n", BUXU_EMOTICON, libpath);
        return;
    }


    if (handle != NULL)
    {
        bruter_push(libs, (BruterValue){.p = handle}, libpath, 0);
    }
    else 
    {
        printf("%s: %s\n", BUXU_EMOTICON, dlerror());
        printf("%s: could not open library %s\n", BUXU_EMOTICON, libpath);
        return;
    }

    splited = br_str_split(libpath, '/');
    splited2 = br_str_split((char*)splited->data[splited->size - 1].p, '.');
    libpath_local = (char*)splited2->data[0].p;

    // now lets get the init_name function
    tmp = br_str_format("init_%s", libpath_local);
    tmp_func = dlsym(handle, tmp);
    free(tmp);
    if (tmp_func != NULL)
    {
        init_func = NULL;
        memcpy(&init_func, &tmp_func, sizeof(void*));
        init_func(context);
    }
    else 
    {
        printf("%s: %s\n", BUXU_EMOTICON, dlerror());
        printf("%s: init_%s not found in %s\n", BUXU_EMOTICON, libpath_local, libpath);
        // then lets close the library
        dlclose(handle);
        bruter_pop_int(libs);
        return;
    }

    for (BruterInt i = 0; i < splited->size; i++)
    {
        free(splited->data[i].p);
    }

    for (BruterInt i = 0; i < splited2->size; i++)
    {
        free(splited2->data[i].p);
    }

    bruter_free(splited);
    bruter_free(splited2);
}

static void buxu_dl_close(char* libpath)
{
    for (BruterInt i = 0; i < libs->size; i++)
    {
        if (strcmp(libpath, libs->keys[i]) == 0)
        {
            dlclose(bruter_get_pointer(context, libs->data[i].i));
            bruter_fast_remove_int(libs, i);
            return;
        }
    }
    printf("%s: library %s is not loaded.\n", BUXU_EMOTICON, libpath);
}

static BR_FUNCTION(brl_main_dl_open)
{
    char* str = (char*)br_arg_get_pointer(context, args, 0);
    if (strstr(str, ".brl") != NULL)
    {
        buxu_dl_open(str);
    }
    else 
    {
        char* path = br_str_format(".bupm/%s/%s.brl", str, str);
        buxu_dl_open(path);
        free(path);
    }
    return -1;
}

static BR_FUNCTION(brl_main_dl_close)
{
    char* str = (char*)br_arg_get_pointer(context, args, 0);
    if (strstr(str, ".brl") != NULL)
    {
        buxu_dl_close(str);
    }
    else 
    {
        char* path = br_str_format("./.bupm/%s/%s.brl", str, str);
        buxu_dl_close(path);
        free(path);
    }
    return -1;
}


static void free_at_exit(void)
{
    if (libs->size > 0)
    {
        while (libs->size > 0)
        {
            dlclose(bruter_pop_pointer(libs));
        }
    }
    bruter_free(libs);

    for (BruterInt i = 0; i < args->size; i++)
    {
        free(args->data[i].p);
    }

    if (buxu_run_code != NULL)
    {
        free(buxu_run_code);
    }

    br_free_context(context);
}

int main(int argc, char **argv)
{
    // result
    BruterInt result = -2; // -2 because no valid buxu program will ever return -2, this is used to detect if eval was called, if so do not start repl
    BruterList *parser = NULL;
    BruterList *langloadargs = bruter_new(2, false, false); // create a new list for the language loading arguments

    // free all at exit
    if (atexit(free_at_exit) != 0)
    {
        printf("%s: could not register the atexit function\n", BUXU_EMOTICON);
        return 1;
    }

    // virtual machine startup
    context = br_new_context(16); // global context
    parser = br_get_parser(context); // get the parser from the context

    // arguments startup
    args = bruter_new(sizeof(void*), false, false);

    // dynamic library functions
    br_add_function(context, "load", brl_main_dl_open);
    br_add_function(context, "unload", brl_main_dl_close);
    
    // dynamic libraries lists startup
    libs = bruter_new(sizeof(void*), true, false);

    // arguments parsing
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) // version
        {
            printf("buxu v%s\n", BUXU_VERSION);
            printf("bruter-representation v%s\n", BR_VERSION);
            printf("bruter v%s\n", BRUTER_VERSION);
            return 0;
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) // help
        {
            printf("buxu v%s\n", BUXU_VERSION);
            printf("%s: usage: %s [file]", BUXU_EMOTICON, argv[0]);
            printf("  -v, --version\t\tprint version\n");
            printf("  -h, --help\t\tprint this help\n");
            printf("  -l, \t\t\tload a library\n");
            printf("  -p, --path\t\tadd a path to the library search path\n");
            printf("  -e, --eval\t\teval a string\n");
            return 0;
        }
        else if (argv[i][0] == '-' && argv[i][1] == 'l') // load
        {
            langloadargs->size = 0; // clear the language loading arguments list
            bruter_push_int(langloadargs, bruter_find_key(context, "load"), NULL, 0); // add the load function to the args
            bruter_push_int(langloadargs, br_new_var(context, (BruterValue){.p = argv[i]+2}, NULL, 0), NULL, 0); // add the bruterlang library to the args
            bruter_call(context, langloadargs); // call the load function to load the bruterlang library
        }
        else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--eval") == 0) // eval
        {
            result = br_eval(context, parser, argv[i+1]);
            i+=1;// skip to the next argument
        }
        else // push to args
        {
            bruter_push(args, (BruterValue){.p = br_str_duplicate(argv[i])}, NULL, 0);
        }
    }

    bruter_free(langloadargs); // free the language loading arguments list

    if(parser->size == 0) // if the parser is empty
    {
        printf("%s: no parser step found, please load a language first\n", BUXU_EMOTICON);
        return 1;
    }
    else if (args->size == 0 && result == -2) // repl, only if no arguments and no eval rant
    {
        repl(context, parser);
    }
    else if (args->size > 0) // run files
    {
        char* file_pointer = (char*)bruter_shift_pointer(args);
    
        buxu_run_code = file_read(file_pointer);

        if (buxu_run_code == NULL)
        {
            printf("%s: could not read file %s\n", BUXU_EMOTICON, file_pointer);
            return 1;
        }
        
        br_new_var(context, (BruterValue){.p = args}, "args", BR_TYPE_LIST);
    
        result = br_eval(context, parser, buxu_run_code);
        free(file_pointer);
    }
    return (int)result;
}
