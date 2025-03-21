#!/bin/bash

# this buxu building front-end is a non-essential script meant for linux,
# all filepath are unix based, the commands are also unix based
# should work well on mac, though i haven't tested it, and have no plans to do so.
# mingw might work with some tweaks, but i haven't tested it, might do soon or later.

# the same applies to src/cli.c and buxuc, both are made with linux in mind.
# but they are only tools to build and interpret the language, so they are not really essential and better tools can be made for each platform.

# libbuxu, it is, buxu.c and buxu.h are platform independent, and should(and need to) work on any platform that has a C compiler.
# libbuxu use no other libraries beside the standard C library
# if libbuxu is does not work on a platform, it is a bug, and should be reported.

# usage function
usage() {
    echo "[=°-°=]: usage: $0 [--debug] [--debug-file] [--cc gcc] [--lib libfile.c] [-h || --help] [--extra 'extra cc tags'] [--install] [--install-at path] [--uninstall] [--uninstall-from] [--no-buxuc] [--no-shared] [--no-static]"
    exit 1
}

# origin
ORIGIN=$(pwd)

# default values
DEBUG=0
CC="gcc -Wformat=0"
MAIN="src/cli.c"
NOBUXUC=0
EXTRA=""
INSTALL=0
UNINSTALL=0
NO_SHARED=0
NO_STATIC=0
DEBUG_FILE=""
INSTALL_PATH="/usr" # default install path

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
        --install-at) INSTALL=1; INSTALL_PATH="$2"; shift 2 ;;
        --uninstall) UNINSTALL=1; shift ;;
        --uninstall-from) UNINSTALL=1; INSTALL_PATH="$2"; shift 2 ;;
        --no-shared) NO_SHARED=1; shift ;;
        --no-static) NO_STATIC=1; shift ;;
        --help) usage ;;
        -h) usage ;;
        *) echo "[=°~°=]: unknown option: $1"; usage ;;
    esac
done

# remove / if it is the last character
if [[ ${INSTALL_PATH: -1} == "/" ]]; then
    INSTALL_PATH=${INSTALL_PATH::-1}
fi

