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
            Function __eval = (Function)vm->stack->data[hashfind(vm, "eval")]->value.pointer;
            
            char* _str = strndup(str + 1, strlen(str) - 2);
            //free(_str);

            IntList *args = makeIntList();
            StackPush(*args, newString(vm, _str));
            Int res = __eval(vm, args);
            printf("res: %f\n", vm->stack->data[res]->value.number);
            if (res == -1) 
            {
                StackPush(*result, newError(vm, "Error in eval"));
            } 
            else 
            {
                StackPush(*result, res);
            }
            //freeVar(vm, res);
            free(_str);
            StackFree(*args);
        }
        else if (str[0] == '!' && str[1] == '(') // literal string
        {
            char* _str = strndup(str + 2, strlen(str) - 3);
            StackPush(*result, newString(vm, _str));
            free(_str);
        }
        else if (str[0] == '@') 
        {
            if (strlen(str) == 1) 
            {
                StackPush(*result, newError(vm, "Variable name not found"));
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
                    StackPush(*result, newError(vm, "Error in eval"));
                } 
                else 
                {
                    StackPush(*result, res);
                    res = __eval(vm, args);
                    if (res == -1) 
                    {
                        StackPush(*result, newError(vm, "Error in eval"));
                    } 
                    else 
                    {
                        StackPush(*result, res);
                    }
                }
                free(_str);
                StackFree(*args);
            }
            else if(str[1] == '@') // @@varname, get variable @varname and use its result as the key to get the correct variable
            {
                Int index = hashfind(vm, str + 2);
                Variable *var1 = vm->stack->data[index];
                Int index2 = hashfind(vm, var1->value.string);

                if (index2 == -1) 
                {
                    StackPush(*result, newError(vm, "Variable not found"));
                } 
                else 
                {
                    StackPush(*result, index2);
                }
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
                    StackPush(*result, newError(vm, "Variable not found"));
                } 
                else 
                {
                    StackPush(*result, index);
                }
                free(_str);
            }
            else
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
    Int result = -1;
    while (splited->size > 0)
    {
        char *cmd = StackShift(*splited);
        result = interpret(vm, cmd);
        free(cmd);
        if (result != -1)
        {
            break;
        }
    }
    StackFree(*splited);
    return result;
}

//main
int main(int argc, char *argv[])
{
    char* filename = NULL;
    char* filetxt = NULL;

    //turn args into a string stack
    StringList *args = (StringList*)malloc(sizeof(StringList));
    StackInit(*args);

    for (int i = 1; i < argc; i++)
    {
        StackPush(*args, argv[i]);
    }

    while (args->size > 0)
    {
        char* arg = StackShift(*args);
        if (arg[0] == '-')
        {
            if (strcmp(arg, "-h") == 0)
            {
                printf("Usage: bruter [options] [file]\n");
                printf("Options:\n");
                printf("  -h  Show this help message\n");
                return 0;
            }
            else if (strcmp(arg, "-v") == 0)
            {
                printf("bruter %s\n", VERSION);
                return 0;
            }
            else
            {
                printf("Unknown option: %s\n", arg);
                return 1;
            }
        }
        else if (filename == NULL)
        {
            filename = arg;
        }
    }

    
    if (filename != NULL && filetxt == NULL)
    {
        FILE* file = fopen(filename, "r");
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);
        filetxt = malloc(length + 1);
        fread(filetxt, 1, length, file);
        fclose(file);
        filetxt[length] = '\0';
    }

    free(args);
    

    VirtualMachine *vm = makeVM();
    
    initStd(vm);

    if (filetxt != NULL)
    {
        eval(vm, filetxt);
    }
    else 
    {
        printf("bruter %s\n", VERSION);
        printf("repl yet to be implemented\n");
    }
    // free
    freeVM(vm);
}

