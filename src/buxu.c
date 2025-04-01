#include "buxu.h"

//string functions
char* str_duplicate(const char *str)
{
    char *dup = (char*)malloc(strlen(str) + 1);
    strcpy(dup, str);
    return dup;
}

char* str_nduplicate(const char *str, Int n)
{
    char *dup = (char*)malloc(n + 1);
    for (Int i = 0; i < n; i++)
    {
        dup[i] = str[i];
    }
    dup[n] = '\0';
    return dup;
}

char* str_sub(const char *str, Int start, Int end)
{
    char *sub = (char*)malloc(end - start + 1);
    for (Int i = start; i < end; i++)
    {
        sub[i - start] = str[i];
    }
    sub[end - start] = '\0';
    return sub;
}

char* str_format(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Int size = vsnprintf(NULL, 0, format, args);
    va_end(args);
    char *str = (char*)malloc(size + 1);
    va_start(args, format);
    vsprintf(str, format, args);
    va_end(args);
    return str;
}

char* str_concat(const char *str1, const char *str2)
{
    char *str = (char*)malloc(strlen(str1) + strlen(str2) + 1);
    strcpy(str, str1);
    strcat(str, str2);
    return str;
}

char* str_replace(const char *str, const char *substr, const char *replacement)
{
    const char *pos = strstr(str, substr);
    if (!pos)
    {
        char *newstr = (char *)malloc(strlen(str) + 1);
        strcpy(newstr, str);
        return newstr;
    }

    size_t len_before = pos - str;
    size_t substr_len = strlen(substr);
    size_t replacement_len = strlen(replacement);
    size_t new_len = strlen(str) - substr_len + replacement_len;

    char *newstr = (char *)malloc(new_len + 1);

    strncpy(newstr, str, len_before);
    strcpy(newstr + len_before, replacement);
    strcpy(newstr + len_before + replacement_len, pos + substr_len);

    return newstr;
}


char* str_replace_all(const char *str, const char *substr, const char *replacement)
{
    size_t substr_len = strlen(substr);
    size_t replacement_len = strlen(replacement);

    size_t new_len = 0;
    size_t count = 0;
    for (const char *p = strstr(str, substr); p; p = strstr(p + substr_len, substr))
    {
        count++;
        new_len += (p - str) - new_len;
        new_len += replacement_len;
        str = p + substr_len;
    }
    new_len += strlen(str);

    char *newstr = (char *)malloc(new_len + 1);
    char *dest = newstr;

    str = str - new_len + strlen(newstr);
    const char *p = strstr(str, substr);
    while (p)
    {
        size_t len_before = p - str;
        strncpy(dest, str, len_before);
        dest += len_before;

        strcpy(dest, replacement);
        dest += replacement_len;

        str = p + substr_len;
        p = strstr(str, substr);
    }
    strcpy(dest, str);

    return newstr;
}


Int str_find(const char *str, const char *substr)
{
    return strstr(str, substr) - str;
}

StringList* special_space_split(char *str)
{
    StringList *splited = list_init(StringList);
    
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
                if (str[j] == '(')
                {
                    count++;
                }
                else if (str[j] == ')')
                {
                    count--;
                }
            }
            char *tmp = str_nduplicate(str + i, j - i + 1);
            list_push(*splited, tmp);
            i = j + 1;
        }
        else if (str[i] == '"')
        {
            int j = i;
            j++;
            while (str[j] != '"' && str[j] != '\0')
            {
                j++;
            }
            char *tmp = str_nduplicate(str + i, j - i + 1);
            list_push(*splited, tmp);
            i = j + 1;
        }
        else if (str[i] == '\'')
        {
            int j = i;
            j++;
            while (str[j] != '\'' && str[j] != '\0')
            {
                j++;
            }
            char *tmp = str_nduplicate(str + i, j - i + 1);
            list_push(*splited, tmp);
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
            list_push(*splited, str_nduplicate(str + i, j - i));
            i = j;
        }
    }
    return splited;
}


