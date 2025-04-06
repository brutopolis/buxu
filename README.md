
# buxu

  buxu stands for (B)RUTER lin(UX) (U)tils, it is a set of tools used to build, run and debug bruter scripts and libraries, specifically on unix-like systems;

  buxu includes, but is not limited to:
  - buxu, a bruter interpreter and repl;
  - a stdlib to be used in bruter scripts;
  - bucc, a tool to compile bruter c files into bruter-compatible shared libraries, stants for Buxu C Compiler;
  - buxu's build script, it builds buxu and bruter, can install and uninstall them, can debug etc;

*`UNDER HEAVY DEVELOPMENT`*

# Building instructions

  buxu include its own build script;

    # clean build for the current system;
    
    ./build.sh

    # debug build;
    
    ./build.sh --debug

    # you can also debug a specific file;

    ./build.sh --debug-file path/to/buxu_file.br

    # cc option:
    # you can also define the path to a compiler;

    ./build.sh -cc path/to/compiler

# Usage
  
  "buxu" executable is the interpreter, and it also act as repl if no args are passed;
  
    # this will start in the repl;
    ./buxu 

    # run script;
    ./buxu path/to/script.br

    # print help message;
    ./buxu --help
    ./buxu -h

    # print version;
    ./buxu --version
    ./buxu -v

    # preload a library;
    ./buxu --load path/to/library.so
    ./buxu -l path/to/library.so

    # add a path to the search path;
    ./buxu --path path/to/libraries
    ./buxu -p path/to/libraries

    # eval a string;
    ./buxu --eval "print 'hello world'"
    ./buxu -e "print 'hello world'"

  "bucc" executable is the compiler script, its basically a wrapper for the system compiler(default is gcc) with some extra flags;

    # compile a c file;
    ./bucc path/to/file.c

    # compile a c bruter file with a specific output name;
    ./bucc path/to/file.c -o path/to/output.so

    # compile a c bruter file using a specific compiler;
    ./bucc path/to/file.c -cc path/to/compiler

    # compile a c bruter file with debug symbols;
    ./bucc path/to/file.c --debug

    # print help message;
    ./bucc --help
    ./bucc -h

    # print version;
    ./bucc --version
    ./bucc -v

# Install

  for a fast install, save this to a script and run it;
  
  ```
  #!/bin/bash

  # bupdate - update buxu

  # usage function
  usage() {
      echo "[=°-°=]: usage: $0 [--debug] [-cc || --compiler gcc] [-h || --help] [--extra 'extra cc tags'] [--path path] [--no-bucc] [--no-shared] [--no-static] [--repo repo] [--branch branch]"
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
  CC="gcc -Wformat=0"
  EXTRA=""
  INSTALL_PATH="/usr" # default install path
  CURRENT_DIR=$(pwd)
  BRANCH="main"
  REPO=https://github.com/jardimdanificado/buxu.git

  # parse arguments
  while [[ $# -gt 0 ]]; do
      case $1 in
          --debug) EXTRA="$EXTRA --debug"; DEBUG=1; shift ;;
          -cc) CC="$2"; shift 2 ;;
          --compiler) CC="$2"; shift 2 ;;
          --repo) REPO="$2"; shift 2 ;;
          --branch) BRANCH="$2"; shift 2 ;;
          --path) INSTALL_PATH=1; shift ;;
          --no-bucc) EXTRA="$EXTRA --no-bucc"; shift ;;
          --no-static) EXTRA="$EXTRA --no-static"; shift ;;
          --no-shared) EXTRA="$EXTRA --no-shared"; shift ;;
          --help) usage ;;
          -h) usage ;;
          *) echo "[=°~°=]: unknown option: $1"; usage ;;
      esac
  done

  if [[ -d /tmp/buxu ]]; then
      rm -rf /tmp/buxu
  fi

  git clone $REPO -b $BRANCH /tmp/buxu --depth 1

  if [[ ! -d /tmp/buxu ]]; then
      echo "[=°~°=]: error cloning branch $BRANCH from $REPO"
      echo "[=°~°=]: please check the repository url, the branch name and your internet connection"
      exit 1
  fi

  cd /tmp/buxu

  if [[ ! -f ./build.sh ]]; then
      echo "[=°~°=]: error: build.sh not found"
      echo "[=°~°=]: this is not a valid buxu repository"
      exit 1
  fi

  sudo ./build.sh --update-bruter -cc "$CC" --install-at "$INSTALL_PATH" $EXTRA 

  if [[ $? -ne 0 ]]; then
      echo "[=°~°=]: error: buxu build failed"
      exit 1
  fi

  cd $CURRENT_DIR

  if [[ $INSTALL -eq 1 ]]; then
      echo "[=°~°=]: buxu installed at $INSTALL_PATH"
  else
      echo "[=°~°=]: buxu updated at $INSTALL_PATH"
  fi

  echo "[=°~°=]: buxu build finished"

  ```

# Dynamic libraries

  dynamic libraries functions are built in the cli.c file, while unload is a thing, all dynamic libs are auto-closed at exit;

  | Function    | Description                              | Function Signature                           |
  |-------------|------------------------------------------|----------------------------------------------|
  | `load`   | load a library                           | `void function(string path);`                |
  | `unload` | unload a library                         | `void function(string path);`                |

  # BRUTER

  you might want to know how the BRUTER vm works: https://github.com/jardimdanificado/bruter
