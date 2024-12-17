#include "bruter.h"

function(brl_dycc_test)
{
    printf("Hello, World!\n");
    printf("its working\n");
    return 0;
}

init(dycc_test)
{
    register_builtin(vm, "dycc.test", brl_dycc_test);
}
