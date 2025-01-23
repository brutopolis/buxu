#include "bruter.h"

// functions defitions for bruter
// functions defitions for bruter
// functions defitions for bruter

#ifndef ARDUINO

function(brl_os_file_read)
{
    char *code = readfile(arg(0).string);
    Int index = -1;
    if (code != NULL)
    {
        index = new_var(vm);
        data(index).string = code;
        data_t(index) = TYPE_STRING;
    }
    return index;
}

function(brl_os_file_write)
{
    writefile(arg(0).string, arg(1).string);
    return -1;
}

function(brl_os_file_delete)
{
    remove(arg(0).string);
    return -1;
}

function(brl_os_repl)
{
    return repl(vm);
}

function(brl_os_dofile)
{
    char *code = readfile(arg(0).string);
    Int result = -1;
    if (code != NULL)
    {
        result = eval(vm, code, context);
        free(code);
    }
    else 
    {
        printf("Error: file %s not found\n", arg(0).string);
    }
    return result;
}

function(brl_os_file_exists)
{
    FILE *file = fopen(arg(0).string, "r");
    Int result = new_number(vm, file != NULL);
    if (file != NULL)
    {
        fclose(file);
    }
    return result;
}

function(brl_os_file_rename)
{
    rename(arg(0).string, arg(1).string);
    return -1;
}

function(brl_os_file_copy)
{
    FILE *file = fopen(arg(0).string, "r");
    if (file == NULL)
    {
        return -1;
    }
    fclose(file);
    char *code = readfile(arg(0).string);
    writefile(arg(1).string, code);
    free(code);
    return -1;
}

function(brl_os_file_size)
{
    FILE *file = fopen(arg(0).string, "r");
    if (file == NULL)
    {
        return -1;
    }
    fseek(file, 0, SEEK_END);
    Int result = new_number(vm, ftell(file));
    fclose(file);
    return result;
}

function(brl_os_system)
{
    system(arg(0).string);
    return -1;
}

#ifndef __EMSCRIPTEN__
function(brl_os_time_now)
{
    return new_number(vm, time(NULL));
}

function(brl_os_time_clock)
{
    return new_number(vm, clock());
}
#endif

#endif

function(brl_std_hash_set)
{
    if (context != NULL)
    {
        HashList *global_context = vm->hashes;
        vm->hashes = context;
        hash_set(vm, arg(0).string, arg_i(1));
        vm->hashes = global_context;
    }
    else 
    {
        hash_set(vm, arg(0).string, arg_i(1));
    }
    return -1;
}

function(brl_std_hash_get)
{
    if (context != NULL)
    {
        HashList *global_context = vm->hashes;
        vm->hashes = context;
        Int index = hash_find(vm, arg(0).string);
        vm->hashes = global_context;
        return index;
    }
    else 
    {
        return hash_find(vm, arg(0).string);
    }
    return -1;
}

function(brl_std_hash_delete)
{
    if (context != NULL)
    {
        HashList *global_context = vm->hashes;
        vm->hashes = context;
        hash_unset(vm, arg(0).string);
        vm->hashes = global_context;
    }
    else 
    {
        hash_unset(vm, arg(0).string);
    }
    return -1;
}

function(brl_std_hash_rename)
{
    if (context != NULL)
    {
        HashList *global_context = vm->hashes;
        vm->hashes = context;
        Int index = hash_find(vm, arg(0).string);
        if (index != -1)
        {
            hash_set(vm, arg(1).string, index);
            hash_unset(vm, arg(0).string);
        }
        vm->hashes = global_context;
    }
    else 
    {
        Int index = hash_find(vm, arg(0).string);
        if (index != -1)
        {
            hash_set(vm, arg(1).string, index);
            hash_unset(vm, arg(0).string);
        }
    }
    return -1;
}

function(brl_std_io_print)
{
    for (Int i = 0; i < args->size; i++)
    {
        if (arg_i(i) >= 0 || arg_i(i) < vm->stack->size)
        {
            print_element(vm, arg_i(i));
        }
        else 
        {
            printf("{out of stack}");
        }

        if (args->size > 0)
        {
            printf(" ");
        }
    }
    printf("\n");
    return -1;
}

function(brl_std_io_ls)
{
    Int _var = -1;
    if (args->size > 0)
    {
        _var = arg_i(0);
    }

    if (_var > -1 && data_t(_var) == TYPE_LIST)
    {
        IntList *list = (IntList*)vm->stack->data[(Int)data(_var).number].pointer;
        for (Int i = 0; i < list->size; i++)
        {
            printf("%d[%d]: ", (Int)data(_var).number, i);
            print_element(vm, list->data[i]);
            printf("\n");
        }
    }
    else 
    {
        for (Int i = 0; i < vm->stack->size; i++)
        {
            printf("[%ld]: ", i);
            print_element(vm, i);
            printf("\n");
        }
    }

    return -1;
}

