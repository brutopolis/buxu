# check out tigr https://github.com/erkkah/tigr 

WINDOWS=0
MAC=0
LIBS="-lGL -lGLU -lX11"
CC="gcc"

usage() {
    echo "[=°-°=]: usage: $0 [--win] [--mac] [--help] [-h] [--compiler || -cc compiler] [--clean]"
    exit 1
}



while [[ $# -gt 0 ]]; do
    case $1 in
        --compiler) CC="$2"; shift 2 ;;
        -cc) CC="$2"; shift 2 ;;
        --win) WINDOWS=1; shift ;;
        --mac) MAC=1; shift ;;
        --help) usage ;;
        --clean) rm -rf tigr.so tigr; exit 0 ;;
        -h) usage ;;
        *) echo "[=°~°=]: unknown option: $1"; usage ;;
    esac
done


# if tigr is not cloned, clone it
if [ ! -d "tigr" ]; then
    git clone https://github.com/erkkah/tigr
fi


if [[ $WINDOWS -eq 1 ]]; then
    echo "[=°-°=]: building for Windows"
    LIBS="-lopengl32 -lgdi32"
elif [[ $MAC -eq 1 ]]; then
    echo "[=°-°=]: building for Mac"
    LIBS="-framework OpenGL -framework Cocoa"
    cd ..
else
    echo "[=°-°=]: building for Linux"
fi

cd tigr

echo "[=°-°=]: compiler: $CC"

rm -rf ../../tigr.so 

bucc -o ../../tigr.so tigr.c ../tigr.c --extra "$LIBS" -cc "$CC"
cd ..