#ifndef BUXU_H
#define BUXU_H 1
// bruter
#include "bruter.h"

#define BUXU_VERSION "0.0.4"

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

#endif