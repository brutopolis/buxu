#include "bruter.h"

// interpreter declaration
Int default_interpreter(void *_vm, char *cmd, HashList *context);

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
    const char *pos = strstr(str, substr); // Localiza a primeira ocorrência de 'substr'
    if (!pos)
    {
        // Se não encontrou, retorna uma cópia da string original
        char *newstr = (char *)malloc(strlen(str) + 1);
        strcpy(newstr, str);
        return newstr;
    }

    // Calcula os tamanhos
    size_t len_before = pos - str;
    size_t substr_len = strlen(substr);
    size_t replacement_len = strlen(replacement);
    size_t new_len = strlen(str) - substr_len + replacement_len;

    // Aloca memória para a nova string
    char *newstr = (char *)malloc(new_len + 1);

    // Constrói a nova string
    strncpy(newstr, str, len_before); // Parte antes da substring
    strcpy(newstr + len_before, replacement); // Substituição
    strcpy(newstr + len_before + replacement_len, pos + substr_len); // Parte após a substring

    return newstr;
}


char* str_replace_all(const char *str, const char *substr, const char *replacement)
{
    size_t substr_len = strlen(substr);
    size_t replacement_len = strlen(replacement);

    // Contar substrings e calcular tamanho necessário em uma única passagem
    size_t new_len = 0;
    size_t count = 0;
    for (const char *p = strstr(str, substr); p; p = strstr(p + substr_len, substr))
    {
        count++;
        new_len += (p - str) - new_len; // Adiciona a parte antes da substring
        new_len += replacement_len;    // Adiciona o tamanho da substituição
        str = p + substr_len;          // Avança para o próximo trecho
    }
    new_len += strlen(str); // Adiciona o restante da string

    // Aloca memória para a nova string
    char *newstr = (char *)malloc(new_len + 1);
    char *dest = newstr;

    // Construir a nova string
    str = str - new_len + strlen(newstr); // Reiniciar ponteiro para o início da string original
    const char *p = strstr(str, substr);
    while (p)
    {
        size_t len_before = p - str;
        strncpy(dest, str, len_before);    // Copiar parte antes da substring
        dest += len_before;

        strcpy(dest, replacement);        // Copiar substituição
        dest += replacement_len;

        str = p + substr_len;             // Avançar na string original
        p = strstr(str, substr);
    }
    strcpy(dest, str);                    // Copiar o restante da string original

    return newstr;
}


Int str_find(const char *str, const char *substr)
{
    return strstr(str, substr) - str;
}

StringList* special_space_split(char *str)
{
    StringList *splited;
    splited = (StringList*)malloc(sizeof(StringList));
    stack_init(*splited);
    
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
            stack_push(*splited, tmp);
            i = j + 1;
        }
        else if (str[i] == '"')
        {
            int j = i;
            j++;  // Avança para depois da abertura de aspas duplas
            while (str[j] != '"' && str[j] != '\0')
            {
                j++;
            }
            char *tmp = str_nduplicate(str + i, j - i + 1);
            stack_push(*splited, tmp);
            i = j + 1;  // Avança para após o fechamento de aspas duplas
        }
        else if (str[i] == '\'')
        {
            int j = i;
            j++;  // Avança para depois da abertura de aspas simples
            while (str[j] != '\'' && str[j] != '\0')
            {
                j++;
            }
            char *tmp = str_nduplicate(str + i, j - i + 1);
            stack_push(*splited, tmp);
            i = j + 1;  // Avança para após o fechamento de aspas simples
        }
        else if (is_space(str[i]))
        {
            i++;
        }
        else
        {
            int j = i;
            while (!is_space(str[j]) && str[j] != '\0' && str[j] != '(' && str[j] != ')' && str[j] != '"' && str[j] != '\'')
            {
                j++;
            }
            stack_push(*splited, str_nduplicate(str + i, j - i));
            i = j;
        }
    }
    return splited;
}


