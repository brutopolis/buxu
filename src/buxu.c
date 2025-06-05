
// this is the buxu's interpreter frontend
// there are some tweaks that try be compatible with mac and windows, totally untested;

// buxu header
#include "bruter-representation.h"

// dynamic library loading
#include <dlfcn.h> 

#define BUXU_VERSION "0.1.9"

// emoticons
#define EMOTICON_DEFAULT "[=°-°=]"
#define EMOTICON_IDLE "[=° °=]"
#define EMOTICON_WARNING "[=°~°=]"
#define EMOTICON_ERROR "[=°x°=]"
#define EMOTICON_CONFUSED "[=º?°=]"

// define a macro to printf but it always starts with [=°-°=] and ends with a newline
#define BUXU_PRINT(emoticon, ...) printf(emoticon ": "); printf(__VA_ARGS__); printf("\n")
#define BUXU_WARN(...) printf(EMOTICON_WARNING ": warning: "); printf(__VA_ARGS__); printf("\n")
#define BUXU_ERROR(...) printf(EMOTICON_ERROR ": error: "); printf(__VA_ARGS__); printf("\n")
#define BUXU_INFO(...) printf(EMOTICON_DEFAULT ": info: "); printf(__VA_ARGS__); printf("\n")

BruterList *args;
BruterList* libs;
BruterList* context; // global context
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
        printf("BUXU_ERROR: could not allocate memory for file\n");
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
            printf("BUXU_ERROR: could not reallocate memory while reading file\n");
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

BruterInt repl(BruterList *context, BruterList* parser)
{
    BUXU_PRINT(EMOTICON_DEFAULT, "BRUTER v%s", VERSION);
    BUXU_PRINT(EMOTICON_DEFAULT, "buxu v%s", BUXU_VERSION);
    char cmd[1024];
    BruterInt result = -1;
    int junk = 0;
    while (result == -1)
    {
        printf(EMOTICON_IDLE ": ");
        junk = scanf("%[^\n]%*c", cmd);
        if (junk == 0)
        {
            break;
        }
        result = br_eval(context, parser, cmd);
    }

    printf("%s: ", EMOTICON_DEFAULT);

    printf("%ld", bruter_get(context, result).i);
    
    printf("\n");
    return result;
}

void buxu_dl_open(char* libpath)
{
    // check if the library is already loaded
    BruterInt found = bruter_find(libs, bruter_value_p(NULL), libpath);
    if (found != -1)
    {
        BUXU_ERROR("library %s already loaded", libpath);
        return;
    }

    void *handle = dlopen(libpath, RTLD_LAZY | RTLD_GLOBAL);

    if (handle != NULL)
    {
        bruter_push(libs, (BruterValue){.p = handle}, libpath);
    }
    else 
    {
        BUXU_ERROR("%s", dlerror());
        return;
    }

    BruterList *splited = br_str_split(libpath, '/');
    BruterList *splited2 = br_str_split(splited->data[splited->size - 1].s, '.');
    char *_libpath = splited2->data[0].s;

    // now lets get the init_name function
    char* tmp = br_str_format("init_%s", _libpath);
    void (*_init)(BruterList*) = dlsym(handle, tmp);
    free(tmp);
    if (_init != NULL)
    {
        _init(context);
    }
    else 
    {
        BUXU_ERROR("%s", dlerror());
        BUXU_ERROR("init_%s not found", _libpath);
        // then lets close the library
        dlclose(handle);
        bruter_pop(libs);
        return;
    }

    for (BruterInt i = 0; i < splited->size; i++)
    {
        free(splited->data[i].s);
    }

    for (BruterInt i = 0; i < splited2->size; i++)
    {
        free(splited2->data[i].s);
    }

    bruter_free(splited);
    bruter_free(splited2);
}

void buxu_dl_close(char* libpath)
{
    for (BruterInt i = 0; i < libs->size; i++)
    {
        if (strcmp(libpath, libs->keys[i]) == 0)
        {
            dlclose(bruter_get(context, libs->data[i].i).p);
            bruter_fast_remove(libs, i);
            return;
        }
    }
    BUXU_ERROR("library %s is not loaded.", libpath);
}

