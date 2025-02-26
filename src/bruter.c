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
            j++;  // Avança para depois da abertura de aspas duplas
            while (str[j] != '"' && str[j] != '\0')
            {
                j++;
            }
            char *tmp = str_nduplicate(str + i, j - i + 1);
            list_push(*splited, tmp);
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
            list_push(*splited, tmp);
            i = j + 1;  // Avança para após o fechamento de aspas simples
        }
        else if (isspace(str[i]))
        {
            i++;
        }
        else
        {
            int j = i;
            while (!isspace(str[j]) && str[j] != '\0' && str[j] != '(' && str[j] != ')' && str[j] != '"' && str[j] != '\'')
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
            list_push(*splited, tmp);
            last_i = i + 1;
        }
        else if (str[i + 1] == '\0') // Checagem para o último token
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


// print 
void print_element(VirtualMachine *vm, Int index)
{
    if (index < 0 || index >= vm->stack->size)
    {
        printf("(return (get: -1))");
        return;
    }

    Value temp = vm->stack->data[index];

    switch (data_t(index))
    {
        char* _str;
        char* strbak;
        char* stringified;
        case TYPE_ANY:
            printf("%ld", data(index).integer);
            break;
        case TYPE_LIST:
            stringified = list_stringify(vm, (IntList*)data(index).pointer);
            printf("%s", stringified);
            free(stringified);
            break;
        case TYPE_NUMBER:
            if (((Int)data(index).number) == data(index).number)
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
            printf("%d", data(index).pointer);
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

    printf("repl returned: [@%ld]", result);
    print_element(vm, result);
    printf("\n");
    return result;
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
            dup.pointer = list_init(IntList);
            IntList *list = (IntList*)value.pointer;
            for (Int i = 0; i < list->size; i++)
            {
                list_push(*((IntList*)dup.pointer), list->data[i]);
            }
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
    if (hash_find(vm, varname) != -1)
    {
        hash_unset(vm, varname);
    }
    Hash hash;
    hash.key = str_duplicate(varname);
    hash.index = index;
    list_push(*vm->hashes, hash);
}

void hash_unset(VirtualMachine *vm, char* varname)
{
    Int index = hash_find(vm, varname);
    if (index != -1)
    {
        free(vm->hashes->data[index].key);
        vm->hashes->data[index].key = NULL;
        list_fast_remove(*vm->hashes, index);
    }
}

//variable functions

VirtualMachine* make_vm()
{
    VirtualMachine *vm = (VirtualMachine*)malloc(sizeof(VirtualMachine));
    vm->stack = list_init(ValueList);
    vm->typestack = list_init(CharList);
    vm->hashes = list_init(HashList);
    vm->interpret = default_interpreter;
    vm->unused = list_init(IntList);

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

Int new_list(VirtualMachine *vm)
{
    Int id = new_var(vm);
    vm->stack->data[id].pointer = list_init(IntList);
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
            case TYPE_LIST:
                list_free(*((IntList*)value.pointer));
                break;
            default:
                break;
        }
    }

    while (vm->hashes->size > 0)
    {
        Hash hash = list_pop(*vm->hashes);
        free(hash.key);
    }

    list_free(*vm->unused);

    list_free(*vm->stack);
    list_free(*vm->typestack);

    list_free(*vm->hashes);

    free(vm);
}

