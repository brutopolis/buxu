# this is for linux, should work on macos and cygwin also, though I haven't tested it
# in future i might add build scripts for other platforms

rm -rf build
mkdir build

mkdir build/lib
mkdir build/include
mkdir build/example
cp ./src/main.c build/include/main.c
cp ./lib/* build/lib/ -r
cp ./include/* build/include/ -r
cp ./example/* build/example/ -r
cd build

rm -rf bruter.a


for file in ./lib/*.c; do
    filename="${file##*/}"  
    filename="${filename%.*}" 

    sed -i "s/<libraries header>/<libraries header>\\nvoid init_$filename(VirtualMachine* vm);/g" include/bruter.h
    sed -i "s/<libraries init>/<libraries init>\\ninit_$filename(vm);/g" include/*.c 
    sed -i "s/<libraries init>/<libraries init>\\ninit_$filename(vm);/g" lib/*.c
done

gcc ../src/bruter.c -c -O3 -lm -I./include -g

gcc ./lib/*.c -c -O3 -lm -I./include -g

ar rcs lib/bruter.a *.o

rm -rf lib/*.c

gcc ./include/main.c lib/* -o bruter -O3 -lm -g

rm -rf *.o
rm -rf lib/*.c
rm -rf include/*.c
rm -rf lib/*.o

valgrind --tool=massif --stacks=yes --detailed-freq=1 --verbose  ./bruter ../example/list.br 
ms_print massif.out.* > massif-out.txt
rm -rf massif.out.*

valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --log-file=valgrind-out.txt \
    --verbose ./bruter ../example/list.br

#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=valgrind-out.txt --verbose build/bruter example/threads.br