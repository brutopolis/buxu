#!/bin/bash

# this is for linux, should work on macos and mingw also, though I haven't tested it
# in future i might add build scripts for other platforms

# usage function
usage() {
    echo "[=°-°=]: usage: $0 [--debug] [--debug-file] [--cc gcc] [--lib libfile.c] [-h || --help] [--extra 'extra cc tags'] [--install] [--uninstall] [--no-buxuc] [--no-shared] [--no-static]"
    exit 1
}

# origin
ORIGIN=$(pwd)

# default values
DEBUG=0
CC="gcc -Wformat=0"
MAIN="src/main.c"
NOBUXUC=0
EXTRA=""
INSTALL=0
UNINSTALL=0
NO_SHARED=0
DEBUG_FILE=""
NO_STATIC=0

# parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug) DEBUG=1; shift ;;
        --debug-file) DEBUG=1; DEBUG_FILE="$2"; shift 2 ;;
        --cc) CC="$2"; shift 2 ;;
        --lib) LIB="$2"; shift 2 ;;
        --no-buxuc) NOBUXUC=1; shift ;;
        --extra) EXTRA="$2"; shift 2 ;;
        --install) INSTALL=1; shift ;;
        --uninstall) UNINSTALL=1; shift ;;
        --no-shared) NO_SHARED=1; shift ;;
        --no-static) NO_STATIC=1; shift ;;
        --help) usage ;;
        -h) usage ;;
        *) echo "[=°~°=]: unknown option: $1"; usage ;;
    esac
done

if [[ $UNINSTALL -eq 1 ]]; then
    # verify if user has sudo
    if [[ $(sudo -n echo 2>&1 | grep -c "requires") -eq 1 ]]; then
        echo "sudo required"
        exit 1
    fi

    # remove buxu, and possibly buxuc from /usr/bin
    sudo rm -f /usr/bin/buxu
    sudo rm -f /usr/bin/buxuc

    # remove buxu.h from /usr/include
    sudo rm -f /usr/include/buxu.h

    # remove libbuxu.so from /usr/lib
    sudo rm -f /usr/lib/libbuxu.so

    # verify if buxu, buxu.h, and buxuc are removed
    if [[ -f /usr/bin/buxu ]]; then
        echo "[=°~°=]: failed to uninstall buxu"
        exit 1
    fi

    echo "uninstalled buxu from /usr/bin/buxu"
    exit 0
fi

if [[ $DEBUG -eq 1 ]]; then
    DEBUGARGS='-g'
    echo "[=°-°=]: debug mode enabled"
else
    DEBUGARGS=""
fi

echo "[=°-°=]: compiler: $CC"

rm -rf build
mkdir -p build

if [[ $NO_SHARED -eq 0 ]]; then # also build a shared library
    echo "[=°-°=]: building shared library"
    $CC src/buxu.c -o build/libbuxu.so -shared -fPIC -O3 -lm -Iinclude $DEBUGARGS $EXTRA
fi

if [[ $NO_STATIC -eq 0 ]]; then
    echo "[=°-°=]: building static library"
    $CC src/buxu.c -o build/libbuxu.a -c -O3 -lm -Iinclude $DEBUGARGS $EXTRA
fi

