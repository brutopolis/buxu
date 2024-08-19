#include "bruter.h"

//var.c
//string.c

void freeVM(VirtualMachine *vm)
{
    for (Int i = 0; i < vm->stack->size; i++)
    {
        if (vm->stack->data[i] != NULL)
        {
            freeVar(vm, i);
        }
    }
    while (vm->hashes->size > 0)
    {
        Hash *temp = StackPop(*vm->hashes);
        free(temp->key);
        temp->key = NULL;
        free(temp);
        temp = NULL;
    }
    StackFree(*vm->stack);
    StackFree(*vm->empty);
    StackFree(*vm->hashes);
    free(vm);
}

void printMemUsage(VirtualMachine *vm)
{
    printf("Memory usage: %ld bytes\n", vm->bytesInUse);
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
            Function __eval = (Function)vm->stack->data[hashfind(vm, "eval")]->value.pointer;
            
            char* _str = strndup(str + 1, strlen(str) - 2);
            //free(_str);

            IntList *args = makeIntList();
            StackPush(*args, newString(vm, _str));
            Int res = __eval(vm, args);
            if (res == -1) 
            {
                //StackPush(*result, newError(vm, "Error in eval"));
            } 
            else 
            {
                StackPush(*result, res);
            }
            //freeVar(vm, res);
            free(_str);
            _str = NULL;
            StackFree(*args);
        }
        else if (str[0] == '!' && str[1] == '(') // literal string
        {
            char* _str = strndup(str + 2, strlen(str) - 3);
            StackPush(*result, newString(vm, _str));
            free(_str);
            _str = NULL;
        }
        else if (str[0] == '@') 
        {
            if (strlen(str) == 1) 
            {
                //StackPush(*result, newError(vm, "Variable name not found"));
            }
            else if (str[1] == '(') 
            {
                Function __eval = (Function)vm->stack->data[hashfind(vm, "eval")]->value.pointer;
                char* _str = strndup(str + 2, strlen(str) - 3);
                IntList *args = makeIntList();
                StackPush(*args, newString(vm, _str));
                Int res = __eval(vm, args);
                if (res == -1) 
                {
                    //StackPush(*result, newError(vm, "Error in eval"));
                } 
                else 
                {
                    StackPush(*result, res);
                    res = __eval(vm, args);
                    if (res == -1) 
                    {
                        //StackPush(*result, newError(vm, "Error in eval"));
                    } 
                    else 
                    {
                        StackPush(*result, res);
                    }
                }
                free(_str);
                _str = NULL;
                StackFree(*args);
            }
            else if(str[1] >= '0' && str[1] <= '9') 
            {
                StackPush(*result, atoi(str + 1));
            }
            else if(str[1] == '!' && str[2] == '(')
            {
                char * _str = strndup(str + 3, strlen(str) - 4);
                Int index = hashfind(vm, _str);
                if (index == -1) 
                {
                    //StackPush(*result, newError(vm, "Variable not found"));
                } 
                else 
                {
                    StackPush(*result, index);
                }
                free(_str);
                _str = NULL;
            }
            else
            {
                Int index = hashfind(vm, str + 1);

                if (index == -1) 
                {
                    //StackPush(*result, newError(vm, "Variable not found"));
                } 
                else 
                {
                    Int var = newVar(vm);
                    setVar(vm, var, vm->stack->data[index]->type, vm->stack->data[index]->value, Nil);
                    StackPush(*result, var);
                }
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
        str = NULL;
    }
    StackFree(*splited);
    return result;
}

Int interpret(VirtualMachine *vm, char* str) 
{    
    Int result;
    IntList *args = parse(vm, str);

    Reference funcname = refget(vm, StackShift(*args));

    /*if (funcname.variable->type == TYPE_ERROR) 
    {
        printf("%s\n", toString(funcname.variable->value.pointer));
        return -1;
    }*/

    //printMemUsage(vm);

    if (funcname.variable->type != TYPE_STRING) 
    {
        printf("First argument must be a string\n");
        return -1;
    }

    char *name = toString(funcname.variable->value.pointer);
    Int index = hashfind(vm, name);
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
    free(name);
    return result;
}

Int eval(VirtualMachine *vm, char *str)
{
    StringList *splited = splitString(str, ";");
    Int result = -1;
    while (splited->size > 0)
    {
        char *cmd = StackShift(*splited);
        result = interpret(vm, cmd);
        free(cmd);
        cmd = NULL;
        if (result != -1)
        {
            break;
        }
    }
    StackFree(*splited);
    return result;
}

//main
//main
int main()
{
    VirtualMachine *vm = makeVM();
    initStd(vm);

    char* cmd = "set a 50;"
                "set b 100;"
                "set c 150;"
                "set d 200;"
                "set e 250;"
                "set f 300;"
                "set g 350;"
                "set h 400;"
                "set i 450;"
                "set j 500;"
                "set k 550;"
                "set l 600;"
                "set abc abuble;"
                "set abcd !(opa iae);"
                "set lst (list);"
                "print @abc;"
                "ls;";
    
    eval(vm, cmd);
    // free
    freeVM(vm);
    return 0;
}