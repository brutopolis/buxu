#!/bin/bash

# this buxu building front-end is a non-essential script meant for linux,
# all filepath are unix based, the commands are also unix based
# should work well on mac, though i haven't tested it
# mingw might work with some tweaks, but i haven't tested it

# the same applies to src/cli.c and bucc, both are made with linux in mind.
# but they are only tools to build and interpret the language, so they are not really essential and better tools can be made for each platform.

# usage function
usage() {
    echo "[=°-°=]: usage: $0 [--debug] [--debug-file] [-cc || --compiler gcc] [-h || --help] [--extra 'extra cc tags'] [--install] [--install-at path] [--uninstall] [--uninstall-from] [--no-bucc] [--no-shared] [--no-static] [--update-bruter] [--branch branch]"
    exit 1
}

# origin
ORIGIN=$(pwd)

# check if sudo is available
SUDO=sudo
if [[ $(which sudo | wc -l) -eq 0 ]]; then
    SUDO=""
fi

# default values
DEBUG=0
CC="gcc -Wformat=0"
MAIN="src/cli.c"
NOBUCC=0
EXTRA=""
INSTALL=0
UNINSTALL=0
NO_SHARED=0
NO_STATIC=0
UPDATE_BRUTER=0
DEBUG_FILE=""
INSTALL_PATH="/usr" # default install path
BRANCH="main"

# parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug) DEBUG=1; shift ;;
        --debug-file) DEBUG=1; DEBUG_FILE="$2"; shift 2 ;;
        -cc) CC="$2"; shift 2 ;;
        --compiler) CC="$2"; shift 2 ;;
        --no-bucc) NOBUCC=1; shift ;;
        --extra) EXTRA="$2"; shift 2 ;;
        --install) INSTALL=1; shift ;;
        --install-at) INSTALL=1; INSTALL_PATH="$2"; shift 2 ;;
        --uninstall) UNINSTALL=1; shift ;;
        --uninstall-from) UNINSTALL=1; INSTALL_PATH="$2"; shift 2 ;;
        --no-shared) NO_SHARED=1; shift ;;
        --no-static) NO_STATIC=1; shift ;;
        --update-bruter) UPDATE_BRUTER=1; shift ;;
        --branch) BRANCH="$2"; UPDATE_BRUTER=1; shift 2 ;;
        --help) usage ;;
        -h) usage ;;
        *) echo "[=°~°=]: unknown option: $1"; usage ;;
    esac
done

# if no bruter folder is found, or if UPDATE is set to 1, clone the bruter repo
if [[ ! -d bruter || $UPDATE_BRUTER -eq 1 || ! $BRANCH == "main" ]]; then
    rm -rf ./bruter
    git clone https://github.com/jardimdanificado/bruter -b $BRANCH --depth 1
    cd bruter
    ./build.sh -cc "$CC" # we want both shared and static
    cd ..
fi

# remove / if it is the last character
if [[ ${INSTALL_PATH: -1} == "/" ]]; then
    INSTALL_PATH=${INSTALL_PATH::-1}
fi

if [[ $UNINSTALL -eq 1 ]]; then
    # verify if user has $SUDO
    if [[ $($SUDO -n echo 2>&1 | grep -c "requires") -eq 1 ]]; then
        echo "$SUDO required"
        exit 1
    fi

    # remove buxu, and possibly bucc from /usr/bin
    $SUDO rm -f $INSTALL_PATH/bin/buxu
    $SUDO rm -f $INSTALL_PATH/bin/bucc

    # remove buxu.h from /usr/include
    $SUDO rm -f $INSTALL_PATH/include/buxu.h
    $SUDO rm -f $INSTALL_PATH/include/bruter.h

    # remove libbruter.so from /usr/lib
    $SUDO rm -f $INSTALL_PATH/lib/libbruter.so

    # verify if buxu, buxu.h, and bucc are removed
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

