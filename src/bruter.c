#include "bruter.h"

// bools

char is_true(Value value, char __type)
{
    if (__type == TYPE_NUMBER)
    {
        return value.number != 0;
    }
    else if (__type == TYPE_STRING)
    {
        return 1;
    }
    else if (__type == TYPE_LIST)
    {
        return 1;
    }
    else if (__type == TYPE_ERROR)
    {
        return 0;
    }
    else if (__type == TYPE_NIL)
    {
        return 0;
    }
    else if (__type == TYPE_UNUSED)
    {
        return 0;
    }
    else if (__type == TYPE_FUNCTION)
    {
        return 1;
    }
    else if (__type == TYPE_POINTER)
    {
        return value.number != -1;
    }
    return 0;
}

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
    Int len = strlen(str);
    Int sublen = strlen(substr);
    Int replen = strlen(replacement);
    Int count = 0;
    for (Int i = 0; i < len; i++)
    {
        if (str[i] == substr[0])
        {
            Int j = 0;
            while (j < sublen && str[i + j] == substr[j])
            {
                j++;
            }
            if (j == sublen)
            {
                count++;
            }
        }
    }
    char *newstr = (char*)malloc(len + count * (replen - sublen) + 1);
    Int i = 0;
    Int k = 0;
    while (str[i] != '\0')
    {
        if (str[i] == substr[0])
        {
            Int j = 0;
            while (j < sublen && str[i + j] == substr[j])
            {
                j++;
            }
            if (j == sublen)
            {
                for (Int l = 0; l < replen; l++)
                {
                    newstr[k] = replacement[l];
                    k++;
                }
                i += sublen;
            }
            else
            {
                newstr[k] = str[i];
                k++;
                i++;
            }
        }
        else
        {
            newstr[k] = str[i];
            k++;
            i++;
        }
    }
    newstr[k] = '\0';
    return newstr;
}

Int str_find(const char *str, const char *substr)
{
    Int len = strlen(str);
    Int sublen = strlen(substr);
    for (Int i = 0; i < len; i++)
    {
        if (str[i] == substr[0])
        {
            Int j = 0;
            while (j < sublen && str[i + j] == substr[j])
            {
                j++;
            }
            if (j == sublen)
            {
                return i;
            }
        }
    }
    return -1;
}

StringList* special_space_split(char *str)
{
    StringList *splited;
    splited = (StringList*)malloc(sizeof(StringList));
    stack_init(*splited);
    
    Int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '(')
        {
            Int j = i;
            Int count = 1;
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
            stack_push(*splited, tmp);
            i = j + 1;
        }
        else if ((str[i] == '!' && str[i + 1] == '(') ||
                 (str[i] == '@' && str[i + 1] == '(') ||
                 (str[i] == '$' && str[i + 1] == '('))
        {
            Int j = i + 1;
            Int count = 1;
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
            stack_push(*splited, tmp);
            i = j + 1;
        }
        else if (str[i] == '$' && str[i + 1] == '!' && strlen(str) > i + 2 && str[i + 2] == '(')
        {
            Int j = i + 2;
            Int count = 1;
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
            stack_push(*splited, tmp);
            i = j + 1;
        }
        else if (is_space(str[i]))
        {
            i++;
        }
        else
        {
            Int j = i;
            while (!is_space(str[j]) && str[j] != '\0' && str[j] != '(' && str[j] != ')')
            {
                j++;
            }
            stack_push(*splited, str_nduplicate(str + i, j - i));
            i = j;
        }
    }
    return splited;
}

StringList* special_split(char *str, char delim)// regular split but doesnt split if the delimiter is inside parenthesis
{
    StringList *splited;
    splited = (StringList*)malloc(sizeof(StringList));
    stack_init(*splited);
    Int recursion = 0;
    Int i = 0;
    Int last_i = 0;

    while (str[i] != '\0')
    {
        if (str[i] == '(')
        {
            recursion++;
        }
        else if (str[i] == ')')
        {
            recursion--;
        }

        if (str[i] == delim && recursion == 0)
        {
            char* tmp = str_nduplicate(str + last_i, i - last_i);
            stack_push(*splited, tmp);
            last_i = i + 1;
        }
        else if (str[i+1] == '\0')
        {
            char* tmp = str_nduplicate(str + last_i, i - last_i + 1);
            stack_push(*splited, tmp);
        }

        i++;
    }
    return splited;
}

StringList* splitString(char *str, char *delim)
{
    StringList *splited;
    splited = (StringList*)malloc(sizeof(StringList));
    stack_init(*splited);
    
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
            stack_push(*splited, str_nduplicate(str + i, j - i));
            i = j;
        }
    }

    return splited;
}

