#include "bruter.h"

// functions defitions for bruter
// functions defitions for bruter
// functions defitions for bruter

// MAX priority is always 0
// MIN priority is BASE_PRIORITY
Int BASE_PRIORITY = 10;

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
    if (context != NULL)
    {
        HashList *global_context = vm->hashes;
        vm->hashes = context;
        
        if (hash_find(vm, arg(0).string) != -1)
        {
            hash_unset(vm, arg(0).string);
        }
        
        hash_set(vm, arg(0).string, arg_i(1));
        vm->hashes = global_context;
    }
    else 
    {
        if (hash_find(vm, arg(0).string) != -1)
        {
            hash_unset(vm, arg(0).string);
        }
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

function(brl_std_hash_priority)
{
    if (args->size == 1)
    {
        BASE_PRIORITY = (Int)arg(0).number;
        return -1;
    }
    else if (args->size == 0)
    {
        return new_number(vm, BASE_PRIORITY);
    }

    char* hash_name = arg(0).string;
    Int priority = arg(1).number;
    Int hash_index = -1;
    
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (strcmp(hash_name, hash(i).key) == 0)
        {
            hash_index = i;
            break;
        }
    }

    if (hash_index < 0)
    {
        printf("could not find hash \"%s\"\n", hash_name);
    }

    Hash obj = list_fast_remove(*vm->hashes, hash_index);
    Int position = priority * (Int)(vm->hashes->size/BASE_PRIORITY);
    list_insert(*vm->hashes, position, obj);
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
    for (Int i = 0; i < vm->stack->size; i++)
    {
        printf("[%ld]: ", i);
        print_element(vm, i);
        printf("\n");
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
            printf("[%s] {%d} @%ld: ", vm->hashes->data[i].key, vm->typestack->data[vm->hashes->data[i].index], vm->hashes->data[i].index);
            print_element(vm, vm->hashes->data[i].index);
            printf("\n");
        }
    }
    return -1;
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
        list_push(*list, arg_i(i));
    }
    return index;
}

