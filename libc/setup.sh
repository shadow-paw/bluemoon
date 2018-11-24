#!/bin/bash -e

MAKE_J=4

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
Choose action:
1) Install newlib for i686 target
2) Install newlib for x86-64 target
A) Install everything
Q) Cancel
EOM
    read -n 1 choice
    echo
    case $choice in
    1)
        detect_host
        install_package cvs
        download_cvs
        build_newlib "i686-elf"
        ;;
    2)
        detect_host
        install_package cvs
        download_cvs
        build_newlib "x86_64-elf"
        ;;
    A|a)
        detect_host
        install_package cvs
        download_cvs
        build_newlib "i686-elf"
        build_newlib "x86_64-elf"
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
        MAKE_J=`nproc`
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
function install_package {
    local PACKAGE=$1
    case "${HOST}" in
    macosx )
        echo "[ ] brew install ${PACKAGE}"
        brew install ${PACKAGE}
        ;;
    linux )
        INSTALLED=`dpkg --get-selections | grep -P "${PACKAGE}\t" | grep -P "\tinstall" | wc -l | tr -d '[:space:]'`
        if [ "${INSTALLED}" = "0" ]; then
            echo "[ ] sudo apt-get install -y ${PACKAGE}"
            sudo apt-get install -y ${PACKAGE}
        fi
        ;;
    esac
}
function download_cvs {
    echo "[ ] cvs newlib"
    cvs -z 9 -d :pserver:anoncvs@sourceware.org:/cvs/src co newlib
}
function build_newlib {
    local TARGET=$1
    rm -rf "build-${TARGET}"
    mkdir "build-${TARGET}"
    (
      cd "build-${TARGET}"
      ../src/configure --target=${TARGET} --prefix="`pwd`/../"
      make -j${MAKE_J}
      make install
    )
    rm -rf "build-${TARGET}"
}
BASEDIR=$(cd $(dirname "$0") && pwd)
(
    cd "${BASEDIR}"
    main $@
)
