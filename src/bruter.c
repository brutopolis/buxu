#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define Int long
#define Float double

const char* Version = "0.4.2c";

// type -1 is error, it can contain a string with the error message
const enum 
{
    TYPE_ERROR = -1,
    TYPE_NIL = 0,
    //TYPE_REFERENCE = 1,
    TYPE_NUMBER = 2,
    TYPE_STRING = 3,
    TYPE_FUNCTION = 4,
    TYPE_LIST = 5,
} Types;

#define isSpace(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f')

//stack implementation
#define Stack(T) struct \
{ \
    T *data; \
    int size; \
    int capacity; \
}

#define StackInit(s) do \
{ \
    (s).data = NULL; \
    (s).size = 0; \
    (s).capacity = 0;\
} while (0)

#define StackPush(s, v) do { \
    if ((s).size == (s).capacity) { \
        (s).capacity = (s).capacity == 0 ? 1 : (s).capacity * 2; \
        (s).data = realloc((s).data, (s).capacity * sizeof(*(s).data)); \
    } \
    (s).data[(s).size++] = (v); \
} while (0)

#define StackPop(s) ((s).data[--(s).size])

#define StackShift(s) ({ \
    typeof((s).data[0]) ret = (s).data[0]; \
    for (int i = 0; i < (s).size - 1; i++) { \
        (s).data[i] = (s).data[i + 1]; \
    } \
    (s).size--; \
    ret; \
})

#define StackFree(s) ({free((s).data);free(&s);})

//swap elements from index i1 to index i2
#define StackMove(s, i1, i2) do { \
    typeof((s).data[i1]) tmp = (s).data[i1]; \
    (s).data[i1] = (s).data[i2]; \
    (s).data[i2] = tmp; \
} while (0)

//insert element v at index i
#define StackInsert(s, i, v) do { \
    if ((s).size == (s).capacity) { \
        (s).capacity = (s).capacity == 0 ? 1 : (s).capacity * 2; \
        (s).data = realloc((s).data, (s).capacity * sizeof(*(s).data)); \
    } \
    for (int j = (s).size; j > i; j--) { \
        (s).data[j] = (s).data[j - 1]; \
    } \
    (s).data[i] = (v); \
    (s).size++; \
} while (0)

//remove element at index i and return it
#define StackRemove(s, i) ({ \
    typeof((s).data[i]) ret = (s).data[i]; \
    for (int j = i; j < (s).size - 1; j++) { \
        (s).data[j] = (s).data[j + 1]; \
    } \
    (s).size--; \
    ret; \
})



//Value
typedef union 
{
    Float number;
    //Int index;
    char* string;
    void* pointer;
} Value;

//Variable
typedef struct 
{
    Value value;
    char type;
} Variable;

//Hash
typedef struct 
{
    char *key;
    Int index;
} Hash;

//List
typedef Stack(Variable*) List;
typedef Stack(Hash*) HashList;
typedef Stack(Int) IntList;
typedef Stack(char*) StringList;

typedef struct
{
    List *stack;
    HashList *hashes;
    IntList *empty;
} VirtualMachine;

//Function
typedef Int (*Function)(VirtualMachine*, List*);

Variable* createVariable(char type, Value value)
{
    Variable *var = (Variable*)malloc(sizeof(Variable));
    var->type = type;
    var->value = value;
    return var;
}

List* createList()
{
    List *list = (List*)malloc(sizeof(List));
    StackInit(*list);
    return list;
}

VirtualMachine* createVM()
{
    VirtualMachine *vm = (VirtualMachine*)malloc(sizeof(VirtualMachine));
    vm->stack = createList();
    vm->hashes = (HashList*)malloc(sizeof(HashList));
    StackInit(*vm->hashes);
    vm->empty = (IntList*)malloc(sizeof(IntList));
    StackInit(*vm->empty);

    return vm;
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
            char *tmp = strndup(str + i, j - i + 1);
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
            StackPush(*splited, strndup(str + i, j - i));
            i = j;
        }
    }

    return splited;
}

StringList* splitString(char *str, char *delim)
{
    StringList *splited = (StringList*)malloc(sizeof(StringList));
    StackInit(*splited);
    
    char *token = strtok(str, delim);
    while (token != NULL)
    {
        StackPush(*splited, strdup(token));
        token = strtok(NULL, delim);
    }

    return splited;
}

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
        vm->stack->data[id] = createVariable(TYPE_NIL, (Value){0});
        return id;
    }
    else 
    {
        StackPush(*vm->stack, createVariable(TYPE_NIL, (Value){0}));
        return vm->stack->size-1;
    }
}

