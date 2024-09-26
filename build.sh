rm -rf bin/bruter

gcc src/main.c\
    src/bruter.c\
    src/std.c\
    src/std_os.c\
    src/multiprocess.c\
 -o bin/bruter -O3 -lm

bin/bruter example/multiproc.br
