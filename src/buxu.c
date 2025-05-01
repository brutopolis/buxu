#include "buxu.h"

// string stuff
char* str_duplicate(const char *str)
{
    Int len = strlen(str);
    char *dup = (char*)malloc(len + 1);
    
    if (dup == NULL)
    {
        printf("BRUTER_ERROR: failed to allocate memory for string duplication\n");
        exit(EXIT_FAILURE);
    }
    
    memcpy(dup, str, len + 1);
    return dup;
}

char* str_nduplicate(const char *str, UInt n)
{
    char *dup = (char*)malloc(n + 1);
    
    if (dup == NULL)
    {
        printf("BRUTER_ERROR: failed to allocate memory for string duplication\n");
        exit(EXIT_FAILURE);
    }
    
    memcpy(dup, str, n);
    dup[n] = '\0';
    return dup;
}

// file stuff
char* readfile(char *filename)
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

char* str_format(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Int size = vsnprintf(NULL, 0, format, args);
    va_end(args);
    char *str = (char*)malloc(size + 1);
    if (str == NULL)
    {
        printf("BRUTER_ERROR: failed to allocate memory for formatted string\n");
        exit(EXIT_FAILURE);
    }
    va_start(args, format);
    vsnprintf(str, size + 1, format, args);
    va_end(args);
    return str;
}

List* special_space_split(char *str)
{
    List *splited = list_init(sizeof(void*), false);
    
    int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '(')
        {
            int j = i;
            int count = 1;
            while (count != 0)
            {
                j++;
                if (str[j] == '(' && (j == 0 || str[j-1] != '\\'))  // não conta se for escapado
                {
                    count++;
                }
                else if (str[j] == ')' && (j == 0 || str[j-1] != '\\'))  // não conta se for escapado
                {
                    count--;
                }
            }
            char *tmp = str_nduplicate(str + i, j - i + 1);
            list_push(splited, (Value){.s = tmp}, NULL);
            i = j + 1;
        }
        else if (str[i] == '{')
        {
            int j = i;
            int count = 1;
            while (count != 0)
            {
                j++;
                if (str[j] == '{' && (j == 0 || str[j-1] != '\\'))  // não conta se for escapado
                {
                    count++;
                }
                else if (str[j] == '}' && (j == 0 || str[j-1] != '\\'))  // não conta se for escapado
                {
                    count--;
                }
            }
            char *tmp = str_nduplicate(str + i, j - i + 1);
            list_push(splited, (Value){.s = tmp}, NULL);
            i = j + 1;
        }
        else if (isspace(str[i]))
        {
            i++;
        }
        else
        {
            int j = i;
            while (!isspace(str[j]) && str[j] != '\0' && str[j] != '(' && str[j] != ')')
            {
                j++;
            }
            list_push(splited, (Value) {.s = str_nduplicate(str + i, j - i)}, NULL);
            i = j;
        }
    }
    return splited;
}


List* special_split(char *str, char delim)
{
    List *splited = list_init(sizeof(void*), NULL);
    
    int recursion = 0;
    int curly = 0;
    
    int i = 0;
    int last_i = 0;

    while (str[i] != '\0')
    {
        if (str[i] == '(' && (i == 0 || str[i-1] != '\\') && !curly)
        {
            recursion++;
        }
        else if (str[i] == ')' && (i == 0 || str[i-1] != '\\') && !curly)
        {
            recursion--;
        }
        else if (str[i] == '{' && (i == 0 || str[i-1] != '\\') && !recursion)
        {
            curly++;
        }
        else if (str[i] == '}' && (i == 0 || str[i-1] != '\\') && !recursion)
        {
            curly--;
        }

        if (str[i] == delim && !recursion && !curly)
        {
            char* tmp = str_nduplicate(str + last_i, i - last_i);
            list_push(splited, (Value){.s = tmp}, NULL);
            last_i = i + 1;
        }
        else if (str[i + 1] == '\0')
        {
            char* tmp = str_nduplicate(str + last_i, i - last_i + 1);
            list_push(splited, (Value){.s = tmp}, NULL);
        }

        i++;
    }
    return splited;
}


