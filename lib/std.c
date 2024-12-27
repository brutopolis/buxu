#include "bruter.h"

// functions defitions for internal use
// functions defitions for internal use
// functions defitions for internal use
// functions defitions for internal use
// functions defitions for internal use


char is(VirtualMachine *vm, char* str, HashList *context)
{
    char condition_result = 0;
    Float cond[4] = {0, 0, 0, 0};
    StringList *splited = special_split(str, ' ');
    Int index = -1;
    Int step = -1;
    char _new_condition_result = 0;
    
    while (index < splited->size)
    {
        index++;
        step++;
        switch(step)
        {
            case 1:
                if (index >= splited->size)
                {
                    _new_condition_result = (cond[0] != 0);
                    goto condition_processing;
                }

                switch (splited->data[index][0])
                {
                    case '=':
                        cond[1] = 1;
                        break;
                    case '!':
                        cond[1] = 2;
                        break;
                    case '>':
                        cond[1] = 3;
                        break;
                    case '<': 
                        cond[1] = 4;
                        break;
                    default:
                        // error
                        printf("error: cant handle this condition (%s)\n", splited->data[index]);
                        exit(1);
                        break;
                }
                break;
            case 3:
                _new_condition_result = 0;

                switch ((Int)cond[1])
                {
                    case 1: // ==
                        _new_condition_result = (cond[0] == cond[2]);
                        break;
                    case 2: // !=
                        _new_condition_result = (cond[0] != cond[2]);
                        break;
                    case 3: // >
                        _new_condition_result = (cond[0] >  cond[2]);
                        break;
                    case 4: // <
                        _new_condition_result = (cond[0] <  cond[2]);
                        break;
                    case 5: // >=
                        _new_condition_result = (cond[0] >= cond[2]);
                        break;
                    case 6: // <=
                        _new_condition_result = (cond[0] <= cond[2]);
                        break;
                }
                condition_processing:

                switch ((Int)cond[3])
                {    
                    case 1:
                        _new_condition_result = condition_result && _new_condition_result;
                        break;
                    case 2:
                        _new_condition_result = condition_result || _new_condition_result;
                        break;
                }

                
                condition_result = _new_condition_result;


                if (index < splited->size-1)
                {
                    if (strcmp(splited->data[index], "&&") == 0)
                    {
                        cond[3] = 1;
                    }
                    else if (strcmp(splited->data[index], "||") == 0)
                    {
                        cond[3] = 2;
                    }
                    else
                    {
                        // error
                        printf("error: cant handle this symbol (%s)\n", splited->data[index]);
                        exit(1);
                    }
                }
                else 
                {
                    return condition_result;
                }

                step = -1;
                break;
            default:// 0 or 2
                if (splited->data[index][0] == '@')
                {
                    cond[step] = data(atoi(splited->data[index]+1)).number;
                }
                else if ((splited->data[index][0] >= '0' && splited->data[index][0] <= '9') || splited->data[index][0] == '-')
                {
                    cond[step] = atof(splited->data[index]);
                }
                else if (splited->data[index][0] == '(')
                {
                    if (splited->data[index][1] == ':')
                    {
                        char* _str = str_nduplicate(splited->data[index]+2, strlen(splited->data[index]) - 3);
                        cond[step] = eval(vm, _str, context);
                        free(_str);
                    }
                    else 
                    {
                        char* _str = str_sub(splited->data[index], 1, strlen(splited->data[index]) - 1);
                        cond[step] = is(vm, _str, context);
                        free(_str);
                    }
                }
                else
                {
                    Int _index;
                    if (context != NULL)
                    {
                        void *backup = vm->hashes;
                        vm->hashes = context;
                        _index = hash_find(vm, splited->data[index]);
                        vm->hashes = backup;
                        if (_index == -1)
                        {
                            _index = hash_find(vm, splited->data[index]);
                        }
                        cond[step] = data(_index).number;
                    }
                    else 
                    {
                        _index = hash_find(vm, splited->data[index]);
                        cond[step] = data(_index).number;
                    }
                }
                break;
        }
        
    }
    printf("error: misformed condition: %s\n", str);
    return -1;
}

