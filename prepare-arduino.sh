rm -rf arduino
mkdir arduino
#rm -rf valgrind-out.txt
mkdir arduino/bruter
mkdir arduino/bruter/src
cp include/bruter.h arduino/bruter/src/bruter.h
cp src/bruter.c arduino/bruter/src/bruter.c
cp lib/* arduino/bruter/src/
cp src/main.c arduino/bruter/bruter.ino

for file in ./lib/*.c; do
    filename="${file##*/}"  
    filename="${filename%.*}" 

    sed -i "s/<libraries header>/<libraries header>\\nvoid init_$filename(VirtualMachine* vm);/g" arduino/bruter/src/bruter.h
    sed -i "s/<libraries init>/<libraries init>\\ninit_$filename(vm);/g" arduino/bruter/bruter.ino
    sed -i "s/<libraries init>/<libraries init>\\ninit_$filename(vm);/g" arduino/bruter/src/*.c
done