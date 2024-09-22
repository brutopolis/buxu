rm -rf bin/bruter
#rm -rf valgrind-out.txt


gcc src/main.c\
    src/bruter.c\
    src/std.c\
 -o bin/bruter -lm -Os -g

valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --log-file=valgrind-out.txt \
    --verbose bin/bruter example/prototype.br
