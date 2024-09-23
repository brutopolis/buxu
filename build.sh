rm -rf bin/bruter

gcc src/main.c\
    src/bruter.c\
    src/std.c\
 -o bin/bruter -O3 -lm

bin/bruter example/prototype.br