Int solve_index(VirtualMachine *vm, char* token, HashList *context)
{
    Int result = 0;
    if (token[0] == '@')
    {
        result = data(atoi(token+1)).number;
    }
    else if ((token[0] >= '0' && token[0] <= '9') || token[0] == '-')
    {
        result = atoi(token);
    }
    else if (token[0] == '(')
    {
        if (token[1] == ':')
        {
            char* _str = str_nduplicate(token+2, strlen(token) - 3);
            result = eval(vm, _str, context);
            free(_str);
        }
        else
        {
            char* _str = str_sub(token, 1, strlen(token) - 1);
            result = imath(vm, _str, context);
            free(_str);
        }
    }
    else
    {
        Int _index;
        if (context != NULL)
        {
            void *backup = vm->hashes;
            vm->hashes = context;
            _index = hash_find(vm, token);
            vm->hashes = backup;
            if (_index == -1)
            {
                _index = hash_find(vm, token);
            }
            result = _index;
        }
        else 
        {
            _index = hash_find(vm, token);
            result = _index;
        }
    }
    return result;
}

Int imath(VirtualMachine *vm, char* str, HashList *context)
{
    Int a = 0;
    Int b = 0;
    StringList *splited = special_split(str, ' ');
    char* token = stack_shift(*splited);
    char operator = 0;

    a = solve_index(vm, token, context);
    free(token);
    
    while (splited->size > 0)
    {
        token = stack_shift(*splited);
        operator = token[0];
        free(token);
        token = stack_shift(*splited);
        b = solve_index(vm, token, context);
        free(token);
        switch (operator)
        {
            case '+':
                a += b;
                break;
            case '-':
                a -= b;
                break;
            case '*':
                a *= b;
                break;
            case '/':
                a /= b;
                break;
            case '%':
                a %= b;
                break;
            case '&': // bitwise and
                a = (Int)a & (Int)b;
                break;
            case '|':
                a = (Int)a | (Int)b;
                break;
            case '>':
                a = (Int)a >> (Int)b;
                break;
            case '<':
                a = (Int)a << (Int)b;
                break;
            case '^':
                a = (Int)a ^ (Int)b;
                break;
            case '~':
                a = ~(Int)a;
                break;
            default:
                // error
                printf("error: cant handle this operator (%c)\n", operator);
                exit(1);
                break;
        }
    }
    stack_free(*splited);
    return a;
}

Float solve_number(VirtualMachine *vm, char* token, HashList *context)
{
    Float result = 0;
    if (token[0] == '@')
    {
        result = data(atoi(token+1)).number;
    }
    else if ((token[0] >= '0' && token[0] <= '9') || token[0] == '-')
    {
        result = atof(token);
    }
    else if (token[0] == '(')
    {
        if (token[1] == ':')
        {
            char* _str = str_nduplicate(token+2, strlen(token) - 3);
            result = data(eval(vm, _str, context)).number;
            free(_str);
        }
        else
        {
            char* _str = str_sub(token, 1, strlen(token) - 1);
            result = math(vm, _str, context);
            free(_str);
        }
    }
    else
    {
        Int _index;
        if (context != NULL)
        {
            void *backup = vm->hashes;
            vm->hashes = context;
            _index = hash_find(vm, token);
            vm->hashes = backup;
            if (_index == -1)
            {
                _index = hash_find(vm, token);
            }
            result = data(_index).number;
        }
        else 
        {
            _index = hash_find(vm, token);
            result = data(_index).number;
        }
    }
    return result;
}

Float math(VirtualMachine *vm, char* str, HashList *context)
{
    Float a = 0;
    Float b = 0;
    StringList *splited = special_split(str, ' ');
    char* token = stack_shift(*splited);
    char operator = 0;

    a = solve_number(vm, token, context);
    free(token);
    
    while (splited->size > 0)
    {
        token = stack_shift(*splited);
        operator = token[0];
        free(token);
        token = stack_shift(*splited);
        b = solve_number(vm, token, context);
        free(token);
        switch (operator)
        {
            case '+':
                a += b;
                break;
            case '-':
                a -= b;
                break;
            case '*':
                a *= b;
                break;
            case '/':
                a /= b;
                break;
            case '%':
                a = fmod(a, b);
                break;
            case '&': // bitwise and
                a = (Int)a & (Int)b;
                break;
            case '|':
                a = (Int)a | (Int)b;
                break;
            case '>':
                a = (Int)a >> (Int)b;
                break;
            case '<':
                a = (Int)a << (Int)b;
                break;
            case '^':
                a = (Int)a ^ (Int)b;
                break;
            case '~':
                a = ~(Int)a;
                break;
            default:
                // error
                printf("error: cant handle this operator (%c)\n", operator);
                exit(1);
                break;
        }
    }
    stack_free(*splited);
    return a;
}

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

