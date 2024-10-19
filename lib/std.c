#include "bruter.h"

Int std_mem_clear(VirtualMachine *vm, IntList *args)
{
    while (vm->temp->size > 0)
    {
        unuse_var(vm, stack_shift(*vm->temp));
    }

    return -1;
}

Int std_mem_hold(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    if (args->size > 0)
    {
        Int as = stack_shift(*args);
        Int _type = vm->typestack->data[index];
    }
    hold_var(vm, index);
    return -1;
}

Int std_mem_keep(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    if (args->size > 0)
    {
        Int as = stack_shift(*args);
        Int _type = vm->typestack->data[index];
    }
    hold_var(vm, index);
    return index;
}

Int std_mem_unhold(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    unhold_var(vm, index);
    return -1;
}

Int std_mem_rebase(VirtualMachine *vm, IntList *args)
{
    Int index = vm->stack->size - 1;
    while (vm->unused->size > 0)
    {
        index = stack_pop(*vm->unused);
        for (Int i = 0; i < vm->hashes->size; i++)
        {
            if (vm->hashes->data[i].index == index)
            {
                free(vm->hashes->data[i].key);
                stack_remove(*vm->hashes, i);
            }
        }
        for (Int i = 0; i < vm->unused->size; i++)
        {
            if (vm->unused->data[i] == index)
            {
                stack_remove(*vm->unused, i);
            }
        }
        for (Int i = 0; i < vm->temp->size; i++)
        {
            if (vm->temp->data[i] == index)
            {
                stack_remove(*vm->temp, i);
            }
        }
        for (Int i = 0; i < vm->stack->size; i++)
        {
            if (vm->typestack->data[i] == TYPE_LIST)
            {
                IntList *list = (IntList*)vm->stack->data[i].pointer;
                for (Int j = 0; j < list->size; j++)
                {
                    if (list->data[j] == index)
                    {
                        list->data[j] = -1;
                    }
                    else if (list->data[j] > index)
                    {
                        list->data[j]--;
                    }
                }
            }
        }
        for (Int i = 0; i < vm->temp->size; i++)
        {
            if (vm->temp->data[i] == index)
            {
                vm->temp->data[i] = -1;
            }
            else if (vm->temp->data[i] > index)
            {
                vm->temp->data[i]--;
            }
        }
        for (Int i = 0; i < vm->unused->size; i++)
        {
            if (vm->unused->data[i] == index)
            {
                vm->unused->data[i] = -1;
            }
            else if (vm->unused->data[i] > index)
            {
                vm->unused->data[i]--;
            }
        }
        for (Int i = 0; i < vm->hashes->size; i++)
        {
            if (vm->hashes->data[i].index == index)
            {
                vm->hashes->data[i].index = -1;
            }
            else if (vm->hashes->data[i].index > index)
            {
                vm->hashes->data[i].index--;
            }
        }

        stack_remove(*vm->stack, index);
        stack_remove(*vm->typestack, index);
    }

    return -1;
}

Int std_mem_collect(VirtualMachine *vm, IntList *args)
{
    collect_garbage(vm);
    return -1;
}

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

