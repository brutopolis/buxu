#include "bruter.h"

// makes
// make a Variable/List/IntList/VirtualMachine and return a pointer to it

Variable* makeVariable(char type, Value value)
{
    Variable *var = (Variable*)malloc(sizeof(Variable));
    var->type = type;
    var->value = value;
    var->isTemp = 0;
    var->isRef = 0;
    return var;
}

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

VirtualMachine* makeVM()
{
    VirtualMachine *vm = (VirtualMachine*)malloc(sizeof(VirtualMachine));
    vm->stack = makeList();
    vm->hashes = (HashList*)malloc(sizeof(HashList));
    StackInit(*vm->hashes);
    vm->empty = (IntList*)malloc(sizeof(IntList));
    StackInit(*vm->empty);

    return vm;
}














// Var
// variable 

Int newVar(VirtualMachine *vm)
{
    if (vm->empty->size > 0)
    {
        Int id = StackShift(*vm->empty);
        if (vm->stack->data[id] != NULL)
        {
            free(vm->stack->data[id]);
            vm->stack->data[id] = NULL;
        }
        vm->stack->data[id] = makeVariable(TYPE_NIL, (Value){0});
        return id;
    }
    else
    {
        StackPush(*vm->stack, makeVariable(TYPE_NIL, (Value){0}));
        return vm->stack->size-1;
    }
}

void freeString(VirtualMachine *vm, CharList *list)
{
    CharList *tmp = list;
    free(tmp->data);
    free(tmp);
}

void freeList(VirtualMachine *vm, IntList *list)
{
    IntList *tmp = list;
    free(tmp->data);
    free(tmp);
}

void freeVar(VirtualMachine *vm, Int index)
{
    StackPush(*vm->empty, index);

    if (vm->stack->data[index]->isRef) // if is a reference, don't free the value
    {
        //exists
    }
    else if (vm->stack->data[index]->type == TYPE_LIST)
    {
        freeList(vm, (IntList*)vm->stack->data[index]->value.pointer);
    }
    else if (vm->stack->data[index]->type == TYPE_STRING)
    {
        freeString(vm, (CharList*)vm->stack->data[index]->value.pointer);
    }

    free(vm->stack->data[index]);
    vm->stack->data[index] = NULL;
}



void setVar(VirtualMachine *vm, Int index, char type, Value value, char isRef)
{
    if (vm->stack->data[index] == NULL)
    {
        vm->stack->data[index] = makeVariable(type, value);
    }
    else if(vm->stack->data[index]->type == TYPE_STRING || 
            vm->stack->data[index]->type == TYPE_LIST)
    {
        free(vm->stack->data[index]->value.pointer);
        vm->stack->data[index]->value.pointer = NULL;
    }
    vm->stack->data[index]->type = type;
    vm->stack->data[index]->value = value;
    vm->stack->data[index]->isRef = isRef;
}













// new
// new a variable and return its index

Int newNumber(VirtualMachine *vm, Float number)
{
    Int index = newVar(vm);
    vm->stack->data[index]->type = TYPE_NUMBER;
    vm->stack->data[index]->value.number = number;
    return index;
}

Int newString(VirtualMachine *vm, char *string)
{
    Int index = newVar(vm);
    vm->stack->data[index]->type = TYPE_STRING;
    vm->stack->data[index]->value.pointer = makeCharList();
    for (int i = 0; string[i] != '\0'; i++)
    {
        StackPush(*((CharList*)vm->stack->data[index]->value.pointer), string[i]);
    }
    
    return index;
}

Int newFunction(VirtualMachine *vm, Function function)
{
    Int index = newVar(vm);
    vm->stack->data[index]->type = TYPE_FUNCTION;
    vm->stack->data[index]->value.pointer = function;
    return index;
}

Int newList(VirtualMachine *vm)
{
    Int index = newVar(vm);
    vm->stack->data[index]->type = TYPE_LIST;
    vm->stack->data[index]->value.pointer = makeIntList();
    return index;
}

/*Int newError(VirtualMachine *vm, int errorid)
{
    Int index = newVar(vm);
    setVar(vm, index, TYPE_ERROR, (Value){number: errorid}, Nil);
    return index;
}
*/














// spawn
// create a new variable, assing a hash to it and return its index

Int spawnNumber(VirtualMachine *vm, char *varname, Float number)
{
    Int index = newNumber(vm, number);
    hashset(vm, varname, index);
    return index;
}

Int spawnString(VirtualMachine *vm, char *varname, char *string)
{
    Int index = newString(vm, string);
    hashset(vm, varname, index);
    return index;
}

Int spawnFunction(VirtualMachine *vm, char *varname, Function function)
{
    Int index = newFunction(vm, function);
    hashset(vm, varname, index);
    return index;
}

Int spawnList(VirtualMachine *vm, char *varname)
{
    Int index = newList(vm);
    hashset(vm, varname, index);
    return index;
}

/*Int spawnError(VirtualMachine *vm, char *varname, int errorid)
{
    Int index = newError(vm, errorid);
    hashset(vm, varname, index);
    return index;
}*/

















// create
// create a new variable, and return a pointer to it

Variable* createNumber(VirtualMachine *vm, Float number)
{
    Int index = newNumber(vm, number);
    return vm->stack->data[index];
}

Variable* createString(VirtualMachine *vm, char *string)
{
    Int index = newString(vm, string);
    return vm->stack->data[index];
}

Variable* createFunction(VirtualMachine *vm, Function function)
{
    Int index = newFunction(vm, function);
    return vm->stack->data[index];
}

Variable* createList(VirtualMachine *vm)
{
    Int index = newList(vm);
    return vm->stack->data[index];
}

/*Variable* createError(VirtualMachine *vm, int errorid)
{
    Int index = newError(vm, errorid);
    return vm->stack->data[index];
}*/















//ref

Reference refget(VirtualMachine *vm, Int index)
{
    Reference ref;
    ref.index = index;
    ref.variable = vm->stack->data[index];
    return ref;
}

void freeref(VirtualMachine *vm, Reference ref)
{
    freeVar(vm, ref.index);
}
