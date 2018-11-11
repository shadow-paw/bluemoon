#!/bin/bash -e

INSTALL_PATH="$HOME/.local/cross"
MAKE_J=4

BINUTILS_URL=http://ftp.gnu.org/gnu/binutils/binutils-2.31.tar.bz2
GCC_URL=http://ftp.gnu.org/gnu/gcc/gcc-8.2.0/gcc-8.2.0.tar.xz
GDB_URL=http://ftp.gnu.org/gnu/gdb/gdb-8.2.tar.xz


function main {
    cat << EOM
`tput setaf 2`_____ _
| __  | |_ _ ___ _____ ___ ___ ___
| __ -| | | | -_|     | . | . |   |
|_____|_|___|___|_|_|_|___|___|_|_|
`tput setaf 7``tput dim`
github.com/shadow-paw/bluemoon
#osdev
`tput sgr0`
Choose target architecture:
1) Install toolchain for i686 target
2) Install toolchain for x86-64 target
3) Install toolchain for ARM target
4) Install toolchain for ARM64 target
A) Install toolchain for all targets
Q) Cancel
EOM
    read -n 1 choice
    echo
    case $choice in
    1)
        detect_host
        install_mtools
        install_nasm
        install_binutils "i686-elf"
        install_gcc      "i686-elf"
        install_gdb      "i686-elf"
        install_qemu
        post_install
        ;;
    2)
        detect_host
        install_mtools
        install_nasm
        install_binutils "x86_64-elf"
        install_gcc      "x86_64-elf"
        install_gdb      "x86_64-elf"
        install_qemu
        post_install
        ;;
    3)
        detect_host
        install_mtools
        install_binutils "arm-none-eabi"
        install_gcc      "arm-none-eabi"
        install_gdb      "arm-none-eabi"
        install_qemu
        post_install
        ;;
    4)
        detect_host
        install_mtools
        install_binutils "aarch64-none-elf"
        install_gcc      "aarch64-none-elf"
        install_gdb      "aarch64-none-elf"
        install_qemu
        post_install
        ;;
    A|a)
        detect_host
        install_mtools
        install_nasm
        install_binutils "i686-elf"
        install_gcc      "i686-elf"
        install_gdb      "i686-elf"
        install_binutils "x86_64-elf"
        install_gcc      "x86_64-elf"
        install_gdb      "x86_64-elf"
        install_binutils "arm-none-eabi"
        install_gcc      "arm-none-eabi"
        install_gdb      "arm-none-eabi"
        install_binutils "aarch64-none-elf"
        install_gcc      "aarch64-none-elf"
        install_gdb      "aarch64-none-elf"
        install_qemu
        post_install
        ;;
    esac
}

function detect_host {
    case $(uname -a) in
    *raspberrypi* )
        HOST=linux
        ;;
    *armv7* )
        HOST=android
        ;;
    *BSD* )
        HOST=bsd
        ;;
    *Linux* )
        HOST=linux
        ;;
    *Darwin\ Kernel* )
        HOST=macosx
        ;;
    *SunOS* )
        HOST=solaris
        ;;
    *)
        echo "Error: Unable to detect host platform."
        exit
        ;;
    esac
}

function install_nasm {
    case "${HOST}" in
    macosx )
        echo "[ ] brew install nasm"
        brew install nasm
        ;;
    linux )
        echo "[ ] sudo apt-get install -y nasm"
        sudo apt-get install -y nasm
        ;;
    esac
}

