#ifndef BUXU_H
#define BUXU_H 1
// bruter
#include <bruter.h>

#define BUXU_VERSION "0.0.9"

// emoticons
#define EMOTICON_DEFAULT "[=°-°=]"
#define EMOTICON_IDLE "[=° °=]"
#define EMOTICON_WARNING "[=°~°=]"
#define EMOTICON_ERROR "[=°x°=]"
#define EMOTICON_CONFUSED "[=º?°=]"

// define a macro to printf but it always starts with [=°-°=] and ends with a newline
#define buxu_print(emoticon, ...) printf(emoticon ": "); printf(__VA_ARGS__); printf("\n")
#define buxu_warn(...) printf(EMOTICON_WARNING ": warning: "); printf(__VA_ARGS__); printf("\n")
#define buxu_error(...) printf(EMOTICON_ERROR ": error: "); printf(__VA_ARGS__); printf("\n")
#define buxu_info(...) printf(EMOTICON_DEFAULT ": info: "); printf(__VA_ARGS__); printf("\n")

#define add_function(vm, name, func) \
({ \
    Int index = new_var(vm, name); \
    data(index).p = func; \
    index; \
})

#ifndef FILEUTILS_H
#define FILEUTILS_H 1

static char* readfile(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return NULL;
    }

    char *code = (char*)malloc(1);
    if (code == NULL)
    {
        buxu_error("could not allocate memory for file");
        fclose(file);
        return NULL;
    }

    code[0] = '\0';

    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, file) != -1)
    {
        size_t new_size = strlen(code) + strlen(line) + 1;
        char *temp = realloc(code, new_size);
        if (temp == NULL)
        {
            buxu_error("could not reallocate memory while reading file");
            free(code);
            free(line);
            fclose(file);
            return NULL;
        }
        code = temp;
        strcat(code, line);
    }

    free(line);
    fclose(file);
    return code;
}

static void writefile(char *filename, char *code)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        return;
    }

    fprintf(file, "%s", code);
    fclose(file);
}

static bool file_exists(char* filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return false;
    }

    fclose(file);
    return true;
}

#endif // FILEUTILS_H
#endif