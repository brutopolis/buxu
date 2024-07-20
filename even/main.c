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

typedef Stack(char*) strlist;

strlist splitString(const char* str) 
{
    strlist stack;
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

int main() 
{
    const char* input = "abc :string,hello,world,!: :set,var,from,func,arg1,arg2:print,$a $(string a b c d e f g 1 2 3 4 5) $var )";
    strlist tokens = splitString(input);

    for (int i = 0; i < tokens.size; ++i) {
        printf("Token %d: %s\n", i, tokens.data[i]);
    }

    StackFree(tokens);

    return 0;
}