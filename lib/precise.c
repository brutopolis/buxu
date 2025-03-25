#include "buxu.h"

// functions to deal with all number types of the union Value
// u8 u16 u32 u64 i8 i16 i32 i64 f32 f64

function(bu_precise_u8_set)
{
    if (args->size == 2)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float))
        {
            arg(0).u8[arg_i(0)] = arg(1).number;
        }
    }
    return -1;
}

function(bu_precise_u8_get)
{
    if (args->size == 1)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float))
        {
            return new_number(vm, arg(0).u8[arg_i(0)]);
        }
    }
    return -1;
}

function(bu_precise_u16_set)
{
    if (args->size == 2)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float) / 2)
        {
            arg(0).u16[arg_i(0)] = arg(1).number;
        }
    }
    return -1;
}

function(bu_precise_u16_get)
{
    if (args->size == 1)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float) / 2)
        {
            return new_number(vm, arg(0).u16[arg_i(0)]);
        }
    }
    return -1;
}

function(bu_precise_u32_set)
{
    if (args->size == 2)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float) / 4)
        {
            arg(0).u32[arg_i(0)] = arg(1).number;
        }
    }
    return -1;
}

function(bu_precise_u32_get)
{
    if (args->size == 1)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float) / 4)
        {
            return new_number(vm, arg(0).u32[arg_i(0)]);
        }
    }
    return -1;
}

function(bu_precise_u64_set)
{
    if (args->size == 2)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float) / 8)
        {
            arg(0).u64[arg_i(0)] = arg(1).number;
        }
    }
    return -1;
}

function(bu_precise_u64_get)
{
    if (args->size == 1)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float) / 8)
        {
            return new_number(vm, arg(0).u64[arg_i(0)]);
        }
    }
    return -1;
}

function(bu_precise_i8_set)
{
    if (args->size == 2)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float))
        {
            arg(0).i8[arg_i(0)] = arg(1).number;
        }
    }
    return -1;
}

function(bu_precise_i8_get)
{
    if (args->size == 1)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float))
        {
            return new_number(vm, arg(0).i8[arg_i(0)]);
        }
    }
    return -1;
}

function(bu_precise_i16_set)
{
    if (args->size == 2)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float) / 2)
        {
            arg(0).i16[arg_i(0)] = arg(1).number;
        }
    }
    return -1;
}

function(bu_precise_i16_get)
{
    if (args->size == 1)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float) / 2)
        {
            return new_number(vm, arg(0).i16[arg_i(0)]);
        }
    }
    return -1;
}

function(bu_precise_i32_set)
{
    if (args->size == 2)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float) / 4)
        {
            arg(0).i32[arg_i(0)] = arg(1).number;
        }
    }
    return -1;
}

function(bu_precise_i32_get)
{
    if (args->size == 1)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float) / 4)
        {
            return new_number(vm, arg(0).i32[arg_i(0)]);
        }
    }
    return -1;
}

function(bu_precise_i64_set)
{
    if (args->size == 2)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float) / 8)
        {
            arg(0).i64[arg_i(0)] = arg(1).number;
        }
    }
    return -1;
}

function(bu_precise_i64_get)
{
    if (args->size == 1)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float) / 8)
        {
            return new_number(vm, arg(0).i64[arg_i(0)]);
        }
    }
    return -1;
}

function(bu_precise_f32_set)
{
    if (args->size == 2)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float) / 4)
        {
            arg(0).f32[arg_i(0)] = arg(1).number;
        }
    }
    return -1;
}

function(bu_precise_f32_get)
{
    if (args->size == 1)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float) / 4)
        {
            return new_number(vm, arg(0).f32[arg_i(0)]);
        }
    }
    return -1;
}

function(bu_precise_f64_set)
{
    if (args->size == 2)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float) / 8)
        {
            arg(0).number = arg(1).number;
        }
    }
    return -1;
}

function(bu_precise_f64_get)
{
    if (args->size == 1)
    {
        if (arg_i(0) >= 0 && arg_i(0) < sizeof(Float) / 8)
        {
            return new_number(vm, arg(0).f64[arg_i(0)]);
        }
    }
    return -1;
}

