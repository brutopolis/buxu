# this is for linux, should work on macos cygwin also, though I haven't tested it
# in future i might add build scripts for other platforms

rm -rf build
mkdir build
mkdir build/lib
mkdir build/include

cd build

rm -rf bruter.a

gcc ../src/bruter.c\
    ../src/std.c\
    ../src/std_os.c\
    ../src/std_linux.c -c -O3 -lm

ar rcs lib/bruter.a bruter.o std.o std_os.o std_linux.o

gcc ../src/main.c lib/bruter.a -o bruter -O3 -lm
cp ../src/bruter.h include/bruter.h
rm -rf *.o