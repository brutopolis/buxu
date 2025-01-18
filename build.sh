#!/bin/bash

# this is for linux, should work on macos and mingw also, though I haven't tested it
# in future i might add build scripts for other platforms

# usage function
usage() {
    echo "usage: $0 [--outpath build] [--debug] [--cc gcc] [--ino] [--emcc emcc] [--wasicc wasicc] [--exclude libfile] [--exec main.br]"
    exit 1
}

# origin
ORIGIN=$(pwd)

# default values
OUTPATH="build"
DEBUG=0
CC="gcc -Wformat=0"
INO=0
EMCC=""
WASICC=""
EXCLUDE=""
MAIN="src/main.c"
EXEC=""

rm -rf build_tmp build
mkdir build_tmp
cp -r * build_tmp/
cd build_tmp

# parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --exec) EXEC="$2"; shift 2 ;;
        --outpath) OUTPATH="$2"; shift 2 ;;
        --debug) DEBUG=1; shift ;;
        --cc) CC="$2"; shift 2 ;;
        --ino) INO=1; shift ;;
        --emcc) EMCC="$2"; shift 2 ;;
        --wasicc) WASICC="$2"; shift 2 ;;
        --exclude) EXCLUDE="$2"; shift 2 ;;
        *) echo "unknown option: $1"; usage ;;
    esac
done

rm -rf "$OUTPATH" build
mkdir -p "$OUTPATH/lib" "$OUTPATH/include" "$OUTPATH/example"
cp -r ./src "$OUTPATH/src"
cp -r ./lib/* "$OUTPATH/lib/"
cp -r ./include/* "$OUTPATH/include/"
cp -r ./example/* "$OUTPATH/example/"

# verify if a folder called packages exists, if so, copy the contents of each folder inside packages like: packages/name/lib/* to build/lib/, packages/name/include/* to build/include/, packages/name/src/* to build/src/
copy_packages() {
    for folder in ./packages/*; do
        if [[ -d "$folder" ]]; then
            cp -r "$folder/lib"/* "$OUTPATH/lib/"
            cp -r "$folder/include"/* "$OUTPATH/include/"
            cp -r "$folder/src"/* "$OUTPATH/src/"
        fi
    done
}

if [[ -d ./packages ]]; then
    copy_packages
fi

cd $ORIGIN/build_tmp || exit 1

cd "$OUTPATH" || exit 1

rm -rf libbruter.a

if [[ $DEBUG -eq 1 ]]; then
    DEBUGARGS='-g'
    echo "debug mode enabled"
else
    DEBUGARGS=""
fi

echo "compiler: $CC"

if [[ $INO -eq 1 ]]; then
    echo "preparing arduino files..."
    rm -rf arduino
    mkdir -p arduino/bruter/src
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

if [[ -n $EXCLUDE ]]; then
    echo "deleting: $EXCLUDE"
    cd lib || exit 1
    rm -f "$EXCLUDE"
    cd ..
fi

if [[ -n $EXEC ]]; then
    MAIN="./src/main_exec.c"
    cp "../$EXEC" ./src/main.br
    xxd -i src/main.br > include/entrypoint.h
fi

if [[ -z "$(ls -A lib)" ]]; then
    echo "no libs to compile, building main..."
    $CC $MAIN ./src/bruter.c -o bruter -O3 -lm -I./include $DEBUGARGS
else
    for file in ./lib/*.c; do
        filename="${file##*/}"  
        filename="${filename%.*}" 
        echo "compiling $filename"
        sed -i "s/<libraries header>/<libraries header>\\nvoid init_$filename(VirtualMachine* vm);/g" include/bruter.h
        sed -i "s/<libraries init>/<libraries init>\\ninit_$filename(vm);/g" src/*.c 
        sed -i "s/<libraries init>/<libraries init>\\ninit_$filename(vm);/g" lib/*.c
    done
fi

if [[ -n $EMCC ]]; then
    echo "compiling to webassembly using $EMCC..."
    mkdir -p web
    cd web || exit 1
    cp ../src/index.html ./index.html
    $EMCC ../src/bruter.c \
        ../lib/*.c \
        -o bruter.js \
        -s EXPORT_NAME="'_Bruter'" \
        -sEXPORTED_FUNCTIONS='["_wasm_new_vm", "_wasm_destroy_vm", "_wasm_eval", "_malloc", "_free"]' \
        -sEXPORTED_RUNTIME_METHODS='["UTF8ToString", "stringToUTF8", "lengthBytesUTF8"]' \
        -sNO_EXIT_RUNTIME=1 \
        -sMODULARIZE=1 \
        -O3 \
        -lm \
        -sALLOW_MEMORY_GROWTH=1 \
        -L../lib \
        -I../include $DEBUGARGS
    cd ..
fi

if [[ -n $WASICC ]]; then
    echo "compile to wasi using $WASICC..."
    $WASICC -o bruter.wasm $MAIN ./src/bruter.c ./lib/*.c -O3 -lm -I./include
    echo 'wasmtime --dir=. bruter.wasm $@' > run_bruter.sh && chmod +x run_bruter.sh
fi

if [[ -n "$(ls -A lib)" ]]; then
    $CC ./src/bruter.c -c -O3 -lm -I./include $DEBUGARGS
    $CC ./lib/*.c -c -O3 -lm -I./include $DEBUGARGS
    ar rcs lib/libbruter.a *.o
    $CC $MAIN lib/*.a -o bruter -O3 -lm $DEBUGARGS
fi


rm -rf *.o lib/*.o src

cd ../..
mv build_tmp/build ./build
rm -rf build_tmp

echo "done building."