function(bu_precise_cast)
{
    Int source = arg_i(0);
    Int type = arg_i(1);
    Int index = 0;
    if (args->size == 3)
    {
        index = arg_i(2);
    }

    if (source < 0 || source >= vm->stack->size)
    {
        buxu_error("source index %ld out of range", source);
        return -1;
    }

    if (type < 0 || type >= vm->stack->size)
    {
        buxu_error("type index %ld out of range", type);
        return -1;
    }

    switch (arg_t(0))
    {
        case TYPE_NUMBER:
            switch ((Int)arg(1).number)
            {
                case TYPE_ARRAY+1:
                    arg(0).u8[0] = arg(0).number;
                    break;
                case TYPE_ARRAY+2:
                    arg(0).u16[0] = arg(0).number;
                    break;
                case TYPE_ARRAY+3:
                    arg(0).u32[0] = arg(0).number;
                    break;
                case TYPE_ARRAY+4:
                    arg(0).u64[0] = arg(0).number;
                    break;
                case TYPE_ARRAY+5:
                    arg(0).i8[0] = arg(0).number;
                    break;
                case TYPE_ARRAY+6:
                    arg(0).i16[0] = arg(0).number;
                    break;
                case TYPE_ARRAY+7:
                    arg(0).i32[0] = arg(0).number;
                    break;
                case TYPE_ARRAY+8:
                    arg(0).i64[0] = arg(0).number;
                    break;
                case TYPE_ARRAY+9:
                    arg(0).f32[0] = arg(0).number;
                    break;
                case TYPE_ARRAY+10:
                    arg(0).f64[0] = arg(0).number;
                    break;
                case TYPE_ARRAY+11:
                    arg(0).integer = arg(0).number;
                    break;
                default:
                    buxu_error("precise cast cannot cast to type %ld", type);
                    break;
            }
            break;
        case TYPE_ARRAY+1:
            switch ((Int)arg(1).number)
            {
                case TYPE_NUMBER:
                    arg(0).number = arg(0).u8[index];
                    break;
                case TYPE_ARRAY+2:
                    arg(0).u16[0] = arg(0).u8[index];
                    break;
                case TYPE_ARRAY+3:
                    arg(0).u32[0] = arg(0).u8[index];
                    break;
                case TYPE_ARRAY+4:
                    arg(0).u64[0] = arg(0).u8[index];
                    break;
                case TYPE_ARRAY+5:
                    arg(0).i8[0] = arg(0).u8[index];
                    break;
                case TYPE_ARRAY+6:
                    arg(0).i16[0] = arg(0).u8[index];
                    break;
                case TYPE_ARRAY+7:
                    arg(0).i32[0] = arg(0).u8[index];
                    break;
                case TYPE_ARRAY+8:
                    arg(0).i64[0] = arg(0).u8[index];
                    break;
                case TYPE_ARRAY+9:
                    arg(0).f32[0] = arg(0).u8[index];
                    break;
                case TYPE_ARRAY+10:
                    arg(0).f64[0] = arg(0).u8[index];
                    break;
                case TYPE_ARRAY+11:
                    arg(0).integer = arg(0).u8[index];
                    break;
                default:
                    buxu_error("precise cast cannot cast to type %ld", type);
                    break;
            }
            break;
        case TYPE_ARRAY+2:
            switch ((Int)arg(1).number)
            {
                case TYPE_NUMBER:
                    arg(0).number = arg(0).u16[index];
                    break;
                case TYPE_ARRAY+1:
                    arg(0).u8[0] = arg(0).u16[index];
                    break;
                case TYPE_ARRAY+3:
                    arg(0).u32[0] = arg(0).u16[index];
                    break;
                case TYPE_ARRAY+4:
                    arg(0).u64[0] = arg(0).u16[index];
                    break;
                case TYPE_ARRAY+5:
                    arg(0).i8[0] = arg(0).u16[index];
                    break;
                case TYPE_ARRAY+6:
                    arg(0).i16[0] = arg(0).u16[index];
                    break;
                case TYPE_ARRAY+7:
                    arg(0).i32[0] = arg(0).u16[index];
                    break;
                case TYPE_ARRAY+8:
                    arg(0).i64[0] = arg(0).u16[index];
                    break;
                case TYPE_ARRAY+9:
                    arg(0).f32[0] = arg(0).u16[index];
                    break;
                case TYPE_ARRAY+10:
                    arg(0).f64[0] = arg(0).u16[index];
                    break;
                case TYPE_ARRAY+11:
                    arg(0).integer = arg(0).u16[index];
                    break;
                default:
                    buxu_error("precise cast cannot cast to type %ld", type);
                    break;
            }
            break;
        case TYPE_ARRAY+3:
            switch ((Int)arg(1).number)
            {
                case TYPE_NUMBER:
                    arg(0).number = arg(0).u32[index];
                    break;
                case TYPE_ARRAY+1:
                    arg(0).u8[0] = arg(0).u32[index];
                    break;
                case TYPE_ARRAY+2:
                    arg(0).u16[0] = arg(0).u32[index];
                    break;
                case TYPE_ARRAY+4:
                    arg(0).u64[0] = arg(0).u32[index];
                    break;
                case TYPE_ARRAY+5:
                    arg(0).i8[0] = arg(0).u32[index];
                    break;
                case TYPE_ARRAY+6:
                    arg(0).i16[0] = arg(0).u32[index];
                    break;
                case TYPE_ARRAY+7:
                    arg(0).i32[0] = arg(0).u32[index];
                    break;
                case TYPE_ARRAY+8:
                    arg(0).i64[0] = arg(0).u32[index];
                    break;
                case TYPE_ARRAY+9:
                    arg(0).f32[0] = arg(0).u32[index];
                    break;
                case TYPE_ARRAY+10:
                    arg(0).f64[0] = arg(0).u32[index];
                    break;
                case TYPE_ARRAY+11:
                    arg(0).integer = arg(0).u32[index];
                    break;
                default:
                    buxu_error("precise cast cannot cast to type %ld", type);
                    break;
            }
            break;
        case TYPE_ARRAY+4:
            switch ((Int)arg(1).number)
            {
                case TYPE_NUMBER:
                    arg(0).number = arg(0).u64[index];
                    break;
                case TYPE_ARRAY+1:
                    arg(0).u8[0] = arg(0).u64[index];
                    break;
                case TYPE_ARRAY+2:
                    arg(0).u16[0] = arg(0).u64[index];
                    break;
                case TYPE_ARRAY+3:
                    arg(0).u32[0] = arg(0).u64[index];
                    break;
                case TYPE_ARRAY+5:
                    arg(0).i8[0] = arg(0).u64[index];
                    break;
                case TYPE_ARRAY+6:
                    arg(0).i16[0] = arg(0).u64[index];
                    break;
                case TYPE_ARRAY+7:
                    arg(0).i32[0] = arg(0).u64[index];
                    break;
                case TYPE_ARRAY+8:
                    arg(0).i64[0] = arg(0).u64[index];
                    break;
                case TYPE_ARRAY+9:
                    arg(0).f32[0] = arg(0).u64[index];
                    break;
                case TYPE_ARRAY+10:
                    arg(0).f64[0] = arg(0).u64[index];
                    break;
                case TYPE_ARRAY+11:
                    arg(0).integer = arg(0).u64[index];
                    break;
                default:
                    buxu_error("precise cast cannot cast to type %ld", type);
                    break;
            }
            break;
        case TYPE_ARRAY+5:
            switch ((Int)arg(1).number)
            {
                case TYPE_NUMBER:
                    arg(0).number = arg(0).i8[index];
                    break;
                case TYPE_ARRAY+1:
                    arg(0).u8[0] = arg(0).i8[index];
                    break;
                case TYPE_ARRAY+2:
                    arg(0).u16[0] = arg(0).i8[index];
                    break;
                case TYPE_ARRAY+3:
                    arg(0).u32[0] = arg(0).i8[index];
                    break;
                case TYPE_ARRAY+4:
                    arg(0).u64[0] = arg(0).i8[index];
                    break;
                case TYPE_ARRAY+6:
                    arg(0).i16[0] = arg(0).i8[index];
                    break;
                case TYPE_ARRAY+7:
                    arg(0).i32[0] = arg(0).i8[index];
                    break;
                case TYPE_ARRAY+8:
                    arg(0).i64[0] = arg(0).i8[index];
                    break;
                case TYPE_ARRAY+9:
                    arg(0).f32[0] = arg(0).i8[index];
                    break;
                case TYPE_ARRAY+10:
                    arg(0).f64[0] = arg(0).i8[index];
                    break;
                case TYPE_ARRAY+11:
                    arg(0).integer = arg(0).i8[index];
                    break;
                default:
                    buxu_error("precise cast cannot cast to type %ld", type);
                    break;
            }
            break;
        case TYPE_ARRAY+6:
            switch ((Int)arg(1).number)
            {
                case TYPE_NUMBER:
                    arg(0).number = arg(0).i16[index];
                    break;
                case TYPE_ARRAY+1:
                    arg(0).u8[0] = arg(0).i16[index];
                    break;
                case TYPE_ARRAY+2:
                    arg(0).u16[0] = arg(0).i16[index];
                    break;
                case TYPE_ARRAY+3:
                    arg(0).u32[0] = arg(0).i16[index];
                    break;
                case TYPE_ARRAY+4:
                    arg(0).u64[0] = arg(0).i16[index];
                    break;
                case TYPE_ARRAY+5:
                    arg(0).i8[0] = arg(0).i16[index];
                    break;
                case TYPE_ARRAY+7:
                    arg(0).i32[0] = arg(0).i16[index];
                    break;
                case TYPE_ARRAY+8:
                    arg(0).i64[0] = arg(0).i16[index];
                    break;
                case TYPE_ARRAY+9:
                    arg(0).f32[0] = arg(0).i16[index];
                    break;
                case TYPE_ARRAY+10:
                    arg(0).f64[0] = arg(0).i16[index];
                    break;
                case TYPE_ARRAY+11:
                    arg(0).integer = arg(0).i16[index];
                    break;
                default:
                    buxu_error("precise cast cannot cast to type %ld", type);
                    break;
            }
            break;
        case TYPE_ARRAY+7:
            switch ((Int)arg(1).number)
            {
                case TYPE_NUMBER:
                    arg(0).number = arg(0).i32[index];
                    break;
                case TYPE_ARRAY+1:
                    arg(0).u8[0] = arg(0).i32[index];
                    break;
                case TYPE_ARRAY+2:
                    arg(0).u16[0] = arg(0).i32[index];
                    break;
                case TYPE_ARRAY+3:
                    arg(0).u32[0] = arg(0).i32[index];
                    break;
                case TYPE_ARRAY+4:
                    arg(0).u64[0] = arg(0).i32[index];
                    break;
                case TYPE_ARRAY+5:
                    arg(0).i8[0] = arg(0).i32[index];
                    break;
                case TYPE_ARRAY+6:
                    arg(0).i16[0] = arg(0).i32[index];
                    break;
                case TYPE_ARRAY+8:
                    arg(0).i64[0] = arg(0).i32[index];
                    break;
                case TYPE_ARRAY+9:
                    arg(0).f32[0] = arg(0).i32[index];
                    break;
                case TYPE_ARRAY+10:
                    arg(0).f64[0] = arg(0).i32[index];
                    break;
                case TYPE_ARRAY+11:
                    arg(0).integer = arg(0).i32[index];
                    break;
                default:
                    buxu_error("precise cast cannot cast to type %ld", type);
                    break;
            }
            break;
        case TYPE_ARRAY+8:
            switch ((Int)arg(1).number)
            {
                case TYPE_NUMBER:
                    arg(0).number = arg(0).i64[index];
                    break;
                case TYPE_ARRAY+1:
                    arg(0).u8[0] = arg(0).i64[index];
                    break;
                case TYPE_ARRAY+2:
                    arg(0).u16[0] = arg(0).i64[index];
                    break;
                case TYPE_ARRAY+3:
                    arg(0).u32[0] = arg(0).i64[index];
                    break;
                case TYPE_ARRAY+4:
                    arg(0).u64[0] = arg(0).i64[index];
                    break;
                case TYPE_ARRAY+5:
                    arg(0).i8[0] = arg(0).i64[index];
                    break;
                case TYPE_ARRAY+6:
                    arg(0).i16[0] = arg(0).i64[index];
                    break;
                case TYPE_ARRAY+7:
                    arg(0).i32[0] = arg(0).i64[index];
                    break;
                case TYPE_ARRAY+9:
                    arg(0).f32[0] = arg(0).i64[index];
                    break;
                case TYPE_ARRAY+10:
                    arg(0).f64[0] = arg(0).i64[index];
                    break;
                case TYPE_ARRAY+11:
                    arg(0).integer = arg(0).i64[index];
                    break;
                default:
                    buxu_error("precise cast cannot cast to type %ld", type);
                    break;
            }
            break;
        case TYPE_ARRAY+9:
            switch ((Int)arg(1).number)
            {
                case TYPE_NUMBER:
                    arg(0).number = arg(0).f32[index];
                    break;
                case TYPE_ARRAY+1:
                    arg(0).u8[0] = arg(0).f32[index];
                    break;
                case TYPE_ARRAY+2:
                    arg(0).u16[0] = arg(0).f32[index];
                    break;
                case TYPE_ARRAY+3:
                    arg(0).u32[0] = arg(0).f32[index];
                    break;
                case TYPE_ARRAY+4:
                    arg(0).u64[0] = arg(0).f32[index];
                    break;
                case TYPE_ARRAY+5:
                    arg(0).i8[0] = arg(0).f32[index];
                    break;
                case TYPE_ARRAY+6:
                    arg(0).i16[0] = arg(0).f32[index];
                    break;
                case TYPE_ARRAY+7:
                    arg(0).i32[0] = arg(0).f32[index];
                    break;
                case TYPE_ARRAY+8:
                    arg(0).i64[0] = arg(0).f32[index];
                    break;
                case TYPE_ARRAY+10:
                    arg(0).f64[0] = arg(0).f32[index];
                    break;
                case TYPE_ARRAY+11:
                    arg(0).integer = arg(0).f32[index];
                    break;
                default:
                    buxu_error("precise cast cannot cast to type %ld", type);
                    break;
            }
            break;
        case TYPE_ARRAY+10:
            switch ((Int)arg(1).number)
            {
                case TYPE_NUMBER:
                    arg(0).number = arg(0).f64[index];
                    break;
                case TYPE_ARRAY+1:
                    arg(0).u8[0] = arg(0).f64[index];
                    break;
                case TYPE_ARRAY+2:
                    arg(0).u16[0] = arg(0).f64[index];
                    break;
                case TYPE_ARRAY+3:
                    arg(0).u32[0] = arg(0).f64[index];
                    break;
                case TYPE_ARRAY+4:
                    arg(0).u64[0] = arg(0).f64[index];
                    break;
                case TYPE_ARRAY+5:
                    arg(0).i8[0] = arg(0).f64[index];
                    break;
                case TYPE_ARRAY+6:
                    arg(0).i16[0] = arg(0).f64[index];
                    break;
                case TYPE_ARRAY+7:
                    arg(0).i32[0] = arg(0).f64[index];
                    break;
                case TYPE_ARRAY+8:
                    arg(0).i64[0] = arg(0).f64[index];
                    break;
                case TYPE_ARRAY+9:
                    arg(0).f32[0] = arg(0).f64[index];
                    break;
                case TYPE_ARRAY+11:
                    arg(0).integer = arg(0).f64[index];
                    break;
                default:
                    buxu_error("precise cast cannot cast to type %ld", type);
                    break;
            }
            break;
        case TYPE_ARRAY+11:
            switch ((Int)arg(1).number)
            {
                case TYPE_NUMBER:
                    arg(0).number = arg(0).integer;
                    break;
                case TYPE_ARRAY+1:
                    arg(0).u8[0] = arg(0).integer;
                    break;
                case TYPE_ARRAY+2:
                    arg(0).u16[0] = arg(0).integer;
                    break;
                case TYPE_ARRAY+3:
                    arg(0).u32[0] = arg(0).integer;
                    break;
                case TYPE_ARRAY+4:
                    arg(0).u64[0] = arg(0).integer;
                    break;
                case TYPE_ARRAY+5:
                    arg(0).i8[0] = arg(0).integer;
                    break;
                case TYPE_ARRAY+6:
                    arg(0).i16[0] = arg(0).integer;
                    break;
                case TYPE_ARRAY+7:
                    arg(0).i32[0] = arg(0).integer;
                    break;
                case TYPE_ARRAY+8:
                    arg(0).i64[0] = arg(0).integer;
                    break;
                case TYPE_ARRAY+9:
                    arg(0).f32[0] = arg(0).integer;
                    break;
                case TYPE_ARRAY+10:
                    arg(0).f64[0] = arg(0).integer;
                    break;
                default:
                    buxu_error("precise cast cannot cast to type %ld", type);
                    break;
            }
            break;
        default:
            buxu_error("precise cast cannot cast from type %ld", source);
            break;
    }
}

