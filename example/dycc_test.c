#include "bruter.h"

Int brl_dycc_test(VirtualMachine* vm, IntList* args)
{
    printf("Hello, World!\n");
    printf("its working\n");
    return 0;
}

void init_dycc_test(VirtualMachine* vm)
{
    register_builtin(vm, "dycc.test", brl_dycc_test);
}