void freeVar(VirtualMachine *vm, Int index)
{
    StackPush(*vm->empty, index);

    if (vm->stack->data[index]->type == TYPE_LIST)
    {
        StackFree(*((IntList*)vm->stack->data[index]->value.pointer));
    }
    else if (vm->stack->data[index]->type == TYPE_STRING || 
             vm->stack->data[index]->type == TYPE_ERROR)
    {
        free(vm->stack->data[index]->value.string);
    }
    free(vm->stack->data[index]);
    vm->stack->data[index] = NULL;
}

void setVar(VirtualMachine *vm, Int index, char type, Value value)
{
    if (vm->stack->data[index] == NULL)
    {
        vm->stack->data[index] = createVariable(type, value);
    }
    else
    if (vm->stack->data[index]->type == TYPE_STRING || 
        vm->stack->data[index]->type == TYPE_ERROR ||
        
        vm->stack->data[index]->type == TYPE_LIST)
    {
        free(vm->stack->data[index]->value.string);
    }
    vm->stack->data[index]->type = type;
    vm->stack->data[index]->value = value;
}




Int hashfind(VirtualMachine *vm, char *varname)
{
    for (Int i = 0; i < vm->hashes->size; i++)
    {
        if (strcmp(vm->hashes->data[i]->key, varname) == 0)
        {
            return vm->hashes->data[i]->index;
        }
    }
    return -1;
}

void hashset(VirtualMachine *vm, char* varname, Int index)
{
    if (hashfind(vm, varname) != -1)
    {
        vm->hashes->data[hashfind(vm, varname)]->index = index;
    }
    else 
    {
        Hash *hash = (Hash*)malloc(sizeof(Hash));
        hash->key = strdup(varname);
        hash->index = index;
        StackPush(*vm->hashes, hash);
    }
}


// by types
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
    vm->stack->data[index]->value.string = strdup(string);
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
    IntList *list = (IntList*)malloc(sizeof(IntList));
    StackInit(*list);

    setVar(vm, index, TYPE_LIST, (Value){pointer: list});
    return index;
}

Int newError(VirtualMachine *vm, char *string)
{
    Int index = newVar(vm);
    setVar(vm, index, TYPE_ERROR, (Value){string: string});
    return index;
}

//create (just like new but for hash)

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

Int spawnError(VirtualMachine *vm, char *varname, char *string)
{
    Int index = newError(vm, string);
    hashset(vm, varname, index);
    return index;
}


//list
//list
//list

void listpush(VirtualMachine *vm, Int list, Int value)
{
    StackPush(*((IntList*)vm->stack->data[list]->value.pointer), value);
}

void listunshift(VirtualMachine *vm, Int list, Int value)
{
    StackInsert(*((IntList*)vm->stack->data[list]->value.pointer), 0, value);
}

Int listpop(VirtualMachine *vm, Int list)
{
    return StackPop(*((IntList*)vm->stack->data[list]->value.pointer));
}

Int listshift(VirtualMachine *vm, Int list)
{
    return StackShift(*((IntList*)vm->stack->data[list]->value.pointer));
}

void listinsert(VirtualMachine *vm, Int list, Int index, Int value)
{
    StackInsert(*((IntList*)vm->stack->data[list]->value.pointer), index, value);
}

Int listremove(VirtualMachine *vm, Int list, Int index)
{
    return StackRemove(*((IntList*)vm->stack->data[list]->value.pointer), index);
}

Int listsize(VirtualMachine *vm, Int list)
{
    return ((IntList*)vm->stack->data[list]->value.pointer)->size;
}

