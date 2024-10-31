#include "bruter.h"

Int raw_byte_set(VirtualMachine *vm, IntList *args)
{
    Int _block = stack_shift(*args);
    Int index = (Int)vm->stack->data[stack_shift(*args)].number;
    Int value = stack_shift(*args);
    if (index >= 0 && index < sizeof(Float))
    {
        vm->stack->data[_block].byte[index] = (unsigned char)vm->stack->data[value].number;
    }
    return -1;
}

Int raw_byte_get(VirtualMachine *vm, IntList *args)
{
    Int value = stack_shift(*args);
    Int index = (Int)vm->stack->data[stack_shift(*args)].number;
    return new_number(vm, vm->stack->data[value].byte[index]);
}

// integer math

Int raw_integer_add(VirtualMachine *vm, IntList *args)
{
    Int result = new_var(vm);
    vm->typestack->data[result] = TYPE_RAW;
    vm->stack->data[result].integer = vm->stack->data[stack_shift(*args)].integer;
    while (args->size > 0)
    {
        vm->stack->data[result].integer += vm->stack->data[stack_shift(*args)].integer;
    }
    return result;
}

Int raw_integer_sub(VirtualMachine *vm, IntList *args)
{
    Int result = new_var(vm);
    vm->typestack->data[result] = TYPE_RAW;
    vm->stack->data[result].integer = vm->stack->data[stack_shift(*args)].integer;
    while (args->size > 0)
    {
        vm->stack->data[result].integer -= vm->stack->data[stack_shift(*args)].integer;
    }
    return result;
}

Int raw_integer_mul(VirtualMachine *vm, IntList *args)
{
    Int result = new_var(vm);
    vm->typestack->data[result] = TYPE_RAW;
    vm->stack->data[result].integer = vm->stack->data[stack_shift(*args)].integer;
    while (args->size > 0)
    {
        vm->stack->data[result].integer *= vm->stack->data[stack_shift(*args)].integer;
    }
    return result;
}

Int raw_integer_div(VirtualMachine *vm, IntList *args)
{
    Int result = new_var(vm);
    vm->typestack->data[result] = TYPE_RAW;
    vm->stack->data[result].integer = vm->stack->data[stack_shift(*args)].integer;
    while (args->size > 0)
    {
        vm->stack->data[result].integer /= vm->stack->data[stack_shift(*args)].integer;
    }
    return result;
}

Int raw_integer_mod(VirtualMachine *vm, IntList *args)
{
    Int result = new_var(vm);
    vm->typestack->data[result] = TYPE_RAW;
    vm->stack->data[result].integer = vm->stack->data[stack_shift(*args)].integer;
    while (args->size > 0)
    {
        vm->stack->data[result].integer %= vm->stack->data[stack_shift(*args)].integer;
    }
    return result;
}

Int raw_from_string(VirtualMachine *vm, IntList *args)
{
    Int result = new_var(vm);
    vm->typestack->data[result] = TYPE_RAW;
    Int _str = stack_shift(*args);
    for (Int i = 0; i < sizeof(Float); i++)
    {
        if (i < strlen(vm->stack->data[_str].string))
        {
            vm->stack->data[result].byte[i] = (unsigned char)vm->stack->data[_str].string[i];
        }
        else
        {
            vm->stack->data[result].byte[i] = 0;
        }
    }
    return result;
}

Int raw_to_string(VirtualMachine *vm, IntList *args)
{
    Int result = new_var(vm);
    vm->typestack->data[result] = TYPE_STRING;
    char* str = (char*)malloc(sizeof(char) * (sizeof(Float) + 1));
    for (Int i = 0; i < sizeof(Float); i++)
    {
        str[i] = vm->stack->data[stack_shift(*args)].byte[i];
    }
    str[sizeof(Float)] = '\0';
    vm->stack->data[result].string = str;
    return result;
}

Int raw_from_float(VirtualMachine *vm, IntList *args)
{
    Int result = new_var(vm);
    vm->typestack->data[result] = TYPE_RAW;
    vm->stack->data[result].integer = (Int)vm->stack->data[stack_shift(*args)].number;
    return result;
}

Int raw_to_float(VirtualMachine *vm, IntList *args)
{
    Int result = new_var(vm);
    vm->typestack->data[result] = TYPE_NUMBER;
    vm->stack->data[result].number = (Float)vm->stack->data[stack_shift(*args)].integer;
    return result;
}

Int raw_from_bytes(VirtualMachine *vm, IntList *args)
{
    Int result = new_var(vm);
    vm->typestack->data[result] = TYPE_RAW;
    for (Int i = 0; i < sizeof(Float); i++)
    {
        if (args->size > 0)
        {
            vm->stack->data[result].byte[i] = (unsigned char)vm->stack->data[stack_shift(*args)].number;
        }
        else
        {
            vm->stack->data[result].byte[i] = 0;
        }
    }
    return result;
}

// byte math

Int raw_byte_add(VirtualMachine *vm, IntList *args)
{
    Int _block = stack_shift(*args);
    Int _index = stack_shift(*args);
    Int _value = stack_shift(*args);
    vm->stack->data[_block].byte[(Int)vm->stack->data[_index].number] += vm->stack->data[_value].number;
    return -1;
}

Int raw_byte_sub(VirtualMachine *vm, IntList *args)
{
    Int _block = stack_shift(*args);
    Int _index = stack_shift(*args);
    Int _value = stack_shift(*args);
    vm->stack->data[_block].byte[(Int)vm->stack->data[_index].number] -= vm->stack->data[_value].number;
    return -1;
}

void init_raw(VirtualMachine *vm)
{
    registerBuiltin(vm, "byte.set", raw_byte_set);
    registerBuiltin(vm, "byte.get", raw_byte_get);
    registerBuiltin(vm, "byte.add", raw_byte_add);
    registerBuiltin(vm, "byte.sub", raw_byte_sub);

    registerBuiltin(vm, "int.add", raw_integer_add);
    registerBuiltin(vm, "int.sub", raw_integer_sub);
    registerBuiltin(vm, "int.mul", raw_integer_mul);
    registerBuiltin(vm, "int.div", raw_integer_div);
    registerBuiltin(vm, "int.mod", raw_integer_mod);

    registerBuiltin(vm, "int.to.float", raw_to_float);
    registerBuiltin(vm, "int.from.float", raw_from_float);

    registerBuiltin(vm, "raw.to.string", raw_to_string);
    registerBuiltin(vm, "raw.from.string", raw_from_string);
    registerBuiltin(vm, "raw.from.bytes", raw_from_bytes);
}