function(brl_os_time_now)
{
    return new_number(vm, time(NULL));
}

function(brl_os_time_clock)
{
    return new_number(vm, clock());
}

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
    while (args->size > 1)
    {
        stack_push(*varnames, str_duplicate(data(stack_shift(*args)).string));
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

function(brl_std_gindex)
{
    return new_number(vm, arg_i(0));
}

function(brl_std_type_get)
{
    Int result = new_number(vm, arg_t(0));
    return result;
}

function(brl_std_type_set)
{
    arg_t(0) = (Int)arg(0).number;
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

// tree-walker math
function(brl_std_math)
{
    char* str = arg(0).string;
    char* _str = str_nduplicate(str, strlen(str));
    Int result = new_number(vm, math(vm, _str, context));
    free(_str);
    return result;
}

function(brl_std_imath)
{
    char* str = arg(0).string;
    char* _str = str_nduplicate(str, strlen(str));
    Int result = imath(vm, _str, context);
    free(_str);
    return result;
}


// list functions
// list functions
// list functions
// list functions

function(brl_std_list_new)
{
    Int index = new_list(vm);
    IntList *list = (IntList*)data(index).pointer;
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
    return -1;   
}

function(brl_std_list_pop)// returns the removed element
{
    if (arg_t(0) == TYPE_LIST)
    {
        IntList *list = (IntList*)arg(0).pointer;
        return stack_pop(*list);
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
    return -1;
}

function(brl_std_list_concat)
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

function(brl_std_list_find)
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
    return -1;
}

function(brl_std_list_get)
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
            return -1;
        }
    }
    return -1;
}

function(brl_std_list_set)
{
    Int list = arg_i(0);
    Int index = arg(1).number;
    Int value = arg_i(2);
    if (arg_t(0) == TYPE_LIST)
    {
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
    }
    return -1;
}

function(brl_std_list_length)
{
    Int list = arg_i(0);
    return -1;
}

function(brl_std_list_last)
{
    Int list = arg_i(0);
    if (arg_t(0) == TYPE_LIST)
    {
        IntList *lst = (IntList*)data(list).pointer;
        return lst->data[lst->size - 1];
    }
    return -1;
}

// std string

function(brl_std_string_concat)
{
    Int _newstr = new_string(vm, "");
    char* newstr = data(_newstr).string;
    for (Int i = 0; i < args->size; i++)
    {
        if (vm->typestack->data[args->data[i]] == TYPE_STRING)
        {
            strcat(newstr, vm->stack->data[args->data[i]].string);
        }
    }
    return _newstr;
}

function(brl_std_string_find)
{
    return new_number(vm, str_find(arg(0).string, arg(1).string));
}

function(brl_std_string_ndup)
{
    char* _str = str_nduplicate(arg(0).string, arg(1).number);
    Int result = new_string(vm, _str);
    return result;
}

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

function(brl_std_string_replace)
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

function(brl_std_string_replace_all)
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

function(brl_std_string_to_number)
{
    Int str = arg_i(0);
    return new_number(vm, atof(data(str).string));
}

function(brl_std_string_length)
{
    Int str = arg_i(0);
    return new_number(vm, strlen(data(str).string));
}