// Parser functions
IntList* parse(void *_vm, char *cmd, HashList *context) 
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
            if(str[1] == '@' && str[2] == '@') //string
            {
                char* temp = str + 3;
                temp[strlen(temp) - 1] = '\0';
                Int var = new_string(vm, temp);
                list_push(*result, var);            
            }
            else
            {
                char* temp = str + 1;
                temp[strlen(temp) - 1] = '\0';
                Int index = eval(vm, temp, context);
                list_push(*result, index);
            }
        }
        else if (str[0] == '@') 
        {
            if (isdigit(str[1]))
            {
                list_push(*result, atoi(str + 1));
            }
            else 
            {
                list_push(*result, new_number(vm, hash_find(vm, str + 1)));
            }
        }
        else if (str[0] == '"' || str[0] == '\'') // string
        {
            char* token = strchr(str + 1, '$');
            char* backup = str;

            while (token != NULL)
            {
                if (token[1] != 0)
                {
                    if (isalpha(token[1]))// replace $name by the value of 'name'
                    {
                        char* space_token = strchr(token + 1, ' ');
                        if (space_token == NULL)
                        {
                            space_token = strchr(token + 1, '"');
                            if (space_token == NULL)
                            {
                                space_token = strchr(token + 1, '\'');
                                if (space_token == NULL)
                                {
                                    space_token = strchr(token + 1, '\0');
                                }
                            }
                        }

                        char* varname = str_nduplicate(token, space_token - token);
                        Int index = hash_find(vm, varname + 1);

                        if (index != -1)
                        {
                            char* replacement;
                            switch (data_t(index))
                            {
                                case TYPE_STRING:
                                    str = str_replace(str, varname, data(index).string);
                                    free(backup);
                                    break;
                                case TYPE_NUMBER:
                                    if (data(index).number == (Int)data(index).number)
                                    {
                                        replacement = str_format("%ld", (Int)data(index).number);
                                        str = str_replace(str, varname, replacement);
                                    }
                                    else
                                    {
                                        replacement = str_format("%f", data(index).number);
                                        str = str_replace(str, varname, replacement);
                                    }
                                    free(backup);
                                    break;
                                case TYPE_LIST:
                                    replacement = list_stringify(vm, (IntList*)data(index).pointer);
                                    str = str_replace(str, varname, replacement);
                                    free(backup);
                                    break;
                                default:
                                    replacement = str_format("%ld", data(index).integer);
                                    str = str_replace(str, varname, replacement);
                                    free(backup);
                                    break;
                            }
                            free(replacement);
                       }
                       free(varname);
                    }
                    else if (isdigit(token[1])) // replace $1 by the value of the entry 1 of the stack
                    {
                        Int index = atoi(token + 1);
                        if (index >= 0 && index < vm->stack->size)
                        {
                            switch (data_t(index))
                            {
                                case TYPE_STRING:
                                    str = str_replace(str, token, data(index).string);
                                    break;
                                case TYPE_NUMBER:
                                    if (data(index).number == (Int)data(index).number)
                                    {
                                        str = str_replace(str, token, str_format("%ld", (Int)data(index).number));
                                    }
                                    else
                                    {
                                        str = str_replace(str, token, str_format("%f", data(index).number));
                                    }
                                    break;
                                case TYPE_LIST:
                                    str = str_replace(str, token, list_stringify(vm, (IntList*)data(index).pointer));
                                    break;
                                default:
                                    str = str_replace(str, token, str_format("%ld", data(index).integer));
                                    break;
                            }
                        }
                    }
                }
                token = strchr(token + 1, '$');
            };

            token = strchr(str + 1, '\\');

            while (token != NULL)
            {
                if (token[1] != 0)
                {
                    
                    // octal
                    if (token[1] == '0' && (strlen(token+1)>=3) && isdigit(token[2]) && isdigit(token[3]))
                    {
                        char* octal = str_nduplicate(token + 1, 3);
                        str = str_replace(str, token, str_format("%c", strtol(octal, NULL, 8)));
                        free(octal);
                        free(backup);
                    }
                    else if (isdigit(token[1]))
                    {
                        str = str_replace(str, token, str_format("%c", atoi(token + 1)));
                        free(backup);
                    }
                    // hex
                    else if (token[1] == 'x' && (strlen(token+1)>=3) && isxdigit(token[2]) && isxdigit(token[3]))
                    {
                        char* hex = str_nduplicate(token + 2, 2);
                        str = str_replace(str, token, str_format("%c", strtol(hex, NULL, 16)));
                        free(hex);
                        free(backup);
                    }
                    else if (token[1] == 'n')
                    {
                        str = str_replace(str, token, "\n");
                        free(backup);
                    }
                    else if (token[1] == 't')
                    {
                        str = str_replace(str, token, "\t");
                        free(backup);
                    }
                    else if (token[1] == 'r')
                    {
                        str = str_replace(str, token, "\r");
                        free(backup);
                    }
                    else if (token[1] == '0')
                    {
                        str = str_replace(str, token, "\0");
                        free(backup);
                    }
                    else if (token[1] == '\\')
                    {
                        str = str_replace(str, token, "\\");
                        free(backup);
                    }
                    else if (token[1] == '"')
                    {
                        str = str_replace(str, token, "\"");
                        free(backup);
                    }
                    else if (token[1] == '\'')
                    {
                        str = str_replace(str, token, "'");
                        free(backup);
                    }

                }
                token = strchr(token + 1, '\\');
            }

            char* temp = str + 1;
            temp[strlen(temp) - 1] = '\0';
            Int var = new_string(vm, temp);
            list_push(*result, var);
        }
        else if (isdigit(str[0]) || (str[0] == '-' && str[1] > '\0')) // number
        {
            Int var = new_number(vm, atof(str));
            list_push(*result, var);
        }
        else if(str[0] == '.' && str[1] == '.' && str[2] == '.' && isalpha(str[3])) // spread
        {
            char* _str = str+3;
            Int lst = hash_find(vm, _str);
            if (lst == -1)
            {
                printf("cannot spread non-existent variable: %s\n", _str);
                list_push(*result, -1);
                free(str);
                continue;
            }
            if (data_t(lst) == TYPE_LIST)
            {
                IntList *list = data(lst).pointer;
                for (Int i = 0; i < list->size; i++)
                {
                    list_push(*result, list->data[i]);
                }
            }
            else if (data_t(lst) == TYPE_STRING)
            {
                char *string = data(lst).string;
                for (Int i = 0; i < strlen(string); i++)
                {
                    list_push(*result, new_number(vm, string[i]));
                }
            }
            else
            {
                printf("cannot spread non-list variable: %s\n", _str);
                list_push(*result, -1);
            }
        }
        else if (str[0] == '/' && str[1] == '/') // comment
        {
            free(str);
            while (splited->size > 0)
            {
                free(list_pop(*splited));
            }
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
                list_push(*result, -1);
            }
            else 
            {
                list_push(*result, index);
            }
        }
        free(str);
    }
    list_free(*splited);
    return result;
}