StringList* special_split(char *str, char delim)
{
    StringList *splited;
    splited = (StringList*)malloc(sizeof(StringList));
    stack_init(*splited);
    
    int recursion = 0;
    int inside_double_quotes = 0;
    int inside_single_quotes = 0;
    int i = 0;
    int last_i = 0;

    while (str[i] != '\0')
    {
        if (str[i] == '(' && inside_double_quotes == 0 && inside_single_quotes == 0)
        {
            recursion++;
        }
        else if (str[i] == ')' && inside_double_quotes == 0 && inside_single_quotes == 0)
        {
            recursion--;
        }
        else if (str[i] == '"' && recursion == 0 && inside_single_quotes == 0)
        {
            // Alterna o estado de dentro/fora de aspas duplas
            inside_double_quotes = !inside_double_quotes;
        }
        else if (str[i] == '\'' && recursion == 0 && inside_double_quotes == 0)
        {
            // Alterna o estado de dentro/fora de aspas simples
            inside_single_quotes = !inside_single_quotes;
        }

        // Se encontramos o delimitador e não estamos dentro de parênteses nem de aspas simples ou duplas
        if (str[i] == delim && recursion == 0 && inside_double_quotes == 0 && inside_single_quotes == 0)
        {
            char* tmp = str_nduplicate(str + last_i, i - last_i);
            stack_push(*splited, tmp);
            last_i = i + 1;
        }
        else if (str[i + 1] == '\0') // Checagem para o último token
        {
            char* tmp = str_nduplicate(str + last_i, i - last_i + 1);
            stack_push(*splited, tmp);
        }

        i++;
    }
    return splited;
}

StringList* str_split(char *str, char *delim)
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


StringList* str_split_char(char *str, char delim)
{
    StringList *splited;
    splited = (StringList*)malloc(sizeof(StringList));
    stack_init(*splited);

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
            stack_push(*splited, str_nduplicate(str + i, j - i));
            i = j;
        }
    }

    return splited;
}


// print 
void print_element(VirtualMachine *vm, Int index)
{
    if (index < 0 || index >= vm->stack->size)
    {
        printf("{invalid}");
        return;
    }

    Value temp = vm->stack->data[index];

    switch (data_t(index))
    {
        case TYPE_NIL:
            printf("{nil}");
            break;
        case TYPE_INTEGER:
            printf("%ld", data(index).integer);
            break;
        case TYPE_BUILTIN:
            printf("%ld", data(index).pointer);
            break;
        case TYPE_LIST:
            printf("[");
            IntList *list = (IntList*)data(index).pointer;
            for (Int i = 0; i < list->size; i++)
            {
                printf("%ld", list->data[i]);
                if (i < list->size - 1)
                {
                    printf(", ");
                }
            }
            printf("]");
            break;
        case TYPE_OTHER:
            printf("%ld", data(index).pointer);
            break;
        case TYPE_NUMBER:
            if (round(data(index).number) == data(index).number)
            {
                printf("%ld", (Int)data(index).number);
            }
            else
            {
                printf("%f", data(index).number);
            }
            break;
        case TYPE_STRING:
            printf("%s", data(index).string);
            break;

        default:
            printf("{unknown} %d", data(index).pointer);
            break;
    }
}

#ifndef ARDUINO

// repl 
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
        result = eval(vm, cmd, NULL);
        free(cmd);
    }

    printf("repl returned: @%ld\n", result);
    print_element(vm, result);
    printf("\n");
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

#endif

// value functions

