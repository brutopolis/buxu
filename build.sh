#!/bin/bash

# this is for linux, should work on macos and mingw also, though I haven't tested it
# in future i might add build scripts for other platforms

# usage function
usage() {
    echo "usage: $0 [--debug] [--debug-file] [--cc gcc] [--lib libfile.c] [-h || --help]"
    exit 1
}

# origin
ORIGIN=$(pwd)

# default values
DEBUG=0
CC="gcc -Wformat=0"
MAIN="src/main.c"
EXEC=""

# parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --exec) EXEC="$2"; shift 2 ;;
        --debug) DEBUG=1; shift ;;
        --debug-file) DEBUG=1; DEBUG_FILE="$2"; shift 2 ;;
        --cc) CC="$2"; shift 2 ;;
        --lib) LIB="$2"; shift 2 ;;
        --help) usage ;;
        -h) usage ;;
        *) echo "unknown option: $1"; usage ;;
    esac
done

if [[ -n $LIB ]]; then
    # replace .c with .so
    LIB2=$(echo $LIB | sed 's/\.c/\.so/g')
    $CC src/bruter.c $LIB -Iinclude -shared -o $LIB2 -O3 -lm -fPIC
    exit;
fi


if [[ $DEBUG -eq 1 ]]; then
    DEBUGARGS='-g'
    echo "debug mode enabled"
else
    DEBUGARGS=""
fi

echo "compiler: $CC"

rm -rf build
mkdir -p build

$CC $MAIN ./src/bruter.c -o build/bruter -O3 -lm -Iinclude $DEBUGARGS

if [ -n "$DEBUG_FILE" ]; then
    valgrind --tool=massif --stacks=yes --detailed-freq=1 --verbose  ./build/bruter $DEBUG_FILE
    ms_print massif.out.* > ./build/massif-out.txt
    rm -rf massif.out.*

    valgrind \
        --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --log-file=./build/valgrind-out.txt \
        --verbose ./build/bruter $DEBUG_FILE
    ls
    cd ./build
fi

cd ..

echo "done building."
