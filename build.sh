# this is for linux, should work on macos and cygwin also, though I haven't tested it
# in future i might add build scripts for other platforms

rm -rf build
mkdir build
mkdir build/lib
mkdir build/ext
mkdir build/include
cp ./src/bruter.h build/include/bruter.h
cp ./src/main.c build/include/main.c
cp ./ext/*.c build/ext/
cd build

rm -rf bruter.a


for file in ./ext/*; do
    filename="${file##*/}"  
    filename="${filename%.*}" 

    sed -i "s/<extensions>/<extensions>\\nvoid init_$filename(VirtualMachine* vm);\\n/g" include/bruter.h

    for _file in include/*.c; do

        sed -i "s/<extensions>/<extensions>\\ninit_$filename(vm);\\n/g" $_file
    done
done

gcc ../src/bruter.c -c -O3 -lm

gcc ../ext/*.c -c -O3 -lm

ar rcs lib/bruter.a *.o

gcc ./include/main.c lib/bruter.a -o bruter -O3 -lm