function(brl_std_list_push)
{
    if (args->size == 1) // push to global vm->stack
    {
        list_push(*vm->stack, value_duplicate(arg(0), arg_t(0)));
        list_push(*vm->typestack, arg_t(0));
    }
    else if (arg_t(0) == TYPE_LIST)
    {
        IntList *list = (IntList*)arg(0).pointer;
        for (Int i = 1; i < args->size; i++)
        {
            list_push(*list, arg_i(i));
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
        list_unshift(*vm->stack, value_duplicate(arg(0), arg_t(0)));
        list_unshift(*vm->typestack, arg_t(0));
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
            list_unshift(*list, arg_i(i));
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
        list_pop(*vm->stack);
        list_pop(*vm->typestack);
    }
    else if (arg_t(0) == TYPE_LIST)
    {
        IntList *list = (IntList*)arg(0).pointer;
        return list_pop(*list);
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
        list_shift(*vm->stack);
        list_shift(*vm->typestack);
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
        return list_shift(*list);
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
        list_push(*vm->stack, v);
        list_push(*vm->typestack, arg_t(0));
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
        list_insert(*list, (Int)arg(1).number, arg_i(2));
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
        list_remove(*vm->stack, arg_i(0));
        list_remove(*vm->typestack, arg_i(0));
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
        list_remove(*list, (Int)arg(1).number);
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

function(brl_std_deplace)
{
    Int newindex = new_var(vm);
    Int func = arg_i(0);
    data(newindex) = value_duplicate(data(arg_i(1)), data_t(arg_i(1)));
    data_t(newindex) = data_t(arg_i(1));
    arg_i(1) = newindex;
    interpret_args(vm, args, context);
    return newindex;
}


function(brl_std_list_concat)
{
    if (args->size == 1) // duplicate and concat each element from the list
    {
        IntList* list = (IntList*)data(arg_i(0)).pointer;
        for (Int i = 0; i < list->size; i++)
        {
            list_push(*vm->stack, value_duplicate(data(list->data[i]), data_t(list->data[i])));
            list_push(*vm->typestack, data_t(list->data[i]));
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
                    list_push(*newlist, list->data[j]);
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
        Int value = arg(0).integer;
        for (Int i = 0; i < vm->stack->size; i++)
        {
            if (data(i).integer == value)
            {
                return new_number(vm, i);
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
                    return new_number(vm, i);
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


function(brl_std_list_sub)
{
    if (args->size == 2)// create a list with the indexes of the elements in the range
    {
        IntList *list = list_init(IntList);
        Int start = arg_i(0);
        Int end = arg_i(1);
        for (Int i = start; i < end; i++)
        {
            list_push(*list, i);
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
        IntList *newlist = list_init(IntList);
        for (Int i = 0; i < arg(1).number; i++)
        {
            list_push(*newlist, list->data[i]);
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
        IntList *list = list_init(IntList);
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
                    list_push(*sub, j);
                }
                list_push(*list, sublist);
                start = i + 1;
            }
        }
        Int sublist = new_list(vm);
        IntList *sub = (IntList*)data(sublist).pointer;
        for (Int j = start; j < vm->stack->size; j++)
        {
            list_push(*sub, j);
        }
        list_push(*list, sublist);
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
            list_push(*__splited, _str);
        }
        for (Int i = 0; i < splited->size; i++)
        {
            free(splited->data[i]);
        }
        list_free(*splited);
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
                list_push(*__splited, temp);
                temp = new_list(vm);
                _temp = (IntList*)data(temp).pointer;
            }
            else 
            {
                list_push(*_temp, list->data[i]);
            }
        }
        list_push(*__splited, temp);
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
                list_push(*newlist, replacement);
                done = 1; // replace only the first occurence
            }
            else 
            {
                list_push(*newlist, list->data[i]);
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
        }
    }
    return -1;
}

// std type
// std type
// std type

function(brl_std_type_get)
{
    arg(0).integer = arg_t(0);
    return -1;
}

function(brl_std_type_set)
{
    arg_t(0) = (Int)arg(1).number;
    return -1;
}


function(brl_std_type_cast)
{
    switch (arg_t(0))
    {
        char* _str;
        Int index;
        case TYPE_NUMBER:
            switch ((Int)arg(1).number)
            {
                case TYPE_STRING:
                    _str =  str_format("%ld", (Int)arg(0).number);
                    arg(0).string = _str;
                    arg_t(0) = TYPE_STRING;
                    return -1;
                    break;
                case TYPE_ANY:
                    arg(0).integer = arg(0).number;
                    arg_t(0) = TYPE_ANY;
                    return -1;
                    break;
            }
            break;
        case TYPE_STRING:
            _str = arg(0).string;
            switch ((Int)arg(1).number)
            {
                case TYPE_NUMBER:
                    arg(0).number = atof(arg(0).string);
                    arg_t(0) = TYPE_NUMBER;
                    free(_str);
                    return -1;
                    break;
                case TYPE_ANY:
                    arg(0).integer = atoi(arg(0).string);
                    arg_t(0) = TYPE_ANY;
                    free(_str);
                    return -1;
                    break;
            }
            break;
        case TYPE_LIST:
            switch ((Int)arg(1).number)
            {
                case TYPE_STRING:
                {
                    IntList* backup = (IntList*)arg(0).pointer;
                    char* _str = list_stringify(vm, (IntList*)arg(0).pointer);
                    arg(0).string = _str;
                    arg_t(0) = TYPE_STRING;
                    list_free(*backup);
                    return -1;
                    break;
                }
            }
            break;
        case TYPE_ANY:
            switch ((Int)arg(1).number)
            {
                case TYPE_STRING:
                    _str = str_format("%ld", arg(0).integer);
                    arg(0).string = _str;
                    arg_t(0) = TYPE_STRING;
                    return -1;
                    break;
                case TYPE_NUMBER:
                    arg(0).number = arg(0).integer;
                    arg_t(0) = TYPE_NUMBER;
                    return -1;
                    break;
            }
            break;
    }
}

// math functions
// math functions
// math functions
// math functions


function(brl_std_math_add)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            for (Int i = 1; i < args->size; i++)
            {
                arg(0).number += arg(i).number;
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                arg(0).integer += (Int)arg(i).integer;
            }
            break;
    }
    return -1;
}

function(brl_std_math_sub)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            for (Int i = 1; i < args->size; i++)
            {
                arg(0).number -= arg(i).number;
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                arg(0).integer -= arg(i).integer;
            }
            break;
    }
    return -1;
}

