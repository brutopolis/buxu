#ifndef ARDUINO

#include "bruter.h"
#include <libtcc.h>

IntList *tcc_states;

Int new_state(VirtualMachine *vm, IntList *args)
{
    TCCState *tcc = tcc_new();
    if (!tcc) 
    {
        fprintf(stderr, "Erro ao inicializar TCC\n");
        return -1;
    }

    // Configurar saída para memória
    tcc_set_output_type(tcc, TCC_OUTPUT_MEMORY);
    Int result = new_var(vm);
    vm->stack->data[result].pointer = tcc;
    vm->typestack->data[result] = TYPE_OTHER;
    hold_var(vm, result);
    stack_push(*tcc_states, (Int)tcc);
    return result;
}

Int delete_state(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    TCCState *tcc = (TCCState *)vm->stack->data[index].pointer;
    tcc_delete(tcc);
    unuse_var(vm, index);
    index = stack_find(*tcc_states, (Int)tcc);
    if (index >= 0) 
    {
        stack_remove(*tcc_states, index);
    }
    return -1;
}

Int compile_string(VirtualMachine *vm, IntList *args)
{
    Int _state = stack_shift(*args);
    Int _code = stack_shift(*args);
    TCCState *tcc = (TCCState *)vm->stack->data[_state].pointer;
    char *code = vm->stack->data[_code].string;
    if (tcc_compile_string(tcc, code) < 0) 
    {
        fprintf(stderr, "Erro ao compilar código\n");
        return -1;
    }
    return -1;
}

Int br_tcc_relocate(VirtualMachine *vm, IntList *args)
{
    Int index = stack_shift(*args);
    TCCState *tcc = (TCCState *)vm->stack->data[index].pointer;
    if (tcc_relocate(tcc) < 0) 
    {
        fprintf(stderr, "Erro ao realocar código\n");
        return -1;
    }
    return -1;
}

Int get_symbol(VirtualMachine *vm, IntList *args)
{
    Int _tcc = stack_shift(*args);
    Int _symbol = stack_shift(*args);
    TCCState *tcc = (TCCState *)vm->stack->data[_tcc].pointer;
    char *symbol = vm->stack->data[_symbol].string;
    void *func = tcc_get_symbol(tcc, symbol);
    if (!func) 
    {
        fprintf(stderr, "Erro ao obter símbolo '%s'\n", symbol);
        return -1;
    }
    Int result = new_var(vm);
    vm->stack->data[result].pointer = func;
    vm->typestack->data[result] = TYPE_BUILTIN;
    hold_var(vm, result);
    return result;
}

Int add_path(VirtualMachine *vm, IntList *args)
{
    Int _tcc = stack_shift(*args);
    Int _path = stack_shift(*args);
    TCCState *tcc = (TCCState *)vm->stack->data[_tcc].pointer;
    char *path = vm->stack->data[_path].string;
    tcc_add_include_path(tcc, path);
    return -1;
}

Int clear_tcc_states(VirtualMachine *vm, IntList *args)
{
    while (tcc_states->size > 0) 
    {
        TCCState *tcc = (TCCState *)stack_shift(*tcc_states);
        tcc_delete(tcc);
    }
    return -1;
}

void _terminate_tcc_at_exit_handler()
{
    clear_tcc_states(NULL, NULL);
    stack_free(*tcc_states);
}

void init_tcc(VirtualMachine* vm)
{
    tcc_states = make_int_list(vm);
    registerBuiltin(vm, "tcc.new", new_state);
    registerBuiltin(vm, "tcc.delete", delete_state);
    registerBuiltin(vm, "tcc.compile", compile_string);
    registerBuiltin(vm, "tcc.relocate", br_tcc_relocate);
    registerBuiltin(vm, "tcc.get.symbol", get_symbol);
    registerBuiltin(vm, "tcc.add.path", add_path);
    registerBuiltin(vm, "tcc.clear", clear_tcc_states);

    atexit(_terminate_tcc_at_exit_handler);
}

#endif