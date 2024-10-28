#include "bruter.h"

Int std_hash_set(VirtualMachine *vm, IntList *args)
{
    Int varname = stack_shift(*args);
    Int value = stack_shift(*args);

    char * name = vm->stack->data[varname].string;
    hash_set(vm, name, value);
    return -1;
}

Int std_hash_delete(VirtualMachine *vm, IntList *args)
{
    Int varname = stack_shift(*args);
    hash_unset(vm, vm->stack->data[varname].string);
    return -1;
}

Int std_hash_rename(VirtualMachine *vm, IntList *args)
{
    Int varname = stack_shift(*args);
    Int newname = stack_shift(*args);
    Int index = hash_find(vm, vm->stack->data[varname].string);
    if (index >= 0)
    {
        free(vm->hashes->data[index].key);
        vm->hashes->data[index].key = strdup(vm->stack->data[newname].string);
    }
    return -1;
}

Int std_type_set(VirtualMachine *vm, IntList *args)
{
    Int var = stack_shift(*args);
    Int type = stack_shift(*args);
    
    vm->typestack->data[var] = (Int)vm->stack->data[type].number;
    
    return -1;
}

void print_element(VirtualMachine *vm, Int index)
{
    if (index < 0 || index >= vm->stack->size)
    {
        printf("{invalid}");
        return;
    }

    Int _type = -1;
    Value temp = vm->stack->data[index];
    _type = vm->typestack->data[index];
    
    if (_type == TYPE_BUILTIN)
    {
        printf("%p", temp.pointer);
    }
    else if (_type == TYPE_NUMBER)
    {
        if (round(temp.number) == temp.number)
        {
            printf("%ld", (Int)temp.number);
        }
        else
        {
            printf("%f", temp.number);
        }
    }
    else if (_type == TYPE_STRING)
    {
        printf("%s", temp.string);
    }
    else if (_type == TYPE_LIST)
    {
        printf("[");
        IntList *list = (IntList*)temp.pointer;
        for (Int i = 0; i < (list->size-1); i++)
        {
            printf("%d, ", list->data[i]);
        }
        if (list->size > 0)
        {
            printf("%d]", list->data[list->size-1]);
        }
        else
        {
            printf("]");
        }
    }
    else if (_type == TYPE_OTHER)
    {
        printf("{other} %p", temp.pointer);
    }
    else if (_type == TYPE_NIL)
    {
        printf("{nil}");
    }
    else
    {
        printf("{unknown} %d", temp.integer);
    }
}

Int std_io_print(VirtualMachine *vm, IntList *args)
{
    while (args->size > 0)
    {
        Int var = stack_shift(*args);
        if (var >= 0 || var < vm->stack->size)
        {
            print_element(vm, var);
        }
        else 
        {
            printf("{nil}");
        }

        if (args->size > 0)
        {
            printf(" ");
        }
    }
    printf("\n");
    return -1;
}