function(brl_std_io_ls_types)
{
    for (Int i = 0; i < vm->stack->size; i++)
    {
        printf("[%ld]: %d\n", i, data_t(i));
    }
    return -1;
}

function(brl_std_io_ls_hashes)
{
    if (context != NULL)
    {
        for (Int i = 0; i < context->size; i++)
        {
            printf("[%s] {%d} @%ld: ", context->data[i].key, vm->typestack->data[context->data[i].index], context->data[i].index);
            print_element(vm, context->data[i].index);
            printf("\n");
        }
    }
    else 
    {
        for (Int i = 0; i < vm->hashes->size; i++)
        {
            printf("[%s] {%d} @%ld: ", hash(i).key, vm->typestack->data[hash(i).index], hash(i).index);
            print_element(vm, hash(i).index);
            printf("\n");
        }
    }
    return -1;
}

function(brl_std_do)
{
    return eval(vm, arg(0).string, context);
}



// functions
function(brl_std_function)
{
    StringList *varnames = (StringList*)malloc(sizeof(StringList));
    stack_init(*varnames);
    stack_reverse(*args);
    while (args->size > 1)
    {
        stack_push(*varnames, str_duplicate(data(stack_pop(*args)).string));
    }

    char *code = str_duplicate(arg(args->size - 1).string);

    InternalFunction *func = (InternalFunction*)malloc(sizeof(InternalFunction));
    func->varnames = varnames;
    func->code = code;

    Int index = new_var(vm);
    data(index).pointer = func;
    data_t(index) = TYPE_FUNCTION;
    return index;
}

function(brl_std_ignore)
{
    return -1;
}

function(brl_std_return)
{
    return arg_i(0);
}

function(brl_std_type_get)
{
    Int result = new_number(vm, arg_t(0));
    return result;
}

function(brl_std_type_set)
{
    arg_t(0) = (Int)arg(1).number;
    return -1;
}

// math functions
// math functions
// math functions
// math functions


function(brl_std_math_add)
{
    Int result = 0;
    for (Int i = 0; i < args->size; i++)
    {
        result += arg(i).number;
    }
    return new_number(vm, result);
}

function(brl_std_math_sub)
{
    Int result = arg(0).number;
    for (Int i = 1; i < args->size; i++)
    {
        result -= arg(i).number;
    }
    return new_number(vm, result);
}

function(brl_std_math_mul)
{
    Int result = 1;
    for (Int i = 0; i < args->size; i++)
    {
        result *= arg(i).number;
    }
    return new_number(vm, result);
}

function(brl_std_math_div)
{
    Int result = arg(0).number;
    for (Int i = 1; i < args->size; i++)
    {
        result /= arg(i).number;
    }
    return new_number(vm, result);
}

function(brl_std_math_mod)
{
    return new_number(vm, (Int)arg(0).number % (Int)arg(1).number);
}

function(brl_std_math_pow)
{
    return new_number(vm, pow(arg(0).number, arg(1).number));
}

function(brl_std_math_abs)
{
    return new_number(vm, fabs(arg(0).number));
}

function(brl_std_math_random)
{
    return new_number(vm, rand());
}

function(brl_std_math_seed)
{
    srand(arg(0).number);
    return -1;
}

function(brl_std_math_floor)
{
    return new_number(vm, floor(arg(0).number));
}

function(brl_std_math_ceil)
{
    return new_number(vm, ceil(arg(0).number));
}

function(brl_std_math_round)
{
    return new_number(vm, round(arg(0).number));
}

function(brl_std_math_increment)
{
    for (Int i = 0; i < args->size; i++)
    {
        arg(i).number++;
    }
    return -1;
}

function(brl_std_math_decrement)
{
    for (Int i = 0; i < args->size; i++)
    {
        arg(i).number--;
    }
    return -1;
}

function(brl_std_to_float)
{
    return new_number(vm, (Float)arg(0).integer);
}

function(brl_std_to_int)
{
    Int result = new_var(vm);
    data_t(result) = TYPE_INTEGER;
    data(result).integer = (Int)arg(1).number;
    return result;
}

function(brl_std_to_index)
{
    return ((Int)arg(0).number);
}

// list functions
// list functions
// list functions
// list functions

function(brl_std_list_new)
{
    Int index = new_list(vm);
    IntList *list = (IntList*)data(index).pointer;
    for (Int i = 0; i < args->size; i++)
    {
        stack_push(*list, arg_i(i));
    }
    return index;
}

function(brl_std_list_push)
{
    if (arg_t(0) == TYPE_LIST)
    {
        IntList *list = (IntList*)arg(0).pointer;
        for (Int i = 1; i < args->size; i++)
        {
            stack_push(*list, arg_i(i));
        }
    }
    else if (arg_t(0) == TYPE_STRING)
    {
        arg(0).string = (char*)realloc(arg(0).string, strlen(arg(0).string) + 1);
        arg(0).string[strlen(arg(0).string) + 1] = '\0';
        arg(0).string[strlen(arg(0).string)] = arg_i(1);
    }
    return -1;
}

