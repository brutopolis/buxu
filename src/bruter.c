#include "bruter.h"

// Define the function to create a new table
Table* createNewTable() {
    Table* newTable = (Table*)malloc(sizeof(Table));
    HashTableInit(*newTable);
    return newTable;
}

StringStack specialSplit(const char* str) {
    StringStack stack;
    StackInit(stack);

    int n = strlen(str);
    char token[1024];
    int tokenIndex = 0;
    int insideParens = 0;

    for (int i = 0; i <= n; ++i) {
        char c = str[i];

        // Check for delimiters
        if (c == '(') {
            insideParens++;
        } else if (c == ')') {
            if (insideParens > 0) {
                insideParens--;
            }
        }

        // Add character to current token
        if (i < n && (!isspace(c) || insideParens > 0)) {
            token[tokenIndex++] = c;
        } else if (tokenIndex > 0) {
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

StringStack stringSplit(const char* str, const char delim) {
    StringStack splited;
    StackInit(splited);
    int n = 0;
    //split string by ; but not in parentheses
    while (n < strlen(str)) {
        char token[1024];
        int tokenIndex = 0;
        int insideParens = 0;
        while (n < strlen(str)) {
            char c = str[n];
            if (c == '(') {
                insideParens++;
            } else if (c == ')') {
                if (insideParens > 0) {
                    insideParens--;
                }
            }
            if (c == delim && insideParens == 0) {
                break;
            }
            token[tokenIndex++] = c;
            n++;
        }
        token[tokenIndex] = '\0';
        char* newToken = (char*)malloc((tokenIndex + 1) * sizeof(char));
        strcpy(newToken, token);
        StackPush(splited, newToken);
        n++;
    }
    return splited;
}

Variable recursiveGet(Table *state, char* key) {
    StringStack splited = stringSplit(key, '.');
    char* currentKey = StackShift(splited);
    Variable v = HashTableGet(Variable, *state, currentKey);

    while (splited.size > 0) {
        if (v.type == 1) // Verifica se é do tipo tabela
        {
            currentKey = StackShift(splited);
            v = HashTableGet(Variable, *(Table*)v.value.p, currentKey);
        } else {
            v.type = -1; // Define um tipo de erro se não for uma tabela
            v.value.s = strdup("Not a table");
            break;
        }
    }

    return v;
}

void recursiveSet(Table *state, char* key, Variable value) {
    StringStack splited = stringSplit(key, '.');

    if (splited.size == 1) {
        HashTableInsert(*state, key, value);
        return;
    }

    char* currentKey = StackShift(splited);
    Variable v = HashTableGet(Variable, *state, currentKey);

    while (splited.size > 1) {
        if (v.type != 1) // Cria uma nova tabela se não for uma tabela
        {
            Table *newTable = createNewTable(); // Função fictícia para criar uma nova tabela
            Variable newVar;
            newVar.type = 1;
            newVar.value.p = newTable;
            HashTableInsert(*state, currentKey, newVar);
            v = newVar;
        }

        state = (Table*)v.value.p;
        currentKey = StackShift(splited);
        v = HashTableGet(Variable, *state, currentKey);
    }

    if (v.type == 1) {
        currentKey = StackShift(splited);
        HashTableInsert(*(Table*)v.value.p, currentKey, value);
    } else {
        HashTableInsert(*state, currentKey, value);
    }
}

void recursiveUnset(Table *state, char* key) {
    StringStack splited = stringSplit(key, '.');

    if (splited.size == 1) {
        HashTableRemove(*state, key);
        return;
    }

    char* currentKey = StackShift(splited);
    Variable v = HashTableGet(Variable, *state, currentKey);

    while (splited.size > 1) {
        if (v.type == 1) // Verifica se é do tipo tabela
        {
            state = (Table*)v.value.p;
            currentKey = StackShift(splited);
            v = HashTableGet(Variable, *state, currentKey);
        } else {
            v.type = -1; // Define um tipo de erro se não for uma tabela
            v.value.s = strdup("Not a table");
            return;
        }
    }

    if (v.type == 1) {
        currentKey = StackShift(splited);
        HashTableRemove(*(Table*)v.value.p, currentKey);
    } else {
        HashTableRemove(*state, key);
    }
}

List parse(Table *state, char *cmd) {
    List result;
    StackInit(result);

    StringStack splited = specialSplit(cmd);

    for (int i = 0; i < splited.size; ++i) {
        char* str = splited.data[i];

        if (str[0] == '$') {
            // Variable
            Variable v;
            if(str[1] == '(') {
                v.value.s = strndup(str + 2, strlen(str) - 3);
                v.type = 3;
            } else {
                v = recursiveGet(state, strdup(str + 1));
            }

            StackPush(result, v);
        } else if (str[0] == '(') {
            // expression
            Variable v;
            Variable expression;
            List args = parse(state, strndup(str + 1, strlen(str) - 2));

            v = ((Function)HashTableGet(Variable, *state, "interpret").value.p)(state, &args);
            StackPush(result, v);
        } else if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-') {
            // Number
            Variable v;
            v.value.f = atof(str);
            v.type = 2;
            StackPush(result, v);
        } else {
            // string
            Variable v;
            v.value.s = strdup(str);
            v.type = 3;
            StackPush(result, v);
        }

        free(str);
    }

    StackFree(splited);

    return result;
}

Variable interpret(Table *state, char* input) {
    List args = parse(state, input);
    char* funcName = StackShift(args).value.s;
    Variable result = Nil;
    if (HashTableGet(Variable, *state, funcName).type == 4) {
        result = ((Function)HashTableGet(Variable, *state, funcName).value.p)(state, &args);
        StackFree(args);
        return result;
    } else {
        result.type = -1;
        result.value.s = strdup("Unknown function");
    }
    StackFree(args);
    return result;
}

Variable bulkInterpret(Table *state, const char* input) {
    StringStack splited = stringSplit(input, ';');
    Variable result = Nil;
    while (splited.size > 0) {
        result = interpret(state, StackShift(splited));
        if (result.type != 0) {
            break;
        }
    }
    StackFree(splited);
    return result;
}

Variable _interpret(Table *state, List* args) {
    Variable result = Nil;
    if (args->size == 1) {
        return bulkInterpret(state, StackShift(*args).value.s);
    } else {
        char* funcName = StackShift(*args).value.s;
        if (HashTableGet(Variable, *state, funcName).type == 4) {
            result = ((Function)HashTableGet(Variable, *state, funcName).value.p)(state, args);
            StackFree(*args);
            return result;
        } else {
            result.type = -1;
            result.value.s = strdup("Unknown function");
        }
        StackFree(*args);
        return result;
    }
}
Variable _table(Table *state, List* args) 
{
    Table* newTable = (Table*)malloc(sizeof(Table));
    HashTableInit(*newTable);
    return (Variable){.type = 1, .value = {.p = newTable}};
}

Variable _unset(Table *state, List* args) 
{
    Variable _key = StackShift(*args);
    recursiveUnset(state, _key.value.s);
    return Nil;
}

Variable _set(Table *state, List* args) 
{
    Variable _key = StackShift(*args);
    Variable _value = StackShift(*args);
    recursiveSet(state, _key.value.s, _value);

    return Nil;
}

Variable _print(Table *state, List* args) 
{
    while (args->size > 0) 
    {
        Variable v = StackShift(*args);
        if (v.type == 2) 
        {
            printf("%f ", v.value.f);
        }
        else if (v.type == 3) 
        {
            printf("%s ", v.value.s);
        }
        else if (v.type == 1) 
        {
            printf("[table] ");
        }
        else if (v.type == 4) 
        {
            printf("[function] ");
        }
    }
    printf("\n");
    return Nil;
}

Variable _ls(Table *_state, List* args) 
{
    Table *state;
    if (args->size == 0) 
    {
        state = _state;
    }
    else
    {
        Variable _ = StackShift(*args);
        printf("_ type = %d\n", _.type);
        //printf("_ value = %s\n", _.value.s);
        if (_.type == 1) 
        {
            state = _.value.p;
        }
        else
        {
            return Nil;
        }
    }

    for (int i = 0; i < state->size; i++) 
    {
        switch (state->ValueStack[i].type) 
        {
            case -1:
                printf("[error] %s : \n", state->keys[i], state->ValueStack[i].value.s);
                break;
            case 0:
                printf("[void] %s\n", state->keys[i]);
                break;
            case 1:
                printf("[table] %s\n", state->keys[i]);
                break;
            case 2:
                printf("[number] %s : %f\n", state->keys[i], state->ValueStack[i].value.f);
                break;
            case 3:
                printf("[string] %s : %s\n", state->keys[i], state->ValueStack[i].value.s);
                break;
            case 4:
                printf("[function] %s : %p\n", state->keys[i], state->ValueStack[i].value.p);
                break;
        } 
    }
    return Nil;
}

Variable _exit(Table *state, List* args) 
{
    exit(0);
    return Nil;
}

// math functions

Variable _add(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = StackShift(*args).value.f + StackShift(*args).value.f;
    result.type = 2;
    return result;
}

Variable _sub(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = StackShift(*args).value.f - StackShift(*args).value.f;
    result.type = 2;
    return result;
}

Variable _mul(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = StackShift(*args).value.f * StackShift(*args).value.f;
    result.type = 2;
    return result;
}

Variable _div(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = StackShift(*args).value.f / StackShift(*args).value.f;
    result.type = 2;
    return result;
}

Variable _mod(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = (int)StackShift(*args).value.f % (int)StackShift(*args).value.f;
    result.type = 2;
    return result;
}

Variable _pow(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = pow(StackShift(*args).value.f, StackShift(*args).value.f);
    result.type = 2;
    return result;
}

Variable _ceil(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = ceil(StackShift(*args).value.f);
    result.type = 2;
    return result;
}

Variable _floor(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = floor(StackShift(*args).value.f);
    result.type = 2;
    return result;
}

Variable _round(Table *state, List* args) 
{
    Variable result = Nil;
    result.value.f = round(StackShift(*args).value.f);
    result.type = 2;
    return result;
}


// Table functions
void registerFunction(Table *state, char* name, Function func) 
{
    Variable tempFunc = Nil;
    tempFunc.type = 4;
    tempFunc.value.p = func;
    HashTableInsert(*state, name, tempFunc);
}

void repl(Table *state) 
{
    char input[1024];
    printf("bruter %s\n", Version);
    while (1) 
    {
        printf("br> ");
        fgets(input, 1024, stdin);
        input[strlen(input) - 1] = '\0';
        Variable result = bulkInterpret(state, input);
        if (result.type > 0) 
        {
            if (result.type == 2) 
            {
                printf("returned: %f\n", result.value.f);
            } 
            else if (result.type == 3) 
            {
                printf("returned: %s\n", result.value.s);
            }
        }
        else if(result.type == -1)
        {
            printf("Error(%s): %s\n", result.value.s, input);
        }
    }
}

int main(int argc, char** argv) 
{
    Table state;
    HashTableInit(state);
    char* filename;
    char* filetxt;

    //turn args into a string stack
    StringStack args;
    StackInit(args);

    for (int i = 1; i < argc; i++)
    {
        StackPush(args, argv[i]);
    }


    while (args.size > 0)
    {
        char* arg = StackShift(args);
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
                printf("bruter %s\n", Version);
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

    /*if (filename == NULL)
    {
        printf("No file specified\n");
        return 1;
    }
    else */
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

    
    
    registerFunction(&state, "add", _add);
    registerFunction(&state, "sub", _sub);
    registerFunction(&state, "mul", _mul);
    registerFunction(&state, "div", _div);
    registerFunction(&state, "mod", _mod);
    registerFunction(&state, "pow", _pow);
    registerFunction(&state, "ceil", _ceil);
    registerFunction(&state, "floor", _floor);
    registerFunction(&state, "round", _round);
    

    registerFunction(&state, "set", _set);
    registerFunction(&state, "unset", _unset);
    registerFunction(&state, "print", _print);
    registerFunction(&state, "interpret", _interpret);
    registerFunction(&state, "ls", _ls);
    registerFunction(&state, "exit", _exit);

    registerFunction(&state, "table", _table);

    if (filetxt != NULL)
    {
        bulkInterpret(&state, filetxt);
    }
    else 
    {
        repl(&state);
    }
    
    return 0;
}