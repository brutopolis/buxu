rm -rf bin/bruter
#rm -rf valgrind-out.txt

#gcc src/bruter.c \
#    src/variable.c \
#    src/string.c \
#    src/hashtable.c \
#    src/args.c \
#    src/list.c \
#    src/std.c \
#    -o bin/bruter\
#    -lm -Os -g

gcc src/bruter.c\
    src/std.c\
 -o bin/bruter -lm -O3

bin/bruter example/functions.br
