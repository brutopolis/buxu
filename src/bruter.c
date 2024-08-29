#include "bruter.h"

//string functions

char* strduplicate(const char *str)
{
    char *dup = (char*)malloc(strlen(str) + 1);
    strcpy(dup, str);
    return dup;
}

char* strnduplicate(const char *str, Int n)
{
    char *dup = (char*)malloc(n + 1);
    for (Int i = 0; i < n; i++)
    {
        dup[i] = str[i];
    }
    dup[n] = '\0';
    return dup;
}

char* strf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Int size = vsnprintf(NULL, 0, format, args);
    va_end(args);
    char *str = (char*)malloc(size + 1);
    va_start(args, format);
    vsprintf(str, format, args);
    va_end(args);
    return str;
}

StringList* specialSplit(char *str)
{
    StringList *splited;
    splited = (StringList*)malloc(sizeof(StringList));
    StackInit(*splited);
    
    Int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '(')
        {
            Int j = i;
            Int count = 1;
            while (count != 0)
            {
                j++;
                if (str[j] == '(')
                {
                    count++;
                }
                else if (str[j] == ')')
                {
                    count--;
                }
            }
            char *tmp = strnduplicate(str + i, j - i + 1);
            StackPush(*splited, tmp);
            i = j + 1;
        }
        else if (str[i] == '!' && str[i + 1] == '(')
        {
            Int j = i + 1;
            Int count = 1;
            while (count != 0)
            {
                j++;
                if (str[j] == '(')
                {
                    count++;
                }
                else if (str[j] == ')')
                {
                    count--;
                }
            }
            char *tmp = strnduplicate(str + i, j - i + 1);
            StackPush(*splited, tmp);
            i = j + 1;
        }
        else if (str[i] == '#' && str[i + 1] == '(')
        {
            Int j = i + 1;
            Int count = 1;
            while (count != 0)
            {
                j++;
                if (str[j] == '(')
                {
                    count++;
                }
                else if (str[j] == ')')
                {
                    count--;
                }
            }
            char *tmp = strnduplicate(str + i, j - i + 1);
            StackPush(*splited, tmp);
            i = j + 1;
        }
        else if (isSpace(str[i]))
        {
            i++;
        }
        else
        {
            Int j = i;
            while (!isSpace(str[j]) && str[j] != '\0' && str[j] != '(' && str[j] != ')')
            {
                j++;
            }
            StackPush(*splited, strnduplicate(str + i, j - i));
            i = j;
        }
    }

    return splited;
}

StringList* splitString(char *str, char *delim)
{
    StringList *splited;
    splited = (StringList*)malloc(sizeof(StringList));
    StackInit(*splited);
    
    Int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == delim[0])
        {
            i++;
        }
        else
        {
            Int j = i;
            while (str[j] != delim[0] && str[j] != '\0')
            {
                j++;
            }
            StackPush(*splited, strnduplicate(str + i, j - i));
            i = j;
        }
    }

    return splited;
}

// value functions

Value valueDuplicate(Value value, char type)
{
    Value dup;
    if (type == TYPE_NUMBER)
    {
        dup.number = value.number;
    }
    else if (type == TYPE_STRING)
    {
        dup.string = strduplicate(value.string);
    }
    else if (type == TYPE_LIST)
    {
        dup.pointer = makeIntList();
        IntList *list = (IntList*)value.pointer;
        for (Int i = 0; i < list->size; i++)
        {
            StackPush(*((IntList*)dup.pointer), list->data[i]);
        }
    }
    else if (type == TYPE_ERROR)
    {
        dup.string = strduplicate(value.string);
    }
    else if (type == TYPE_FUNCTION)
    {
        dup.pointer = value.pointer;
    }
    else if (type == TYPE_POINTER)
    {
        dup.number = value.number;
    }
    return dup;
}

//hash functions

Int hashfind(VirtualMachine *vm, char *varname)
{
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (strcmp(vm->hashes->data[i].key, varname) == 0)
        {
            return vm->hashes->data[i].index;
        }
    }
    return -1;
}