StringList* special_split(char *str, char delim)
{
    StringList *splited = list_init(StringList);
    
    int recursion = 0;
    bool inside_double_quotes = 0;
    bool inside_single_quotes = 0;
    int i = 0;
    int last_i = 0;

    while (str[i] != '\0')
    {
        if (str[i] == '(' && !inside_double_quotes && !inside_single_quotes)
        {
            recursion++;
        }
        else if (str[i] == ')' && !inside_double_quotes && !inside_single_quotes)
        {
            recursion--;
        }
        else if (str[i] == '"' && !recursion && !inside_single_quotes)
        {
            inside_double_quotes = !inside_double_quotes;
        }
        else if (str[i] == '\'' && !recursion && !inside_double_quotes)
        {
            inside_single_quotes = !inside_single_quotes;
        }

        if (str[i] == delim && !recursion && !inside_double_quotes && !inside_single_quotes)
        {
            char* tmp = str_nduplicate(str + last_i, i - last_i);
            list_push(*splited, tmp);
            last_i = i + 1;
        }
        else if (str[i + 1] == '\0')
        {
            char* tmp = str_nduplicate(str + last_i, i - last_i + 1);
            list_push(*splited, tmp);
        }

        i++;
    }
    return splited;
}

StringList* str_split(char *str, char *delim)
{
    StringList *splited = list_init(StringList);
    
    Int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == delim[0])
        {
            i++;
        }
        else
        {
            Int j = i;
            while (str[j] != delim[0] && str[j] != '\0')
            {
                j++;
            }
            list_push(*splited, str_nduplicate(str + i, j - i));
            i = j;
        }
    }

    return splited;
}


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
            list_push(*splited, str_nduplicate(str + i, j - i));
            i = j;
        }
    }

    return splited;
}

#ifndef ARDUINO

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

#endif

// value functions

Value value_duplicate(Value value, Byte type)
{
    Value dup;

    switch (type)
    {
        case TYPE_STRING:
            dup.string = str_duplicate(value.string);
            break;
        default:
            dup.integer = value.integer;
            break;
    }

    return dup;
}

//hash functions

Int hash_find(VirtualMachine *vm, char *varname)
{
    for (Int i = 0; i < vm->hash_names->size; i++)
    {
        if (!strcmp(vm->hash_names->data[i], varname))
        {
            return i;
        }
    }
    return -1;
}

void hash_set(VirtualMachine *vm, char* varname, Int index)
{
    Int found = hash_find(vm, varname);
    if (found != -1)
    {
        vm->hash_indexes->data[found] = index;
    }
    else 
    {
        list_push(*vm->hash_names, str_duplicate(varname));
        list_push(*vm->hash_indexes, index);
    }
}

void hash_unset(VirtualMachine *vm, char* varname)
{
    Int index = hash_find(vm, varname);
    if (index != -1)
    {
        free(list_fast_remove(*vm->hash_names, index));
        list_fast_remove(*vm->hash_indexes, index);
    }
}

//variable functions

VirtualMachine* make_vm()
{
    VirtualMachine *vm = (VirtualMachine*)malloc(sizeof(VirtualMachine));
    vm->stack = list_init(ValueList);
    vm->typestack = list_init(ByteList);
    vm->hash_names = list_init(StringList);
    vm->hash_indexes = list_init(IntList);
    vm->unused = list_init(IntList);

    // @0 = null
    register_var(vm, "null");

    return vm;
}

// var new 
Int new_var(VirtualMachine *vm)
{ 
    if (vm->unused->size > 0)
    {
        return list_pop(*vm->unused);
    }
    else
    {
        Value value;
        value.pointer = NULL;
        list_push(*vm->stack, value);
        list_push(*vm->typestack, TYPE_ANY);
        return vm->stack->size-1;
    }
}

Int new_number(VirtualMachine *vm, Float number)
{
    Int id = new_var(vm);
    vm->stack->data[id].number = number;
    vm->typestack->data[id] = TYPE_NUMBER;
    return id;
}

Int new_string(VirtualMachine *vm, char *string)
{
    Int id = new_var(vm);
    vm->stack->data[id].string = str_duplicate(string);
    vm->typestack->data[id] = TYPE_STRING;
    return id;
}

Int new_builtin(VirtualMachine *vm, Function function)
{
    Int id = new_var(vm);
    vm->stack->data[id].pointer = function;
    vm->typestack->data[id] = TYPE_ANY;
    return id;
}

Int register_var(VirtualMachine *vm, char* varname)
{
    Int index = new_var(vm);
    hash_set(vm, varname, index);
    return index;
}

Int register_number(VirtualMachine *vm, char* varname, Float number)
{
    Int index = new_number(vm, number);
    hash_set(vm, varname, index);
    return index;
}

Int register_string(VirtualMachine *vm, char* varname, char* string)
{
    Int index = new_string(vm, string);
    hash_set(vm, varname, index);
    return index;
}