Int std_io_ls(VirtualMachine *vm, IntList *args)
{
    Int _var = -1;
    if (args->size > 0)
    {
        _var = stack_shift(*args);
    }

    if (_var > -1 && vm->typestack->data[_var] == TYPE_LIST)
    {
        IntList *list = (IntList*)vm->stack->data[(Int)vm->stack->data[_var].number].pointer;
        for (Int i = 0; i < list->size; i++)
        {
            printf("%d[%d]: ", (Int)vm->stack->data[_var].number, i);
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

Int std_io_ls_types(VirtualMachine *vm, IntList *args)
{
    for (Int i = 0; i < vm->stack->size; i++)
    {
        printf("[%ld]: %d\n", i, vm->typestack->data[i]);
    }
    return -1;
}

Int std_io_ls_hashes(VirtualMachine *vm, IntList *args)
{
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        printf("[%s] {%d} @%ld: ", vm->hashes->data[i].key, vm->typestack->data[vm->hashes->data[i].index], vm->hashes->data[i].index);
        print_element(vm, vm->hashes->data[i].index);
        printf("\n");
    }
    return -1;
}

Int std_io_ls_temp(VirtualMachine *vm, IntList *args)
{
    for (Int i = 0; i < vm->temp->size; i++)
    {
        printf("[%ld] {temp}: ", vm->temp->data[i]);
        print_element(vm, vm->temp->data[i]);
        printf("\n");
    }
    return -1;
}

Int std_io_ls_unused(VirtualMachine *vm, IntList *args)
{
    for (Int i = 0; i < vm->unused->size; i++)
    {
        printf("[%ld] {unused}: ", vm->unused->data[i]);
        print_element(vm, vm->unused->data[i]);
        printf("\n");
    }
    return -1;
}

Int std_eval(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    char* _str = vm->stack->data[str].string;
    Int result = eval(vm, _str);
    return result;
}

Int std_do(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    char* _str = vm->stack->data[str].string;
    Int _args = new_list(vm);
    while (args->size > 0)
    {
        stack_push(*((IntList*)vm->stack->data[_args].pointer), stack_shift(*args));
    }
    hash_set(vm, "args", _args);
    Int result = eval(vm, _str);
    return result;
}


Int std_ignore(VirtualMachine *vm, IntList *args)
{
    while (args->size > 0)
    {
        stack_shift(*args);
    }
    return -1;
}

Int std_return(VirtualMachine *vm, IntList *args)
{
    return stack_shift(*args);
}

Int std_type_get(VirtualMachine *vm, IntList *args)
{
    Int var = stack_shift(*args);
    Int result = new_number(vm, vm->typestack->data[var]);
    return result;
}


// math functions
// math functions
// math functions
// math functions


Int std_math_add(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = new_number(vm, vm->stack->data[a].number + vm->stack->data[b].number);
    return result;
}

Int std_math_sub(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = new_number(vm, vm->stack->data[a].number - vm->stack->data[b].number);
    return result;
}

Int std_math_mul(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = new_number(vm, vm->stack->data[a].number * vm->stack->data[b].number);
    return result;
}

Int std_math_div(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = new_number(vm, vm->stack->data[a].number / vm->stack->data[b].number);
    return result;
}

Int std_math_mod(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = new_number(vm, (Int)vm->stack->data[a].number % (Int)vm->stack->data[b].number);
    return result;
}

Int std_math_pow(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = new_number(vm, pow(vm->stack->data[a].number, vm->stack->data[b].number));
    return result;
}

Int std_math_sqrt(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int result = new_number(vm, sqrt(vm->stack->data[a].number));
    return result;
}

Int std_math_abs(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int result = new_number(vm, fabs(vm->stack->data[a].number));
    return result;
}

Int std_math_random(VirtualMachine *vm, IntList *args)
{
    Int ___min = stack_shift(*args);
    Int ___max = stack_shift(*args);
    Int result = new_number(vm, rand() % (Int)vm->stack->data[___max].number + (Int)vm->stack->data[___min].number);
    return result;
}

Int std_math_seed(VirtualMachine *vm, IntList *args)
{
    Int seed = stack_shift(*args);
    srand((Int)vm->stack->data[seed].number);
    return -1;
}

Int std_math_floor(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int result = new_number(vm, floor(vm->stack->data[a].number));
    return result;;
}

Int std_math_ceil(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int result = new_number(vm, ceil(vm->stack->data[a].number));
    return result;
}

Int std_math_round(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int result = new_number(vm, round(vm->stack->data[a].number));
    return result;
}

Int std_math_increment(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    vm->stack->data[a].number++;
    return a;
}

Int std_math_decrement(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    vm->stack->data[a].number--;
    return a;
}

// list functions
// list functions
// list functions
// list functions

Int std_list_new(VirtualMachine *vm, IntList *args) 
{
    Int index = new_list(vm);
    IntList *list = (IntList*)vm->stack->data[index].pointer;
    while (args->size > 0)
    {
        Int var = stack_shift(*args);
        Int tmp = new_var(vm);
        vm->stack->data[tmp] = value_duplicate(vm->stack->data[var], vm->typestack->data[var]);
        vm->typestack->data[tmp] = vm->typestack->data[var];
        stack_push(*list, tmp);
    }
    return index;
}
Int std_list_insert(VirtualMachine *vm, IntList *args)
{
    Int list = stack_shift(*args);
    Int index = stack_shift(*args);
    Int value = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        stack_insert(*lst, (Int)vm->stack->data[index].number, value);
    }
    return -1;
}
Int std_list_push(VirtualMachine *vm, IntList *args)
{
    Int list = stack_shift(*args);
    Int value = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        stack_push(*lst, value);
    }
    return -1;
}
Int std_list_unshift(VirtualMachine *vm, IntList *args) 
{
    Int list = stack_shift(*args);
    Int value = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        stack_unshift(*lst, value);
    }
    return -1;   
}
Int std_list_remove(VirtualMachine *vm, IntList *args)// returns the removed element
{
    Int list = stack_shift(*args);
    Int index = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        Int result = stack_remove(*lst, (Int)vm->stack->data[index].number);
        return result;
    }
    return -1;
}