Int interpret(VirtualMachine *vm, IntList *args, HashList *context)
{
    Int func = list_shift(*args);
    Int result = -1;

    if (func > -1)
    {
        HashList *global_context;
        HashList *_context;
        IntList  *_list;
        Function _function;
        Int etc;
        Hash hash;
        char* code;
        Int firstelement;
        Int firstelement2;
        IntList *varnames;
        IntList *values;

        switch (vm->typestack->data[func])
        {
            case TYPE_ANY:
                _function = vm->stack->data[func].pointer;
                result = _function(vm, args, context);
                list_unshift(*args, func);
                break;

                
            case TYPE_STRING:
                _context = list_init(HashList);
                global_context = vm->hashes;
                vm->hashes = _context;
                
                etc = register_list(vm, "...");
                while (args->size > 0)
                {
                    list_push(*((IntList*)data(etc).pointer), list_pop(*args));
                }

                vm->hashes = global_context;

                result = eval(vm, data(func).string, _context);

                while (_context->size > 0)
                {
                    hash = list_pop(*_context);
                    free(hash.key);
                }

                list_free(*_context);

                list_unshift(*args, func);
                break;


            case TYPE_LIST:
                firstelement = list_get(*((IntList*)data(func).pointer), 0);
                _list = (IntList*)data(func).pointer;
                
                if  (data_t(list_get(*_list, 0)) == TYPE_LIST)
                {
                    switch (data_t(list_get(*_list, 1)))
                    {
                        case TYPE_STRING:
                            varnames = (IntList*)data(firstelement).pointer;

                            _context = list_init(HashList);
                            
                            code = list_get(*vm->stack, list_get(*_list, 1)).string;

                            list_reverse(*args);

                            global_context = vm->hashes;

                            vm->hashes = _context;

                            for (Int i = 0; i < varnames->size; i++)
                            {
                                hash_set(vm, data(varnames->data[i]).string, list_pop(*args));
                            }
                            
                            if (args->size > 0)
                            {
                                Int etc = register_list(vm, "...");
                                for (Int i = 0; i < args->size; i++)
                                {
                                    list_push(*((IntList*)data(etc).pointer), list_pop(*args));
                                }
                            }

                            vm->hashes = global_context;

                            result = eval(vm, code, _context);

                            while (_context->size > 0)
                            {
                                free(list_pop(*_context).key);
                            }

                            list_free(*_context);
                            list_push(*args, func);
                            list_reverse(*args);
                            break;
                        case TYPE_LIST: // a switch
                            varnames = (IntList*)data(firstelement).pointer;
                            values = (IntList*)data(list_get(*_list, 1)).pointer;
                            if (data_t(list_get(*varnames,0)) == TYPE_STRING)
                            {
                                for (Int i = 0; i < varnames->size; i++)
                                {
                                    if (strcmp(data(varnames->data[i]).string, data(args->data[0]).string) == 0)
                                    {
                                        result = list_get(*values, i);
                                        break;
                                    }
                                }
                                break;
                            }
                            else
                            {
                                for (Int i = 0; i < varnames->size; i++)
                                {
                                    if (data(varnames->data[i]).integer == data(args->data[0]).integer)
                                    {
                                        result = list_get(*values, i);
                                    }
                                }
                            }
                            break;
                    };
                }
                break;
        }
    }
    else 
    {
        printf("Error: %d is not a function or script\n", func);
    }
    return result;
}