function(brl_std_list_unshift)
{
    if (arg_t(0) == TYPE_LIST)
    {
        IntList *list = (IntList*)arg(0).pointer;
        for (Int i = 1; i < args->size; i++)
        {
            stack_unshift(*list, arg_i(i));
        }
    }
    else if (arg_t(0) == TYPE_STRING)
    {
        arg(0).string = (char*)realloc(arg(0).string, strlen(arg(0).string) + 1);
        arg(0).string[strlen(arg(0).string) + 1] = '\0';
        for (Int i = strlen(arg(0).string); i > 0; i--)
        {
            arg(0).string[i] = arg(0).string[i - 1];
        }
        arg(0).string[0] = arg_i(1);
    }
    return -1;   
}

// :pop
function(brl_std_list_pop)// returns the removed element
{
    if (arg_t(0) == TYPE_LIST)
    {
        IntList *list = (IntList*)arg(0).pointer;
        return stack_pop(*list);
    }
    else if (arg_t(0) == TYPE_STRING)
    {
        char *str = arg(0).string;
        Int result = str[strlen(str) - 2];
        str[strlen(str) - 1] = '\0';
        return result;
    }
    return -1;
}

function(brl_std_list_shift)// returns the removed element
{
    if (arg_t(0) == TYPE_LIST)
    {
        IntList *list = (IntList*)arg(0).pointer;
        return stack_shift(*list);
    }
    else if (arg_t(0) == TYPE_STRING)
    {
        char *str = arg(0).string;
        char c = str[0];
        for (Int i = 0; i < strlen(str); i++)
        {
            str[i] = str[i+1];
        }
        return c;
    }
    return -1;
}

function(brl_std_list_insert)
{
    if (arg_t(0) == TYPE_LIST)
    {
        IntList *list = (IntList*)arg(0).pointer;
        stack_insert(*list, (Int)arg(1).number, arg_i(2));
    }
    else if (arg_t(0) == TYPE_STRING)
    {
        char *str = arg(0).string;
        char c = arg_i(2);
        Int index = arg(1).number;
        str = (char*)realloc(str, strlen(str) + 1);
        str[strlen(str) + 1] = '\0';
        for (Int i = strlen(str); i > index; i--)
        {
            str[i] = str[i - 1];
        }
        str[index] = c;
    }
    return -1;
}

function(brl_std_list_remove)
{
    if (arg_t(0) == TYPE_LIST)
    {
        IntList *list = (IntList*)arg(0).pointer;
        stack_remove(*list, (Int)arg(1).number);
    }
    else if (arg_t(0) == TYPE_STRING)
    {
        char *str = arg(0).string;
        Int index = arg(1).number;
        for (Int i = index; i < strlen(str); i++)
        {
            str[i] = str[i + 1];
        }
    }
    return -1;
}

function(brl_std_list_concat)
{
    if (arg_t(0) == TYPE_LIST)
    {
        Int _newlist = new_list(vm);
        IntList *newlist = (IntList*)data(_newlist).pointer;
        for (Int i = 0; i < args->size; i++)
        {
            if (arg_t(i) == TYPE_LIST)
            {
                IntList *list = (IntList*)arg(i).pointer;
                for (Int j = 0; j < list->size; j++)
                {
                    stack_push(*newlist, list->data[j]);
                }
            }
        }
        return _newlist;
    }
    else if (arg_t(0) == TYPE_STRING)
    {
        Int _newstr = new_string(vm, "");
        char* newstr = data(_newstr).string;
        for (Int i = 0; i < args->size; i++)
        {
            if (arg_t(i) == TYPE_STRING)
            {
                strcat(newstr, arg(i).string);
            }
        }
        return _newstr;
    }
    return -1;
}

function(brl_std_list_find)
{
    if (arg_t(0) == TYPE_LIST)
    {
        Int list = arg_i(0);
        Int value = arg_i(1);
        if (arg_t(0) == TYPE_LIST)
        {
            IntList *lst = (IntList*)data(list).pointer;
            for (Int i = 0; i < lst->size; i++)
            {
                if (lst->data[i] == list)
                {
                    return i;
                }
            }
        }
    }
    else if (arg_t(0) == TYPE_STRING)
    {
        char *str = arg(0).string;
        char *substr = arg(1).string;
        return(new_number(vm, str_find(str, substr)));
    }

    return -1;
}

function(brl_std_list_get)
{
    if (arg_t(0) == TYPE_LIST)
    {
        Int list = arg_i(0);
        Int index = arg(1).number;
        if (arg_t(0) == TYPE_LIST)
        {
            IntList *lst = (IntList*)data(list).pointer;
            if (index >= 0 && index < lst->size)
            {
                return lst->data[index];
            }
            else 
            {
                printf("error: index %d out of range in list %d of size %d\n", index, list, lst->size);
                print_element(vm, list);
            }
        }
    }
    else if (arg_t(0) == TYPE_STRING)
    {
        char *str = arg(0).string;
        Int index = arg(1).number;
        if (index >= 0 && index < strlen(str))
        {
            return str[index];
        }
        else 
        {
            printf("error: index %d out of range in string %d of size %d\n", index, arg(0).string, strlen(str));
            print_element(vm, arg_i(0));
        }
    }
    else // accest bytes of a value
    {
        Int index = (Int)arg(1).number;
        Int value = arg_i(0);
        if (index >= 0 && index < sizeof(Float))
        {
            return ((char)arg(0).byte[index]);
        }
    }
    return -1;
}