void hashset(VirtualMachine *vm, char* varname, Int index)
{
    if (hashfind(vm, varname) != -1)
    {
        vm->hashes->data[hashfind(vm, varname)].index = index;
    }
    else 
    {
        Hash hash;
        hash.key = strduplicate(varname);
        hash.index = index;
        StackPush(*vm->hashes, hash);
    }
}

void hashunset(VirtualMachine *vm, char* varname)
{
    Int index = hashfind(vm, varname);
    if (index != -1)
    {
        free(vm->hashes->data[index].key);
        vm->hashes->data[index].key = NULL;
        //free(vm->hashes->data[index]);
        //vm->hashes->data[index] = NULL;
        StackRemove(*vm->hashes, index);
    }
}

//variable functions

List* makeList()
{
    List *list = (List*)malloc(sizeof(List));
    StackInit(*list);
    return list;
}

IntList* makeIntList()
{
    IntList *list = (IntList*)malloc(sizeof(IntList));
    StackInit(*list);
    return list;
}

CharList* makeCharList()
{
    CharList *list = (CharList*)malloc(sizeof(CharList));
    StackInit(*list);
    return list;
}

VariableList* makeVariableList()
{
    VariableList *list = (VariableList*)malloc(sizeof(VariableList));
    StackInit(*list);
    return list;
}

VirtualMachine* makeVM()
{
    VirtualMachine *vm = (VirtualMachine*)malloc(sizeof(VirtualMachine));
    vm->stack = makeList();
    vm->typestack = makeCharList();
    vm->hashes = (HashList*)malloc(sizeof(HashList));
    StackInit(*vm->hashes);
    vm->empty = (IntList*)malloc(sizeof(IntList));
    StackInit(*vm->empty);


    return vm;
}

// var new 

Int newvar(VirtualMachine *vm)
{
    if (vm->empty->size > 0)
    {
        Int id = StackShift(*vm->empty);
        return id;
    }
    else
    {
        StackPush(*vm->stack, (Value){0});
        StackPush(*vm->typestack, TYPE_NIL);
        return vm->stack->size-1;
    }
}

Int newNumber(VirtualMachine *vm, Float number)
{
    Int id = newvar(vm);
    vm->stack->data[id].number = number;
    vm->typestack->data[id] = TYPE_NUMBER;
    return id;
}

Int newString(VirtualMachine *vm, char *string)
{
    Int id = newvar(vm);
    vm->stack->data[id].string = strduplicate(string);
    vm->typestack->data[id] = TYPE_STRING;
    return id;
}

Int newFunction(VirtualMachine *vm, Function function)
{
    Int id = newvar(vm);
    vm->stack->data[id].pointer = function;
    vm->typestack->data[id] = TYPE_FUNCTION;
    return id;
}

Int newError(VirtualMachine *vm, char *error)
{
    Int id = newvar(vm);
    vm->stack->data[id].string = strduplicate(error);
    vm->typestack->data[id] = TYPE_ERROR;
    return id;
}

Int newPointer(VirtualMachine *vm, Int index)
{
    Int id = newvar(vm);
    vm->stack->data[id].number = index;
    vm->typestack->data[id] = TYPE_POINTER;
    return id;
}

Int newList(VirtualMachine *vm)
{
    Int id = newvar(vm);
    vm->stack->data[id].pointer = makeIntList();
    vm->typestack->data[id] = TYPE_LIST;
    return id;
}

// var spawn

void spawnVar(VirtualMachine *vm, char* varname)
{
    Int index = newvar(vm);
    hashset(vm, varname, index);
}

void spawnNumber(VirtualMachine *vm, char* varname, Float number)
{
    Int index = newNumber(vm, number);
    hashset(vm, varname, index);
}

void spawnString(VirtualMachine *vm, char* varname, char* string)
{
    Int index = newString(vm, string);
    hashset(vm, varname, index);
}

void spawnFunction(VirtualMachine *vm, char* varname, Function function)
{
    Int index = newFunction(vm, function);
    hashset(vm, varname, index);
}

void spawnError(VirtualMachine *vm, char* varname, char* error)
{
    Int index = newError(vm, error);
    hashset(vm, varname, index);
}

void spawnPointer(VirtualMachine *vm, char* varname, Int index)
{
    Int id = newPointer(vm, index);
    hashset(vm, varname, id);
}