Int std_list_pop(VirtualMachine *vm, IntList *args)// returns the removed element
{
    Int list = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        Int result = stack_pop(*lst);
        return result;
    }
    return -1;
}

Int std_list_shift(VirtualMachine *vm, IntList *args)// returns the removed element
{
    Int list = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        Int result = stack_shift(*lst);
        return result;
    }
    return -1;
}

Int std_list_concat(VirtualMachine *vm, IntList *args)
{
    Int list1 = stack_shift(*args);
    Int list2 = stack_shift(*args);
    Int _newlist = new_list(vm);
    IntList *newlist = (IntList*)vm->stack->data[_newlist].pointer;
    if (vm->typestack->data[list1] == TYPE_LIST && vm->typestack->data[list2] == TYPE_LIST)
    {
        IntList *lst1 = (IntList*)vm->stack->data[list1].pointer;
        IntList *lst2 = (IntList*)vm->stack->data[list2].pointer;
        for (Int i = 0; i < lst1->size; i++)
        {
            stack_push(*newlist, lst1->data[i]);
        }
        for (Int i = 0; i < lst2->size; i++)
        {
            stack_push(*newlist, lst2->data[i]);
        }
    }
    return _newlist;
}
Int std_list_find(VirtualMachine *vm, IntList *args)
{
    Int list = stack_shift(*args);
    Int value = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
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
Int std_list_get(VirtualMachine *vm, IntList *args)
{
    Int list = stack_shift(*args);
    Int index = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        if (vm->typestack->data[index] == TYPE_NUMBER)
        {
            if (vm->stack->data[index].number >= 0 && vm->stack->data[index].number < lst->size)
            {
                return lst->data[(Int)vm->stack->data[index].number];
            }
        }
    }
    return -1;
}
Int std_list_length(VirtualMachine *vm, IntList *args)
{
    Int list = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        return new_number(vm, lst->size);
    }
    return -1;
}


// std string

Int std_string_concat(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = -1;

    char* _newstr = str_concat(vm->stack->data[a].string, vm->stack->data[b].string);
    result = new_string(vm, _newstr);
    free(_newstr);
    return result;
}

Int std_string_find(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int substr = stack_shift(*args);
    Int result = -1;
    if ((vm->typestack->data[str] == TYPE_STRING) && (vm->typestack->data[substr] == TYPE_STRING))
    {
        char* _str = vm->stack->data[str].string;
        char* _substr = vm->stack->data[substr].string;
        char* _result = strstr(_str, _substr);
        if (_result != NULL)
        {
            result = new_number(vm, _result - _str);
        }
    }
    return result;
}

Int std_string_ndup(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int start = stack_shift(*args);
    Int end = stack_shift(*args);
    Int result = -1;
    if ((vm->typestack->data[str] == TYPE_STRING) && vm->typestack->data[start] == TYPE_NUMBER && vm->typestack->data[end] == TYPE_NUMBER)
    {
        char* _str = strndup(vm->stack->data[str].string + (Int)vm->stack->data[start].number, (Int)vm->stack->data[end].number - (Int)vm->stack->data[start].number);
        result = new_string(vm, _str);
        free(_str);
    }
    return result;
}

Int std_string_split(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int delim = stack_shift(*args);
    if ((vm->typestack->data[str] == TYPE_STRING) && (vm->typestack->data[delim] == TYPE_STRING))
    {
        StringList *list = splitString(vm->stack->data[str].string, vm->stack->data[delim].string);
        char * _tmp = str_format("array.new %d", list->size);
        Int _arr = eval(vm, _tmp);
        free(_tmp);
        Int index = 1;
        while (list->size > 0)
        {
            vm->stack->data[_arr + index].string = stack_shift(*list);
            vm->typestack->data[_arr + index] = TYPE_STRING;
            index++;
        }
        stack_free(*list);
        return _arr;
    }
    return -1;
}

