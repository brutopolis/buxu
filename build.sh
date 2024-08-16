rm -rf bin/bruter
#rm -rf valgrind-out.txt

gcc src/bruter.c \
    src/variable.c \
    src/string.c \
    src/hashtable.c \
    src/args.c \
    src/list.c \
    src/std.c \
    -o bin/bruter\
    -lm -Os -g

valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --log-file=valgrind-out.txt \
    --verbose bin/bruter example/test1.br