// var new 
Int new_var(List *context, char* key)
{
    Value value;
    value.p = NULL;
    list_push(context, value, key);
    return context->size-1;
}

Int new_block(List *context, Int size, char* key)
{
    if (!size)
    {
        return -1;
    }

    list_push(context, (Value){.i = 0}, key);

    Int index = context->size-1;
    
    for (Int i = 0; i < size-1; i++)
    {
        list_push(context, (Value){.i = 0}, NULL);
    }
    
    return index;
}

Int new_string(List *context, char* str, char* key)
{
    Int len = strlen(str);
    Int blocks = (len + 1 + sizeof(void*) - 1) / sizeof(void*);
    Int var = new_block(context, blocks, key);
    memcpy(&context->data[var].u8[0], str, len);
    context->data[var].u8[len] = '\0';
    return var;
}

Value parse_number(char *str)
{
    Value result;
    if (str[0] == '0' && str[1] == 'x') // hex
    {
        result.i = strtol(str+2, NULL, 16);
    }
    else if (str[0] == '0' && str[1] == 'b') // bin
    {
        result.i = strtol(str+2, NULL, 2);
    }
    else if (str[0] == '0' && str[1] == 'o') // oct
    {
        result.i = strtol(str+2, NULL, 8);
    }
    else if (strchr(str, '.')) // float
    {
        result.f = atof(str);
    }
    else // int
    {
        result.i = atol(str);
    }
    return result;
}

// Parser functions
List* parse(void *_context, char *cmd) 
{
    List* context = (List*)_context;
    List *result = list_init(sizeof(void*), NULL);
    
    List *splited = special_space_split(cmd);
    char* str = NULL;
    Int i = 0;

    // remove empty or whitespace-only strings using isspace
    Int last = splited->size - 1;
    

    for (i = 0; i < splited->size; i++)
    {
        str = splited->data[i].s;        
        if (str[0] == '(')
        {
            char* temp = str + 1;
            temp[strlen(temp) - 1] = '\0';
            Int res = eval(context, temp);
            list_push(result, (Value){.i = res}, NULL);
        }
        else if (str[0] == '{') // string
        {
            Int len = strlen(str);
            str[len - 1] = '\0';

            list_push(result, (Value){.i = new_string(context, str + 1, NULL)}, NULL);
        }
        else if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-') // number
        {
            Int index = new_var(context, NULL);
            context->data[index] = parse_number(str);
            list_push(result, (Value){.i = index}, NULL);
        }
        else
        {
            Int index = list_find(context, (Value){.p = NULL}, str);
            
            if (index != -1)
            {
                list_push(result, (Value){.i = index}, NULL);
            }
            else 
            {
                buxu_error("variable %s not found", str);
                list_push(result, (Value){.i = -1}, NULL);
            }
        }

        free(str);
    }

    list_free(splited);
    return result;
}

Int eval(List *context, char *cmd)
{
    List *splited = special_split(cmd, ';');

    // remove empty or whitespace-only strings using isspace
    Int last = splited->size - 1;
    for (; last >= 0; last--)
    {
        if (strlen(splited->data[last].s) == 0)
        {
            free(list_pop(splited).p);
        }
        else
        {
            Int i = 0;
            while (splited->data[last].s[i] != '\0' && isspace(splited->data[last].s[i]))
            {
                i++;
            }

            if (splited->data[last].s[i] == '\0')
            {
                free(splited->data[last].s);
                list_pop(splited);
            }
        }
        last--;
    }

    Int result = -1;
    char* str = NULL;
    for (Int i = 0; i < splited->size; i++)
    {        
        str = splited->data[i].s;
        List *args = parse(context, str);

        args->data[0].p = context->data[args->data[0].i].p;
        result = list_call(context, args);
        free(str);
        list_free(args);

        if (result > 0)
        {
            for (Int j = i + 1; j < splited->size; j++)
            {
                free(splited->data[j].s);
            }
            break;
        }
        
        
    }
    list_free(splited);
    return result;
}