if [[ $NOBUCC -eq 0 ]]; then

    BUCC_SCRIPT="build/bucc"

    echo '#!/bin/bash' > $BUCC_SCRIPT
    echo '' >> $BUCC_SCRIPT

    echo 'CC="gcc"' >> $BUCC_SCRIPT
    echo 'EXTRA=""' >> $BUCC_SCRIPT
    echo 'DEBUG=0' >> $BUCC_SCRIPT
    echo '' >> $BUCC_SCRIPT

    echo 'usage() {' >> $BUCC_SCRIPT
    echo '    echo "[=°-°=]: usage: $0 [--compiler || -cc gcc] [--extra \"extra cc tags\"] [--debug] [-o output] [--help | -h] file1.c file2.c ..."' >> $BUCC_SCRIPT
    echo '    exit 1' >> $BUCC_SCRIPT
    echo '}' >> $BUCC_SCRIPT
    echo '' >> $BUCC_SCRIPT

    #if no args, print help
    echo 'if [[ $# -eq 0 ]]; then' >> $BUCC_SCRIPT
    echo '    echo "[=°x°=]: no arguments provided"' >> $BUCC_SCRIPT
    echo '    usage' >> $BUCC_SCRIPT
    echo 'fi' >> $BUCC_SCRIPT
    echo '' >> $BUCC_SCRIPT
    echo 'FILES=""' >> $BUCC_SCRIPT
    echo '' >> $BUCC_SCRIPT
    echo 'while [[ $# -gt 0 ]]; do' >> $BUCC_SCRIPT
    echo '    case $1 in' >> $BUCC_SCRIPT
    echo '        --compiler) CC="$2"; shift 2 ;;' >> $BUCC_SCRIPT
    echo '        -cc) CC="$2"; shift 2 ;;' >> $BUCC_SCRIPT
    echo '        --extra) EXTRA="$2"; shift 2 ;;' >> $BUCC_SCRIPT
    echo '        --debug) DEBUG=1; shift ;;' >> $BUCC_SCRIPT
    echo '        -o) OUTPUT_NAME="$2"; shift 2 ;;' >> $BUCC_SCRIPT
    echo '        --version) buxu --version; exit 0 ;;' >> $BUCC_SCRIPT
    echo '        -v) buxu -v; exit 0 ;;' >> $BUCC_SCRIPT
    echo '        --help) usage ;;' >> $BUCC_SCRIPT
    echo '        -h) usage ;;' >> $BUCC_SCRIPT
        # concat file if its name does not start with -
    echo '        *) if [[ ${1:0:1} != "-" ]]; then' >> $BUCC_SCRIPT
    echo '            FILES="$FILES $1"' >> $BUCC_SCRIPT
    echo '        fi' >> $BUCC_SCRIPT
    echo '        shift ;;' >> $BUCC_SCRIPT
    echo '    esac' >> $BUCC_SCRIPT
    echo 'done;' >> $BUCC_SCRIPT
    echo '' >> $BUCC_SCRIPT
    
    echo 'if [[ $FILES == "" ]]; then' >> $BUCC_SCRIPT
    echo '    echo "[=°x°=]: no files provided"' >> $BUCC_SCRIPT
    echo '    exit 1' >> $BUCC_SCRIPT
    echo 'fi' >> $BUCC_SCRIPT
    echo '' >> $BUCC_SCRIPT

    echo 'if [[ $DEBUG -eq 1 ]]; then' >> $BUCC_SCRIPT
    echo '    DEBUGARGS="-g"' >> $BUCC_SCRIPT
    echo '    echo "[=°-°=]: debug mode enabled"' >> $BUCC_SCRIPT
    echo 'else' >> $BUCC_SCRIPT
    echo '    DEBUGARGS=""' >> $BUCC_SCRIPT
    echo 'fi' >> $BUCC_SCRIPT

    echo 'if [[ -z $OUTPUT_NAME ]]; then' >> $BUCC_SCRIPT
    echo '    OUTPUT_NAME="a.out"' >> $BUCC_SCRIPT
    echo 'fi' >> $BUCC_SCRIPT

    echo '' >> $BUCC_SCRIPT
    echo 'echo "[=°-°=]: $CC $FILES -o $OUTPUT_NAME -shared -fPIC -O3 -lm -lbruter $EXTRA $DEBUGARGS -Wl,-rpath=$INSTALL_PATH/lib"' >> $BUCC_SCRIPT
    echo "\$CC \$FILES -o \$OUTPUT_NAME -shared -fPIC -O3 -lm -lbruter \$EXTRA \$DEBUGARGS -Wl,-rpath=\$INSTALL_PATH/lib" >> $BUCC_SCRIPT
    echo 'echo "[=°-°=]: $OUTPUT_NAME" has been compiled.' >> $BUCC_SCRIPT
    chmod +x $BUCC_SCRIPT
fi

$CC $MAIN bruter/build/libbruter.a -o build/buxu -O3 -lm -Iinclude $DEBUGARGS $EXTRA -Ibruter/src

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
    if [[ $($SUDO -n echo 2>&1 | grep -c "requires") -eq 1 ]]; then
        echo "[=°~°=]: $SUDO required"
        exit 1
    fi

    # lets remove it before installing
    if [[ -f $INSTALL_PATH/bin/buxu ]]; then
        $SUDO rm -f $INSTALL_PATH/bin/buxu
        $SUDO rm -f $INSTALL_PATH/bin/bucc
        $SUDO rm -f $INSTALL_PATH/include/buxu.h
    fi

    # copy buxu, and possibly bucc to /usr/bin
    $SUDO cp ./build/buxu $INSTALL_PATH/bin/buxu
    
    # copy the header files to /usr/include
    $SUDO cp src/buxu.h $INSTALL_PATH/include/
    $SUDO cp bruter/src/bruter.h $INSTALL_PATH/include/

    if [[ $NOBUCC -eq 0 ]]; then
        $SUDO cp ./build/bucc $INSTALL_PATH/bin/
    fi

    if [[ $NO_SHARED -eq 0 ]]; then
        $SUDO cp ./bruter/build/libbruter.so $INSTALL_PATH/lib/
    fi

    # verify if buxu, buxu.h, and bucc are in the correct place
    if [[ ! -f $INSTALL_PATH/bin/buxu ]]; then
        echo "[=°x°=]: failed to install buxu, buxu not found in $INSTALL_PATH/bin"
        exit 1
    fi

    echo "[=°-°=]: installed buxu to $INSTALL_PATH/bin/buxu"

    exit 0
fi