if [[ $UNINSTALL -eq 1 ]]; then
    # verify if user has $SUDO
    if [[ $(-n echo 2>&1 | grep -c "requires") -eq 1 ]]; then
        echo "required"
        exit 1
    fi

    # remove buxu, and possibly buxuc from /usr/bin
    rm -f $INSTALL_PATH/bin/buxu
    rm -f $INSTALL_PATH/bin/buxuc

    # remove buxu.h from /usr/include
    rm -f $INSTALL_PATH/include/buxu.h

    # remove libbuxu.so from /usr/lib
    rm -f $INSTALL_PATH/lib/libbuxu.so

    # verify if buxu, buxu.h, and buxuc are removed
    if [[ -f $INSTALL_PATH/bin/buxu ]]; then
        echo "[=°~°=]: failed to uninstall buxu"
        exit 1
    fi

    echo "uninstalled buxu from $INSTALL_PATH/bin/buxu"
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
    echo '' >> $BUXUC_SCRIPT

    echo 'usage() {' >> $BUXUC_SCRIPT
    echo '    echo "[=°-°=]: usage: $0 [--cc gcc] [--extra \"extra cc tags\"] [--debug] [-o output] [--help | -h] file1.c file2.c ..."' >> $BUXUC_SCRIPT
    echo '    exit 1' >> $BUXUC_SCRIPT
    echo '}' >> $BUXUC_SCRIPT
    echo '' >> $BUXUC_SCRIPT

    #if no args, print help
    echo 'if [[ $# -eq 0 ]]; then' >> $BUXUC_SCRIPT
    echo '    echo "[=°x°=]: no arguments provided"' >> $BUXUC_SCRIPT
    echo '    usage' >> $BUXUC_SCRIPT
    echo 'fi' >> $BUXUC_SCRIPT
    echo '' >> $BUXUC_SCRIPT
    echo 'FILES=""' >> $BUXUC_SCRIPT
    echo '' >> $BUXUC_SCRIPT
    echo 'while [[ $# -gt 0 ]]; do' >> $BUXUC_SCRIPT
    echo '    case $1 in' >> $BUXUC_SCRIPT
    echo '        --cc) CC="$2"; shift 2 ;;' >> $BUXUC_SCRIPT
    echo '        --extra) EXTRA="$2"; shift 2 ;;' >> $BUXUC_SCRIPT
    echo '        --debug) DEBUG=1; shift ;;' >> $BUXUC_SCRIPT
    echo '        -o) OUTPUT_NAME="$2"; shift 2 ;;' >> $BUXUC_SCRIPT
    echo '        --help) usage ;;' >> $BUXUC_SCRIPT
    echo '        -h) usage ;;' >> $BUXUC_SCRIPT
        # concat file if its name does not start with -
    echo '        *) if [[ ${1:0:1} != "-" ]]; then' >> $BUXUC_SCRIPT
    echo '            FILES="$FILES $1"' >> $BUXUC_SCRIPT
    echo '        fi' >> $BUXUC_SCRIPT
    echo '        shift ;;' >> $BUXUC_SCRIPT
    echo '    esac' >> $BUXUC_SCRIPT
    echo 'done;' >> $BUXUC_SCRIPT
    echo '' >> $BUXUC_SCRIPT
    
    echo 'if [[ $FILES == "" ]]; then' >> $BUXUC_SCRIPT
    echo '    echo "[=°x°=]: no files provided"' >> $BUXUC_SCRIPT
    echo '    exit 1' >> $BUXUC_SCRIPT
    echo 'fi' >> $BUXUC_SCRIPT
    echo '' >> $BUXUC_SCRIPT

    echo 'if [[ $DEBUG -eq 1 ]]; then' >> $BUXUC_SCRIPT
    echo '    DEBUGARGS="-g"' >> $BUXUC_SCRIPT
    echo '    echo "[=°-°=]: debug mode enabled"' >> $BUXUC_SCRIPT
    echo 'else' >> $BUXUC_SCRIPT
    echo '    DEBUGARGS=""' >> $BUXUC_SCRIPT
    echo 'fi' >> $BUXUC_SCRIPT

    echo 'if [[ -z $OUTPUT_NAME ]]; then' >> $BUXUC_SCRIPT
    echo '    OUTPUT_NAME="a.out"' >> $BUXUC_SCRIPT
    echo 'fi' >> $BUXUC_SCRIPT

    echo '' >> $BUXUC_SCRIPT
    echo 'echo "[=°-°=]: $CC $FILES -o $OUTPUT_NAME -shared -fPIC -O3 -lm -lbuxu $EXTRA $DEBUGARGS -Wl,-rpath=$INSTALL_PATH/lib"' >> $BUXUC_SCRIPT
    echo "\$CC \$FILES -o \$OUTPUT_NAME -shared -fPIC -O3 -lm -lbuxu \$EXTRA \$DEBUGARGS -Wl,-rpath=\$INSTALL_PATH/lib" >> $BUXUC_SCRIPT
    echo 'echo "[=°-°=]: $OUTPUT_NAME" has been compiled.' >> $BUXUC_SCRIPT
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
    echo "[=°-°=]: installing buxu at $INSTALL_PATH"
    # if install path does not exist, create it(if install path first character is not /)
    if [[ ! -d $INSTALL_PATH ]]; then
        if [[ ${INSTALL_PATH:0:1} != "/" ]]; then
            mkdir -p $INSTALL_PATH
            mkdir -p $INSTALL_PATH/bin
            mkdir -p $INSTALL_PATH/include
            mkdir -p $INSTALL_PATH/lib
        else
            echo "[=°~°=]: $INSTALL_PATH does not exist, and will not be created because its a absolute path"
            exit 1
        fi
    fi

    if [[ ! -f ./build/buxu ]]; then
        echo "[=°~°=]: ./build/ not found, cant install buxu"
        exit 1
    fi


    # verify if user has $SUDO
    if [[ $(-n echo 2>&1 | grep -c "requires") -eq 1 ]]; then
        echo "[=°~°=]: required"
        exit 1
    fi


    # copy buxu, and possibly buxuc to /usr/bin
    cp ./build/buxu $INSTALL_PATH/bin/buxu
    
    # copy the header files to /usr/include
    cp include/buxu.h $INSTALL_PATH/include/

    if [[ $NOBUXUC -eq 0 ]]; then
        cp ./build/buxuc $INSTALL_PATH/bin/
    fi

    if [[ $NO_SHARED -eq 0 ]]; then
        cp build/libbuxu.so $INSTALL_PATH/lib/
    fi

    # verify if buxu is in the correct place
    if [[ ! -f $INSTALL_PATH/bin/buxu ]]; then
        echo "[=°x°=]: failed to install buxu, buxu not found in $INSTALL_PATH/bin"
        exit 1
    fi

    echo "[=°-°=]: installed buxu to $INSTALL_PATH/bin/buxu"

    exit 0
fi