function(brl_std_list_set)
{
    
    if (arg_t(0) == TYPE_LIST)
    {
        Int list = arg_i(0);
        Int index = arg(1).number;
        Int value = arg_i(2);
        IntList *lst = (IntList*)data(list).pointer;
        if (index >= 0 && index < lst->size)
        {
            lst->data[index] = value;
        }
        else 
        {
            printf("error: index %d out of range in list %d of size %d\n", index, list, lst->size);
            print_element(vm, list);
        }
        return -1;
    }
    else if (arg_t(0) == TYPE_STRING)
    {
        char *str = arg(0).string;
        Int index = arg(1).number;
        char c = arg_i(2);
        if (index >= 0 && index < strlen(str))
        {
            str[index] = c;
        }
        else 
        {
            printf("error: index %d out of range in string %d of size %d\n", index, arg(0).string, strlen(str));
            print_element(vm, arg_i(0));
        }
        return -1;
    }
    else 
    {
        Int index = (Int)arg(1).number;
        Int value = arg_i(0);
        if (index >= 0 && index < sizeof(Float))
        {
            arg(0).byte[index] = (char)value;
        }
    }
    return -1;
}

function(brl_std_list_length)
{
    if (arg_t(0) == TYPE_LIST)
    {
        return new_number(vm, ((IntList*)arg(0).pointer)->size);
    }
    else if (arg_t(0) == TYPE_STRING)
    {
        return new_number(vm, strlen(arg(0).string));
    }
    return -1;
}


function(brl_std_list_reverse)
{
    Int list = arg_i(0);
    if (arg_t(0) == TYPE_LIST)
    {
        IntList *lst = (IntList*)data(list).pointer;
        stack_reverse(*lst);
    }
    else if (arg_t(0) == TYPE_STRING)
    {
        char *str = arg(0).string;
        for (Int i = 0; i < strlen(str) / 2; i++)
        {
            char temp = str[i];
            str[i] = str[strlen(str) - i - 1];
            str[strlen(str) - i - 1] = temp;
        }
    }
    return -1;
}

function(brl_std_list_sub)
{
    if (arg_t(0) == TYPE_STRING)
    {
        char* _str = str_nduplicate(arg(0).string, arg(1).number);
        Int result = new_string(vm, _str);
        return result;
    }
    else if (arg_t(0) == TYPE_LIST)
    {
        IntList *list = (IntList*)arg(0).pointer;
        IntList *newlist = make_int_list();
        for (Int i = 0; i < arg(1).number; i++)
        {
            stack_push(*newlist, list->data[i]);
        }
        Int result = new_var(vm);
        data(result).pointer = newlist;
        data_t(result) = TYPE_LIST;
        return result;
    }
    return -1;
}

function(brl_std_list_split)
{
    if (arg_t(0) == TYPE_STRING)
    {
        Int _splited = new_list(vm);
        IntList *__splited = (IntList*)data(_splited).pointer;
        char* str = arg(0).string;
        Int separator = arg_i(1);
        StringList *splited = str_split_char(str, separator);
        for (Int i = 0; i < splited->size; i++)
        {
            Int _str = new_string(vm, splited->data[i]);
            stack_push(*__splited, _str);
        }
        for (Int i = 0; i < splited->size; i++)
        {
            free(splited->data[i]);
        }
        stack_free(*splited);
        return _splited;
    }
    else if (arg_t(0) == TYPE_LIST)
    {
        Int _splited = new_list(vm);
        IntList *__splited = (IntList*)data(_splited).pointer;
        IntList *list = (IntList*)arg(0).pointer;
        Int separator = arg_i(1);
        Int temp = new_list(vm);
        IntList *_temp = (IntList*)data(temp).pointer;
        for (Int i = 0; i < list->size; i++)
        {
            if (list->data[i] == separator)
            {
                stack_push(*__splited, temp);
                temp = new_list(vm);
                _temp = (IntList*)data(temp).pointer;
            }
            else 
            {
                stack_push(*_temp, list->data[i]);
            }
        }
        stack_push(*__splited, temp);
        return _splited;
    }
}