Int std_string_replace(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int substr = stack_shift(*args);
    Int replacement = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[str] == TYPE_STRING && vm->typestack->data[substr] == TYPE_STRING && vm->typestack->data[replacement] == TYPE_STRING)
    {
        char* _str = vm->stack->data[str].string;
        char* _substr = vm->stack->data[substr].string;
        char* _replacement = vm->stack->data[replacement].string;
        char* _newstr = str_replace(_str, _substr, _replacement);
        result = new_string(vm, _newstr);
        free(_newstr);
    }
    return result;
}

Int std_string_replace_all(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int substr = stack_shift(*args);
    Int replacement = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[str] == TYPE_STRING && vm->typestack->data[substr] == TYPE_STRING && vm->typestack->data[replacement] == TYPE_STRING)
    {
        char* _str = vm->stack->data[str].string;
        char* _substr = vm->stack->data[substr].string;
        char* _replacement = vm->stack->data[replacement].string;
        char* _newstr = str_replace_all(_str, _substr, _replacement);
        result = new_string(vm, _newstr);
        free(_newstr);
    }
    return result;
}

Int std_string_to_number(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[str] == TYPE_STRING)
    {
        result = new_number(vm, atof(vm->stack->data[str].string));
    }
    return result;
}

Int std_string_length(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[str] == TYPE_STRING)
    {
        result = new_number(vm, strlen(vm->stack->data[str].string));
    }
    return result;
}

Int std_string_format(VirtualMachine *vm, IntList *args)
{
    Int str = stack_shift(*args);
    Int result = -1;
    char* _str = vm->stack->data[str].string;
    for (Int i = 0; i < strlen(_str); i++)
    {
        if (_str[i] == '%')
        {
            if (_str[i+1] == 'd')
            {
                Int value = stack_shift(*args);
                char* _value = str_format("%ld", (Int)vm->stack->data[value].number);
                char* _newstr = str_replace(_str, "%ld", _value);
                free(_value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 's')
            {
                Int value = stack_shift(*args);
                char* _value = vm->stack->data[value].string;
                char* _newstr = str_replace(_str, "%s", _value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 'f')
            {
                Int value = stack_shift(*args);
                char* _value = str_format("%f", vm->stack->data[value].number);
                char* _newstr = str_replace(_str, "%f", _value);
                free(_value);
                free(_str);
                _str = _newstr;
            }
            else if (_str[i+1] == 'p')
            {
                Int value = stack_shift(*args);
                char* _value = str_format("%p", vm->stack->data[value].pointer);
                char* _newstr = str_replace(_str, "%p", _value);
                free(_value);
                free(_str);
                _str = _newstr;
            }
        }
    }
    result = new_var(vm);
    vm->stack->data[result].string = _str;
    vm->typestack->data[result] = TYPE_STRING;
    return result;
}

// std conditions
// std conditions
// std conditions
Int std_condition_equals(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = new_number(vm, vm->stack->data[a].number == vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = new_number(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) == 0);
        }
    }
    return result;
}

Int std_condition_not_equals(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = new_number(vm, vm->stack->data[a].number != vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = new_number(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) != 0);
        }
    }
    
    return result;
}

Int std_condition_greater(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = new_number(vm, vm->stack->data[a].number > vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = new_number(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) > 0);
        }
    }
    return result;
}

Int std_condition_less(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = new_number(vm, vm->stack->data[a].number < vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = new_number(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) < 0);
        }
    }
    return result;
}

Int std_condition_greater_equals(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = new_number(vm, vm->stack->data[a].number >= vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = new_number(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) >= 0);
        }
    }
    return result;
}

Int std_condition_less_equals(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = -1;
    if (vm->typestack->data[a] == vm->typestack->data[b])
    {
        if (vm->typestack->data[a] == TYPE_NUMBER)
        {
            result = new_number(vm, vm->stack->data[a].number <= vm->stack->data[b].number);
        }
        else if (vm->typestack->data[a] == TYPE_STRING)
        {
            result = new_number(vm, strcmp(vm->stack->data[a].string, vm->stack->data[b].string) <= 0);
        }
    }
    return result;
}

Int std_condition_and(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    return new_number(vm, (is_true(vm->stack->data[a], vm->typestack->data[a]) && is_true(vm->stack->data[b], vm->typestack->data[b])));
}