void spawnList(VirtualMachine *vm, char* varname)
{
    Int index = newList(vm);
    hashset(vm, varname, index);
}



void freevar(VirtualMachine *vm, Int index)
{
    if (vm->typestack->data[index] == TYPE_STRING)
    {
        free(vm->stack->data[index].string);
    }
    else if (vm->typestack->data[index] == TYPE_LIST)
    {
        while (((IntList*)vm->stack->data[index].pointer)->size > 0)
        {
            StackShift(*((IntList*)vm->stack->data[index].pointer));
        }
        StackFree(*((IntList*)vm->stack->data[index].pointer));
    }
    StackRemove(*vm->stack, index);
    StackRemove(*vm->typestack, index);
}

void freerawvar(Variable var)
{
    if (var.type == TYPE_STRING)
    {
        free(var.value.string);
    }
    else if (var.type == TYPE_LIST)
    {
        while (((IntList*)var.value.pointer)->size > 0)
        {
            StackShift(*((IntList*)var.value.pointer));
        }
        StackFree(*((IntList*)var.value.pointer));
    }
}

void freevm(VirtualMachine *vm)
{
    while (vm->stack->size > 0)
    {
        freevar(vm, 0);
    }

    while (vm->hashes->size > 0)
    {
        Hash hash = StackShift(*vm->hashes);
        free(hash.key);
    }

    while (vm->empty->size > 0)
    {

        StackShift(*vm->empty);
    }

    StackFree(*vm->stack);
    StackFree(*vm->typestack);

    StackFree(*vm->hashes);
    StackFree(*vm->empty);
    free(vm);
}

void unusevar(VirtualMachine *vm, Int index)
{
    //if type is string free the string, if type is list free the list
    if (vm->typestack->data[index] == TYPE_STRING)
    {
        free(vm->stack->data[index].string);
    }
    else if (vm->typestack->data[index] == TYPE_LIST)
    {
        while (((IntList*)vm->stack->data[index].pointer)->size > 0)
        {
            StackShift(*((IntList*)vm->stack->data[index].pointer));
        }
        StackFree(*((IntList*)vm->stack->data[index].pointer));
    }
    vm->typestack->data[index] = TYPE_UNUSED;
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (vm->hashes->data[i].index == index)
        {
            free(vm->hashes->data[i].key);
            StackRemove(*vm->hashes, i);
            //break;
        }
    }
    StackPush(*vm->empty, index);
}


void registerVar(VirtualMachine *vm, char *varname, Variable var)
{
    Int index = hashfind(vm, varname);
    if (index == -1)
    {
        index = newvar(vm);
        hashset(vm, varname, index);
    }
    vm->stack->data[index] = valueDuplicate(var.value, var.type);
    vm->typestack->data[index] = var.type;
}