function(brl_std_list_replace)
{
    if (arg_t(0) == TYPE_STRING)
    {
        Int str = arg_i(0);
        Int substr = arg_i(1);
        Int replacement = arg_i(2);
        char* _str = data(str).string;
        char* _substr = data(substr).string;
        char* _replacement = data(replacement).string;
        char* _newstr = str_replace(_str, _substr, _replacement);
        Int result = new_string(vm, _newstr);
        free(_newstr);
        return result;
    }
    else if (arg_t(0) == TYPE_LIST)
    {
        Int _newlist = new_list(vm);
        IntList *newlist = (IntList*)data(_newlist).pointer;
        IntList *list = (IntList*)arg(0).pointer;
        Int substr = arg_i(1);
        Int replacement = arg_i(2);
        char done = 0;
        for (Int i = 0; i < list->size; i++)
        {
            if (list->data[i] == substr && !done)
            {
                stack_push(*newlist, replacement);
                done = 1; // replace only the first occurence
            }
            else 
            {
                stack_push(*newlist, list->data[i]);
            }
        }
        return _newlist;
    }
    return -1;
}

function(brl_std_list_replace_all)
{
    if (arg_t(0) == TYPE_STRING)
    {
        Int str = arg_i(0);
        Int substr = arg_i(1);
        Int replacement = arg_i(2);
        char* _str = data(str).string;
        char* _substr = data(substr).string;
        char* _replacement = data(replacement).string;
        char* _newstr = str_replace_all(_str, _substr, _replacement);
        Int result = new_string(vm, _newstr);
        free(_newstr);
        return result;
    }
    else if (arg_t(0) == TYPE_LIST)
    {
        Int _newlist = new_list(vm);
        IntList *newlist = (IntList*)data(_newlist).pointer;
        IntList *list = (IntList*)arg(0).pointer;
        Int substr = arg_i(1);
        Int replacement = arg_i(2);
        for (Int i = 0; i < list->size; i++)
        {
            if (list->data[i] == substr)
            {
                stack_push(*newlist, replacement);
            }
            else 
            {
                stack_push(*newlist, list->data[i]);
            }
        }
        return _newlist;
    }
    return -1;
}

// string functions

function(brl_std_string_split)
{
    Int _splited = new_list(vm);
    IntList *__splited = (IntList*)data(_splited).pointer;
    char* str = arg(0).string;
    char* separator = arg(1).string;
    StringList *splited = str_split(str, separator);
    for (Int i = 0; i < splited->size; i++)
    {
        Int _str = new_string(vm, splited->data[i]);
        stack_push(*__splited, _str);
    }
    for (Int i = 0; i < splited->size; i++)
    {
        free(splited->data[i]);
    }
    stack_free(*splited);
    return _splited;
}

function(brl_std_string_to_number)
{
    Int str = arg_i(0);
    return new_number(vm, atof(data(str).string));
}