function install_binutils {
    local TARGET=$1
    local BINUTILS_FILE=${BINUTILS_URL##*/}
    local BINUTILS_PATH=$(basename ${BINUTILS_FILE} .tar.bz2)
    if [ ! -d "${BINUTILS_PATH}" ]; then
        if [ ! -f "${BINUTILS_FILE}" ]; then
            echo "[I] Download ${BINUTILS_URL}..."
            curl "${BINUTILS_URL}" -o "${BINUTILS_FILE}"
        fi
        echo "[I] Extracting ${BINUTILS_FILE}..."
        tar xf "${BINUTILS_FILE}"
    fi
    rm -rf "build-binutils-${TARGET}"
    mkdir -p "build-binutils-${TARGET}"
    (
        cd "build-binutils-${TARGET}"
        echo "[I] Building binutils for ${TARGET}..."
        ../${BINUTILS_PATH}/configure --prefix="${INSTALL_PATH}" \
            --target="${TARGET}" \
            --with-sysroot \
            --enable-interwork --enable-multilib \
            --disable-nls --disable-werror
        make -j${MAKE_J}
        echo "[I] Install binutils for ${TARGET}..."
        make install
        echo "[I] Successfully installed binutils for ${TARGET}..."
    )
    rm -rf "build-binutils-${TARGET}"
}

function install_gcc {
    local TARGET=$1
    local GCC_FILE=${GCC_URL##*/}
    local GCC_PATH=$(basename ${GCC_FILE} .tar.xz)

    case "${HOST}" in
    macosx )
        echo "[ ] brew install gmp mpfr libmpc"
        brew install gmp mpfr libmpc
        ;;
    linux )
        echo "[ ] sudo apt-get install -y libgmp3-dev libmpfr-dev libmpc-dev zlib1g-dev"
        sudo apt-get install -y libgmp3-dev libmpfr-dev libmpc-dev zlib1g-dev
        ;;
    esac

    if [ ! -d "${GCC_PATH}" ]; then
        if [ ! -f "${GCC_FILE}" ]; then
            echo "[I] Download ${GCC_URL}..."
            curl "${GCC_URL}" -o "${GCC_FILE}"
        fi
        echo "[I] Extracting ${GCC_FILE}..."
        tar xf "${GCC_FILE}"
    fi
    rm -rf "build-gcc-${TARGET}"
    mkdir -p "build-gcc-${TARGET}"
    (
        cd "build-gcc-${TARGET}"
        echo "[I] Building gcc for ${TARGET}..."
        ../${GCC_PATH}/configure --prefix="${INSTALL_PATH}" \
            --target="${TARGET}" \
            --enable-language=c,c++ --without-headers \
            --enable-interwork --enable-multilib \
            --disable-bootstrap --disable-libmpx \
            --disable-nls \
            --with-system-zlib
        make -j${MAKE_J} all-gcc
        make -j${MAKE_J} all-target-libgcc
        echo "[I] Install gcc for ${TARGET}..."
        make install-gcc
        make install-target-libgcc
        echo "[I] Successfully installed gcc for ${TARGET}..."
    )
    rm -rf "build-gcc-${TARGET}"
}

function install_gdb {
    local TARGET=$1
    local GDB_FILE=${GDB_URL##*/}
    local GDB_PATH=$(basename ${GDB_FILE} .tar.xz)

    case "${HOST}" in
    macosx )
        echo "[ ] brew install texinfo"
        brew install texinfo
        ;;
    linux )
        echo "[ ] sudo apt-get install -y texinfo"
        sudo apt-get install -y texinfo
        ;;
    esac

    if [ ! -d "${GDB_PATH}" ]; then
        if [ ! -f "${GDB_FILE}" ]; then
            echo "[I] Download ${GDB_URL}..."
            curl "${GDB_URL}" -o "${GDB_FILE}"
        fi
        echo "[I] Extracting ${GDB_FILE}..."
        tar xf "${GDB_FILE}"
    fi
    rm -rf "build-gdb-${TARGET}"
    mkdir -p "build-gdb-${TARGET}"
    (
        cd "build-gdb-${TARGET}"
        echo "[I] Building gdb for ${TARGET}..."
        ../${GDB_PATH}/configure --prefix="${INSTALL_PATH}" \
            --target="${TARGET}" \
            --program-prefix="${TARGET}-" \
            --with-gmp=/usr/local \
            --with-libelf=/usr/local \
            --with-build-libsubdir=/usr/local \
            CFLAGS="-I/usr/local/include"
        make -j${MAKE_J}
        echo "[I] Install gdb for ${TARGET}..."
        make install
        echo "[I] Successfully installed gdb for ${TARGET}..."
    )
    rm -rf "build-gdb-${TARGET}"
}

function install_mtools {
    case "${HOST}" in
    macosx )
        echo "[ ] brew install mtools"
        brew install qemu
        ;;
    linux )
        echo "[ ] sudo apt-get install -y mtools"
        sudo apt-get install -y mtools
        ;;
    esac
}

function install_qemu {
    case "${HOST}" in
    macosx )
        echo "[ ] brew install qemu"
        brew install qemu
        ;;
    linux )
        echo "[ ] sudo apt-get install -y qemu"
        sudo apt-get install -y qemu
        ;;
    esac
}

function post_install {
    cat << EOM
Installation completed!

To use the toolchain you need to setup the following:

echo 'export PATH="\$PATH:${INSTALL_PATH}/bin"' >> ~/.bash_profile
EOM
}

BASEDIR=$(cd $(dirname "$0") && pwd)
(
    cd "${BASEDIR}"
    main $@
)
