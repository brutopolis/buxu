#include "bruter.h"

Int brl_dyc_test(VirtualMachine* vm, IntList* args)
{
    printf("Hello, World!\n");
    printf("its working\n");
    return 0;
}

void init_dyc_test(VirtualMachine* vm)
{
    register_builtin(vm, "dyc.test", brl_dyc_test);
}