Int register_builtin(VirtualMachine *vm, char* varname, Function function)
{
    Int index = new_builtin(vm, function);
    hash_set(vm, varname, index);
    return index;
}

//frees
void free_vm(VirtualMachine *vm)
{
    Value value;
    while (vm->stack->size > 0)
    {
        value = list_pop(*vm->stack);
        switch (list_pop(*vm->typestack))
        {
            case TYPE_STRING:
                free(value.string);
                break;
            default:
                break;
        }
    }

    while (vm->hash_names->size > 0)
    {
        free(list_pop(*vm->hash_names));
    }

    list_free(*vm->unused);

    list_free(*vm->stack);
    list_free(*vm->typestack);

    list_free(*vm->hash_names);
    list_free(*vm->hash_indexes);

    free(vm);
}

// Parser functions
IntList* parse(void *_vm, char *cmd) 
{
    VirtualMachine* vm = (VirtualMachine*)_vm;
    IntList *result = list_init(IntList);
    
    StringList *splited = special_space_split(cmd);
    list_reverse(*splited);

    //Int current = 0;
    while (splited->size > 0)
    {
        char* str = list_pop(*splited);
        
        if (str[0] == '(')
        {
            if(str[1] == '@') //string
            {
                char* temp = str + 2;
                temp[strlen(temp) - 1] = '\0';
                Int var = new_string(vm, temp);
                list_push(*result, var);            
            }
            else
            {
                char* temp = str + 1;
                temp[strlen(temp) - 1] = '\0';
                Int index = eval(vm, temp);
                list_push(*result, index);
            }
        }
        else if (str[0] == '"' || str[0] == '\'') // string
        {
            char* temp = str_nduplicate(str + 1, strlen(str) - 2);
            Int var = new_string(vm, temp);
            list_push(*result, var);
            free(temp);
        }
        else if (isdigit(str[0]) || (str[0] == '-' && isdigit(str[1]))) // number
        {
            Int var = new_number(vm, atof(str));
            list_push(*result, var);
        }
        else //variable 
        {
            int hashindex = -1;
            hashindex = hash_find(vm, str);

            if (hashindex == -1) 
            {
                buxu_error("variable %s not found", str);
            }
            else 
            {
                list_push(*result, vm->hash_indexes->data[hashindex]);
            }
        }

        free(str);
    }
    list_free(*splited);
    return result;
}

Int interpret(VirtualMachine *vm, char* cmd)
{
    IntList *args = parse(vm, cmd);

    if (!args->size)
    {
        list_free(*args);
        return -1;
    }
    
    Int func = list_shift(*args);
    Int result = -1;
    if (func > -1)
    {
        Function _function;

        switch (vm->typestack->data[func])
        {
            case TYPE_ANY:
                _function = vm->stack->data[func].pointer;
                result = _function(vm, args);
                list_unshift(*args, func);
                break;

                
            case TYPE_STRING:
                // eval
                result = eval(vm, data(func).string);
                break;
        }
    }
    else 
    {
        buxu_error("%ld is not a function or script", func);
    }
    
    list_free(*args);
    
    return result;
}

Int eval(VirtualMachine *vm, char *cmd)
{
    if(!strchr(cmd, ';')) // if == NULL
    {
        return interpret(vm, cmd);
    }

    StringList *splited = special_split(cmd, ';');

    // remove empty or whitespace only strings using isspace
    Int last = splited->size - 1;
    while (last >= 0)
    {
        if (!strlen(splited->data[last]))
        {
            free(list_pop(*splited));
        }
        else
        {
            Int i = 0;
            while (splited->data[last][i] != '\0' && isspace(splited->data[last][i]))
            {
                i++;
            }

            if (splited->data[last][i] == '\0')
            {
                free(splited->data[last]);
                list_pop(*splited);
            }
        }
        last--;
    }

    list_reverse(*splited);
    Int result = -1;
    while (splited->size > 0)
    {
        
        char *str = list_pop(*splited);
        if (!strlen(str))
        {
            free(str);
            continue;
        }
        result = interpret(vm, str);
        free(str);
        if (result > 0)
        {
            while (splited->size > 0)
            {
                free(list_pop(*splited));
            }
            break;
        }
    }
    list_free(*splited);
    return result;
}

void unuse_var(VirtualMachine *vm, Int index)
{
    switch (data_t(index))
    {
        case TYPE_STRING:
            free(data(index).string);
            break;
        default:
            break;
    }
    data(index).integer = 0;
    data_t(index) = TYPE_ANY;
    list_push(*vm->unused, index);
}

