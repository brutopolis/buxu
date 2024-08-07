rm -rf bin/bruter
#rm -rf valgrind-out.txt
gcc src/bruter.c -o bin/bruter -lm -Os
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt bin/bruter
#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt bin/bruter example/test.br