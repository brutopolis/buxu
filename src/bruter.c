#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MaxSize int

#define FloatSize double


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

const char* Version = "0.4.2";

// type -1 is error, it can contain a string with the error message
const enum 
{
    TYPE_ERROR = -1,
    TYPE_NIL = 0,
    TYPE_REFERENCE = 1,
    TYPE_NUMBER = 2,
    TYPE_STRING = 3,
    TYPE_FUNCTION = 4,
    TYPE_LIST = 5,
} Types;



//Value
typedef union 
{
    FloatSize number;
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
    MaxSize index;
} Hash;

//List
typedef Stack(Variable*) List;
typedef Stack(Hash*) HashList;
typedef Stack(int) IntList;
typedef Stack(char*) StringList;

typedef struct
{
    IntList *array;
    HashList *hashes;
} PseudoList;

typedef struct
{
    List *stack;
    HashList *hashes;
    IntList *empty;
} VirtualMachine;

//Function
typedef MaxSize (*Function)(VirtualMachine*, List*);

Variable* createVariable(char type, Value value)
{
    Variable *var = (Variable*)malloc(sizeof(Variable));
    var->type = type;
    var->value = value;
    return var;
}

List* createList(VirtualMachine *vm)
{
    List *list = (List*)malloc(sizeof(List));
    StackInit(*list);
    return list;
}

VirtualMachine* createVM()
{
    VirtualMachine *vm = (VirtualMachine*)malloc(sizeof(VirtualMachine));
    vm->stack = createList(vm);
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
    
    int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '(')
        {
            int j = i;
            int count = 1;
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
        else if (str[i] == ' ')
        {
            i++;
        }
        else
        {
            int j = i;
            while (str[j] != ' ' && str[j] != '\0' && str[j] != '(' && str[j] != ')')
            {
                j++;
            }
            StackPush(*splited, strndup(str + i, j - i));
            i = j;
        }
    }

    return splited;
}

// Parser functions
List* parse(VirtualMachine *vm, char *cmd) 
{
    List *result = createList(vm);
    StringList *splited = specialSplit(cmd);

    for (int i = 0; i < splited->size; ++i) 
    {
        char* str = StackShift(*splited);

        if (str[0] == '$') 
        {
            // Variable
            if(str[1] == '(') 
            {
                StackPush(*result, createVariable(TYPE_STRING, (Value){string: strndup(str + 2, strlen(str) - 3)}));
            } 
            else 
            {
                
            }
        } 
        else if (str[0] == '(') 
        {
            // expression
            
        } 
        else if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-') 
        {
            
        } 
        else 
        {
            
        }

        free(str);
    }

    return result;
}

void freeVM(VirtualMachine *vm)
{
    for (int i = 0; i < vm->stack->size; i++)
    {
        if (vm->stack->data[i] != NULL)
        {
            /*if (vm->stack->data[i]->type == TYPE_STRING || vm->stack->data[i]->type == TYPE_ERROR)
            {
                free(vm->stack->data[i]->value.string);
            }
            else */
            if (vm->stack->data[i]->type == TYPE_LIST)
            {
                StackFree(*((PseudoList*)vm->stack->data[i]->value.pointer)->array);
                StackFree(*((PseudoList*)vm->stack->data[i]->value.pointer)->hashes);
                free(vm->stack->data[i]->value.pointer);
            }
            free(vm->stack->data[i]);
        }
    }
    StackFree(*vm->stack);
    StackFree(*vm->hashes);
    StackFree(*vm->empty);
    free(vm);
}

