#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//stack implementation
#define Stack(T) struct { T *data; int size; int capacity; }
#define StackInit(s) do { (s).data = NULL; (s).size = 0; (s).capacity = 0; } while (0)
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
#define StackFree(s) free((s).data)
//swap elements from index i1 to index i2
#define StackMove(s, i1, i2) do { \
    typeof((s).data[i1]) tmp = (s).data[i1]; \
    (s).data[i1] = (s).data[i2]; \
    (s).data[i2] = tmp; \
} while (0)


//hashtables implementation
#define HashTable(T) struct { char **keys; T *ValueStack; int size; int capacity; }
#define HashTableInit(h) do { (h).keys = NULL; (h).ValueStack = NULL; (h).size = 0; (h).capacity = 0; } while (0)
#define HashTableFree(h) do { free((h).keys); free((h).ValueStack); } while (0)
#define HashTableInsert(h, k, v) do { \
    if ((h).size == (h).capacity) { \
        (h).capacity = (h).capacity == 0 ? 1 : (h).capacity * 2; \
        (h).keys = realloc((h).keys, (h).capacity * sizeof(*(h).keys)); \
        (h).ValueStack = realloc((h).ValueStack, (h).capacity * sizeof(*(h).ValueStack)); \
    } \
    (h).keys[(h).size] = strdup(k); \
    (h).ValueStack[(h).size++] = (v); \
} while (0)
#define HashTableRemove(h, k) do { \
    for (int i = 0; i < (h).size; i++) { \
        if (strcmp((h).keys[i], k) == 0) { \
            free((h).keys[i]); \
            for (int j = i; j < (h).size - 1; j++) { \
                (h).keys[j] = (h).keys[j + 1]; \
                (h).ValueStack[j] = (h).ValueStack[j + 1]; \
            } \
            (h).size--; \
            break; \
        } \
    } \
} while (0)
#define HashTableGet(T, h, k) ({ \
    T ret = {0}; \
    for (int i = 0; i < (h).size; i++) { \
        if (strcmp((h).keys[i], k) == 0) { \
            ret = (h).ValueStack[i]; \
            break; \
        } \
    } \
    ret; \
})

typedef union 
{
    int i;
    float f;
    char* s;
    void* p;
} Value;

typedef struct 
{
    Value value;
    int type;
} Generic;

typedef Stack(Generic) GenericStack;
typedef Stack(char*) StringStack;

typedef HashTable(Generic) GenericHashTable;



StringStack splitString(const char* str) 
{
    StringStack stack;
    StackInit(stack);

    int n = strlen(str);
    char token[1024];
    int tokenIndex = 0;
    int insideParens = 0;

    for (int i = 0; i <= n; ++i) 
    {
        char c = str[i];

        // Check for delimiters
        if (c == '(') 
        {
            insideParens++;
        } 
        else if (c == ')') 
        {
            if (insideParens > 0) 
            {
                insideParens--;
            }
        }

        // Add character to current token
        if (i < n && (!isspace(c) || insideParens > 0)) 
        {
            token[tokenIndex++] = c;
        } 
        else if (tokenIndex > 0) 
        {
            // End of token
            token[tokenIndex] = '\0';
            char* newToken = (char*)malloc((tokenIndex + 1) * sizeof(char));
            strcpy(newToken, token);
            StackPush(stack, newToken);
            tokenIndex = 0;
        }
    }

    return stack;
}

GenericStack parseArgs(StringStack strs, GenericHashTable globals)
{
    GenericStack stack;
    StackInit(stack);

    for (int i = 0; i < strs.size; ++i) 
    {
        char* str = strs.data[i];

        if (str[0] == ':') 
        {
            // String
            Generic v;
            // Remove the colon and the parentheses
            v.value.s = strndup(str + 2, strlen(str) - 3);
            v.type = 3;
            StackPush(stack, v);
        }
        else if (str[0] == '$') 
        {
            // Variable
            Generic v;
            v = HashTableGet(Generic, globals, str + 1);
            
            StackPush(stack, v);
        }
        else if (str[0] == '(') 
        {
        }
        else if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-') 
        {
            // Number
            Generic v;
            if (strchr(str, '.')) 
            {
                v.value.f = atof(str);
                v.type = 2;
            } 
            else 
            {
                v.value.i = atoi(str);
                v.type = 1;
            }
            StackPush(stack, v);
        }
        else 
        {
            // string
            Generic v;
            v.value.s = strdup(str);
            v.type = 3;
            StackPush(stack, v);
        }
        
        free(str);

    }



    return stack;
}


int main() 
{
    const char* input = "abc :(hello world!) 555 1234 1.423421 :(string a b c d e f g 1 2 3 4 5) ";
    GenericHashTable vht;
    GenericStack args = parseArgs(splitString(input), vht);

    for (int i = 0; i < args.size; ++i) 
    {
        Generic v = args.data[i];
        if (v.type == 1) 
        {
            printf("int: %d\n", v.value.i);
        } 
        else if (v.type == 2) 
        {
            printf("Float: %f\n", v.value.f);
        }
        else if (v.type == 3) 
        {
            printf("String: %s\n", v.value.s);
        }
    }

    StringStack tokens = splitString(input);

    for (int i = 0; i < tokens.size; ++i) {
        printf("Token %d: %s\n", i, tokens.data[i]);
    }

    StackFree(tokens);

    // hashtable test
    HashTable(int) table;
    HashTableInit(table);

    HashTableInsert(table, "a", 1);
    HashTableInsert(table, "b", 2);
    HashTableInsert(table, "c", 3);

    printf("Value of a: %d\n", HashTableGet(int, table, "a"));
    printf("Value of b: %d\n", HashTableGet(int, table, "b"));
    printf("Value of c: %d\n", HashTableGet(int, table, "c"));

    HashTableRemove(table, "b");

    printf("Value of f: %d\n", HashTableGet(int, table, "f"));

    HashTableFree(table);
    
    return 0;
}