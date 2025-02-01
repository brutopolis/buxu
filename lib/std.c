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

function(brl_os_repl)
{
    return repl(vm);
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

function(brl_std_hash_new)
{
    if (hash_find(vm, arg(0).string) != -1)
    {
        hash_unset(vm, arg(0).string);
    }

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

function(brl_std_io_scan) // always get string
{
    char *str = (char*)malloc(1024);
    fgets(str, 1024, stdin);
    str[strlen(str) - 1] = '\0';
    Int index = new_var(vm);
    data(index).string = str;
    data_t(index) = TYPE_STRING;
    return index;
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
    if (args->size == 1) // push to global vm->stack
    {
        stack_push(*vm->stack, value_duplicate(arg(0), arg_t(0)));
        stack_push(*vm->typestack, arg_t(0));
    }
    else if (arg_t(0) == TYPE_LIST)
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
    if (args->size == 1) // unshift to global vm->stack
    {
        stack_unshift(*vm->stack, value_duplicate(arg(0), arg_t(0)));
        stack_unshift(*vm->typestack, arg_t(0));
        // update hashes and lists indexes
        for (Int i = 0; i < vm->hashes->size; i++)
        {
            vm->hashes->data[i].index++;
        }

        for (Int i = 0; i < vm->stack->size; i++)
        {
            if (data_t(i) == TYPE_LIST)
            {
                IntList *list = (IntList*)data(i).pointer;
                for (Int j = 0; j < list->size; j++)
                {
                    list->data[j]++;
                }
            }
        }
    }
    else if (arg_t(0) == TYPE_LIST)
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
    if (args->size == 0) // pop from global vm->stack
    {
        stack_pop(*vm->stack);
        stack_pop(*vm->typestack);
    }
    else if (arg_t(0) == TYPE_LIST)
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
    if (args->size == 0) // shift from global vm->stack
    {
        stack_shift(*vm->stack);
        stack_shift(*vm->typestack);
        // update hashes and lists indexes
        for (Int i = 0; i < vm->hashes->size; i++)
        {
            vm->hashes->data[i].index--;
        }

        for (Int i = 0; i < vm->stack->size; i++)
        {
            if (data_t(i) == TYPE_LIST)
            {
                IntList *list = (IntList*)data(i).pointer;
                for (Int j = 0; j < list->size; j++)
                {
                    list->data[j]--;
                }
            }
        }
    }
    else if (arg_t(0) == TYPE_LIST)
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
    if (args->size == 2) // insert to global vm->stack
    {
        // lets do it  with push
        Value v = value_duplicate(arg(0), arg_t(0));
        stack_push(*vm->stack, v);
        stack_push(*vm->typestack, arg_t(0));
        // relocate the element to the desired index
        for (Int i = vm->stack->size - 1; i > arg_i(1); i--)
        {
            vm->stack->data[i] = vm->stack->data[i - 1];
        }
        vm->stack->data[arg_i(1)] = v;

        // update hashes and lists indexes
        for (Int i = 0; i < vm->hashes->size; i++)
        {
            if (hash(i).index >= arg_i(1))
            {
                hash(i).index++;
            }
        }
    }
    else if (arg_t(0) == TYPE_LIST)
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
    if (args->size == 1) // remove from global vm->stack
    {
        stack_remove(*vm->stack, arg_i(0));
        stack_remove(*vm->typestack, arg_i(0));
        // update hashes and lists indexes
        for (Int i = 0; i < vm->hashes->size; i++)
        {
            if (hash(i).index >= arg_i(0))
            {
                hash(i).index--;
            }
        }
    }
    else if (arg_t(0) == TYPE_LIST)
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
    if (args->size == 1) // duplicate and concat each element from the list
    {
        IntList* list = (IntList*)data(arg_i(0)).pointer;
        for (Int i = 0; i < list->size; i++)
        {
            stack_push(*vm->stack, value_duplicate(data(list->data[i]), data_t(list->data[i])));
            stack_push(*vm->typestack, data_t(list->data[i]));
        }
    }
    else if (arg_t(0) == TYPE_LIST)
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
    if (args->size == 1)
    {
        Int value = arg_i(0);
        for (Int i = 0; i < vm->stack->size; i++)
        {
            if (data(i).integer == value)
            {
                return i;
            }
        }
    }
    else if (arg_t(0) == TYPE_LIST)
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
    if (args->size == 1)
    {
        return((Int)arg(0).number);
    }
    else if (arg_t(0) == TYPE_LIST)
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
    if (args->size == 2)
    {
        if (arg_i(0) >= 0 && arg_i(0) < vm->stack->size)
        {
            // if the value is a list or a string, we need to free it before
            if (arg_t(0) == TYPE_LIST || arg_t(0) == TYPE_STRING)
            {
                unuse_var(vm, arg_i(0));
            }

            arg(0) = arg(1);
            arg_t(0) = arg_t(1);
        }
    }
    else if (arg_t(0) == TYPE_LIST)
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
        if (args->size > 3) // receive a bunch of chars
        {
            for (Int i = 3; i < args->size - 2; i++)
            {
                str[index + i] = arg_i(i + 2);
            }
        }
        else if (index >= 0 && index < strlen(str))
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
    if (args->size == 0)
    {
        return new_number(vm, vm->stack->size);
    }
    else if (arg_t(0) == TYPE_LIST)
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
    if (args->size == 0)
    {
        stack_reverse(*vm->stack);
        stack_reverse(*vm->typestack);
        // update hashes and lists indexes
        for (Int i = 0; i < vm->hashes->size; i++)
        {
            hash(i).index = vm->stack->size - hash(i).index - 1;
        }

        for (Int i = 0; i < vm->stack->size; i++)
        {
            if (data_t(i) == TYPE_LIST)
            {
                for (Int j = 0; j < ((IntList*)data(i).pointer)->size; j++)
                {
                    ((IntList*)data(i).pointer)->data[j] = vm->stack->size - ((IntList*)data(i).pointer)->data[j] - 1;
                }
            }
        }
    }
    else if (arg_t(0) == TYPE_LIST)
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
    if (args->size == 2)// create a list with the indexes of the elements in the range
    {
        IntList *list = make_int_list();
        Int start = arg_i(0);
        Int end = arg_i(1);
        for (Int i = start; i < end; i++)
        {
            stack_push(*list, i);
        }
        Int result = new_var(vm);
        data(result).pointer = list;
        data_t(result) = TYPE_LIST;
        return result;
    }
    else if (arg_t(0) == TYPE_STRING)
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
    if (args->size == 1)// split the global stack, create lists with the indexes of each slice
    {
        IntList *list = make_int_list();
        Int separator = arg(0).number;
        Int start = 0;
        for (Int i = 0; i < vm->stack->size; i++)
        {
            if (vm->stack->data[i].integer == separator)
            {
                Int sublist = new_list(vm);
                IntList *sub = (IntList*)data(sublist).pointer;
                for (Int j = start; j < i; j++)
                {
                    stack_push(*sub, j);
                }
                stack_push(*list, sublist);
                start = i + 1;
            }
        }
        Int sublist = new_list(vm);
        IntList *sub = (IntList*)data(sublist).pointer;
        for (Int j = start; j < vm->stack->size; j++)
        {
            stack_push(*sub, j);
        }
        stack_push(*list, sublist);
        Int result = new_var(vm);
        data(result).pointer = list;
        data_t(result) = TYPE_LIST;
        return result;
    }
    else if (arg_t(0) == TYPE_STRING)
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
    return -1;
}

function(brl_std_list_replace)
{
    if (args->size == 2)// replace elements from the global stack
    {
        Int target = arg_i(0);
        Int replacement = arg_i(1);
        for (Int i = 0; i < vm->stack->size; i++)
        {
            if (vm->stack->data[i].integer == vm->stack->data[target].integer)
            {
                // if list or string we need to free it before
                if (data_t(i) == TYPE_LIST || data_t(i) == TYPE_STRING)
                {
                    unuse_var(vm, i);
                }
                vm->stack->data[i] = vm->stack->data[replacement];
                break;
            }
        }
    }
    else if (arg_t(0) == TYPE_STRING)
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

function(brl_std_list_swap)
{
    if (args->size == 2)// swap elements from the global stack
    {
        Int index1 = arg_i(0);
        Int index2 = arg_i(1);
        Value temp = vm->stack->data[index1];
        vm->stack->data[index1] = vm->stack->data[index2];
        vm->stack->data[index2] = temp;
    }
    else if (arg_t(0) == TYPE_LIST)
    {
        Int list = arg_i(0);
        Int index1 = arg(1).number;
        Int index2 = arg(2).number;
        IntList *lst = (IntList*)data(list).pointer;
        if (index1 >= 0 && index1 < lst->size && index2 >= 0 && index2 < lst->size)
        {
            Int temp = lst->data[index1];
            lst->data[index1] = lst->data[index2];
            lst->data[index2] = temp;
        }
        else 
        {
            printf("error: index %d or %d out of range in list %d of size %d\n", index1, index2, list, lst->size);
            print_element(vm, list);
        }
    }
    else if (arg_t(0) == TYPE_STRING)
    {
        char *str = arg(0).string;
        Int index1 = arg(1).number;
        Int index2 = arg(2).number;
        if (index1 >= 0 && index1 < strlen(str) && index2 >= 0 && index2 < strlen(str))
        {
            char temp = str[index1];
            str[index1] = str[index2];
            str[index2] = temp;
        }
        else 
        {
            printf("error: index %d or %d out of range in string %d of size %d\n", index1, index2, arg(0).string, strlen(str));
            print_element(vm, arg_i(0));
        }
    }
    return -1;
}

// string functions

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
            else if (isdigit(_str[i+1]))
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

function(brl_mem_copy)
{
    Int newvar = new_var(vm);
    data(newvar) = value_duplicate(data(arg_i(0)), data_t(arg_i(0)));
    return newvar;
}

function(brl_mem_delete)
{
    for (Int i = 0; i < args->size; i++)
    {
        unuse_var(vm, arg_i(i));
    }
    return -1;
}


// inits
#ifndef ARDUINO
void init_os(VirtualMachine *vm)
{
    register_builtin(vm, "file.read", brl_os_file_read);
    register_builtin(vm, "file.write", brl_os_file_write);
    register_builtin(vm, "system", brl_os_system);

#ifndef __wasm__
    register_builtin(vm, "os.time", brl_os_time_now);
    register_builtin(vm, "os.clock", brl_os_time_clock);
#endif

    register_builtin(vm, "repl", brl_os_repl);
}
#endif

void init_basics(VirtualMachine *vm)
{
    register_builtin(vm, "#", brl_std_ignore);
    register_builtin(vm, "do", brl_std_do);
    register_builtin(vm, "return", brl_std_return);
    register_builtin(vm, "ls", brl_std_io_ls);
    register_builtin(vm, "help", brl_std_io_ls_hashes);
    register_builtin(vm, "function", brl_std_function);
    
    
    // string format
    register_builtin(vm, "format", brl_std_string_format);

    register_builtin(vm, "print", brl_std_io_print);

#ifndef ARDUINO
    register_builtin(vm, "scan", brl_std_io_scan);// not avaliable on arduino, so its here
#endif
}

void init_loop(VirtualMachine *vm)
{
    register_builtin(vm, "while", brl_std_loop_while);
    register_builtin(vm, "repeat", brl_std_loop_repeat);
}

void init_hash(VirtualMachine *vm)
{
    register_builtin(vm, "#new", brl_std_hash_new);
    register_builtin(vm, "#delete", brl_std_hash_delete);
}

void init_list(VirtualMachine *vm)
{
    register_builtin(vm, "list:", brl_std_list_new);

    register_builtin(vm, "pop:", brl_std_list_pop);
    register_builtin(vm, "get:", brl_std_list_get);
    register_builtin(vm, "set:", brl_std_list_set);
    register_builtin(vm, "len:", brl_std_list_length);
    register_builtin(vm, "push:", brl_std_list_push);
    register_builtin(vm, "find:", brl_std_list_find);
    register_builtin(vm, "shift:", brl_std_list_shift);
    register_builtin(vm, "concat:", brl_std_list_concat);
    register_builtin(vm, "unshift:", brl_std_list_unshift);
    register_builtin(vm, "reverse:", brl_std_list_reverse);
    register_builtin(vm, "insert:", brl_std_list_insert);
    register_builtin(vm, "remove:", brl_std_list_remove);
    register_builtin(vm, "split:", brl_std_list_split);
    register_builtin(vm, "swap:", brl_std_list_swap);
    register_builtin(vm, "sub:", brl_std_list_sub);
    register_builtin(vm, "replace:", brl_std_list_replace);
}

void init_mem(VirtualMachine *vm)
{
    register_builtin(vm, "mem.copy", brl_mem_copy);
    register_builtin(vm, "mem.delete", brl_mem_delete);
}

// std init presets
void init_std(VirtualMachine *vm)
{
    #ifndef ARDUINO
    init_os(vm);
    #endif
    init_basics(vm);
    init_loop(vm);
    init_hash(vm);
    init_list(vm);
    init_mem(vm);
    register_string(vm, "VERSION", VERSION);// version
}