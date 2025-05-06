#ifndef BUXU_H
#define BUXU_H 1
// bruter
#include "bruter.h"

#include <stdarg.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <stddef.h>

#ifndef ARDUINO
char* file_read(char *filename);
bool file_exists(char* filename);
void file_write(char *filename, char *code);
#endif

#define BUXU_VERSION "0.1.3"

#define data(index) context->data[index]
#define data_s(index) &context->data[index].u8[0]
#define data_l(index) context->keys[index]

// we always use the first argument as the function pointer
// the second argument is the first argument and so on
#define arg(index) context->data[args->data[index+1].i]
#define arg_s(index) &context->data[args->data[index+1].i].u8[0]
#define arg_i(index) args->data[index+1].i

#define init(name) void init_##name(List *context)

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

#define add_function(context, name, func) \
({ \
    Int index = new_var(context, name); \
    data(index).p = func; \
    index; \
})

char* str_duplicate(const char *str);
char* str_nduplicate(const char *str, UInt size);

typedef void (*InitFunction)(List*);

List* special_space_split(char *str);
List* special_split(char *str, char delim);

char* str_format(const char *format, ...);

Int new_var(List *context, char* key);
Int new_block(List *context, Int size, char* key);
Int new_string(List *context, char* str, char* key);
Value parse_number(char *str);
List* parse(void *_context, char *cmd);

List* compile_code(List *context, char *code);
Int context_call(List *context, List *args);
List* compile_and_call(List *context, char *cmd);
Int compiled_call(List *context, List *compiled);
void compiled_free(List *compiled);
Int eval(List *context, char *cmd);

#define function(name) Int name(List *context, List *args)

#endif