function(brl_std_string_format)
{
    stack_reverse(*args);
    Int str = stack_pop(*args);
    Int result = -1;
    char* _str = str_duplicate(data(str).string);
    for (Int i = 0; i < strlen(_str); i++)
    {
        if (_str[i] == '%')
        {
            if (_str[i+1] == 'd')
            {
                Int value = stack_pop(*args);
                char* _value = str_format("%ld", (Int)data(value).number);
                char* _newstr = str_replace(_str, "\%d", _value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 's')
            {
                Int value = stack_pop(*args);
                char* _value = data(value).string;
                char* _newstr = str_replace(_str, "\%s", _value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 'f')
            {
                Int value = stack_pop(*args);
                char* _value = str_format("%f", data(value).number);
                char* _newstr = str_replace(_str, "\%f", _value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 'p')
            {
                Int value = stack_pop(*args);
                char* _value = str_format("%p", data(value).pointer);
                char* _newstr = str_replace(_str, "\%p", _value);
                free(_str);
                _str = _newstr;
            }
        }
        else if (_str[i] == '\\')
        {
            if (_str[i+1] == 'n')
            {
                char* _newstr = str_replace(_str, "\\n", "\n");
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 't')
            {
                char* _newstr = str_replace(_str, "\\t", "\t");
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 'r')
            {
                char* _newstr = str_replace(_str, "\\r", "\r");
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] >= '0' && _str[i+1] <= '9')
            {
                char* _newstr = str_format("%s%c", _str + i + 1, (char)atoi(_str + i + 1));
                free(_str);
                _str = _newstr;
            }
        }
    }
    result = new_var(vm);
    data(result).string = _str;
    data_t(result) = TYPE_STRING;
    return result;
}

function(brl_std_string_set)
{
    Int str = arg_i(0);
    Int index = arg(1).number;
    Int value = arg_i(2);
    if (arg_t(0) == TYPE_STRING)
    {
        char* _str = data(str).string;
        if (index >= 0 && index < strlen(_str))
        {
            _str[index] = data(value).number;
        }
        else 
        {
            printf("error: index %d out of range in string %d of size %d\n", index, str, strlen(_str));
            print_element(vm, str);
        }
    }
    return -1;
}



// std conditions
// std conditions
// std conditions
function(brl_std_condition_or)
{
    Int result = 0;
    if (is_true(arg(0), arg_t(0)))
    {
        Int index = new_var(vm);
        if (arg_t(0) == TYPE_LIST || arg_t(0) == TYPE_STRING)
        {
            data(index) = arg(0);
        }
        else
        {
            data(index) = value_duplicate(arg(0), arg_t(0));
        }
        arg_t(0) = arg_t(1);
        result = index;
    }
    else
    {
        Int index = new_var(vm);
        if (arg_t(1) == TYPE_LIST || arg_t(1) == TYPE_STRING)
        {
            data(index) = arg(1);
        }
        else
        {
            data(index) = value_duplicate(arg(1), arg_t(1));
        }
        arg_t(0) = arg_t(1);
        result = index;
    }
    return result;
}

function(brl_std_condition_if)
{
    Int result = -1;
    if (eval(vm, arg(0).string, context))
    {
        result = eval(vm, arg(1).string, context);

    }
    return result;
}

function(brl_std_condition_ifelse)
{
    Int result = -1;
    if (eval(vm, arg(0).string, context))
    {
        result = eval(vm, arg(1).string, context);
    }
    else
    {
        result = eval(vm, arg(2).string, context);
    }
    return result;
}

function(brl_std_condition_equals)
{
    return(arg(0).integer == arg(1).integer);
}

function(brl_std_condition_not_equals)
{
    return(arg(0).integer != arg(1).integer);
}

function(brl_std_condition_greater)
{
    return(arg(0).integer > arg(1).integer);
}

function(brl_std_condition_greater_equals)
{
    return(arg(0).integer >= arg(1).integer);
}

function(brl_std_condition_less)
{
    return(arg(0).integer < arg(1).integer);
}

function(brl_std_condition_less_equals)
{
    return(arg(0).integer <= arg(1).integer);
}

function(brl_std_condition_not)
{
    if (arg(0).integer)
    {
        return 0;
    }
    return 1;
}

function(brl_std_condition_and)
{
    if (arg(0).integer && arg(1).integer)
    {
        return 1;
    }
    return 0;
}

function(brl_std_condition_raw_or)
{
    if (arg(0).integer || arg(1).integer)
    {
        return 1;
    }
    return 0;
}

function(brl_std_group)//group interpreter
{
    stack_reverse(*args);
    Int lst = new_list(vm);
    IntList *list = (IntList*)data(lst).pointer;

    Int _add_index = hash_find(vm, "+");
    Int _sub_index = hash_find(vm, "-");
    Int _from_index = hash_find(vm, "from");
    Int _to_index = hash_find(vm, "to");

    Int from,to;
    from = 0;
    to = vm->stack->size-1;
    while (args->size > 0)
    {
        Int opt = stack_pop(*args);
        if (opt == _from_index)
        {
            from = stack_pop(*args);
        }
        else if (opt == _to_index)
        {
            to = stack_pop(*args);
        }
        else if (opt == _add_index)
        {
            to = from + stack_pop(*args);
        }
        else if (opt == _sub_index)
        {
            to = from - stack_pop(*args);
        }
        else 
        {
            from = opt;
        }
    }

    if (from < 0)
    {
        from = 0;
    }
    else if (from >= vm->stack->size)
    {
        from = vm->stack->size - 1;
    }
    
    if (to < 0)
    {
        to = 0;
    }
    else if (to >= vm->stack->size)
    {
        to = vm->stack->size - 1;
    }

    if (from > to)
    {
        for (Int i = from; i >= to; i--)
        {
            stack_push(*list, i);
        }
    }
    else 
    {
        for (Int i = from; i <= to; i++)
        {
            stack_push(*list, i);
        }
    }

    return lst;
}

// std loop
// std loop
// std loop

function(brl_std_loop_while)
{
    Int result = -1;
    while (eval(vm, arg(0).string, context))
    {
        result = eval(vm, arg(1).string, context);

        if (result >= 0)
        {
            break;
        }
    }
    return result;
}

function(brl_std_loop_repeat)
{
    for (Int i = 0; i < arg(0).number; i++)
    {
        eval(vm, arg(1).string, context);
    }
    return -1;
}

function(brl_std_loop_each)
{
    IntList *list = (IntList*)arg(0).pointer;
    register_var(vm, arg(1).string);
    Int hash_index = -1;
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (strcmp(hash(i).key, arg(1).string) == 0)
        {
            hash_index = i;
            break;
        }
    }

    for (Int i = 0; i < list->size; i++)
    {
        hash(hash_index).index = list->data[i];

        eval(vm, arg(2).string, context);
    }
    return -1;
}

function(brl_mem_swap)
{
    if (args->size == 2)
    {
        if (arg_i(0) >= 0 && arg_i(0) < vm->stack->size && arg_i(1) >= 0 && arg_i(1) < vm->stack->size)
        {
            Value temp = arg(0);
            char type = arg_t(0);
            arg(0) = arg(1);
            arg_t(0) = arg_t(1);
            arg(1) = temp;
            arg_t(1) = type;
        }
    }
    return -1;
}

function(brl_mem_set)
{
    if (arg_i(0) >= 0 && arg_i(0) < vm->stack->size)
    {
        arg(0) = arg(1);
        arg_t(0) = arg_t(1);
    }
    
    return -1;
}

function(brl_mem_copy)
{
    Int newvar = new_var(vm);
    data(newvar) = value_duplicate(data(arg_i(0)), data_t(arg_i(0)));
    return newvar;
}

function(brl_mem_length)
{
    return new_number(vm, vm->stack->size);
}

function(brl_mem_get)
{
    return((Int)arg(0).number);
}

function(brl_mem_delete)
{
    for (Int i = 0; i < args->size; i++)
    {
        unuse_var(vm, arg_i(i));
    }
    return -1;
}

function(brl_mem_next)
{
    for (Int i = 0; i < args->size; i++)
    {
        stack_push(*vm->unused, arg_i(i));
    }
    return -1;
}

function(brl_std_mem_push)
{
    while (args->size > 0)
    {
        stack_push(*vm->stack, arg(0));
        stack_push(*vm->typestack, arg_t(0));
    }
    return -1;
}

function(brl_std_mem_unshift)
{
    for (Int i = args->size - 1; i >= 0; i--)
    {
        stack_unshift(*vm->stack, arg(i));
        stack_unshift(*vm->typestack, arg_t(i));
    }

    for (Int i = 0; i < vm->hashes->size; i++)
    {
        hash(i).index += args->size;
    }
    
    return -1;
}

function(brl_std_mem_pop)
{
    for (Int i = 0; i < arg(0).number; i++)
    {
        unuse_var(vm, vm->stack->size-1);
        stack_pop(*vm->stack);
        stack_pop(*vm->typestack);
    }
    return -1;
}

function(brl_std_mem_shift)
{
    for (Int i = 0; i < arg(0).number; i++)
    {
        unuse_var(vm, 0);
        stack_shift(*vm->stack);
        stack_shift(*vm->typestack);
    }
    
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        hash(i).index -= arg(0).number;
    }

    return -1;
}

