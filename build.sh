#!/bin/bash

# this is for linux, should work on macos and mingw also, though I haven't tested it
# in future i might add build scripts for other platforms

# usage function
usage() {
    echo "usage: $0 [--debug] [--debug-file] [--cc gcc] [--lib libfile.c] [-h || --help] [--no-bruterc]"
    exit 1
}

# origin
ORIGIN=$(pwd)

# default values
DEBUG=0
CC="gcc -Wformat=0"
MAIN="src/main.c"
NOBRUTERC=0
EXTRA=""

# parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug) DEBUG=1; shift ;;
        --debug-file) DEBUG=1; DEBUG_FILE="$2"; shift 2 ;;
        --cc) CC="$2"; shift 2 ;;
        --lib) LIB="$2"; shift 2 ;;
        --no-bruterc) NOBRUTERC=1; shift ;;
        --extra) EXTRA="$2"; shift 2 ;;
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

if [[ $NOBRUTERC -eq 0 ]]; then

    BRUTERC_SCRIPT="build/bruterc"

    echo '#!/bin/sh' > $BRUTERC_SCRIPT
    echo '' >> $BRUTERC_SCRIPT

    echo 'CC="gcc"' >> $BRUTERC_SCRIPT
    echo 'EXTRA=""' >> $BRUTERC_SCRIPT
    echo '' >> $BRUTERC_SCRIPT

    echo 'while [[ $# -gt 0 ]]; do' >> $BRUTERC_SCRIPT
    echo '    case $1 in' >> $BRUTERC_SCRIPT
    echo '        --cc) CC="$2"; shift 2 ;;' >> $BRUTERC_SCRIPT
    echo '        --extra) EXTRA="$2"; shift 2 ;;' >> $BRUTERC_SCRIPT
    # echo '        *) echo "unknown option: $1"; exit 1 ;;' >> $BRUTERC_SCRIPT
    echo '        *) break ;;' >> $BRUTERC_SCRIPT
    echo '    esac' >> $BRUTERC_SCRIPT
    echo 'done' >> $BRUTERC_SCRIPT
    echo '' >> $BRUTERC_SCRIPT
    echo 'rm -rf ./.bruter' >> $BRUTERC_SCRIPT
    echo 'mkdir -p ./.bruter' >> $BRUTERC_SCRIPT
    echo 'cd ./.bruter' >> $BRUTERC_SCRIPT
    echo '' >> $BRUTERC_SCRIPT

    echo 'base64 -d <<EOF | tar xz' >> $BRUTERC_SCRIPT
    tar cz build.sh include/* src/* | base64 -w 0 >> $BRUTERC_SCRIPT
    echo '' >> $BRUTERC_SCRIPT
    echo 'EOF' >> $BRUTERC_SCRIPT
    echo '' >> $BRUTERC_SCRIPT
    echo '# if $1 starts with / then its an absolute path' >> $BRUTERC_SCRIPT
    echo 'if [[ $1 == /* ]]; then' >> $BRUTERC_SCRIPT
    echo './build.sh --lib $1' >> $BRUTERC_SCRIPT
    echo 'else' >> $BRUTERC_SCRIPT
    echo './build.sh --lib ../$1 --extra "$EXTRA"' >> $BRUTERC_SCRIPT
    echo 'fi' >> $BRUTERC_SCRIPT
    echo '' >> $BRUTERC_SCRIPT
    echo 'cd ..' >> $BRUTERC_SCRIPT
    echo 'rm -rf ./.bruter' >> $BRUTERC_SCRIPT
    echo 'exit' >> $BRUTERC_SCRIPT

    chmod +x $BRUTERC_SCRIPT

fi

$CC $MAIN ./src/bruter.c -o build/bruter -O3 -lm -Iinclude $DEBUGARGS $EXTRA

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