int newvar(VirtualMachine *vm)
{
    if (vm->empty->size > 0)
    {
        int id = StackShift(*vm->empty);
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

void freevar(VirtualMachine *vm, int index)
{
    StackPush(*vm->empty, index);
    /*if (vm->stack->data[index]->type == TYPE_STRING || vm->stack->data[index]->type == TYPE_ERROR)
    {
        free(vm->stack->data[index]->value.string);
    }
    else */
    if (vm->stack->data[index]->type == TYPE_LIST)
    {
        StackFree(*((PseudoList*)vm->stack->data[index]->value.pointer)->array);
        StackFree(*((PseudoList*)vm->stack->data[index]->value.pointer)->hashes);
        free(vm->stack->data[index]->value.pointer);
    }
    free(vm->stack->data[index]);
    vm->stack->data[index] = NULL;
}

void setvar(VirtualMachine *vm, int index, char type, Value value)
{
    if (vm->stack->data[index]->type == TYPE_STRING || vm->stack->data[index]->type == TYPE_ERROR)
    {
        free(vm->stack->data[index]->value.string);
    }
    vm->stack->data[index]->type = type;
    vm->stack->data[index]->value = value;
}

// by types
int newNumber(VirtualMachine *vm, FloatSize number)
{
    int index = newvar(vm);
    vm->stack->data[index]->type = TYPE_NUMBER;
    vm->stack->data[index]->value.number = number;
    return index;
}

int newString(VirtualMachine *vm, char *string)
{
    int index = newvar(vm);
    setvar(vm, index, TYPE_STRING, (Value){string: string});
    return index;
}

int newFunction(VirtualMachine *vm, Function function)
{
    int index = newvar(vm);
    setvar(vm, index, TYPE_FUNCTION, (Value){pointer: function});
    return index;
}

int newList(VirtualMachine *vm)
{
    int index = newvar(vm);
    PseudoList *list = (PseudoList*)malloc(sizeof(PseudoList));
    list->array = (IntList*)malloc(sizeof(IntList));
    StackInit(*list->array);
    list->hashes = (HashList*)malloc(sizeof(HashList));
    StackInit(*list->hashes);
    setvar(vm, index, TYPE_LIST, (Value){pointer: list});
    return index;
}

int newReference(VirtualMachine *vm, int index)
{
    int ref = newvar(vm);
    setvar(vm, ref, TYPE_REFERENCE, (Value){number: index});
    return ref;
}

MaxSize _set(VirtualMachine *vm, List *args)
{
    Variable* varname = args->data[0];
    Variable* value = args->data[1];

}


void main()
{
    VirtualMachine *vm = createVM();
    int a = newNumber(vm, 5);
    int b = newNumber(vm, 10);
    int c = newNumber(vm, 15);
    int d = newNumber(vm, 20);
    int e = newNumber(vm, 25);

    int abc = newString(vm, "abc");

    int tbl = newList(vm);

    int func = newFunction(vm, _set);

    StackPush(*((PseudoList*)vm->stack->data[tbl]->value.pointer)->array, a);
    StackPush(*((PseudoList*)vm->stack->data[tbl]->value.pointer)->array, b);
    StackPush(*((PseudoList*)vm->stack->data[tbl]->value.pointer)->array, c);

    printf("%f\n", vm->stack->data[a]->value.number);
    printf("%f\n", vm->stack->data[b]->value.number);
    printf("%f\n", vm->stack->data[c]->value.number);
    printf("%f\n", vm->stack->data[d]->value.number);
    printf("%f\n", vm->stack->data[e]->value.number);
    printf("%s\n", vm->stack->data[abc]->value.string);

    freevar(vm, a);
    freevar(vm, b);
    freevar(vm, c);
    freevar(vm, d);
    freevar(vm, e);
    freevar(vm, abc);
    freevar(vm, tbl);
    freevar(vm, func);
    
    char* cmd = "(+ 1 2) adsdasda 11asdasd -dasdasd -a8d5as asdasdasd(asdasd asda aa aa ( dasda 1 ) (dadasdd(dsadas)));";
    StringList *splited = specialSplit(cmd);
    while (splited->size > 0)
    {
        char * str = StackShift(*splited);
        printf("%s\n", str);
        free(str);
    }
    StackFree(*splited);
    //free(splited);

    freeVM(vm);
}   