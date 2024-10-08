rm -rf arduino
mkdir arduino
#rm -rf valgrind-out.txt
mkdir arduino/bruter
mkdir arduino/bruter/src
cp src/bruter.c arduino/bruter/src/bruter.c
cp src/bruter.h arduino/bruter/src/bruter.h
cp src/std.c arduino/bruter/src/std.c
cp bruter.ino arduino/bruter/bruter.ino