// value functions

Value value_duplicate(Value value, char type)
{
    Value dup;
    if (type == TYPE_NUMBER)
    {
        dup.number = value.number;
    }
    else if (type == TYPE_STRING || type == TYPE_ERROR)
    {
        dup.string = str_duplicate(value.string);
    }
    else if (type == TYPE_LIST)
    {
        dup.pointer = make_int_list();
        IntList *list = (IntList*)value.pointer;
        for (Int i = 0; i < list->size; i++)
        {
            stack_push(*((IntList*)dup.pointer), list->data[i]);
        }
    }
    else if (type == TYPE_FUNCTION)
    {
        dup.pointer = value.pointer;
    }
    else if (type == TYPE_POINTER)
    {
        dup.number = value.number;
    }
    return dup;
}

//hash functions

Int hash_find(VirtualMachine *vm, char *varname)
{
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (strcmp(vm->hashes->data[i].key, varname) == 0)
        {
            return vm->hashes->data[i].index;
        }
    }
    return -1;
}

void hash_set(VirtualMachine *vm, char* varname, Int index)
{
    if (hash_find(vm, varname) != -1)
    {
        vm->hashes->data[hash_find(vm, varname)].index = index;
    }
    else 
    {
        Hash hash;
        hash.key = str_duplicate(varname);
        hash.index = index;
        stack_push(*vm->hashes, hash);
    }
}

void hash_unset(VirtualMachine *vm, char* varname)
{
    Int index = hash_find(vm, varname);
    if (index != -1)
    {
        free(vm->hashes->data[index].key);
        vm->hashes->data[index].key = NULL;
        stack_remove(*vm->hashes, index);
    }
}

//variable functions

ValueList* make_value_list()
{
    ValueList *list = (ValueList*)malloc(sizeof(ValueList));
    stack_init(*list);
    return list;
}

IntList* make_int_list()
{
    IntList *list = (IntList*)malloc(sizeof(IntList));
    stack_init(*list);
    return list;
}

CharList* make_char_list()
{
    CharList *list = (CharList*)malloc(sizeof(CharList));
    stack_init(*list);
    return list;
}

VirtualMachine* make_vm()
{
    VirtualMachine *vm = (VirtualMachine*)malloc(sizeof(VirtualMachine));
    vm->stack = make_value_list();
    vm->typestack = make_char_list();
    vm->hashes = (HashList*)malloc(sizeof(HashList));
    stack_init(*vm->hashes);
    vm->empty = make_int_list();
    vm->temp = make_int_list();


    return vm;
}

// var new 