// Parser functions
VariableList* parse(VirtualMachine *vm, char *cmd) 
{
    VariableList *result = makeVariableList();
    StringList *splited = specialSplit(cmd);
    while (splited->size > 0)
    {
        char* str = StackShift(*splited);
        if (str[0] == '(')
        {
            if (str[1] == ')')
            {
                Variable var;
                var.type = TYPE_LIST;
                var.value.pointer = makeVariableList();
                StackPush(*result, var);
            }
            else
            {
                char* temp = strnduplicate(str + 1, strlen(str) - 2);
                Int index = eval(vm, temp);
                free(temp);
                Variable var;
                var.type = vm->typestack->data[index];
                var.value = valueDuplicate(vm->stack->data[index], var.type);
                StackPush(*result, var);
                unusevar(vm, index);
            }
        }
        else if (str[0] == '!' && str[1] == '(') // literal string
        {
            Variable var;
            var.type = TYPE_STRING;
            var.value.string = strnduplicate(str + 2, strlen(str) - 3);
            //printf("%s\n", var.value.string);
            StackPush(*result, var);
        }
        else if (str[0] == '@') 
        {
            if (strlen(str) == 1) 
            {
                //StackPush(*result, newError(vm, "Variable name not found"));
            }
            else if(str[1] == '(')
            {
                char* temp = strnduplicate(str + 1, strlen(str) - 2);
                Int index = eval(vm, temp);
                free(temp);
                Variable var;
                var.type = TYPE_POINTER;
                var.value.number = index;
                StackPush(*result, var);
            }
            else if(str[1] >= '0' && str[1] <= '9') 
            {
                Variable var;
                var.value = (Value){atoi(str + 1)};
                var.type = TYPE_POINTER;
                StackPush(*result, var);
            }
            else
            {
                Int index = hashfind(vm, str + 1);
                if (index == -1) 
                {
                    //StackPush(*result, newError(vm, "Variable name not found"));
                }
                else 
                {
                    Variable var;
                    var.type = TYPE_POINTER;
                    var.value.number = index;
                    StackPush(*result, var);
                }
            }
        }
        else if (str[0] == '#') //like @ but return the variable itself instead a pointer, use with caution, do not try to free or modify the variable
        {
            if (strlen(str) == 1) 
            {
                //StackPush(*result, newError(vm, "Variable name not found"));
            }
            else if(str[1] >= '0' && str[1] <= '9') 
            {
                Variable var;
                var.value = (Value){atoi(str + 1)};
                var.type = vm->typestack->data[(Int)var.value.number];
                StackPush(*result, var);
            }
            else
            {
                Int index = hashfind(vm, str + 1);
                if (index == -1) 
                {
                    //StackPush(*result, newError(vm, "Variable name not found"));
                }
                else 
                {
                    Variable var;
                    var.type = vm->typestack->data[index];
                    var.value = vm->stack->data[index];
                    StackPush(*result, var);
                }
            }
        }
        else if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-') 
        {
            Variable var;
            var.type = TYPE_NUMBER;
            var.value.number = atof(str);
            StackPush(*result, var);
        }
        else //string 
        {
            Variable var;
            var.type = TYPE_STRING;
            var.value.string = strduplicate(str);
            StackPush(*result, var);
        }
        free(str);
    }
    StackFree(*splited);
    return result;
}

Int interpret(VirtualMachine *vm, char* cmd)
{
    VariableList *parsed = parse(vm, cmd);
    Variable func = StackShift(*parsed);
    Int result = -1;
    

    if (func.type == TYPE_POINTER)
    {
        if (vm->typestack->data[(Int)func.value.number] == TYPE_FUNCTION)
        {
            Function function = vm->stack->data[(Int)func.value.number].pointer;
            result = function(vm, parsed);
        }
        else // make a list
        {
            StackInsert(*parsed, 0, func);
            result = _list(vm, parsed);
        }
    }
    else 
    {
        StackInsert(*parsed, 0, func);
        result = _list(vm, parsed);
    }

    while (parsed->size > 0)
    {
        Variable var = StackShift(*parsed);
        if (var.type == TYPE_NUMBER)
        {
            printf("%f\n", var.value.number);
        }
        else if (var.type == TYPE_STRING)
        {
            printf("%s\n", var.value.string);
        }
        freerawvar(var);
    }

    
    StackFree(*parsed);
    return result;
}

Int eval(VirtualMachine *vm, char *cmd)
{
    StringList *splited = splitString(cmd, ";");
    Int result = -1;
    while (splited->size > 0)
    {
        char *str = StackShift(*splited);
        result = interpret(vm, str);
        free(str);
    }
    StackFree(*splited);
    return result;
}



void main()
{
    VirtualMachine *vm = makeVM();

    initAll(vm);

    
    eval(vm,"@set a (@add 50 11);"
            "@set b (@mul 10 20);"
            "@set c (@div 100 2);"
            "@set d (@sub 100 50);"
            "@set e (@round 3.141592);"
            "@set f (@ceil 3.141);"
            "@set g (@floor 3.141592659);"
            "@set h (@sqrt 100);"
            "@seed 1234;"
            "@set i (@random 5 9);"
            "@set j 500;"
            "@set k 550;"
            "@set l 600;"
            "@set abc abuble;"
            "@set abcd !(opa iae);"
            //"@set lst (1 2);"
            "@print @abc;"
            "@print @abcd;"
            "@unset abc;"
            "@set tbl (1 2 3 4 5 6);"
            "@push @tbl 1;"
            "@print !(dsadas dsadas);"
            "@ls;"
            "@help;");
    
    freevm(vm);
}