if [[ $NOBUXUC -eq 0 ]]; then

    BUXUC_SCRIPT="build/buxuc"

    echo '#!/bin/sh' > $BUXUC_SCRIPT
    echo '' >> $BUXUC_SCRIPT

    echo 'CC="gcc"' >> $BUXUC_SCRIPT
    echo 'EXTRA=""' >> $BUXUC_SCRIPT
    echo 'DEBUG=0' >> $BUXUC_SCRIPT
    echo 'OUTPUT_NAME=""' >> $BUXUC_SCRIPT
    echo '' >> $BUXUC_SCRIPT

    echo 'usage() {' >> $BUXUC_SCRIPT
    echo '    echo "[=°-°=]: usage: $0 [--cc gcc] [--extra \"extra cc tags\"] [--debug] [-o output.so] [--help | -h] file.c"' >> $BUXUC_SCRIPT
    echo '    exit 1' >> $BUXUC_SCRIPT
    echo '}' >> $BUXUC_SCRIPT
    echo '' >> $BUXUC_SCRIPT

    #if no args, print help
    echo 'if [[ $# -eq 0 ]]; then' >> $BUXUC_SCRIPT
    echo '    usage' >> $BUXUC_SCRIPT
    echo 'fi' >> $BUXUC_SCRIPT
    echo '' >> $BUXUC_SCRIPT

    echo 'while [[ $# -gt 0 ]]; do' >> $BUXUC_SCRIPT
    echo '    case $1 in' >> $BUXUC_SCRIPT
    echo '        --cc) CC="$2"; shift 2 ;;' >> $BUXUC_SCRIPT
    echo '        --extra) EXTRA="$2"; shift 2 ;;' >> $BUXUC_SCRIPT
    echo '        --debug) DEBUG=1; shift ;;' >> $BUXUC_SCRIPT
    echo '        -o) OUTPUT_NAME="$2"; shift 2 ;;' >> $BUXUC_SCRIPT
    echo '        --help) usage ;;' >> $BUXUC_SCRIPT
    echo '        -h) usage ;;' >> $BUXUC_SCRIPT
    echo '        *) break ;;' >> $BUXUC_SCRIPT
    echo '    esac' >> $BUXUC_SCRIPT
    echo 'done;' >> $BUXUC_SCRIPT
    echo '' >> $BUXUC_SCRIPT
    echo 'if [[ $OUTPUT_NAME == "" ]]; then' >> $BUXUC_SCRIPT
    echo 'OUTPUT_NAME=$(echo "$1" | sed "s/\.c$/.so/")' >> $BUXUC_SCRIPT
    echo 'fi' >> $BUXUC_SCRIPT
    echo '' >> $BUXUC_SCRIPT
    echo '$CC $1 -o $OUTPUT_NAME -shared -fPIC -O3 -lm -lbuxu $EXTRA $DEBUGARGS -Wl,-rpath=/usr/lib' >> $BUXUC_SCRIPT
    chmod +x $BUXUC_SCRIPT
fi

$CC $MAIN src/buxu.c -o build/buxu -O3 -lm -Iinclude $DEBUGARGS $EXTRA

if [ -n "$DEBUG_FILE" ]; then
    valgrind --tool=massif --stacks=yes --detailed-freq=1 --verbose  ./build/buxu $DEBUG_FILE
    ms_print massif.out.* > ./build/massif-out.txt
    rm -rf massif.out.*

    valgrind \
        --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --log-file=./build/valgrind-out.txt \
        --verbose ./build/buxu $DEBUG_FILE
    ls
    cd ./build
fi

echo "[=°-°=]: done building."

if [[ $INSTALL -eq 1 ]]; then
    
    if [[ ! -f ./build/buxu ]]; then
        echo "[=°~°=]: ./build/ not found"
        exit 1
    fi


    # verify if user has sudo
    if [[ $(sudo -n echo 2>&1 | grep -c "requires") -eq 1 ]]; then
        echo "[=°~°=]: sudo required"
        exit 1
    fi

    # lets remove it before installing
    if [[ -f /usr/bin/buxu ]]; then
        sudo rm -f /usr/bin/buxu
        sudo rm -f /usr/bin/buxuc
        sudo rm -f /usr/include/buxu.h
        sudo rm -f /usr/lib/libbuxu.so
    fi

    # copy buxu, and possibly buxuc to /usr/bin
    sudo cp ./build/buxu /usr/bin/buxu
    
    # copy the header files to /usr/include
    sudo cp include/buxu.h /usr/include/

    if [[ $NOBUXUC -eq 0 ]]; then
        sudo cp ./build/buxuc /usr/bin/
    fi

    if [[ $NO_SHARED -eq 0 ]]; then
        sudo cp ./build/libbuxu.so /usr/lib/
    fi

    # verify if buxu, buxu.h, and buxuc are in the correct place
    if [[ ! -f /usr/bin/buxu ]]; then
        echo "[=°x°=]: failed to install buxu, buxu not found in /usr/bin"
        exit 1
    fi

    echo "[=° °=]: installed buxu to /usr/bin/buxu"

    exit 0
fi