Int new_var(VirtualMachine *vm)
{
    if (vm->empty->size > 0)
    {
        Int id = stack_shift(*vm->empty);
        return id;
    }
    else
    {
        stack_push(*vm->stack, (Value){0});
        stack_push(*vm->typestack, TYPE_NIL);
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

Int new_function(VirtualMachine *vm, Function function)
{
    Int id = new_var(vm);
    vm->stack->data[id].pointer = function;
    vm->typestack->data[id] = TYPE_FUNCTION;
    return id;
}

Int new_error(VirtualMachine *vm, char *error)
{
    Int id = new_var(vm);
    vm->stack->data[id].string = str_duplicate(error);
    vm->typestack->data[id] = TYPE_ERROR;
    return id;
}

Int new_pointer(VirtualMachine *vm, Int index)
{
    Int id = new_var(vm);
    vm->stack->data[id].number = index;
    vm->typestack->data[id] = TYPE_POINTER;
    return id;
}

Int new_list(VirtualMachine *vm)
{
    Int id = new_var(vm);
    vm->stack->data[id].pointer = make_int_list();
    vm->typestack->data[id] = TYPE_LIST;
    return id;
}

// var spawn

void spawn_var(VirtualMachine *vm, char* varname)
{
    Int index = new_var(vm);
    hash_set(vm, varname, index);
}

void spawn_number(VirtualMachine *vm, char* varname, Float number)
{
    Int index = new_number(vm, number);
    hash_set(vm, varname, index);
}

void spawn_string(VirtualMachine *vm, char* varname, char* string)
{
    Int index = new_string(vm, string);
    hash_set(vm, varname, index);
}

void spawn_function(VirtualMachine *vm, char* varname, Function function)
{
    Int index = new_function(vm, function);
    hash_set(vm, varname, index);
}

void spawn_error(VirtualMachine *vm, char* varname, char* error)
{
    Int index = new_error(vm, error);
    hash_set(vm, varname, index);
}

void spawn_pointer(VirtualMachine *vm, char* varname, Int index)
{
    Int id = new_pointer(vm, index);
    hash_set(vm, varname, id);
}

void spawn_list(VirtualMachine *vm, char* varname)
{
    Int index = new_list(vm);
    hash_set(vm, varname, index);
}

//frees

void free_var(VirtualMachine *vm, Int index)
{
    if (vm->typestack->data[index] == TYPE_STRING || vm->typestack->data[index] == TYPE_ERROR)
    {
        free(vm->stack->data[index].string);
    }
    else if (vm->typestack->data[index] == TYPE_LIST)
    {
        while (((IntList*)vm->stack->data[index].pointer)->size > 0)
        {
            stack_shift(*((IntList*)vm->stack->data[index].pointer));
        }
        stack_free(*((IntList*)vm->stack->data[index].pointer));
    }
    stack_remove(*vm->stack, index);
    stack_remove(*vm->typestack, index);
}

void free_vm(VirtualMachine *vm)
{
    while (vm->stack->size > 0)
    {
        free_var(vm, 0);
    }

    while (vm->hashes->size > 0)
    {
        Hash hash = stack_shift(*vm->hashes);
        free(hash.key);
    }

    while (vm->empty->size > 0)
    {

        stack_shift(*vm->empty);
    }

    while (vm->temp->size > 0)
    {
        stack_shift(*vm->temp);
    }

    stack_free(*vm->stack);
    stack_free(*vm->typestack);

    stack_free(*vm->hashes);
    stack_free(*vm->empty);
    stack_free(*vm->temp);

    free(vm);
}

void unuse_var(VirtualMachine *vm, Int index)
{
    //if type is string free the string, if type is list free the list
    if (vm->typestack->data[index] == TYPE_STRING || vm->typestack->data[index] == TYPE_ERROR)
    {
        free(vm->stack->data[index].string);
    }
    else if (vm->typestack->data[index] == TYPE_LIST)
    {
        while (((IntList*)vm->stack->data[index].pointer)->size > 0)
        {
            stack_shift(*((IntList*)vm->stack->data[index].pointer));
        }
        stack_free(*((IntList*)vm->stack->data[index].pointer));
    }
    vm->typestack->data[index] = TYPE_UNUSED;
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (vm->hashes->data[i].index == index)
        {
            free(vm->hashes->data[i].key);
            stack_remove(*vm->hashes, i);
            //break;
        }
    }
    stack_push(*vm->empty, index);
}


// Parser functions
IntList* parse(VirtualMachine *vm, char *cmd) 
{
    IntList *result = make_int_list();
    StringList *splited = special_space_split(cmd);
    //Int current = 0;
    while (splited->size > 0)
    {
        char* str = stack_shift(*splited);
        if (str[0] == '(')
        {
            char* temp = str_nduplicate(str + 1, strlen(str) - 2);
            Int index = eval(vm, temp);
            stack_push(*result, index);
            free(temp);
        }
        else if (str[0] == '!') // literal string
        {
            if (str[1] == '(')
            {
                char* temp = str_nduplicate(str + 2, strlen(str) - 3);
                Int var = new_string(vm, temp);
                free(temp);
                stack_push(*result, var);
            }
            else
            {
                char* temp = str_duplicate(str + 1);
                Int var = new_string(vm, temp);
                free(temp);
                stack_push(*result, var);
            }
        }
        else if (str[0] == '@') 
        {
            if (strlen(str) == 1) 
            {
                stack_push(*result, new_error(vm, "Variable not found"));
            }
            else if(str[1] >= '0' && str[1] <= '9') 
            {
                Int index = new_pointer(vm, atoi(str + 1));
                stack_push(*result, index);
            }
            else
            {
                Int index = hash_find(vm, str + 1);
                if (index == -1) 
                {
                    char* error = str_format("Variable %s not found", str + 1);
                    stack_push(*result, new_error(vm, error));
                    free(error);
                }
                else 
                {
                    Int var = new_pointer(vm, index);
                    stack_push(*result, var);
                }
            }
        }
        else if (str[0] == '$') 
        {
            Int index = -1;
            if (str[1] == '(') 
            {
                char* temp = str_nduplicate(str + 2, strlen(str) - 3);
                index = eval(vm, temp);
                stack_push(*result, index);
                free(temp);
            }
            else if (str[1] == '!') 
            {
                if (str[2] == '(') 
                {
                    char* temp = str_nduplicate(str + 3, strlen(str) - 4);
                    index = new_string(vm, temp);
                    free(temp);
                    stack_push(*result, index);
                }
                else 
                {
                    char* temp = str_duplicate(str + 2);
                    index = new_string(vm, temp);
                    free(temp);
                    stack_push(*result, index);
                }
            }
            else if (str[1] == '@') 
            {
                if (strlen(str) == 2) 
                {
                    index = new_error(vm, "Variable not found");
                    stack_push(*result, index);
                }
                else if (str[2] >= '0' && str[2] <= '9') 
                {
                    index = new_pointer(vm, atoi(str + 2));
                    stack_push(*result, index);
                }
                else 
                {
                    Int _index = hash_find(vm, str + 2);
                    if (_index == -1) 
                    {
                        char* error = str_format("Variable %s not found", str + 2);
                        index = new_error(vm, error);
                        stack_push(*result, index);
                        free(error);
                    }
                    else 
                    {
                        index = new_pointer(vm, _index);
                        stack_push(*result, index);
                    }
                }
            }
            else if ((str[1] >= '0' && str[1] <= '9') || str[1] == '-') 
            {
                index = new_number(vm, atof(str + 1));
                stack_push(*result, index);
            }
            else 
            {
                index = hash_find(vm, str + 1);
                if (index == -1) 
                {
                    char* error = str_format("Variable %s not found", str + 1);
                    stack_push(*result, new_error(vm, error));
                    free(error);
                }
                else 
                {
                    stack_push(*result, index);
                }
            }
            stack_push(*vm->temp, index);
        }
        else if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-') 
        {
            Int var = new_number(vm, atof(str));
            stack_push(*result, var);
        }
        else //string 
        {
            Int index = hash_find(vm, str);
            if (index == -1) 
            {
                char* error = str_format("Variable %s not found", str);
                stack_push(*result, new_error(vm, error));
                free(error);
            }
            else 
            {
                stack_push(*result, index);
            }
        }
        free(str);
        /*char* hashname = str_format("arg%d", current);
        hash_set(vm, hashname, result->data[current]);
        free(hashname);
        current++;*/
    }
    stack_free(*splited);
    return result;
}

Int interpret(VirtualMachine *vm, char* cmd)
{
    IntList *parsed = parse(vm, cmd);
    Int func = stack_shift(*parsed);
    Int result = -1;
      

    if (vm->typestack->data[func] == TYPE_NUMBER)
    {
        if (vm->typestack->data[(Int)vm->stack->data[func].number] == TYPE_FUNCTION)
        {
            Function function = vm->stack->data[(Int)vm->stack->data[func].number].pointer;
            result = function(vm, parsed);
        }
        else // make a list
        {
            stack_insert(*parsed, 0, func);
            result = std_list_new(vm, parsed);
        }
    }
    else if (vm->typestack->data[func] == TYPE_FUNCTION)
    {
        Function function = vm->stack->data[func].pointer;
        result = function(vm, parsed);
    }
    else 
    {
        stack_insert(*parsed, 0, func);
        result = std_list_new(vm, parsed);
    }

    while (parsed->size > 0)
    {
        Int index = stack_shift(*parsed);
        unuse_var(vm, index);
    }


    stack_free(*parsed);
    return result;
}

Int eval(VirtualMachine *vm, char *cmd)
{
    StringList *splited = special_split(cmd, ';');
    Int result = -1;
    while (splited->size > 0)
    {
        char *str = stack_shift(*splited);
        if (strlen(str) == 0)
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
                free(stack_shift(*splited));
            }
            break;
        }
    }
    stack_free(*splited);
    return result;
}

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

