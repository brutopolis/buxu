
// this is the bwin's interpreter frontend
// there are some tweaks that try be compatible with mac and windows, totally untested;

// bwin header
#include "bruter-representation.h" 

#define BWIN_VERSION "0.0.1"

#define BWIN_EMOTICON "[='-'=]"

static BruterList* args = NULL;
static BruterList* context = NULL; // global context
static char *bwin_run_code = NULL;

// file stuff
static char* file_read(char *filename)
{
    long length = 0;
    char *buffer = NULL;
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("%s: could not open file %s\n", BWIN_EMOTICON, filename);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);
    buffer = (char*)calloc(1, (size_t)length + 1);
    if (buffer == NULL)
    {
        printf("%s: could not allocate memory for file %s\n", BWIN_EMOTICON, filename);
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
    printf("bwin v%s\n", BWIN_VERSION);
    while (result == -1)
    {
        printf("%s: ", BWIN_EMOTICON);
        junk = scanf("%[^\n]%*c", cmd);
        if (junk == 0)
        {
            break;
        }
        result = br_eval(received_context, parser, cmd);
    }

    printf("%s: ", BWIN_EMOTICON);

    printf("%ld", bruter_get_int(received_context, result));
    
    printf("\n");
    return result;
}

static void free_at_exit(void)
{

    for (BruterInt i = 0; i < args->size; i++)
    {
        free(args->data[i].p);
    }

    if (bwin_run_code != NULL)
    {
        free(bwin_run_code);
    }

    br_free_context(context);
}

BR_INIT(std);
BR_INIT(io);
BR_INIT(list);
BR_INIT(math);
BR_INIT(condition);
BR_INIT(type);

BR_FUNCTION(fallback_load)
{
	printf("load: bwin doesnt support loading dll's yet, you must manually include the libs.\n");
	return -1;
}

int main(int argc, char **argv)
{
    // result
    BruterInt result = -2; // -2 because no valid bwin program will ever return -2, this is used to detect if eval was called, if so do not start repl
    BruterList *parser = NULL;

    // free all at exit
    if (atexit(free_at_exit) != 0)
    {
        printf("%s: could not register the atexit function\n", BWIN_EMOTICON);
        return 1;
    }

    // virtual machine startup
    context = br_new_context(16); // global context
    
	init_std(context);
	init_io(context);
	init_list(context);
	init_math(context);
	init_condition(context);
	init_type(context);
	
	// must standartize br_add_function 
	br_add_function(context, "load", fallback_load);
	
    parser = br_get_parser(context); // get the parser from the context

    // arguments startup
    args = bruter_new(sizeof(void*), false, false);

    // arguments parsing
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) // version
        {
            printf("bwin v%s\n", BWIN_VERSION);
            printf("bruter-representation v%s\n", BR_VERSION);
            printf("bruter v%s\n", BRUTER_VERSION);
            return 0;
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) // help
        {
            printf("bwin v%s\n", BWIN_VERSION);
            printf("%s: usage: %s [file]", BWIN_EMOTICON, argv[0]);
            printf("  -v, --version\t\tprint version\n");
            printf("  -h, --help\t\tprint this help\n");
            printf("  -e, --eval\t\teval a string\n");
            return 0;
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

    if (args->size == 0 && result == -2) // repl, only if no arguments and no eval rant
    {
        repl(context, parser);
    }
    else if (args->size > 0) // run files
    {
        char* file_pointer = (char*)bruter_shift_pointer(args);
    
        bwin_run_code = file_read(file_pointer);

        if (bwin_run_code == NULL)
        {
            printf("%s: could not read file %s\n", BWIN_EMOTICON, file_pointer);
            return 1;
        }
        
        br_new_var(context, (BruterValue){.p = args}, "args", BR_TYPE_LIST);
    
        result = br_eval(context, parser, bwin_run_code);
        free(file_pointer);
    }
    return (int)result;
}