function(brl_std_mem_find)
{
    for (Int i = 0; i < vm->stack->size; i++)
    {
        if (data(i).integer == arg(0).integer)
        {
            return i;
        }
    }
    return -1;
}

// inits
#ifndef ARDUINO
void init_os(VirtualMachine *vm)
{
    register_builtin(vm, "file.read", brl_os_file_read);
    register_builtin(vm, "file.write", brl_os_file_write);
    register_builtin(vm, "file.delete", brl_os_file_delete);
    register_builtin(vm, "file.exists", brl_os_file_exists);
    register_builtin(vm, "file.rename", brl_os_file_rename);
    register_builtin(vm, "file.copy", brl_os_file_copy);
    register_builtin(vm, "file.size", brl_os_file_size);
    register_builtin(vm, "system", brl_os_system);

#ifndef __wasm__
    register_builtin(vm, "os.time", brl_os_time_now);
    register_builtin(vm, "os.clock", brl_os_time_clock);
#endif

    register_builtin(vm, "dofile", brl_os_dofile);
    register_builtin(vm, "repl", brl_os_repl);
}
#endif

void init_basics(VirtualMachine *vm)
{
    register_builtin(vm, "#", brl_std_ignore);
    register_builtin(vm, "copy", brl_mem_copy);
    register_builtin(vm, "do", brl_std_do);
    register_builtin(vm, "return", brl_std_return);
    register_builtin(vm, "ls", brl_std_io_ls);
    register_builtin(vm, "ls.type", brl_std_io_ls_types);
    register_builtin(vm, "ls.hash", brl_std_io_ls_hashes);
    register_builtin(vm, "print", brl_std_io_print);
    register_builtin(vm, "group", brl_std_group);

    register_builtin(vm, "fn", brl_std_function);
}

void init_type(VirtualMachine *vm)
{
    // types
    register_number(vm, "type.size", sizeof(Value));
    register_number(vm, "type.nil", TYPE_NIL);
    register_number(vm, "type.number", TYPE_NUMBER);
    register_number(vm, "type.integer", TYPE_INTEGER);
    register_number(vm, "type.string", TYPE_STRING);
    register_number(vm, "type.builtin", TYPE_BUILTIN);
    register_number(vm, "type.list", TYPE_LIST);
    register_number(vm, "type.function", TYPE_FUNCTION);
    register_number(vm, "type.other", TYPE_OTHER);

    // type functions
    register_builtin(vm, "type.get", brl_std_type_get);
    register_builtin(vm, "type.set", brl_std_type_set);
}

void init_loop(VirtualMachine *vm)
{
    register_builtin(vm, "while", brl_std_loop_while);
    register_builtin(vm, "repeat", brl_std_loop_repeat);
    register_builtin(vm, "each", brl_std_loop_each);
}