function(brl_std_math_mul)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            for (Int i = 1; i < args->size; i++)
            {
                arg(0).number *= arg(i).number;
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                arg(0).integer *= arg(i).integer;
            }
            break;
    }
    return -1;
}

function(brl_std_math_div)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            for (Int i = 1; i < args->size; i++)
            {
                arg(0).number /= arg(i).number;
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                arg(0).integer /= arg(i).integer;
            }
            break;
    }
    return -1;
}

function(brl_std_math_mod)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            #if __SIZEOF_POINTER__ == 8
                arg(0).number = fmod(arg(0).number, arg(1).number);
            #else
                arg(0).number = fmodf(arg(0).number, arg(1).number);
            #endif
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                arg(0).integer %= arg(i).integer;
            }
            break;
    }
    
    return -1;
}

function(brl_std_math_random)
{
    switch (args->size)
    {
        case 2: // (min, max)
            arg(0).number = rand() % (Int)arg(1).number + (Int)arg(0).number;
            break;
        
        default:
            arg(0).number = rand();
            break;
    }
    return -1;
}

function(brl_std_math_seed)
{
    srand(arg(0).integer);
    return -1;
}

function(brl_std_math_round)
{
    #if __SIZEOF_POINTER__ == 8
        arg(0).number = round(arg(0).number);
    #else
        arg(0).number = roundf(arg(0).number);
    #endif
    return -1;
}

function(brl_std_math_floor)
{
    #if __SIZEOF_POINTER__ == 8
        arg(0).number = floor(arg(0).number);
    #else
        arg(0).number = floorf(arg(0).number);
    #endif
    return -1;
}

function(brl_std_math_ceil)
{
    #if __SIZEOF_POINTER__ == 8
        arg(0).number = ceil(arg(0).number);
    #else
        arg(0).number = ceilf(arg(0).number);
    #endif
    return -1;
}

function(brl_std_math_sin)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            #if __SIZEOF_POINTER__ == 8
                arg(0).number = sin(arg(0).number);
            #else
                arg(0).number = sinf(arg(0).number);
            #endif
            break;
        default:
            arg(0).number = sin((Float)arg(0).integer);
            arg_t(0) = TYPE_NUMBER;
            break;
    }
    return -1;
}

function(brl_std_math_cos)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            #if __SIZEOF_POINTER__ == 8
                arg(0).number = cos(arg(0).number);
            #else
                arg(0).number = cosf(arg(0).number);
            #endif
            break;
        default:
            arg(0).number = cos((Float)arg(0).integer);
            arg_t(0) = TYPE_NUMBER;
            break;
    }
    return -1;  
}

function(brl_std_math_tan)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            #if __SIZEOF_POINTER__ == 8
                arg(0).number = tan(arg(0).number);
            #else
                arg(0).number = tanf(arg(0).number);
            #endif
            break;
        default:
            arg(0).number = tan((Float)arg(0).integer);
            arg_t(0) = TYPE_NUMBER;
            break;
    }
    return -1;
}