Int std_mem_edit(VirtualMachine *vm, IntList *args)
{
    Int variable = stack_shift(*args);
    Int value = stack_shift(*args);
    
    if (variable >= 0 && variable < vm->stack->size)
    {
        if(vm->typestack->data[variable] == TYPE_STRING ||
           vm->typestack->data[variable] == TYPE_LIST ||
           vm->typestack->data[variable] == TYPE_FUNCTION || 
           vm->typestack->data[variable] == TYPE_OTHER)
        {
            
            if (vm->typestack->data[variable] == TYPE_STRING)
            {
                free(vm->stack->data[variable].string);
            }
            else if (vm->typestack->data[variable] == TYPE_LIST)
            {
                stack_free(*((IntList*)vm->stack->data[variable].pointer));
            }
            else
            {
                free(vm->stack->data[variable].pointer);
            }
        }

        if (value >= 0 && value < vm->stack->size)
        {
            vm->stack->data[variable] = vm->stack->data[value];
            vm->typestack->data[variable] = vm->typestack->data[value];
        }
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

void print_element(VirtualMachine *vm, int index)
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
    else if (_type == TYPE_FUNCTION)
    {
        printf("%s", temp.pointer);
    }
    else if (_type == TYPE_NUMBER)
    {
        if (round(temp.number) == temp.number)
        {
            printf("%d", (Int)temp.number);
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
        printf("{unknown}");
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
    if (args->size == 0)
    {
        for (Int i = 0; i < vm->stack->size; i++)
        {
            printf("[%d]: ", i);
            print_element(vm, i);
            printf("\n");
        }
    }
    else
    {
        Int _var = stack_shift(*args);
        if (vm->typestack->data[_var] == TYPE_STRING)
        {
            if(strcmp("hashes", vm->stack->data[_var].string) == 0 ||
               strcmp("hash", vm->stack->data[_var].string) == 0) 
            {
                for (Int i = 0; i < vm->hashes->size; i++)
                {//[name] {type} @index: content
                    printf("[%s] {%d} @%d: ", vm->hashes->data[i].key, vm->typestack->data[vm->hashes->data[i].index], vm->hashes->data[i].index);
                    print_element(vm, vm->hashes->data[i].index);
                    printf("\n");
                }
            }
            else if(strcmp("temp", vm->stack->data[_var].string) == 0)
            {
                for (Int i = 0; i < vm->temp->size; i++)
                {
                    printf("[%d] {temp}: ", vm->temp->data[i]);
                    print_element(vm, vm->temp->data[i]);
                    printf("\n");
                }
            }
            else if(strcmp("unused", vm->stack->data[_var].string) == 0 || 
                    strcmp("free", vm->stack->data[_var].string) == 0)
            {
                for (Int i = 0; i < vm->unused->size; i++)
                {
                    printf("[%d] {unused}: ", vm->unused->data[i]);
                    print_element(vm, vm->unused->data[i]);
                    printf("\n");
                }
            }

        }
        else if (vm->typestack->data[_var] == TYPE_LIST)
        {
            IntList *list = (IntList*)vm->stack->data[(Int)vm->stack->data[_var].number].pointer;
            for (Int i = 0; i < list->size; i++)
            {
                printf("%d[%d]: ", (Int)vm->stack->data[_var].number, i);
                print_element(vm, list->data[i]);
                printf("\n");
            }
        }
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

Int std_mem_delete(VirtualMachine *vm, IntList *args)
{
    while (args->size > 0)
    {
        unuse_var(vm, stack_shift(*args));
    }
    return -1;
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

Int std_mem_length(VirtualMachine *vm, IntList *args)
{
    return new_number(vm, vm->stack->size);
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
        stack_insert(*lst, (Int)vm->stack->data[index].number, (Int)vm->stack->data[value].number);
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
        stack_push(*lst, (Int)vm->stack->data[value].number);
    }
    return -1;
}

Int std_list_unshift(VirtualMachine *vm, IntList *args) // returns the removed element
{
    Int list = stack_shift(*args);
    Int value = stack_shift(*args);
    if (vm->typestack->data[list] == TYPE_LIST)
    {
        IntList *lst = (IntList*)vm->stack->data[list].pointer;
        stack_unshift(*lst, (Int)vm->stack->data[value].number);
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
        Int result = stack_remove(*lst, list);
        return result;
    }
    return -1;
}

Int std_list_pop(VirtualMachine *vm, IntList *args) // returns the removed element
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

Int std_list_shift(VirtualMachine *vm, IntList *args)
{
    Int list = stack_shift(*args);
    if (vm->typestack->data[list]== TYPE_LIST)
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
    Int result = -1;
    if ((vm->typestack->data[str] == TYPE_STRING) && (vm->typestack->data[delim] == TYPE_STRING))
    {
        StringList *list = splitString(vm->stack->data[str].string, vm->stack->data[delim].string);
        result = new_list(vm);
        IntList *newlist = (IntList*)vm->stack->data[result].pointer;
        while (list->size > 0)
        {
            char* _str = stack_shift(*list);
            char* cmd = str_format("list.push @%d @%d", result, new_string(vm, _str));
            eval(vm, cmd);
            free(cmd);
            free(_str);
        }
        stack_free(*list);
    }
    return result;
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
                char* _value = str_format("%d", (Int)vm->stack->data[value].number);
                char* _newstr = str_replace(_str, "%d", _value);
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
        }
    }
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
        if (vm->typestack->data[a] == TYPE_LIST || vm->typestack->data[a] == TYPE_STRING)
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
    Int result = -1;
    if (is_true(vm->stack->data[condition], vm->typestack->data[condition]))
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
    Int result = -1;
    if (is_true(vm->stack->data[condition], vm->typestack->data[condition]))
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
    Int condition_result = eval(vm, vm->stack->data[condition_str].string);
    while (is_true(vm->stack->data[condition_result], vm->typestack->data[condition_result]))
    {   
        condition_result = eval(vm, vm->stack->data[_do_str].string);
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

// std_function

Int std_function(VirtualMachine *vm, IntList *args)
{
    Int name = stack_shift(*args);
    Int script = stack_shift(*args);
    spawn_function(vm, vm->stack->data[name].string, vm->stack->data[script].string);
    return -1;
}

// std prototype
// std prototype
// std prototype

Int std_prototype_copy(VirtualMachine *vm, IntList *args)
{
    Int prototype_name = stack_shift(*args);
    Int new_name = stack_shift(*args);
    for (Int i = vm->hashes->size-1; i >= 0; i--)
    {
        if (str_find(vm->hashes->data[i].key, vm->stack->data[prototype_name].string) == 0)
        {
            Int index = new_var(vm);
            vm->stack->data[index] = value_duplicate(vm->stack->data[vm->hashes->data[i].index], vm->typestack->data[vm->hashes->data[i].index]);
            vm->typestack->data[index] = vm->typestack->data[vm->hashes->data[i].index];
            char* tmpstr = str_replace_all(vm->hashes->data[i].key, vm->stack->data[prototype_name].string, vm->stack->data[new_name].string);
            hash_set(vm, tmpstr, index);
            free(tmpstr);
        }
    }
    return -1;
}

Int std_prototype_compare(VirtualMachine *vm, IntList *args)// compare two prototypes structure and type, return 0 if they are the same, -1 if they are different
{
    Int prototype1 = stack_shift(*args);
    Int prototype2 = stack_shift(*args);

    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (str_find(vm->hashes->data[i].key, vm->stack->data[prototype1].string) == 0)
        {
            char * tmpstr = str_replace_all(vm->hashes->data[i].key, vm->stack->data[prototype1].string, vm->stack->data[prototype2].string);
            Int index = hash_find(vm, tmpstr);
            free(tmpstr);
            if (index == -1)
            {
                return new_number(vm, 0);
            }
        }
        else if (str_find(vm->hashes->data[i].key, vm->stack->data[prototype2].string) == 0)
        {
            char * tmpstr = str_replace_all(vm->hashes->data[i].key, vm->stack->data[prototype2].string, vm->stack->data[prototype1].string);
            Int index = hash_find(vm, tmpstr);
            free(tmpstr);
            if (index == -1)
            {
                return new_number(vm, 0);
            }
        }
    }

    return new_number(vm, 1);
}

Int std_prototype_hold(VirtualMachine *vm, IntList *args)
{
    Int prototype = stack_shift(*args);
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (str_find(vm->hashes->data[i].key, vm->stack->data[prototype].string) == 0)
        {
            hold_var(vm, vm->hashes->data[i].index);
        }
    }
    return -1;
}

Int std_prototype_unhold(VirtualMachine *vm, IntList *args)
{
    Int prototype = stack_shift(*args);
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (str_find(vm->hashes->data[i].key, vm->stack->data[prototype].string) == 0)
        {
            unhold_var(vm, vm->hashes->data[i].index);
        }
    }
    return -1;
}

Int std_prototype_equals(VirtualMachine *vm, IntList *args)
{
    Int prototype1 = stack_shift(*args);
    Int prototype2 = stack_shift(*args);
    Int result = -1;
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (str_find(vm->hashes->data[i].key, vm->stack->data[prototype1].string) == 0)
        {
            char * tmpstr = str_replace_all(vm->hashes->data[i].key, vm->stack->data[prototype1].string, vm->stack->data[prototype2].string);
            Int index = hash_find(vm, tmpstr);
            free(tmpstr);
            if (index == -1)
            {
                return new_number(vm, 0);
            }
        }
    }
    return result;
}

// inits

void init_basics(VirtualMachine *vm)
{
    
    hold_var(vm,spawn_builtin(vm, "#", std_ignore));
    hold_var(vm,spawn_builtin(vm, "eval", std_eval));
    hold_var(vm,spawn_builtin(vm, "return", std_return));
    hold_var(vm,spawn_builtin(vm, "function", std_function));
#ifndef ARDUINO
    hold_var(vm,spawn_builtin(vm, "ls", std_io_ls));
    hold_var(vm,spawn_builtin(vm, "print", std_io_print));
#endif
}

void init_type(VirtualMachine *vm)
{
    // types
    hold_var(vm,spawn_number(vm, "type.size", sizeof(Value)));
    hold_var(vm,spawn_number(vm, "type.nil", TYPE_NIL));
    hold_var(vm,spawn_number(vm, "type.list", TYPE_LIST));
    hold_var(vm,spawn_number(vm, "type.number", TYPE_NUMBER));
    hold_var(vm,spawn_number(vm, "type.string", TYPE_STRING));
    hold_var(vm,spawn_number(vm, "type.builtin", TYPE_BUILTIN));
    hold_var(vm,spawn_number(vm, "type.function", TYPE_FUNCTION));

    // type functions
    hold_var(vm,spawn_builtin(vm, "type.get", std_type_get));
    hold_var(vm,spawn_builtin(vm, "type.set", std_type_set));
}

void init_loop(VirtualMachine *vm)
{
    hold_var(vm,spawn_builtin(vm, "loop.while", std_loop_while));
    hold_var(vm,spawn_builtin(vm, "loop.repeat", std_loop_repeat));
}

void init_hash(VirtualMachine *vm)
{
    hold_var(vm,spawn_builtin(vm, "hash.set", std_hash_set));
    hold_var(vm,spawn_builtin(vm, "hash.delete", std_hash_delete));
    hold_var(vm,spawn_builtin(vm, "hash.rename", std_hash_rename));
}

void init_manual_memory(VirtualMachine *vm)
{
    hold_var(vm,spawn_builtin(vm, "mem.hold", std_mem_hold));
    hold_var(vm,spawn_builtin(vm, "mem.keep", std_mem_keep));
    hold_var(vm,spawn_builtin(vm, "mem.edit", std_mem_edit));
    hold_var(vm,spawn_builtin(vm, "mem.len", std_mem_length));
    hold_var(vm,spawn_builtin(vm, "mem.clear", std_mem_clear));
    hold_var(vm,spawn_builtin(vm, "mem.rebase", std_mem_rebase));
    hold_var(vm,spawn_builtin(vm, "mem.unhold", std_mem_unhold));
    hold_var(vm,spawn_builtin(vm, "mem.delete", std_mem_delete));
    hold_var(vm,spawn_builtin(vm, "mem.collect", std_mem_collect));
}

void init_prototype(VirtualMachine *vm)
{
    hold_var(vm,spawn_builtin(vm, "prototype.copy", std_prototype_copy));
    hold_var(vm,spawn_builtin(vm, "prototype.hold", std_prototype_hold));
    hold_var(vm,spawn_builtin(vm, "prototype.unhold", std_prototype_unhold));
    hold_var(vm,spawn_builtin(vm, "prototype.equals", std_prototype_equals));
    hold_var(vm,spawn_builtin(vm, "prototype.compare", std_prototype_compare));
}

void init_math(VirtualMachine *vm)
{
    hold_var(vm,spawn_builtin(vm, "add", std_math_add));
    hold_var(vm,spawn_builtin(vm, "sub", std_math_sub));
    hold_var(vm,spawn_builtin(vm, "mul", std_math_mul));
    hold_var(vm,spawn_builtin(vm, "div", std_math_div));
    hold_var(vm,spawn_builtin(vm, "mod", std_math_mod));
    hold_var(vm,spawn_builtin(vm, "pow", std_math_pow));
    hold_var(vm,spawn_builtin(vm, "abs", std_math_abs));
    hold_var(vm,spawn_builtin(vm, "sqrt", std_math_sqrt));
    hold_var(vm,spawn_builtin(vm, "ceil", std_math_ceil));
    hold_var(vm,spawn_builtin(vm, "seed", std_math_seed));
    hold_var(vm,spawn_builtin(vm, "floor", std_math_floor));
    hold_var(vm,spawn_builtin(vm, "round", std_math_round));
    hold_var(vm,spawn_builtin(vm, "random", std_math_random));
    hold_var(vm,spawn_builtin(vm, "incr", std_math_increment));
    hold_var(vm,spawn_builtin(vm, "decr", std_math_decrement));
}

void init_list(VirtualMachine *vm)
{
    hold_var(vm,spawn_builtin(vm, "list.new", std_list_new));
    hold_var(vm,spawn_builtin(vm, "list.pop", std_list_pop));
    hold_var(vm,spawn_builtin(vm, "list.get", std_list_get));
    hold_var(vm,spawn_builtin(vm, "list.len", std_list_length));
    hold_var(vm,spawn_builtin(vm, "list.push", std_list_push));
    hold_var(vm,spawn_builtin(vm, "list.find", std_list_find));
    hold_var(vm,spawn_builtin(vm, "list.shift", std_list_shift));
    hold_var(vm,spawn_builtin(vm, "list.insert", std_list_insert));
    hold_var(vm,spawn_builtin(vm, "list.remove", std_list_remove));
    hold_var(vm,spawn_builtin(vm, "list.concat", std_list_concat));
    hold_var(vm,spawn_builtin(vm, "list.unshift", std_list_unshift));
}

void init_string(VirtualMachine *vm)
{
    //spawn_builtin(vm, "string.format", std_string_new);
    hold_var(vm,spawn_builtin(vm, "string.sub", std_string_ndup));
    hold_var(vm,spawn_builtin(vm, "string.find", std_string_find));
    hold_var(vm,spawn_builtin(vm, "string.len", std_string_length));
    hold_var(vm,spawn_builtin(vm, "string.split", std_string_split));
    hold_var(vm,spawn_builtin(vm, "string.concat", std_string_concat));
    hold_var(vm,spawn_builtin(vm, "string.format", std_string_format));
    hold_var(vm,spawn_builtin(vm, "string.replace", std_string_replace));
    hold_var(vm,spawn_builtin(vm, "string.to.number", std_string_to_number));
    hold_var(vm,spawn_builtin(vm, "string.replace.all", std_string_replace_all));
}

void init_condition(VirtualMachine *vm)
{
    hold_var(vm,spawn_builtin(vm, "or", std_condition_or));
    hold_var(vm,spawn_builtin(vm, "if", std_condition_if));
    hold_var(vm,spawn_builtin(vm, "<", std_condition_less));
    hold_var(vm,spawn_builtin(vm, "and", std_condition_and));
    hold_var(vm,spawn_builtin(vm, "not", std_condition_not));
    hold_var(vm,spawn_builtin(vm, "==", std_condition_equals));
    hold_var(vm,spawn_builtin(vm, ">", std_condition_greater));
    hold_var(vm,spawn_builtin(vm, "ifelse", std_condition_ifelse));
    hold_var(vm,spawn_builtin(vm, "!=", std_condition_not_equals));
    hold_var(vm,spawn_builtin(vm, "<=", std_condition_less_equals));
    hold_var(vm,spawn_builtin(vm, ">=", std_condition_greater_equals));
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
    init_manual_memory(vm);
    init_prototype(vm);
    init_math(vm);
    init_list(vm);
    init_string(vm);
    init_condition(vm);
    init_default_vars(vm);
}