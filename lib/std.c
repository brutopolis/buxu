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
        // removes
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

        // updates
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

Int std_mem_edit(VirtualMachine *vm, IntList *args)
{
    Int variable = stack_shift(*args);
    Int value = stack_shift(*args);
    
    if (variable >= 0 && variable < vm->stack->size)
    {
        if(vm->typestack->data[variable] == TYPE_STRING)
        {
            free(vm->stack->data[variable].string);
        }

        if (value >= 0 && value < vm->stack->size)
        {
            vm->stack->data[variable] = vm->stack->data[value];
            vm->typestack->data[variable] = vm->typestack->data[value];
        }
    }
    
    return -1;
}

Int std_mem_copy(VirtualMachine *vm, IntList *args)
{
    Int variable = stack_shift(*args);
    Int value = stack_shift(*args);
    Int newvar = new_var(vm);
    
    if (variable >= 0 && variable < vm->stack->size)
    {
        if (value >= 0 && value < vm->stack->size)
        {
            vm->stack->data[newvar] = value_duplicate(vm->stack->data[value], vm->typestack->data[value]);
            vm->typestack->data[newvar] = vm->typestack->data[value];
        }
    }
    
    return newvar;
}

Int std_mem_get(VirtualMachine *vm, IntList *args)
{
    Int index = (Int)stack_shift(*args);
    if (index >= 0 && index < vm->stack->size)
    {
        return index;
    }
    
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

Int std_io_ls_all(VirtualMachine *vm, IntList *args)
{
    for (Int i = 0; i < vm->stack->size; i++)
    {
        printf("[%ld]: ", i);
        print_element(vm, i);
        printf("\n");
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
        if (vm->typestack->data[a] == TYPE_STRING)
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
        if (vm->typestack->data[b] == TYPE_STRING)
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
// inits

void init_basics(VirtualMachine *vm)
{
    
    registerBuiltin(vm, "#", std_ignore);
    registerBuiltin(vm, "eval", std_eval);
    registerBuiltin(vm, "return", std_return);
#ifndef ARDUINO
    registerBuiltin(vm, "ls", std_io_ls_all);
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

void init_manual_memory(VirtualMachine *vm)
{
    registerBuiltin(vm, "mem.get", std_mem_get);
    registerBuiltin(vm, "mem.hold", std_mem_hold);
    registerBuiltin(vm, "mem.keep", std_mem_keep);
    registerBuiltin(vm, "mem.edit", std_mem_edit);
    registerBuiltin(vm, "mem.copy", std_mem_copy);
    registerBuiltin(vm, "mem.len", std_mem_length);
    registerBuiltin(vm, "mem.clear", std_mem_clear);
    registerBuiltin(vm, "mem.rebase", std_mem_rebase);
    registerBuiltin(vm, "mem.unhold", std_mem_unhold);
    registerBuiltin(vm, "mem.delete", std_mem_delete);
    registerBuiltin(vm, "mem.collect", std_mem_collect);
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
    init_math(vm);
    init_string(vm);
    init_condition(vm);
    init_default_vars(vm);
}