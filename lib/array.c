#include "buxu.h"

#define TYPE_ARRAY 3

Int new_array(VirtualMachine *vm, Int size)
{
    // arrays dont resuse unused indexes
    list_push(*vm->stack, (Value){.f = size});
    list_push(*vm->typestack, TYPE_ARRAY);
    Int id = vm->stack->size - 1;
    for (Int i = 0; i < size; i++)
    {
        list_push(*vm->stack, (Value){.p = NULL});
        list_push(*vm->typestack, TYPE_ANY);
    }
    return id;
}

Int register_array(VirtualMachine *vm, char* varname, Int size)
{
    Int index = new_array(vm, size);
    hash_set(vm, varname, index);
    return index;
}

char* array_stringify(VirtualMachine* vm, Int list_index)
{
    char* _str = str_duplicate("(array: ");
    char* strbak;

    Int list_size = data(list_index).f;

    if (!list_size) // if 0
    {
        strbak = _str;
        _str = str_concat(_str, ")");
        free(strbak);
        return _str;
    }
    else if (list_index + list_size + 1 > vm->stack->size)
    {
        buxu_error("misformed array @%ld, size %ld", list_index, list_size);
        strbak = _str;
        _str = str_concat(_str, ")");
        free(strbak);
        return _str;
    }

    for (Int i = list_index+1; i < list_index + list_size + 1; i++)
    {
        switch (data_t(i))
        {
        case TYPE_STRING:
            strbak = _str;
            _str = str_format("%s(@ %s)", _str, data(i).s); 
            free(strbak);
            break;
        case TYPE_NUMBER:
            strbak = _str;
            
            if (data(i).f == (Int)data(i).f)
            {
                char* str = str_format("%ld", (Int)data(i).f);
                _str = str_concat(_str, str);
                free(str);
            }
            else 
            {
                char* str = str_format("%lf", data(i).f);
                _str = str_concat(_str, str);
                free(str);
            }

            free(strbak);
            break;
        case TYPE_ARRAY:
            strbak = _str;
            char* stringified = array_stringify(vm, i);
            _str = str_concat(_str, stringified);
            free(strbak);
            break;
        default:
            strbak = _str;
            _str = str_format("%s%ld", _str, data(i).i);
            free(strbak);
            break;
        }

        if (i < list_index + list_size)
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


// array functions
// array functions
// array functions
// array functions

function(brl_std_array_new)
{
    Int index;
    if (args->size == 0)
    {
        index = new_array(vm, 0);
    }
    else
    {
        index = new_array(vm, arg(0).f);
    }
    return index;
}

function(brl_std_array_set)
{
    if (args->size == 2)
    {
        if (arg_i(0) >= 0 && arg_i(0) < vm->stack->size)
        {
            // if the value is a string, we need to free it before
            if (arg_t(0) == TYPE_STRING)
            {
                unuse_var(vm, arg_i(0));
            }

            arg(0) = arg(1);
            arg_t(0) = arg_t(1);
        }
    }
    else if (arg_t(0) == TYPE_ARRAY)
    {
        Int list_size = arg(0).f;
        Int index = arg_i(0);

        
        if (index >= 0 && index < arg_i(0) + list_size /*&& index < vm->stack->size*/)
        {
            data(index) = value_duplicate(arg(1), arg_t(1));
        }
        else 
        {
            buxu_error("index %ld out of range in list %ld of size %ld", index, arg_i(0), list_size);
        }
        return -1;
    }
    else if (arg_t(0) == TYPE_STRING)
    {
        char *str = arg(0).s;
        Int index = arg(1).f;
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
            buxu_error("index %ld out of range in string '%s' of size %ld", index, arg(0).s, strlen(str));
        }
        return -1;
    }
    else 
    {
        Int index = (Int)arg(1).f;
        Int value = arg_i(0);
        if (index >= 0 && index < sizeof(Float))
        {
            arg(0).u8[index] = (char)value;
        }
    }
    return -1;
}

function(brl_std_array_length)
{
    if (args->size == 0)
    {
        return new_number(vm, vm->stack->size);
    }
    else if (arg_t(0) == TYPE_ARRAY)
    {
        return new_number(vm, arg(0).f);
    }
    else if (arg_t(0) == TYPE_STRING)
    {
        return new_number(vm, strlen(arg(0).s));
    }
    /*
    else 
    {
        return new_number(vm, sizeof(Float));
    }
    */
    return -1;
}

function(brl_std_array_get)
{
    if (args->size == 1) // global
    {
        return arg(0).f;
    }
    else if (arg_t(0) == TYPE_ARRAY) // array
    {
        Int index = arg_i(0);
        Int list_size = data(index).f;
        Int i = arg_i(1);
        if (i >= 0 && i < list_size)
        {
            return index + 1 + i;
        }
        else 
        {
            buxu_error("index %ld out of range in array %ld of size %ld", i, index, list_size);
        }
    }
    else if (arg_t(0) == TYPE_STRING) // 
    {
        Int index = arg_i(0);
        Int i = arg_i(1);
        if (i >= 0 && i < strlen(arg(0).s))
        {
            return arg(0).s[i];
        }
        else 
        {
            buxu_error("index %ld out of range in string '%s' of size %ld", i, arg(0).s, strlen(arg(0).s));
        }
    }
    else 
    {
        Int index = (Int)arg(0).i;
        Int i = arg_i(1);
        if (i >= 0 && i < sizeof(Float))
        {
            return arg(0).u8[i];
        }
    }
    return -1;
}

function(buxu_std_array_resize)
{
    // we reallocate the stack data
    Int array_index = arg_i(0);
    Int new_size = arg(1).f;
    Int old_size = data(array_index).f;
    Int diff = new_size - old_size;

    if (diff > 0)
    {
        // lets see if the capacity is enough
        if (vm->stack->capacity < vm->stack->size + diff)
        {
            list_double(*vm->stack);
            list_double(*vm->typestack);
        }
        
        // lets move the elements manually with memmove then we update the size
        memmove(&(vm->stack->data[array_index + 1 + new_size]), &(vm->stack->data[array_index + 1 + old_size]), (vm->stack->size - (array_index + 1 + old_size)) * sizeof(Value));
        memmove(&(vm->typestack->data[array_index + 1 + new_size]), &(vm->typestack->data[array_index + 1 + old_size]), (vm->typestack->size - (array_index + 1 + old_size)) * sizeof(Byte));
        
        // lets update the size
        vm->stack->size += diff;
        vm->typestack->size += diff;
        
        // lets update the size of the array
        data(array_index).f = new_size;

        // lets initialize the new elements with zeroes
        for (Int i = 0; i < diff; i++)
        {
            data(array_index + 1 + old_size + i).f = 0;
            data_t(array_index + 1 + old_size + i) = TYPE_ANY;
        }
    }
    else if (diff < 0)
    {
        // lets move the elements manually with memmove then we update the size
        memmove(&(vm->stack->data[array_index + 1 + new_size]), &(vm->stack->data[array_index + 1 + old_size]), (vm->stack->size - (array_index + 1 + new_size)) * sizeof(Value));
        memmove(&(vm->typestack->data[array_index + 1 + new_size]), &(vm->typestack->data[array_index + 1 + old_size]), (vm->typestack->size - (array_index + 1 + new_size)) * sizeof(Byte));
        
        // lets update the size
        vm->stack->size += diff;
        vm->typestack->size += diff;
        
        // lets update the size of the array
        data(array_index).f = new_size;
    }
    else 
    {
        // nothing to do
        buxu_warn("resizing array @%ld to the same size: %ld", array_index, new_size);
        return -1;
    }

    // lets update the hashes
    for (Int i = 0; i < vm->hash_names->size; i++)
    {
        if (vm->hash_indexes->data[i] > array_index + old_size)
        {
            vm->hash_indexes->data[i] += diff;
        }
    }
    return -1;
}

function(buxu_std_array_fill)
{
    if (arg_t(0) == TYPE_ARRAY)
    {
        Int index = arg_i(0);
        Int size = data(index).f;
        memset(&(data(index + 1)), arg_i(1), size * sizeof(Value));
        if (args->size == 3)
        {
            memset(&(data_t(index + 1)), arg_i(2), size * sizeof(Byte));
        }
        else 
        {
            memset(&(data_t(index + 1)), 0, size * sizeof(Byte));
        }
    }
    else 
    {
        buxu_error("@%ld is not an array", arg_i(0));
    }
    return -1;
}

function(buxu_std_array_clear)
{
    if (arg_t(0) == TYPE_ARRAY)
    {
        Int index = arg_i(0);
        Int size = data(index).f;
        memset(&(data(index + 1)), 0, size * sizeof(Value));
        memset(&(data_t(index + 1)), 0, size * sizeof(Byte));
        for (Int i = arg_i(0) + 1; i < arg_i(0) + 1 + data(arg_i(0)).f; i++)
        {
            list_push(*vm->unused, i);
        }
    }
    
    return -1;
}

function(buxu_std_array_print)
{
    char* stringified = array_stringify(vm, arg_i(0));
    printf("%s", stringified);
    free(stringified);
    return -1;
}

init(array)
{
    register_number(vm, "type.array", TYPE_ARRAY);

    register_builtin(vm, "array", brl_std_array_new);
    register_builtin(vm, "set", brl_std_array_set);
    register_builtin(vm, "get", brl_std_array_get);
    register_builtin(vm, "len", brl_std_array_length);
    register_builtin(vm, "resize", buxu_std_array_resize);
    register_builtin(vm, "fill", buxu_std_array_fill);
    register_builtin(vm, "clear", buxu_std_array_clear);

    register_builtin(vm, "array.print", buxu_std_array_print);
}