Value value_duplicate(Value value, char type)
{
    Value dup;

    switch (type)
    {
        case TYPE_STRING:
            dup.string = str_duplicate(value.string);
            break;
        case TYPE_LIST:
            dup.pointer = make_int_list();
            IntList *list = (IntList*)value.pointer;
            for (Int i = 0; i < list->size; i++)
            {
                stack_push(*((IntList*)dup.pointer), list->data[i]);
            }
            break;
        case TYPE_BUILTIN:
            dup.pointer = value.pointer;
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
    Int _index = hash_find(vm, varname);
    if (_index != -1)
    {
        vm->hashes->data[_index].index = index;
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

StringList* make_string_list()
{
    StringList *list = (StringList*)malloc(sizeof(StringList));
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
    vm->interpret = default_interpreter;
    vm->unused = make_int_list();
    return vm;
}

// var new 
Int new_var(VirtualMachine *vm)
{ 
    if (vm->unused->size > 0)
    {
        Int index = stack_pop(*vm->unused);
        unuse_var(vm, index);
        return index;
    }
    else
    {
        Value value;
        value.pointer = NULL;
        stack_push(*vm->stack, value);
        stack_push(*vm->typestack, TYPE_OTHER);
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
    vm->typestack->data[id] = TYPE_BUILTIN;
    return id;
}

Int new_list(VirtualMachine *vm)
{
    Int id = new_var(vm);
    vm->stack->data[id].pointer = make_int_list();
    vm->typestack->data[id] = TYPE_LIST;
    return id;
}

// var register

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

Int register_list(VirtualMachine *vm, char* varname)
{
    Int index = new_list(vm);
    hash_set(vm, varname, index);
    return index;
}

//frees

void free_var(VirtualMachine *vm, Int index)
{
    switch (vm->typestack->data[index])
    {
        case TYPE_STRING:
            free(vm->stack->data[index].string);
            break;
        case TYPE_LIST:
            while (((IntList*)vm->stack->data[index].pointer)->size > 0)
            {
                stack_shift(*((IntList*)vm->stack->data[index].pointer));
            }
            stack_free(*((IntList*)vm->stack->data[index].pointer));
            break;
        case TYPE_FUNCTION:
            for (Int i = 0; i < ((InternalFunction*)vm->stack->data[index].pointer)->varnames->size; i++)
            {
                free(((InternalFunction*)vm->stack->data[index].pointer)->varnames->data[i]);
            }
            stack_free(*((InternalFunction*)vm->stack->data[index].pointer)->varnames);
            free(((InternalFunction*)vm->stack->data[index].pointer)->code);
            free(vm->stack->data[index].pointer);
            break;
        default:
            break;
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

    stack_free(*vm->unused);

    stack_free(*vm->stack);
    stack_free(*vm->typestack);

    stack_free(*vm->hashes);

    free(vm);
}

// interpreter core
function(direct_interpret)
{
    Int func = stack_shift(*args);
    Int result = -1;

    if (func > -1 && vm->typestack->data[func] == TYPE_BUILTIN)
    {
        Function _function = vm->stack->data[func].pointer;
        result = _function(vm, args, context);
    }
    else if (func > -1 && vm->typestack->data[func] == TYPE_FUNCTION)
    {
        InternalFunction *_func = (InternalFunction*)data(func).pointer;
        HashList *_context = (HashList*)malloc(sizeof(HashList));
        stack_init(*_context);

        HashList *global_context = vm->hashes;

        vm->hashes = _context;


        for (Int i = 0; i < _func->varnames->size; i++)
        {
            hash_set(vm, _func->varnames->data[i], stack_shift(*args));
        }
        
        if (args->size > 0)
        {
            Int etc = register_list(vm, "etc");
            while (args->size > 0)
            {
                stack_push(*((IntList*)data(etc).pointer), stack_shift(*args));
            }
        }

        vm->hashes = global_context;

        result = eval(vm, _func->code, _context);
        
        
        while (_context->size > 0)
        {
            Hash hash = stack_shift(*_context);
            free(hash.key);
        }

        stack_free(*_context);
    }
    else 
    {
        printf("Error: %d is not a function\n", func);
    }
    return result;
}

// Parser functions
IntList* parse(void *_vm, char *cmd, HashList *context) 
{
    VirtualMachine* vm = (VirtualMachine*)_vm;
    IntList *result = make_int_list();
    
    StringList *splited = special_space_split(cmd);
    //Int current = 0;
    while (splited->size > 0)
    {
        char* str = stack_shift(*splited);
        if (str[0] == '(')
        {
            if(str[1] == '@' && str[2] == '@')//string
            {
                char* temp = str + 3;
                temp[strlen(temp) - 1] = '\0';
                Int var = new_string(vm, temp);
                stack_push(*result, var);            
            }
            else if (str[1] == '/' && str[2] == '/') 
            {
                // comment
            }
            else if(str[1] == '$')
            {
                char* temp = str + 2;
                // temp to next space
                Int nextspace = str_find(temp, " ");
                char* funcname = str_nduplicate(temp, nextspace);
                temp += nextspace + 1;
                char* code = str_sub(temp, 0, strlen(temp)-1);
                Int __code = new_string(vm, code);
                Int __func = hash_find(vm, funcname);
                free(code);
                Int __arglist = new_list(vm);
                IntList *arglist = (IntList*)data(__arglist).pointer;
                stack_push(*arglist, __func);
                stack_push(*arglist, __code);

                Int _result = direct_interpret(vm, arglist, context);
                stack_push(*result, _result);
                //frees
                free(funcname);
                unuse_var(vm, __arglist);
                data_t(__arglist) = TYPE_NIL;
            }
            else
            {
                char* temp = str + 1;
                temp[strlen(temp) - 1] = '\0';
                Int index = eval(vm, temp, context);
                stack_push(*result, index);
            }
        }
        else if (str[0] == '@') 
        {
            stack_push(*result, atoi(str + 1));
        }
        else if (str[0] == '"' || str[0] == '\'') // string
        {
            char* temp = str + 1;
            temp[strlen(temp) - 1] = '\0';
            Int var = new_string(vm, temp);
            stack_push(*result, var);
        }
        else if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-') 
        {
            Int var = new_number(vm, atof(str));
            stack_push(*result, var);
        }
        else if(str[0] == '.' && str[1] == '.' && str[2] == '.')
        {
            char* _str = str+3;
            Int lst = hash_find(vm, _str);
            if (lst == -1)
            {
                printf("cannot spread non-existent list: %s\n", _str);
                stack_push(*result, -1);
                free(str);
                continue;
            }
            IntList *list = data(lst).pointer;
            for (Int i = 0; i < list->size; i++)
            {
                stack_push(*result, list->data[i]);
            }
        }
        else if (str[0] == '/' && str[1] == '/') // comment
        {
            break;
        }
        else //variable 
        {
            Int index;
            if (context != NULL)
            {
                HashList* _global_context = vm->hashes;
                vm->hashes = context;
                index = hash_find(vm, str);
                vm->hashes = _global_context;
                if (index == -1)
                {
                    index = hash_find(vm, str);
                }
            }
            else
            {
                index = hash_find(vm, str);
            }

            if (index == -1) 
            {
                printf("Variable %s not found\n", str);
                stack_push(*result, -1);
            }
            else 
            {
                stack_push(*result, index);
            }
        }
        free(str);
    }
    stack_free(*splited);
    return result;
}

Int default_interpreter(void *_vm, char* cmd, HashList *context)
{
    VirtualMachine* vm = (VirtualMachine*) _vm;
    IntList *args = parse(vm, cmd, context);
    if (args->size == 0)
    {
        stack_free(*args);
        return -1;
    }
    Int result = direct_interpret(vm, args, context);

    stack_free(*args);
    return result;
}

Int eval(VirtualMachine *vm, char *cmd, HashList *context)
{
    if(strchr(cmd, ';') == NULL)
    {
        return vm->interpret(vm, cmd, context);
    }

    StringList *splited = special_split(cmd, ';');

    // remove empty or whitespace only strings using is_space
    Int last = splited->size - 1;
    while (last >= 0)
    {
        if (strlen(splited->data[last]) == 0)
        {
            free(splited->data[last]);
            stack_remove(*splited, last);
        }
        else
        {
            Int i = 0;
            while (splited->data[last][i] != '\0' && is_space(splited->data[last][i]))
            {
                i++;
            }
            if (splited->data[last][i] == '\0')
            {
                free(splited->data[last]);
                stack_remove(*splited, last);
            }
        }
        last--;
    }


    Int result = -1;
    while (splited->size > 0)
    {
        
        char *str = stack_shift(*splited);
        if (strlen(str) == 0)
        {
            free(str);
            continue;
        }
        result = vm->interpret(vm, str, context);
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

void unuse_var(VirtualMachine *vm, Int index)
{
    //if type is string free the string, if type is list free the list
    if (data_t(index) == TYPE_STRING)
    {
        free(data(index).string);
    }
    else if (vm->typestack->data[index] == TYPE_LIST)
    {
        stack_free(*((IntList*)data(index).pointer));
    }
    else if (vm->typestack->data[index] == TYPE_FUNCTION)
    {
        for (Int i = 0; i < vm->hashes->size; i++)
        {
            if (vm->hashes->data[i].index == index)
            {
                free(vm->hashes->data[i].key);
                stack_remove(*vm->hashes, i);
            }
        }
    }
    
    data_t(index) = TYPE_NIL;
    
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (vm->hashes->data[i].index == index)
        {
            free(vm->hashes->data[i].key);
            stack_remove(*vm->hashes, i);
        }
    }
    stack_push(*vm->unused, index);
}