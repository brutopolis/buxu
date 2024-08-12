#include "bruter.h"

//var.c
//string.c

void freeVM(VirtualMachine *vm)
{
    while (vm->stack->size > 0)
    {
        Variable * temp = StackPop(*vm->stack);
        if (temp != NULL)
        {
            if (temp->type == TYPE_STRING || temp->type == TYPE_ERROR)
            {
                free(temp->value.string);
            }
            else if (temp->type == TYPE_LIST)
            {
                StackFree(*((IntList*)temp->value.pointer));
                free(temp->value.pointer);
                
            }
            free(temp);
        }
    }
    while (vm->hashes->size > 0)
    {
        Hash *temp = StackPop(*vm->hashes);
        free(temp->key);
        free(temp);
    }
    StackFree(*vm->stack);
    StackFree(*vm->hashes);
    StackFree(*vm->empty);
    free(vm);
}

// Parser functions
// idea: parse could use indexes of the args on the stack instead of the args themselves 
IntList* parse(VirtualMachine *vm, char *cmd) 
{
    IntList *result = makeIntList();
    StringList *splited = specialSplit(cmd);

    while (splited->size > 0) 
    {
        char* str = StackShift(*splited);
        if (str[0] == '(') 
        {
            // string
            Function __eval = (Function)vm->stack->data[hashfind(vm, "interpret")]->value.pointer;
            char* _str = strndup(str + 1, strlen(str) - 2);
            Int argsid = newList(vm);
            IntList *args = (IntList*)vm->stack->data[argsid]->value.pointer;

            StackPush(*args, newString(vm, _str));
            
            Int _res = __eval(vm, args);
            StackPush(*result, _res);
            freeVar(vm, _res);
            while (args->size > 0) 
            {
                freeVar(vm, StackShift(*args));
            }
            StackFree(*args);
            free(_str);
        } 
        else if (str[0] == '$') 
        {
            Int index = hashfind(vm, str + 1);

            if (index == -1) 
            {
                StackPush(*result, newError(vm, "Variable not found"));
            } 
            else 
            {
                Int var = newVar(vm);
                setVar(vm, var, vm->stack->data[index]->type, vm->stack->data[index]->value, Nil);
                StackPush(*result, var);
            }
        }
        else if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-') 
        {
            StackPush(*result, newNumber(vm, atof(str)));
        } 
        else //string 
        {
            StackPush(*result, newString(vm, str));
        }

        free(str);
    }
    StackFree(*splited);
    return result;
}

Int interpret(VirtualMachine *vm, char* str) 
{    
    Int result;
    IntList *args = parse(vm, str);

    Reference funcname = refget(vm, StackShift(*args));

    if (funcname.variable->type == TYPE_ERROR) 
    {
        printf("%s\n", funcname.variable->value.string);
        return -1;
    }

    if (funcname.variable->type != TYPE_STRING) 
    {
        printf("First argument must be a string\n");
        return -1;
    }

    Int index = hashfind(vm, funcname.variable->value.string);
    if (index == -1 ) 
    {
        printf("Function not found\n");
        return -1;
    }

    if (vm->stack->data[index] == NULL) 
    {
        printf("function seems null\n");
        return -1;
    }

    if (vm->stack->data[index]->type != TYPE_FUNCTION) 
    {
        printf("Variable is not a function\n");
        return -1;
    }

    Function func = (Function)vm->stack->data[index]->value.pointer;
    result = func(vm, args);
    
    freeref(vm, funcname);
    StackFree(*args);

    return result;
}

Int eval(VirtualMachine *vm, char *str)
{
    StringList *splited = splitString(str, ";");
    Int result = 0;
    while (splited->size > 0)
    {
        char *cmd = StackShift(*splited);
        result = interpret(vm, cmd);
        free(cmd);
    }
    StackFree(*splited);
    return result;
}

//print



//main
void main()
{
    VirtualMachine *vm = makeVM();
    
    initStd(vm);

    Int a = newNumber(vm, 5555);
    Int b = newNumber(vm, 10);
    Int c = newNumber(vm, 15);
    Int d = newNumber(vm, 20);
    Int e = newNumber(vm, 25);

    Int abc = newString(vm, "example string");

    Int __list = newList(vm);

    listpush(vm, __list, a);
    listpush(vm, __list, b);
    listpush(vm, __list, c);
    scanf("%d", &a);
    freeVar(vm, a);
    freeVar(vm, b);
    freeVar(vm, c);
    freeVar(vm, d);
    freeVar(vm, e);
    freeVar(vm, abc);
    freeVar(vm, __list);


    // split test
    spawnNumber(vm, "a", 5);
    char* cmd = "set b 59;set c 44;set d 99;";
    char *cmd2 = "print $b;help;ls;";
    
    printf("sizeof Variable: %ld\n", sizeof(Variable));
    
    eval(vm, cmd);
    eval(vm, cmd2);
    scanf("%d", &a);
    // free
    freeVM(vm);
}