function(brl_std_min)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
        {
            Float min = arg(0).number;
            for (Int i = 1; i < args->size; i++)
            {
                #if __SIZEOF_POINTER__ == 8
                    min = fmin(min, arg(i).number);
                #else
                    min = fminf(min, arg(i).number);
                #endif
            }
            arg(0).number = min;
        }
        default:
        {
            Int min = arg(0).integer;
            for (Int i = 1; i < args->size; i++)
            {
                min = min < arg(i).integer ? min : arg(i).integer;
            }
            arg(0).integer = min;
            break;
        }
    }
    return -1;
}

function(brl_std_max)
{
    switch (arg_t(0))
    {
        case TYPE_NUMBER:
        {
            Float max = arg(0).number;
            for (Int i = 1; i < args->size; i++)
            {
                #if __SIZEOF_POINTER__ == 8
                    max = fmax(max, arg(i).number);
                #else
                    max = fmaxf(max, arg(i).number);
                #endif
            }
            arg(0).number = max;
        }
        default:
        {
            Int max = arg(0).integer;
            for (Int i = 1; i < args->size; i++)
            {
                max = max > arg(i).integer ? max : arg(i).integer;
            }
            arg(0).integer = max;
            break;
        }
    }
    return -1;
}


// std conditions
// std conditions
// std conditions

function(brl_std_condition_if)
{
    Int result = -1;
    if (args->size == 2)
    {
        if (eval(vm, arg(0).string, context))
        {
            result = eval(vm, arg(1).string, context);
        }
    }
    else if (args->size == 3) // ifelse
    {
        if (eval(vm, arg(0).string, context))
        {
            result = eval(vm, arg(1).string, context);
        }
        else
        {
            result = eval(vm, arg(2).string, context);
        }
    }
    return result;
}

function(brl_std_condition_equals)// ==
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                goto outside_the_loop;
            }
        }
        result = 1;
    }
    outside_the_loop:
    
    
    for (Int i = 1; i < args->size; i++)
    {
        if (arg_t(i - 1) != arg_t(i))
        {
            return 0;
        }
        else
        {
            switch (arg_t(i - 1))
            {
                case TYPE_STRING:
                    if (strcmp(arg(i - 1).string, arg(i).string) != 0)
                    {
                        return 0;
                    }
                    break;
                case TYPE_LIST:

                    if (arg(i - 1).pointer == arg(i).pointer)
                    {
                        return result;
                    }
                    else if (((IntList*)arg(i - 1).pointer)->size != ((IntList*)arg(i).pointer)->size)
                    {
                        return 0;
                    }
                    

                    for (Int j = 0; j < ((IntList*)arg(i - 1).pointer)->size; j++)
                    {
                        if (((IntList*)arg(i - 1).pointer)->data[j] != ((IntList*)arg(i).pointer)->data[j])
                        {
                            return 0;
                        }
                    }
                    break;
                default:
                    if (arg(i - 1).integer != arg(i).integer)
                    {
                        return 0;
                    }
                    break;
            }
        }
    }
    return result;
}

function(brl_std_condition_not_equals)// !=
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                goto outside_the_loop;
            }
        }
        result = 1;
    }
    outside_the_loop:

    for (Int i = 1; i < args->size; i++)
    {
        if (arg_t(i - 1) != arg_t(i))
        {
            return result;
        }
        else
        {
            switch (arg_t(i - 1))
            {
                case TYPE_STRING:
                    if (strcmp(arg(i - 1).string, arg(i).string) == 0)
                    {
                        return 0;
                    }
                    break;
                case TYPE_LIST:

                    if (arg(i - 1).pointer == arg(i).pointer)
                    {
                        return 0;
                    }
                    else if (((IntList*)arg(i - 1).pointer)->size != ((IntList*)arg(i).pointer)->size)
                    {
                        return 0;
                    }

                    for (Int j = 0; j < ((IntList*)arg(i - 1).pointer)->size; j++)
                    {
                        if (((IntList*)arg(i - 1).pointer)->data[j] == ((IntList*)arg(i).pointer)->data[j])
                        {
                            return 0;
                        }
                    }
                    break;
                default:
                    if (arg(i - 1).integer == arg(i).integer)
                    {
                        return 0;
                    }
                    break;
            }
        }
    }
    return result;
}


