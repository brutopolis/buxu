rm -rf bin/bruter

gcc src/main.c\
    src/bruter.c\
    src/std.c\
 -o bin/bruter -lm -O3

bin/bruter example/prototype.br