Int std_condition_or(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int b = stack_shift(*args);
    Int result = 0;
    if (is_true(vm->stack->data[a], vm->typestack->data[a]))
    {
        Int index = new_var(vm);
        if (vm->typestack->data[b] == TYPE_LIST || vm->typestack->data[a] == TYPE_STRING)
        {
            vm->stack->data[index] = vm->stack->data[a];
            
        }
        else
        {
            vm->stack->data[index] = value_duplicate(vm->stack->data[a], vm->typestack->data[a]);
            
            
        }
        vm->typestack->data[index] = vm->typestack->data[a];
        result = index;
    }
    else
    {
        Int index = new_var(vm);
        if (vm->typestack->data[b] == TYPE_LIST || vm->typestack->data[b] == TYPE_STRING)
        {
            vm->stack->data[index] = vm->stack->data[b];
        }
        else
        {
            vm->stack->data[index] = value_duplicate(vm->stack->data[b], vm->typestack->data[b]);
        }
    }
    return result;
}

Int std_condition_not(VirtualMachine *vm, IntList *args)
{
    Int a = stack_shift(*args);
    Int result = 0;
    if (vm->typestack->data[a] == TYPE_NUMBER)
    {
        result = new_number(vm, !vm->stack->data[a].number);
    }
    else if (is_true(vm->stack->data[a], vm->typestack->data[a]))
    {
        result = new_number(vm, 0);
    }
    else
    {
        result = new_number(vm, 1);
    }

    return result;
}

Int std_condition_if(VirtualMachine *vm, IntList *args)
{
    Int condition = stack_shift(*args);
    Int _then = stack_shift(*args);
    Int result = eval(vm, vm->stack->data[condition].string);
    if (is_true(vm->stack->data[result], vm->typestack->data[result]))
    {
        result = eval(vm, vm->stack->data[_then].string);
    }
    return result;
}

Int std_condition_ifelse(VirtualMachine *vm, IntList *args)
{
    Int condition = stack_shift(*args);
    Int _then = stack_shift(*args);
    Int _else = stack_shift(*args);
    Int result = eval(vm, vm->stack->data[condition].string);
    if (is_true(vm->stack->data[result], vm->typestack->data[result]))
    {
        result = eval(vm, vm->stack->data[_then].string);
    }
    else
    {
        result = eval(vm, vm->stack->data[_else].string);
    }
    return result;
}

// std loop
// std loop
// std loop

Int std_loop_while(VirtualMachine *vm, IntList *args)
{
    Int condition_str = stack_shift(*args);
    Int _do_str = stack_shift(*args);
    Int condition = eval(vm, vm->stack->data[condition_str].string);
    while (is_true(vm->stack->data[condition], vm->typestack->data[condition]))
    {
        eval(vm, vm->stack->data[_do_str].string);
        condition = eval(vm, vm->stack->data[condition_str].string);
    }
    return -1;
}

Int std_loop_repeat(VirtualMachine *vm, IntList *args)
{
    Int times = stack_shift(*args);
    Int _do_str = stack_shift(*args);
    for (Int i = 0; i < vm->stack->data[times].number; i++)
    {
        eval(vm, vm->stack->data[_do_str].string);
    }
    return -1;
}
// inits

void init_basics(VirtualMachine *vm)
{
    
    registerBuiltin(vm, "#", std_ignore);
    registerBuiltin(vm, "eval", std_eval);
    registerBuiltin(vm, "do", std_do);
    registerBuiltin(vm, "return", std_return);
#ifndef ARDUINO
    registerBuiltin(vm, "ls", std_io_ls);
    registerBuiltin(vm, "ls.type", std_io_ls_types);
    registerBuiltin(vm, "ls.hash", std_io_ls_hashes);
    registerBuiltin(vm, "ls.temp", std_io_ls_temp);
    registerBuiltin(vm, "ls.free", std_io_ls_unused);
    registerBuiltin(vm, "print", std_io_print);
#endif
}

