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
    else if (type == TYPE_REFERENCE)
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

Int newReference(VirtualMachine *vm, Int index)
{
    Int id = newvar(vm);
    vm->stack->data[id].number = index;
    vm->typestack->data[id] = TYPE_REFERENCE;
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

void spawnReference(VirtualMachine *vm, char* varname, Int index)
{
    Int id = newReference(vm, index);
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
                var.type = TYPE_REFERENCE;
                var.value.number = index;
                StackPush(*result, var);
            }
            else if(str[1] >= '0' && str[1] <= '9') 
            {
                Variable var;
                var.value = (Value){atoi(str + 1)};
                var.type = TYPE_REFERENCE;
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
                    var.type = TYPE_REFERENCE;
                    var.value.number = index;
                    StackPush(*result, var);
                }
            }
        }
        else if (str[0] == '#') //like @ but return the variable itself instead a reference, use with caution, do not try to free or modify the variable
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
    

    if (func.type == TYPE_REFERENCE)
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

Int _set(VirtualMachine *vm, VariableList *args)
{
    Variable varname = StackShift(*args);
    Variable value = StackShift(*args);


    if (varname.type == TYPE_STRING)
    {
        char * name = varname.value.string;
        Int index = hashfind(vm, name);

        if (index == -1)
        {
            index = newvar(vm);
            hashset(vm, name, index);
        }
        vm->stack->data[index] = valueDuplicate(value.value, value.type);
        vm->typestack->data[index] = value.type;
    }
    else if (varname.type == TYPE_NUMBER)
    {
        Int index = (Int)varname.value.number;
        if (index >= 0 && index < vm->stack->size)
        {
            unusevar(vm, index);
            vm->stack->data[index] = valueDuplicate(value.value, value.type);
            vm->typestack->data[index] = value.type;
        }
        else 
        {
            //create a new variable
            index = newvar(vm);
            vm->stack->data[index] = valueDuplicate(value.value, value.type);
            vm->typestack->data[index] = value.type;
        }
    }


    freerawvar(varname);
    freerawvar(value);
    return -1;
}

Int _print(VirtualMachine *vm, VariableList *args)
{
    while (args->size > 0)
    {
        Variable var = StackShift(*args);
        Int _type = -1;

        Value temp = var.value;
        _type = var.type;

        if (var.type == TYPE_REFERENCE)
        {
            _type = vm->typestack->data[(Int)var.value.number];
            temp = vm->stack->data[(Int)var.value.number];
        }
        
        if (_type == TYPE_NUMBER)
        {
            printf("number: %f\n", temp.number);
        }
        else if (_type == TYPE_STRING)
        {
            printf("%s\n", temp.string);
        }
        else if (_type == TYPE_LIST)
        {
            printf("[");
            IntList *list = (IntList*)temp.pointer;
            for (Int i = 0; i < (list->size-1); i++)
            {
                printf("%d, ", list->data[i]);
            }
            printf("%d", list->data[list->size-1]);
            printf("]\n");
        }
        else if (_type == TYPE_ERROR)
        {
            printf("Error: %s\n", temp.string);
        }
        else if (_type == TYPE_FUNCTION)
        {
            printf("Function : %p\n", temp.pointer);
        }
        else
        {
            printf("Unknown type\n");
        }
        freerawvar(var);
    }
    return -1;
}

Int _ls(VirtualMachine *vm, VariableList *args)
{
    for (Int i = 0; i < vm->stack->size; i++)
    {
        if (vm->typestack->data[i] == TYPE_FUNCTION)
        {
            printf("[%d] {function}: %p\n", i, vm->stack->data[i].pointer);
        }
        else if (vm->typestack->data[i] == TYPE_NUMBER)
        {
            printf("[%d] {number}: %f\n", i, vm->stack->data[i].number);
        }
        else if (vm->typestack->data[i] == TYPE_STRING)
        {
            char * temp = vm->stack->data[i].string;
            printf("[%d] {string}: %s\n", i, temp);
        }
        else if (vm->typestack->data[i] == TYPE_LIST)
        {
            printf("[%d] {list}: [", i);
            IntList *list = (IntList*)vm->stack->data[i].pointer;
            for (Int j = 0; j < (list->size-1); j++)
            {
                printf("%d, ", list->data[j]);
            }
            if (list->size > 0)
            {
                printf("%d]\n", list->data[list->size-1]);
            }
            else
            {
                printf("]\n");
            }
        }
        else if (vm->typestack->data[i] == TYPE_ERROR)
        {
            printf("[%d] {error}: %s\n", i, vm->stack->data[i].string);
        }
        else if (vm->typestack->data[i] == TYPE_REFERENCE)
        {
            printf("[%d] {reference}: %d\n", i, (Int)vm->stack->data[i].number);
        }
        else if (vm->typestack->data[i] == TYPE_UNUSED)
        {
            printf("[%d] {free slot}\n", i);
        }
        else
        {
            printf("[%d] {unknown type}\n", i);
        }
    }
    return -1;
}