BR_FUNCTION(brl_main_dl_open)
{
    char* str = br_arg(context, args, 0).s;
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

BR_FUNCTION(brl_main_dl_close)
{
    char* str = br_arg(context, args, 0).s;
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

void _free_at_exit()
{
    br_free_context(context);

    if (libs->size > 0)
    {
        while (libs->size > 0)
        {
            dlclose(bruter_pop(libs).p);
        }
    }
    bruter_free(libs);

    for (BruterInt i = 0; i < args->size; i++)
    {
        free(args->data[i].s);
    }
    bruter_free(args);

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
        BUXU_ERROR("could not register the atexit function");
        return 1;
    }

    // result
    BruterInt result = -2; // -2 because no valid buxu program will ever return -2, this is used to detect if eval was called, if so do not start repl
    BruterList *parser = br_simple_parser(); // parser
    // virtual machine startup
    context = bruter_init(16, true); // starts with capacity of 16 vars, to avoid reallocations, it will grow as needed

    // lib search paths
    char* backup;

    // arguments startup
    args = bruter_init(sizeof(void*), false);

    // those could be done automatically when needed, but would print a warning
    // lets push the unused list to the context
    // we do this manually because br_new_var would automatically create the unused list if it does not exist
    bruter_push(context, bruter_value_p(bruter_init(sizeof(BruterValue), false)), "unused");

    // those could be done automatically when needed, but would print a warning
    // lets push the parser to the context
    BruterInt parser_index = br_new_var(context, bruter_value_p(parser), "parser");
    if (parser_index == -1)
    {
        BUXU_ERROR("failed to create parser variable");
        return 1;
    }
    
    // those could be done automatically when needed, but would print a warning
    // lets push the args to the context
    BruterInt allocs_index = br_new_var(context, bruter_value_p(bruter_init(sizeof(BruterValue), false)), "allocs");
    
    // dynamic library functions
    br_add_function(context, "load", brl_main_dl_open);
    br_add_function(context, "unload", brl_main_dl_close);
    
    // dynamic libraries lists startup
    libs = bruter_init(sizeof(void*), true);

    // arguments parsing
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) // version
        {
            BUXU_PRINT(EMOTICON_DEFAULT, "BRUTER v%s", VERSION);
            BUXU_PRINT(EMOTICON_DEFAULT, "buxu v%s", BUXU_VERSION);
            return 0;
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) // help
        {
            BUXU_PRINT(EMOTICON_DEFAULT, "v%s", VERSION);
            BUXU_PRINT(EMOTICON_DEFAULT, "usage: %s [file]", argv[0]);
            printf("  -v, --version\t\tprint version\n");
            printf("  -h, --help\t\tprint this help\n");
            printf("  -l, \t\t\tload a library\n");
            printf("  -p, --path\t\tadd a path to the library search path\n");
            printf("  -e, --eval\t\teval a string\n");
            return 0;
        }
        else if (argv[i][0] == '-' && argv[i][1] == 'l') // load
        {
            char *libname = br_str_format("load {%s}", argv[i] + 2);
            br_eval(context, parser, libname);
            free(libname);
        }
        else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--eval") == 0) // eval
        {
            result = br_eval(context, parser, argv[i+1]);
            i+=1;// skip to the next argument
        }
        else // push to args
        {
            bruter_push(args, (BruterValue){.s = br_str_duplicate(argv[i])}, NULL);
        }
    }

    if (args->size == 0 && result == -2) // repl, only if no arguments and no eval rant
    {
        repl(context, parser);
    }
    else if (args->size > 0) // run files
    {
        char* ___file = bruter_shift(args).s;
    
        _code = file_read(___file);

        if (_code == NULL)
        {
            BUXU_ERROR("file %s not found", ___file);
            return 1;
        }
    
        result = br_eval(context, parser, _code);
        free(___file);
    }
    return result;
}