// Parser functions
List* parse(VirtualMachine *vm, char *cmd) 
{
    List *result = createList();
    StringList *splited = specialSplit(cmd);

    while (splited->size > 0) 
    {
        char* str = StackShift(*splited);
        if (str[0] == '(') 
        {
            // string
            if(str[1] == '$') 
            {
                StackPush(*result, createVariable(TYPE_STRING, (Value){string: strndup(str + 2, strlen(str) - 3)}));
            } 
            else //expression 
            {
                Function __eval = (Function)vm->stack->data[hashfind(vm, "eval")]->value.pointer;
                // fix this, idea: parse use indexes of the args on the stack instead of the args themselves 
                //StackPush(*result, createVariable(TYPE_NUMBER, (Value){number: __eval(vm, parse(vm, strndup(str + 1, strlen(str) - 2)))}));
                //printf("expression\n");
            }
        } 
        else if (str[0] == '$') 
        {
            Int index = hashfind(vm, str + 1);
            if (index == -1) 
            {
                StackPush(*result, createVariable(TYPE_ERROR, (Value){string: strdup("Variable not found")}));
            }
            else 
            {
                if ((str[1] >= '0' && str[1] <= '9')) 
                {
                    StackPush(*result, vm->stack->data[atoi(str+1)]);
                }
                else 
                {
                    StackPush(*result, vm->stack->data[index]);
                }
            }
        }
        else if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-') 
        {
            printf("number\n");
            StackPush(*result, vm->stack->data[newNumber(vm, atof(str))]);
        } 
        else //string 
        {
            StackPush(*result, vm->stack->data[newString(vm, str)]);
        }

        free(str);
    }
    StackFree(*splited);
    return result;
}

int eval(VirtualMachine *vm, char* str) 
{    
    Int result;
    List *args = parse(vm, str);
    
    Variable *funcname = StackShift(*args);
    if (funcname->type == TYPE_ERROR) 
    {
        printf("%s\n", funcname->value.string);
        return -1;
    }
    if (funcname->type != TYPE_STRING) 
    {
        printf("First argument must be a string\n");
        return -1;
    }
    Int index = hashfind(vm, funcname->value.string);
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

    StackFree(*args);

    return result;
}

Int bulkEval(VirtualMachine *vm, char *str)
{
    StringList *splited = splitString(str, ";");
    Int result = 0;
    for (Int i = 0; i < splited->size; i++)
    {
        result = eval(vm, splited->data[i]);
        if (result > 0)
        {
            break;
        }
    }
    StackFree(*splited);
    return result;
}

//print

void print(VirtualMachine *vm, Int index)
{
    if (vm->stack->data[index]->type == TYPE_NUMBER)
    {
        printf("%f\n", vm->stack->data[index]->value.number);
    }
    else if (vm->stack->data[index]->type == TYPE_STRING)
    {
        printf("%s\n", vm->stack->data[index]->value.string);
    }
    else if (vm->stack->data[index]->type == TYPE_LIST)
    {
        printf("[");
        IntList *list = (IntList*)vm->stack->data[index]->value.pointer;
        for (Int i = 0; i < (list->size-1); i++)
        {
            printf("%d, ", list->data[i]);
        }
        printf("%d", list->data[list->size-1]);
        printf("]\n");
    }
    else if (vm->stack->data[index]->type == TYPE_ERROR)
    {
        printf("Error: %s\n", vm->stack->data[index]->value.string);
    }
    else if (vm->stack->data[index]->type == TYPE_FUNCTION)
    {
        printf("Function : [%d] %p\n", index, vm->stack->data[index]->value.pointer);
    }
    else
    {
        printf("Unknown type\n");
    }
}

//functions

Int _set(VirtualMachine *vm, List *args)
{
    Variable* varname = args->data[0];
    Variable* value = args->data[1];
    hashset(vm, varname->value.string, value->value.number);
}

Int _eval(VirtualMachine *vm, List *args)
{
    Variable* str = StackShift(*args);
    Int result = bulkEval(vm, str->value.string);
    free(str);
    return result;
}

//main
void main()
{
    VirtualMachine *vm = createVM();

    Int a = newNumber(vm, 5);
    Int b = newNumber(vm, 10);
    Int c = newNumber(vm, 15);
    Int d = newNumber(vm, 20);
    Int e = newNumber(vm, 25);

    Int abc = newString(vm, "example string");

    Int __list = newList(vm);

    Int func = spawnFunction(vm, "set", _set);
    spawnFunction(vm, "eval", _eval);

    listpush(vm, __list, a);
    listpush(vm, __list, b);
    listpush(vm, __list, c);
    
    print(vm, __list);
    print(vm, func);

    print(vm, a);
    print(vm, b);
    print(vm, c);
    print(vm, d);
    print(vm, e);
    print(vm, abc);

    freeVar(vm, a);
    freeVar(vm, b);
    freeVar(vm, c);
    freeVar(vm, d);
    freeVar(vm, e);
    freeVar(vm, abc);
    freeVar(vm, __list);
    //freeVar(vm, func);


    // split test

    char* cmd = "set a 4;\nset b 8;(set c 4)";
    
    eval(vm, cmd);
    
    // free
    freeVM(vm);
}   