Int std_file_read(VirtualMachine *vm, IntList *args)
{
    Int filename = stack_shift(*args);
    char *code = readfile(vm->stack->data[filename].string);
    Int result = -1;
    if (code == NULL)
    {
        result = new_error(vm, "File not found");
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

Int std_exit(VirtualMachine *vm, IntList *args)
{
    return 0;
}

Int std_repl(VirtualMachine *vm, IntList *args)
{
    printf("bruter v%s\n", VERSION);
    if (hash_find(vm, "exit") == -1)
    {
        spawn_function(vm, "exit", std_exit);
    }
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
    //printf("repl exited with code %d;\n", result);
    printf("repl returned: @%d ", result);
    char * str = str_format("print @%d", result);
    eval(vm, str);
    free(str);
    return result;
}

int main(int argv, char **argc)
{
    VirtualMachine *vm = make_vm();

    init_all(vm);

    spawn_function(vm, "file.read", std_file_read);
    spawn_function(vm, "file.write", std_file_write);

    spawn_function(vm, "repl", std_repl);


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
            case TYPE_ERROR:
                printf("{error}");
                break;
            case TYPE_FUNCTION:
                printf("{function}");
                break;
            case TYPE_POINTER:
                printf("{pointer}");
                break;
            default:
                printf("{unknown}");
                break;
        }
        printf(": ");
        char * str = str_format("print @%d", result);
        eval(vm, str);
        free(str);
    }
    free_vm(vm);
}
#endif