function(brl_std_condition_greater)
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                goto outside_the_loop;
            }
        }
        result = 1;
    }
    outside_the_loop:

    switch (arg_t(1))
    {
        case TYPE_NUMBER:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).number <= arg(i).number)
                {
                    return 0;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).integer <= arg(i).integer)
                {
                    return 0;
                }
            }
            break;
    }
    return result;
}

function(brl_std_condition_greater_equals)
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                goto outside_the_loop;
            }
        }
        result = 1;
    }
    outside_the_loop:

    switch (arg_t(1))
    {
        case TYPE_NUMBER:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).number < arg(i).number)
                {
                    return 0;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).integer < arg(i).integer)
                {
                    return 0;
                }
            }
            break;
    }
    return result;
}

function(brl_std_condition_less)
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                goto outside_the_loop;
            }
        }
        result = 1;
    }
    outside_the_loop:

    switch (arg_t(1))
    {
        case TYPE_NUMBER:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).number >= arg(i).number)
                {
                    return 0;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).integer >= arg(i).integer)
                {
                    return 0;
                }
            }
            break;
    }
    return result;
}

function(brl_std_condition_less_equals)
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                goto outside_the_loop;
            }
        }
        result = 1;
    }
    outside_the_loop:

    switch (arg_t(1))
    {
        case TYPE_NUMBER:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).number > arg(i).number)
                {
                    return 0;
                }
            }
            break;
        default:
            for (Int i = 1; i < args->size; i++)
            {
                if (arg(i - 1).integer > arg(i).integer)
                {
                    return 0;
                }
            }
            break;
    }
    return result;
}

function(brl_std_condition_and)
{
    Int result = arg_i(0);
    if (result == 0)
    {
        for (Int i = 1; i < args->size; i++)
        {
            if (arg_i(i) != 0)
            {
                result = i;
                goto outside_the_loop;
            }
        }
        result = 1;
    }
    outside_the_loop:

    for (Int i = 1; i < args->size; i++)
    {
        if (arg_i(i) == 0)
        {
            return 0;
        }
    }
    return result;
}

function(brl_std_condition_raw_or)
{
    for (Int i = 0; i < args->size; i++)
    {
        if (arg_i(i) > 0)
        {
            return arg_i(i);
        }
    }
    return 0;
}


function(brl_std_loop_while)
{
    Int result = -1;
    if (strchr(arg(1).string, '#') == NULL)
    {
        {
            char* _parentesis = strchr(arg(1).string, '(');
            if (_parentesis != NULL)
            {
                if (_parentesis[1] == '@' && _parentesis[2] == '@')
                {
                    // its a string 
                    goto skip_safety_check;
                }
                else 
                {
                    // its a expression
                    goto regret_optimization;
                }
            }
        }

        skip_safety_check:

        StringList *splited = str_split(arg(1).string, ";");
        
        IntListList *arglist = list_init(IntListList);

        for (Int i = 0; i < splited->size; i++)
        {
            //if splited->data[i] is empty or contains only spacy characters, skip it
            if (strspn(splited->data[i], " \t\n\r\f\v") == strlen(splited->data[i]) || splited->data[i][0] == 0)
            {
                free(splited->data[i]);
                continue;
            }
            IntList *_args = parse(vm, splited->data[i], context);
            list_push(*arglist, *_args);
            free(splited->data[i]);

            free(_args);
        }

        list_free(*splited);
        char cond = eval(vm,arg(0).string,context);
        while (cond)
        {
            cond = eval(vm,arg(0).string,context);
            for (Int i = 0; i < arglist->size; i++)
            {
                result = interpret_args(vm, &arglist->data[i], context);
                if (result > -1)
                {
                    goto skip_to_return;
                }
            }
        }

        skip_to_return:

        for (Int i = 0; i < arglist->size; i++)
        {
            free(arglist->data[i].data);
        }

        list_free(*arglist);
    }
    else
    {
        regret_optimization:
        while (eval(vm,arg(0).string,context))
        {
            result = eval(vm,arg(1).string,context);
            if (result > -1)
            {
                break;
            }
        }
    }
    return result;
}

