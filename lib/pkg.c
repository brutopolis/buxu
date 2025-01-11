#include "bruter.h"

function(brl_std_pkg_name)
{
    hash_set(vm, "package.name", arg_i(1));
    return -1;
}

function(brl_std_pkg_version)
{
    hash_set(vm, "package.version", arg_i(1));
    return -1;
}

function(brl_std_pkg_description)
{
    hash_set(vm, "package.description", arg_i(1));
    return -1;
}

function(brl_std_pkg_dependencies)
{
    Int lst = new_list(vm);
    for (Int i = 1; i < args->size; i++)
    {
        stack_push(*(IntList*)vm->stack->data[lst].pointer, arg_i(i));
    }
    register_var(vm, "package.dependencies");
    vm->hashes->data[hash_find(vm, "package.dependencies")].index = lst;
    return -1;
}

init(pkg)
{
    register_builtin(vm, "name:", brl_std_pkg_name);
    register_builtin(vm, "version:", brl_std_pkg_version);
    register_builtin(vm, "description:", brl_std_pkg_description);
    register_builtin(vm, "dependencies:", brl_std_pkg_dependencies);
}