Int _help(VirtualMachine *vm, VariableList *args)
{
    for (Int i = 0; i < vm->hashes->size; i++)
    {//[name] {type} @index: content
        Int index = vm->hashes->data[i].index;
        char *name = vm->hashes->data[i].key;
        if (vm->typestack->data[index] == TYPE_FUNCTION)
        {
            printf("[%s] {function} @%d: %p\n", name, index, vm->stack->data[index].pointer);
        }
        else if (vm->typestack->data[index] == TYPE_NUMBER)
        {
            printf("[%s] {number} @%d: %f\n", name, index, vm->stack->data[index].number);
        }
        else if (vm->typestack->data[index] == TYPE_STRING)
        {
            printf("[%s] {string} @%d: %s\n", name, index, vm->stack->data[index].string);
        }
        else if (vm->typestack->data[index] == TYPE_LIST)
        {
            printf("[%s] {list} @%d: [", name, index);
            IntList *list = (IntList*)vm->stack->data[index].pointer;
            for (Int j = 0; j < (list->size-1); j++)
            {
                printf("%d, ", list->data[j]);
            }
            if (list->size > 0)
            {
                printf("%d]\n", list->data[list->size-1]);
            }
            else
            {
                printf("]\n");
            }
        }
        else if (vm->typestack->data[index] == TYPE_ERROR)
        {
            printf("[%s] {error} @%d: %s\n", name, index, vm->stack->data[index].string);
        }
        else if (vm->typestack->data[index] == TYPE_REFERENCE)
        {
            printf("[%s] {reference} @%d: %d\n", name, index, (Int)vm->stack->data[index].number);
        }
        else if (vm->typestack->data[index] == TYPE_UNUSED)
        {
            printf("[%s] {free slot}\n", name);
        }
        else
        {
            printf("[%s] {unknown type}\n", name);
        }
    }
    return -1;
}

Int _eval(VirtualMachine *vm, VariableList *args)
{
    Variable str = StackShift(*args);
    char* _str = str.value.string;
    Int result = eval(vm, _str);
    freerawvar(str);
    return result;
}

Int _unset(VirtualMachine *vm, VariableList *args)
{
    Variable varname = StackShift(*args);
    if (varname.type == TYPE_REFERENCE)
    {
        unusevar(vm, varname.value.number);
    }
    else if (varname.type == TYPE_NUMBER)
    {
        unusevar(vm, varname.value.number);
    }
    else
    {
        Int index = hashfind(vm, varname.value.string);
        if (index != -1)
        {
            unusevar(vm, index);
        }
    }
    hashunset(vm, varname.value.string);
    freerawvar(varname);
    return -1;
}

Int _add(VirtualMachine *vm, VariableList *args)
{
    Variable a = StackShift(*args);
    Variable b = StackShift(*args);
    Int result = newNumber(vm, a.value.number + b.value.number);
    freerawvar(a);
    freerawvar(b);
    return result;
}

// list functions

Int _list(VirtualMachine *vm, VariableList *args) 
{
    Int index = newList(vm);
    IntList *list = (IntList*)vm->stack->data[index].pointer;
    while (args->size > 0)
    {
        Variable var = StackShift(*args);
        if (var.type == TYPE_REFERENCE)
        {
            StackPush(*list, (Int)var.value.number);
        }
        else 
        {
            Int tmp = newvar(vm);
            vm->stack->data[tmp] = valueDuplicate(var.value, var.type);
            vm->typestack->data[tmp] = var.type;
            StackPush(*list, tmp);
        }
        freerawvar(var);
    }
    return index;
}

Int _push(VirtualMachine *vm, VariableList *args)
{
    Variable list = StackShift(*args);
    Variable value = StackShift(*args);

    if (list.type == TYPE_REFERENCE)
    {
        if (vm->typestack->data[(Int)list.value.number] == TYPE_LIST)
        {
            IntList *lst = (IntList*)vm->stack->data[(Int)list.value.number].pointer;
            StackPush(*lst, (Int)value.value.number);
        }
    }
    
    //freerawvar(list);
    freerawvar(value);
    return -1;
}

void initStd(VirtualMachine *vm)
{
    spawnFunction(vm, "set", _set);
    spawnFunction(vm, "print", _print);
    spawnFunction(vm, "eval", _eval);
    spawnFunction(vm, "ls", _ls);
    spawnFunction(vm, "help", _help);
    spawnFunction(vm, "unset", _unset);
    spawnFunction(vm, "add", _add);
    spawnFunction(vm, "list", _list);
    spawnFunction(vm, "push", _push);
}

void main()
{
    VirtualMachine *vm = makeVM();

    initStd(vm);

    
    eval(vm,"@set a (@add 50 11);"
            "@set b 100;"
            "@set c 150;"
            "@set d 200;"
            "@set e 250;"
            "@set f 300;"
            "@set g 350;"
            "@set h 400;"
            "@set i 450;"
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