void init_hash(VirtualMachine *vm)
{
    register_builtin(vm, "#set", brl_std_hash_set);
    register_builtin(vm, "#get", brl_std_hash_get);
    register_builtin(vm, "#delete", brl_std_hash_delete);
    register_builtin(vm, "#rename", brl_std_hash_rename);
}

void init_math(VirtualMachine *vm)
{
    register_builtin(vm, "+", brl_std_math_add);
    register_builtin(vm, "-", brl_std_math_sub);
    register_builtin(vm, "*", brl_std_math_mul);
    register_builtin(vm, "/", brl_std_math_div);
    register_builtin(vm, "\%", brl_std_math_mod);
    register_builtin(vm, "pow", brl_std_math_pow);
    register_builtin(vm, "abs", brl_std_math_abs);
    register_builtin(vm, "ceil", brl_std_math_ceil);
    register_builtin(vm, "floor", brl_std_math_floor);
    register_builtin(vm, "round", brl_std_math_round);
    register_builtin(vm, "random", brl_std_math_random);
    register_builtin(vm, "seed", brl_std_math_seed);
    register_builtin(vm, "incr", brl_std_math_increment);
    register_builtin(vm, "decr", brl_std_math_decrement);

    register_builtin(vm, "to.float", brl_std_to_float);
    register_builtin(vm, "to.int", brl_std_to_int);
}

void init_string(VirtualMachine *vm)
{
    register_builtin(vm, "string.set", brl_std_string_set);
    register_builtin(vm, "string.format", brl_std_string_format);
    register_builtin(vm, "string.to.number", brl_std_string_to_number);
    register_builtin(vm, "string.split", brl_std_string_split);
}

void init_condition(VirtualMachine *vm)
{
    register_builtin(vm, "or", brl_std_condition_or);
    
    register_builtin(vm, "if", brl_std_condition_if);
    register_builtin(vm, "ifelse", brl_std_condition_ifelse);

    register_builtin(vm, "==", brl_std_condition_equals);
    register_builtin(vm, "!=", brl_std_condition_not_equals);
    register_builtin(vm, ">", brl_std_condition_greater);
    register_builtin(vm, ">=", brl_std_condition_greater_equals);
    register_builtin(vm, "<", brl_std_condition_less);
    register_builtin(vm, "<=", brl_std_condition_less_equals);
    register_builtin(vm, "not", brl_std_condition_not);
    register_builtin(vm, "&&", brl_std_condition_and);
    register_builtin(vm, "||", brl_std_condition_raw_or);
}

void init_list(VirtualMachine *vm)
{
    register_builtin(vm, "list", brl_std_list_new);

    register_builtin(vm, ":pop", brl_std_list_pop);
    register_builtin(vm, ":get", brl_std_list_get);
    register_builtin(vm, ":set", brl_std_list_set);
    register_builtin(vm, ":len", brl_std_list_length);
    register_builtin(vm, ":push", brl_std_list_push);
    register_builtin(vm, ":find", brl_std_list_find);
    register_builtin(vm, ":shift", brl_std_list_shift);
    register_builtin(vm, ":concat", brl_std_list_concat);
    register_builtin(vm, ":unshift", brl_std_list_unshift);
    register_builtin(vm, ":reverse", brl_std_list_reverse);
    register_builtin(vm, ":insert", brl_std_list_insert);
    register_builtin(vm, ":remove", brl_std_list_remove);
    register_builtin(vm, ":split", brl_std_list_split);

    register_builtin(vm, ":sub", brl_std_list_sub);
    register_builtin(vm, ":replace", brl_std_list_replace);
    register_builtin(vm, ":replace.all", brl_std_list_replace_all);
}

void init_mem(VirtualMachine *vm)
{
    register_number(vm, "mem.size", sizeof(Float));
    register_builtin(vm, "mem.get", brl_mem_get);
    register_builtin(vm, "mem.set", brl_mem_set);
    register_builtin(vm, "mem.copy", brl_mem_copy);
    register_builtin(vm, "mem.len", brl_mem_length);
    register_builtin(vm, "mem.delete", brl_mem_delete);
    register_builtin(vm, "mem.next", brl_mem_next);
    register_builtin(vm, "mem.swap", brl_mem_swap);
    register_builtin(vm, "mem.push", brl_std_mem_push);
    register_builtin(vm, "mem.unshift", brl_std_mem_unshift);
    register_builtin(vm, "mem.pop", brl_std_mem_pop);
    register_builtin(vm, "mem.shift", brl_std_mem_shift);
    register_builtin(vm, "mem.find", brl_std_mem_find);
}

// std init presets
void init_std(VirtualMachine *vm)
{
    #ifndef ARDUINO
    init_os(vm);
    #endif
    init_basics(vm);
    init_type(vm);
    init_loop(vm);
    init_hash(vm);
    init_list(vm);
    init_math(vm);
    init_string(vm);
    init_condition(vm);
    init_mem(vm);
    register_string(vm, "VERSION", VERSION);// version
    hash_set(vm, "from", -2);
    hash_set(vm, "to", -3);
}