function(brl_std_string_format)
{
    Int str = stack_shift(*args);
    Int result = -1;
    char* _str = str_duplicate(data(str).string);
    for (Int i = 0; i < strlen(_str); i++)
    {
        if (_str[i] == '%')
        {
            if (_str[i+1] == 'd')
            {
                Int value = stack_shift(*args);
                char* _value = str_format("%ld", (Int)data(value).number);
                char* _newstr = str_replace(_str, "\%d", _value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 's')
            {
                Int value = stack_shift(*args);
                char* _value = data(value).string;
                char* _newstr = str_replace(_str, "\%s", _value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 'f')
            {
                Int value = stack_shift(*args);
                char* _value = str_format("%f", data(value).number);
                char* _newstr = str_replace(_str, "\%f", _value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 'p')
            {
                Int value = stack_shift(*args);
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
    if (is(vm, arg(0).string, context))
    {
        result = eval(vm, arg(1).string, context);

    }
    return result;
}

function(brl_std_condition_ifelse)
{
    Int result = -1;
    if (is(vm, arg(0).string, context))
    {
        result = eval(vm, arg(1).string, context);
    }
    else
    {
        result = eval(vm, arg(2).string, context);
    }
    return result;
}

function(brl_std_is)
{
    char* str = arg(0).string;
    char* _str = str_nduplicate(str, strlen(str));
    Int result = new_number(vm, is(vm, _str, context));
    free(_str);
    return result;
}

function(brl_std_group)//group interpreter
{
    Int _str = stack_shift(*args);
    StringList *splited = special_split(data(_str).string, ' ');

    Int lst = new_list(vm);
    IntList *list = (IntList*)data(lst).pointer;

    Int from,to;
    from = 0;
    to = vm->stack->size-1;
    while (splited->size > 0)
    {
        char* str = stack_shift(*splited);
        if (strcmp(str, "from") == 0)
        {
            free(str);
            str = stack_shift(*splited);
            from = atoi(str);
        }
        else if (strcmp(str, "to") == 0)
        {
            free(str);
            str = stack_shift(*splited);
            to = atoi(str);
        }
        else if (strcmp(str, "+") == 0)
        {
            free(str);
            str = stack_shift(*splited);
            to = from + atoi(str);
        }
        else if (strcmp(str, "-") == 0)
        {
            free(str);
            str = stack_shift(*splited);
            to = from - atoi(str);
        }
        else if (strcmp(str, "*") == 0)
        {
            free(str);
            str = stack_shift(*splited);
            to = from * atoi(str);
        }
        else if (strcmp(str, "/") == 0)
        {
            free(str);
            str = stack_shift(*splited);
            to = from / atoi(str);
        }
        else if (strcmp(str, "\%") == 0)
        {
            free(str);
            str = stack_shift(*splited);
            to = from % atoi(str);
        }
        else if (strcmp(str, "&") == 0)
        {
            free(str);
            str = stack_shift(*splited);
            to = from & atoi(str);
        }
        else if (strcmp(str, "|") == 0)
        {
            free(str);
            str = stack_shift(*splited);
            to = from | atoi(str);
        }
        else if (strcmp(str, "^") == 0)
        {
            free(str);
            str = stack_shift(*splited);
            to = from ^ atoi(str);
        }
        else if (strcmp(str, "~") == 0)
        {
            free(str);
            to = ~from;
        }
        free(str);
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

    stack_free(*splited);
    return lst;
}

// std loop
// std loop
// std loop

function(brl_std_loop_while)
{
    Int result = -1;
    while (is(vm, arg(0).string, context))
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
    for (Int i = 0; i < list->size; i++)
    {
        hash_set(vm, arg(1).string, list->data[i]);
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

function(brl_mem_get)
{
    if (arg(0).number >= 0 && arg(0).number < vm->stack->size)
    {
        return arg_i(0);
    }
    
    return -1;
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

function(brl_mem_length)
{
    return new_number(vm, vm->stack->size);
}

function(brl_mem_sector_new)
{
    Int index = new_var(vm);
    while ((arg(0).number - 1) > 0)
    {
        new_var(vm);
        arg(0).number--;
    }
    return index;
}

function(brl_mem_sector_copy)
{
    while (arg(2).number > 0)
    {
        if (arg_i(0) >= 0 && arg_i(0) < vm->stack->size)
        {
            if (arg_i(1) >= 0 && arg_i(1) < vm->stack->size)
            {
                arg(1) = value_duplicate(arg(0), arg_t(0));
                arg_t(1) = arg_t(0);
            }
        }
        arg_i(0)++;
        arg_i(1)++;
        arg(2).number--;
    }
    return -1;
}

function(brl_mem_sector_swap)
{
    while (arg(2).number > 0)
    {
        if (arg_i(0) >= 0 && arg_i(0) < vm->stack->size)
        {
            if (arg_i(1) >= 0 && arg_i(1) < vm->stack->size)
            {
                Value temp = arg(0);
                char type = arg_t(0);
                arg(0) = arg(1);
                arg_t(0) = arg_t(1);
                arg(1) = temp;
                arg_t(1) = type;
            }
        }
        arg_i(0)++;
        arg_i(1)++;
        arg(2).number--;
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

    register_builtin(vm, "os.time", brl_os_time_now);
    register_builtin(vm, "os.clock", brl_os_time_clock);

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
    register_builtin(vm, "gindex", brl_std_gindex);
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
    register_builtin(vm, "hash.set", brl_std_hash_set);
    register_builtin(vm, "hash.get", brl_std_hash_get);
    register_builtin(vm, "hash.delete", brl_std_hash_delete);
    register_builtin(vm, "hash.rename", brl_std_hash_rename);
}

void init_math(VirtualMachine *vm)
{
    register_builtin(vm, "add", brl_std_math_add);
    register_builtin(vm, "sub", brl_std_math_sub);
    register_builtin(vm, "mul", brl_std_math_mul);
    register_builtin(vm, "div", brl_std_math_div);
    register_builtin(vm, "mod", brl_std_math_mod);
    register_builtin(vm, "pow", brl_std_math_pow);
    register_builtin(vm, "abs", brl_std_math_abs);
    register_builtin(vm, "ceil", brl_std_math_ceil);
    register_builtin(vm, "seed", brl_std_math_seed);
    register_builtin(vm, "floor", brl_std_math_floor);
    register_builtin(vm, "round", brl_std_math_round);
    register_builtin(vm, "random", brl_std_math_random);
    register_builtin(vm, "incr", brl_std_math_increment);
    register_builtin(vm, "decr", brl_std_math_decrement);
    register_builtin(vm, "math", brl_std_math);
    register_builtin(vm, "index", brl_std_imath);
}

void init_string(VirtualMachine *vm)
{
    register_builtin(vm, "string.sub", brl_std_string_ndup);
    register_builtin(vm, "string.find", brl_std_string_find);
    register_builtin(vm, "string.len", brl_std_string_length);
    register_builtin(vm, "string.split", brl_std_string_split);
    register_builtin(vm, "string.concat", brl_std_string_concat);
    register_builtin(vm, "string.format", brl_std_string_format);
    register_builtin(vm, "string.replace", brl_std_string_replace);
    register_builtin(vm, "string.to.number", brl_std_string_to_number);
    register_builtin(vm, "string.replace.all", brl_std_string_replace_all);
}

void init_condition(VirtualMachine *vm)
{
    register_builtin(vm, "or", brl_std_condition_or);
    register_builtin(vm, "if", brl_std_condition_if);
    register_builtin(vm, "ifelse", brl_std_condition_ifelse);
    register_builtin(vm, "is", brl_std_is);
}

void init_list(VirtualMachine *vm)
{
    register_builtin(vm, "list.new", brl_std_list_new);
    register_builtin(vm, "list.pop", brl_std_list_pop);
    register_builtin(vm, "list.get", brl_std_list_get);
    register_builtin(vm, "list.len", brl_std_list_length);
    register_builtin(vm, "list.push", brl_std_list_push);
    register_builtin(vm, "list.find", brl_std_list_find);
    register_builtin(vm, "list.shift", brl_std_list_shift);
    register_builtin(vm, "list.concat", brl_std_list_concat);
    register_builtin(vm, "list.unshift", brl_std_list_unshift);
    register_builtin(vm, "list.last", brl_std_list_last);
    register_builtin(vm, "list.set", brl_std_list_set);
}

void init_sector(VirtualMachine *vm)
{
    register_builtin(vm, "sector.new", brl_mem_sector_new);
    register_builtin(vm, "sector.copy", brl_mem_sector_copy);
    register_builtin(vm, "sector.swap", brl_mem_sector_swap);
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

    init_sector(vm);
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
}