void init_type(VirtualMachine *vm)
{
    // types
    registerNumber(vm, "type.size", sizeof(Value));
    registerNumber(vm, "type.nil", TYPE_NIL);
    registerNumber(vm, "type.number", TYPE_NUMBER);
    registerNumber(vm, "type.string", TYPE_STRING);
    registerNumber(vm, "type.builtin", TYPE_BUILTIN);
    registerNumber(vm, "type.list", TYPE_LIST);

    // type functions
    registerBuiltin(vm, "type.get", std_type_get);
    registerBuiltin(vm, "type.set", std_type_set);
}

void init_loop(VirtualMachine *vm)
{
    registerBuiltin(vm, "loop.while", std_loop_while);
    registerBuiltin(vm, "loop.repeat", std_loop_repeat);
}

void init_hash(VirtualMachine *vm)
{
    registerBuiltin(vm, "hash.set", std_hash_set);
    registerBuiltin(vm, "hash.delete", std_hash_delete);
    registerBuiltin(vm, "hash.rename", std_hash_rename);
}

void init_math(VirtualMachine *vm)
{
    registerBuiltin(vm, "add", std_math_add);
    registerBuiltin(vm, "sub", std_math_sub);
    registerBuiltin(vm, "mul", std_math_mul);
    registerBuiltin(vm, "div", std_math_div);
    registerBuiltin(vm, "mod", std_math_mod);
    registerBuiltin(vm, "pow", std_math_pow);
    registerBuiltin(vm, "abs", std_math_abs);
    registerBuiltin(vm, "sqrt", std_math_sqrt);
    registerBuiltin(vm, "ceil", std_math_ceil);
    registerBuiltin(vm, "seed", std_math_seed);
    registerBuiltin(vm, "floor", std_math_floor);
    registerBuiltin(vm, "round", std_math_round);
    registerBuiltin(vm, "random", std_math_random);
    registerBuiltin(vm, "incr", std_math_increment);
    registerBuiltin(vm, "decr", std_math_decrement);
}

void init_string(VirtualMachine *vm)
{
    registerBuiltin(vm, "string.sub", std_string_ndup);
    registerBuiltin(vm, "string.find", std_string_find);
    registerBuiltin(vm, "string.len", std_string_length);
    registerBuiltin(vm, "string.split", std_string_split);
    registerBuiltin(vm, "string.concat", std_string_concat);
    registerBuiltin(vm, "string.format", std_string_format);
    registerBuiltin(vm, "string.replace", std_string_replace);
    registerBuiltin(vm, "string.to.number", std_string_to_number);
    registerBuiltin(vm, "string.replace.all", std_string_replace_all);
}

void init_condition(VirtualMachine *vm)
{
    registerBuiltin(vm, "or", std_condition_or);
    registerBuiltin(vm, "if", std_condition_if);
    registerBuiltin(vm, "<", std_condition_less);
    registerBuiltin(vm, "and", std_condition_and);
    registerBuiltin(vm, "not", std_condition_not);
    registerBuiltin(vm, "==", std_condition_equals);
    registerBuiltin(vm, ">", std_condition_greater);
    registerBuiltin(vm, "ifelse", std_condition_ifelse);
    registerBuiltin(vm, "!=", std_condition_not_equals);
    registerBuiltin(vm, "<=", std_condition_less_equals);
    registerBuiltin(vm, ">=", std_condition_greater_equals);
}

void init_list(VirtualMachine *vm)
{
    registerBuiltin(vm, "list.new", std_list_new);
    registerBuiltin(vm, "list.pop", std_list_pop);
    registerBuiltin(vm, "list.get", std_list_get);
    registerBuiltin(vm, "list.len", std_list_length);
    registerBuiltin(vm, "list.push", std_list_push);
    registerBuiltin(vm, "list.find", std_list_find);
    registerBuiltin(vm, "list.shift", std_list_shift);
    registerBuiltin(vm, "list.insert", std_list_insert);
    registerBuiltin(vm, "list.remove", std_list_remove);
    registerBuiltin(vm, "list.concat", std_list_concat);
    registerBuiltin(vm, "list.unshift", std_list_unshift);
}

void init_default_vars(VirtualMachine *vm)
{
    hold_var(vm,spawn_string(vm, "VERSION", VERSION));// version
}

// std init presets
void init_std(VirtualMachine *vm)
{
    init_basics(vm);
    init_type(vm);
    init_loop(vm);
    init_hash(vm);
    init_list(vm);
    init_math(vm);
    init_string(vm);
    init_condition(vm);
    init_default_vars(vm);
}