function(brl_std_loop_repeat)
{
    Int result = -1;
    if (strchr(arg(1).string, '#') == NULL)
    {
        {
            char* _parentesis = strchr(arg(1).string, '(');
            if (_parentesis != NULL)
            {
                if (_parentesis[1] == '@' && _parentesis[2] == '@')
                {
                    // its a string 
                    goto skip_safety_check;
                }
                else 
                {
                    // its a expression
                    goto regret_optimization;
                }
            }
        }

        skip_safety_check:

        StringList *splited = str_split(arg(1).string, ";");
        
        IntListList *arglist = list_init(IntListList);

        for (Int i = 0; i < splited->size; i++)
        {
            //if splited->data[i] is empty or contains only spacy characters, skip it
            if (strspn(splited->data[i], " \t\n\r\f\v") == strlen(splited->data[i]) || splited->data[i][0] == 0)
            {
                free(splited->data[i]);
                continue;
            }
            IntList *_args = parse(vm, splited->data[i], context);
            list_push(*arglist, *_args);
            free(splited->data[i]);

            free(_args);
        }

        list_free(*splited);

        for (Int i = 0; i < arg(0).number; i++)
        {
            for (Int i = 0; i < arglist->size; i++)
            {
                result = interpret_args(vm, &arglist->data[i], context);
                if (result > -1)
                {
                    goto skip_to_return;
                }
            }
        }

        skip_to_return:

        for (Int i = 0; i < arglist->size; i++)
        {
            free(arglist->data[i].data);
        }

        list_free(*arglist);
    }
    else
    {
        regret_optimization:
        for (int i = 0; i < (Int)arg(0).number; i++)
        {
            result = eval(vm,arg(1).string,context);
            if (result > -1)
            {
                break;
            }
        }
    }
    return result;
}

// inits
#ifndef ARDUINO
void init_os(VirtualMachine *vm)
{
    register_builtin(vm, "file.read", brl_os_file_read);
    register_builtin(vm, "file.write", brl_os_file_write);
    register_builtin(vm, "system", brl_os_system);

#ifndef __EMSCRIPTEN__
    register_builtin(vm, "time", brl_os_time_now);
    register_builtin(vm, "clock", brl_os_time_clock);
#endif
}
#endif

void init_basics(VirtualMachine *vm)
{
    register_builtin(vm, "#", brl_std_ignore);
    
    register_builtin(vm, "return", brl_std_return);

    register_builtin(vm, "ls", brl_std_io_ls);
    register_builtin(vm, "ls.hash", brl_std_io_ls_hashes);

    register_builtin(vm, "print", brl_std_io_print);
    register_builtin(vm, "$", brl_std_deplace);

    register_builtin(vm, "while", brl_std_loop_while);
    register_builtin(vm, "repeat", brl_std_loop_repeat);

#ifndef ARDUINO
    register_builtin(vm, "scan", brl_std_io_scan);// not avaliable on arduino
#endif
}

void init_hash(VirtualMachine *vm)
{
    register_builtin(vm, "#new", brl_std_hash_new);
    register_builtin(vm, "#delete", brl_std_hash_delete);
    register_builtin(vm, "#priority", brl_std_hash_priority);
}

