# this is for linux, should work on macos cygwin also, though I haven't tested it
# in future i might add build scripts for other platforms

rm -rf bin/bruter

gcc src/main.c\
    src/bruter.c\
    src/std.c\
    src/std_os.c\
    src/std_linux.c\
 -o bin/bruter -O3 -lm

bin/bruter example/multiproc.br
