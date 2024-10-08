rm -rf build
mkdir build
#rm -rf valgrind-out.txt


gcc src/main.c\
    src/bruter.c\
    src/std.c\
    src/std_os.c\
    src/std_linux.c\
 -o build/bruter -lm -O3 -g

valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --log-file=valgrind-out.txt \
    --verbose build/bruter example/threads.br

#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=valgrind-out.txt --verbose build/bruter example/threads.br