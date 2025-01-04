# this is for linux, should work on macos and cygwin also, though I haven't tested it
# in future i might add build scripts for other platforms

rm -rf build
mkdir build
mkdir build/lib
mkdir build/include
mkdir build/example
cp ./src build/src -r
cp ./lib/* build/lib/ -r
cp ./include/* build/include/ -r
cp ./example/* build/example/ -r
cd build

rm -rf bruter.a



if [ -n "$DEBUG" ]; then
    FILE=$1

    if [ -z "$FILE" ]; then
        FILE="" # repl
    fi
    DEBUGARGS='-g'
fi

if [ -z "$CC" ]; then 
    CC="gcc -Wformat=0" # -Wformat=0 is to disable format string warnings related to Int and Float
fi

if [ -n "$INO" ]; then
    rm -rf arduino
    mkdir arduino

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
fi

echo "using $CC as compiler"

if [ -n "$EXCLUDE" ]; then # EXCLUDE="filename.c" ./build.sh
    cd lib
    rm $EXCLUDE
    echo "excluded libs: $EXCLUDE"
    cd ..
fi

#if all libs are excluded we don't need to do anything but build the main file
if [ -z "$(ls -A lib)" ]; then
    echo "no libraries to build"
    echo "building bruter"
    $CC ./src/main.c ./src/bruter.c -o bruter -O3 -lm -I./include $ARGS
else
    for file in ./lib/*.c; do
        filename="${file##*/}"  
        filename="${filename%.*}" 
        echo "building $filename"
        sed -i "s/<libraries header>/<libraries header>\\nvoid init_$filename(VirtualMachine* vm);/g" include/bruter.h
        sed -i "s/<libraries init>/<libraries init>\\ninit_$filename(vm);/g" src/*.c 
        sed -i "s/<libraries init>/<libraries init>\\ninit_$filename(vm);/g" lib/*.c
    done
fi

if [ -n "$EMCC" ]; then
    mkdir web
    cd web
    cp ../example/example.html ./index.html
    echo "building bruter-web"
    $EMCC ../src/wasm_wrapper.c \
        ../src/bruter.c \
        ../lib/* \
        -o bruter.js \
        -s EXPORT_NAME="'_Bruter'" \
        -sEXPORTED_FUNCTIONS='["_wasm_new_vm", "_wasm_destroy_vm", "_wasm_eval", "_malloc", "_free"]' \
        -sEXPORTED_RUNTIME_METHODS='["UTF8ToString", "stringToUTF8", "lengthBytesUTF8"]' \
        -sNO_EXIT_RUNTIME=1 \
        -sMODULARIZE=1 \
        -O3 \
        -lm \
        -Wformat=0 \
        -I../include $EMARGS $DEBUGARGS
    
    cat ../src/bruter.js >> bruter.js
    cd ..
fi

# experimental wasi support, working, but not tested, you might not want to use this
if [ -n "$WASICC" ]; then
    echo "building bruter-wasi"
    $WASICC -o bruter.wasm ./src/main.c ./src/bruter.c ./lib/*.c -O3 -lm -I./include $WASIARGS
    echo 'wasmtime --dir=. bruter.wasm $@' > run_bruter.sh && chmod +x run_bruter.sh
fi

if [ -n "$(ls -A lib)" ]; then
    echo "building bruter"
    $CC ./src/bruter.c -c -O3 -lm -I./include $DEBUGARGS
    $CC ./lib/*.c -c -O3 -lm -I./include $DEBUGARGS
    ar rcs lib/bruter.a *.o
    rm -rf lib/*.c lib/*.o
    $CC ./src/main.c lib/*.a -o bruter -O3 -lm $ARGS $DEBUGARGS
fi

rm -rf *.o
rm -rf lib/*.c
rm -rf lib/*.o
rm -rf src

if [ -n "$DEBUG" ]; then
    valgrind --tool=massif --stacks=yes --detailed-freq=1 --verbose  ./bruter $FILE
    ms_print massif.out.* > massif-out.txt
    rm -rf massif.out.*

    valgrind \
        --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --log-file=valgrind-out.txt \
        --verbose ./bruter $FILE
fi