Int default_interpreter(void *vm, char* cmd, HashList *context)
{
    IntList *args = parse((VirtualMachine*)vm, cmd, context);

    if (args->size == 0)
    {
        list_free(*args);
        return -1;
    }
    
    
    Int result = interpret((VirtualMachine*)vm, args, context);
    
    list_free(*args);
    
    return result;
}

Int eval(VirtualMachine *vm, char *cmd, HashList *context)
{
    if(strchr(cmd, ';') == NULL)
    {
        return vm->interpret(vm, cmd, context);
    }

    StringList *splited = special_split(cmd, ';');

    // remove empty or whitespace only strings using isspace
    Int last = splited->size - 1;
    while (last >= 0)
    {
        if (strlen(splited->data[last]) == 0)
        {
            free(splited->data[last]);
            list_pop(*splited);
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
        case TYPE_LIST:
            list_free(*((IntList*)data(index).pointer));
            break;
        default:
            break;
    }

    // hashes associated to the index are not removed anymore

    data_t(index) = TYPE_ANY;
    list_push(*vm->unused, index);
}

char* list_stringify(VirtualMachine* vm, IntList *list)
{
    char* _str = str_duplicate("(list: ");
    char* strbak;
    for (Int i = 0; i < list->size; i++)
    {
        switch (data_t(list->data[i]))
        {
        case TYPE_STRING:
            strbak = _str;
            _str = str_concat(_str, "(@@");
            free(strbak);
            strbak = _str;
            _str = str_concat(_str, data(list->data[i]).string);
            free(strbak);
            strbak = _str;
            _str = str_concat(_str, ")");
            free(strbak);
            break;
        case TYPE_NUMBER:
            strbak = _str;
            
            if (data(list->data[i]).number == (Int)data(list->data[i]).number)
            {
                char* str = str_format("%ld", (Int)data(list->data[i]).number);
                _str = str_concat(_str, str);
                free(str);
            }
            else 
            {
                char* str = str_format("%lf", data(list->data[i]).number);
                _str = str_concat(_str, str);
                free(str);
            }

            free(strbak);
            break;
        case TYPE_LIST:
            strbak = _str;
            char* stringified = list_stringify(vm, (IntList*)data(list->data[i]).pointer);
            _str = str_concat(_str, stringified);
            free(strbak);
            break;
        default:
            strbak = _str;
            _str = str_format("%s%ld", _str, data(list->data[i]).integer);
            free(strbak);
            break;
        }
        if (i < list->size - 1)
        {
            strbak = _str;
            _str = str_concat(_str, " ");
            free(strbak);
        }
    }
    strbak = _str;
    _str = str_concat(_str, ")");
    free(strbak);
    return _str;
}

// if web

#ifdef __EMSCRIPTEN__

typedef List(VirtualMachine*) VMList;
VMList *vms;
IntList *unused_vms;

void free_vms_atexit()
{
    for (Int i = 0; i < vms->size; i++)
    {
        free_vm(vms->data[i]);
    }
    free(vms->data);
    free(vms);
}

EMSCRIPTEN_KEEPALIVE
Int wasm_new_vm()
{
    Int index = -1;
    VirtualMachine *vm = make_vm();
    // <libraries init>
init_std(vm);
    if (unused_vms->size > 0)
    {
        index = list_pop(*unused_vms);
        vms->data[index] = vm;
    }
    else
    {
        list_push(*vms, vm);
        index = vms->size - 1;
    }
    return index;
}

EMSCRIPTEN_KEEPALIVE
void wasm_destroy_vm(Int index)
{
    if (index < 0 || index >= vms->size)
    {
        return;
    }
    free_vm(vms->data[index]);
    list_push(*unused_vms, index);
}

EMSCRIPTEN_KEEPALIVE
char* wasm_eval(Int index, char *cmd)
{
    VirtualMachine *vm = vms->data[index];
    Int result = eval(vm, cmd, NULL);
    if (result < 0)
    {
        return NULL;
    }
    else
    {
        Int str = new_var(vm);
        data_t(str) = TYPE_STRING;
        switch (data_t(result))
        {
            case TYPE_NUMBER:
                data(str).string = str_format("\1%f", data(result).number);
                return data(str).string;
                break;
            case TYPE_STRING:
                data(str).string = str_format("\2%s", data(result).string);
                return data(str).string;
                break;
            default:
                data(str).string = str_format("\0%d", data(result).integer);
                return data(str).string;
                break;
        }
    }
}

#endif