void init_list(VirtualMachine *vm)
{
    register_builtin(vm, "list:", brl_std_list_new);
    register_builtin(vm, "pop:", brl_std_list_pop);
    register_builtin(vm, "set:", brl_std_list_set);
    register_builtin(vm, "len:", brl_std_list_length);
    register_builtin(vm, "push:", brl_std_list_push);
    register_builtin(vm, "shift:", brl_std_list_shift);
    register_builtin(vm, "unshift:", brl_std_list_unshift);
    register_builtin(vm, "insert:", brl_std_list_insert);
    register_builtin(vm, "remove:", brl_std_list_remove);
    register_builtin(vm, "concat:", brl_std_list_concat);
    register_builtin(vm, "find:", brl_std_list_find);
    register_builtin(vm, "get:", brl_std_list_get);
    register_builtin(vm, "sub:", brl_std_list_sub);
    register_builtin(vm, "split:", brl_std_list_split);
    register_builtin(vm, "replace:", brl_std_list_replace);
    register_builtin(vm, "swap:", brl_std_list_swap);
}

void init_mem(VirtualMachine *vm)
{
    register_builtin(vm, "mem.copy", brl_mem_copy);
    register_builtin(vm, "mem.delete", brl_mem_delete);
}

// destructive/inplace!!
void init_type(VirtualMachine *vm)
{
    // type size(4 or 8 bytes)
    register_number(vm, "type.size", sizeof(Value));

    // types
    register_number(vm, "type.any", TYPE_ANY);
    register_number(vm, "type.number", TYPE_NUMBER);
    register_number(vm, "type.string", TYPE_STRING);
    register_number(vm, "type.list", TYPE_LIST);

    // type functions are inplace(destructive), you might want to use $ to do non-destructive operations
    // e.g. $ type a; // returns the type of a instead turning a into a number of its type
    // type functions
    register_builtin(vm, "type", brl_std_type_get);
    register_builtin(vm, "pun", brl_std_type_set);
    register_builtin(vm, "cast", brl_std_type_cast);
}

// destructive/inplace!!
void init_std_math(VirtualMachine *vm)
{
    // math functions are inplace(destructive), you might want to use $ to do non-destructive operations
    // e.g. $ + a 2; // a is not changed
    register_builtin(vm, "+", brl_std_math_add);
    register_builtin(vm, "-", brl_std_math_sub);
    register_builtin(vm, "*", brl_std_math_mul);
    register_builtin(vm, "/", brl_std_math_div);
    register_builtin(vm, "\%", brl_std_math_mod);

    register_builtin(vm, "random", brl_std_math_random);
    register_builtin(vm, "seed", brl_std_math_seed);

    register_builtin(vm, "round", brl_std_math_round);
    register_builtin(vm, "floor", brl_std_math_floor);
    register_builtin(vm, "ceil", brl_std_math_ceil);

    register_builtin(vm, "sin", brl_std_math_sin);
    register_builtin(vm, "cos", brl_std_math_cos);
    register_builtin(vm, "tan", brl_std_math_tan);

    register_builtin(vm, "min", brl_std_min);
    register_builtin(vm, "max", brl_std_max);
}

// index-based!!
void init_std_condition(VirtualMachine *vm)
{
    register_builtin(vm, "==", brl_std_condition_equals);
    register_builtin(vm, "!=", brl_std_condition_not_equals);

    register_builtin(vm, ">", brl_std_condition_greater);
    register_builtin(vm, ">=", brl_std_condition_greater_equals);
    register_builtin(vm, "<", brl_std_condition_less);
    register_builtin(vm, "<=", brl_std_condition_less_equals);
    
    register_builtin(vm, "&&", brl_std_condition_and);
    register_builtin(vm, "||", brl_std_condition_raw_or);

    register_builtin(vm, "if", brl_std_condition_if);
}

// std init presets
void init_std(VirtualMachine *vm)
{
    // @0 = NULL
    register_var(vm, "NULL");

    #ifndef ARDUINO
    init_os(vm);
    #endif
    init_basics(vm);
    init_hash(vm);
    init_list(vm);
    init_mem(vm);

    init_type(vm);
    init_std_math(vm);
    init_std_condition(vm);


    register_string(vm, "VERSION", VERSION);// version
}