init(precise)
{
    // declare types
    register_number(vm, "type.u8", TYPE_ARRAY+1);
    register_number(vm, "type.u16", TYPE_ARRAY+2);
    register_number(vm, "type.u32", TYPE_ARRAY+3);
    register_number(vm, "type.u64", TYPE_ARRAY+4);
    register_number(vm, "type.i8", TYPE_ARRAY+5);
    register_number(vm, "type.i16", TYPE_ARRAY+6);
    register_number(vm, "type.i32", TYPE_ARRAY+7);
    register_number(vm, "type.i64", TYPE_ARRAY+8);
    register_number(vm, "type.f32", TYPE_ARRAY+9);
    register_number(vm, "type.f64", TYPE_ARRAY+10);
    register_number(vm, "type.integer", TYPE_ARRAY+11);

    // check if std is loaded
    if (hash_find(vm, "type.any") == -1)
    {
        register_number(vm, "type.float", TYPE_NUMBER);
    }

    // declare limits
    register_number(vm, "u8.min", 0);
    register_number(vm, "u8.max", UINT8_MAX);
    register_number(vm, "u16.min", 0);
    register_number(vm, "u16.max", UINT16_MAX);
    register_number(vm, "u32.min", 0);
    register_number(vm, "u32.max", UINT32_MAX);
    register_number(vm, "u64.min", 0);
    register_number(vm, "u64.max", UINT64_MAX);
    register_number(vm, "i8.min", INT8_MIN);
    register_number(vm, "i8.max", INT8_MAX);
    register_number(vm, "i16.min", INT16_MIN);
    register_number(vm, "i16.max", INT16_MAX);
    register_number(vm, "i32.min", INT32_MIN);
    register_number(vm, "i32.max", INT32_MAX);
    register_number(vm, "i64.min", INT64_MIN);
    register_number(vm, "i64.max", INT64_MAX);
    register_number(vm, "f32.min", -FLT_MAX);
    register_number(vm, "f32.max", FLT_MAX);
    register_number(vm, "f64.min", -DBL_MAX);
    register_number(vm, "f64.max", DBL_MAX);

    // declare functions
    register_builtin(vm, "u8.set", bu_precise_u8_set);
    register_builtin(vm, "u8.get", bu_precise_u8_get);
    register_builtin(vm, "u16.set", bu_precise_u16_set);
    register_builtin(vm, "u16.get", bu_precise_u16_get);
    register_builtin(vm, "u32.set", bu_precise_u32_set);
    register_builtin(vm, "u32.get", bu_precise_u32_get);
    register_builtin(vm, "u64.set", bu_precise_u64_set);
    register_builtin(vm, "u64.get", bu_precise_u64_get);
    register_builtin(vm, "i8.set", bu_precise_i8_set);
    register_builtin(vm, "i8.get", bu_precise_i8_get);
    register_builtin(vm, "i16.set", bu_precise_i16_set);
    register_builtin(vm, "i16.get", bu_precise_i16_get);
    register_builtin(vm, "i32.set", bu_precise_i32_set);
    register_builtin(vm, "i32.get", bu_precise_i32_get);
    register_builtin(vm, "i64.set", bu_precise_i64_set);
    register_builtin(vm, "i64.get", bu_precise_i64_get);
    register_builtin(vm, "f32.set", bu_precise_f32_set);
    register_builtin(vm, "f32.get", bu_precise_f32_get);
    register_builtin(vm, "f64.set", bu_precise_f64_set);
    register_builtin(vm, "f64.get", bu_precise_f64_get);
    register_builtin(vm, "cast.precise", bu_precise_cast);
}