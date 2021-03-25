#!/bin/bash

set -euo pipefail

VERSION="${1:?Missing Boost version}"
INSTALL_DIR="${2:?Missing install dir}"
required_libs="${3:?Missing libs}"
link="${4:-static}"

required_libs=( ${required_libs//,/ } )

function all_libs_exist() {
    for lib in "${required_libs[@]}"; do
        if ! ls "${INSTALL_DIR}/lib/"*"${lib}"*.* &> /dev/null; then
            echo "Missing Boost.${lib}. Building boost..."
			echo "Checked '${INSTALL_DIR}/lib/*${lib}*.*' but found only:"
			ls "${INSTALL_DIR}/lib/" 2> /dev/null || true
            return 1
        fi
    done
    return 0
}

function join {
    local IFS="$1"
    shift
    echo "$*"
}

if all_libs_exist; then
    exit 0
fi

BUILD_DIR=/tmp/boost
BUILD_LOG=/tmp/boost.log

mkdir -p "${BUILD_DIR}" && cd "${BUILD_DIR}"

FILE_NAME="boost_${VERSION//./_}"
URL="https://sourceforge.net/projects/boost/files/boost/${VERSION}/${FILE_NAME}.tar.bz2/download"
wget "${URL}" -qO- | tar jx
if [ ! -f "${FILE_NAME}/bootstrap.sh" ]; then
    echo "Download failed or files invalid" >&2
    exit 1
fi

cd "${FILE_NAME}"

if [[ "${TRAVIS_OS_NAME:-}" == "windows" ]] || [[ "${RUNNER_OS:-}" == "Windows" ]]; then
	TOOLSET="toolset=msvc"
	NPROC=2
else
	# Linux and OSX version
	NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu)
	TOOLSET=
fi

libraries=$(join , "${required_libs[@]}")

if ! ./bootstrap.sh --with-libraries="$libraries" threading=multi > "$BUILD_LOG"; then
    cat "$BUILD_LOG" || true
    cat bootstrap.log
    exit 1
fi
if ! ./b2 link="${link}" ${TOOLSET} variant=release --prefix="${INSTALL_DIR}" -j${NPROC} install > "$BUILD_LOG"; then
    cat "$BUILD_LOG"
    exit 1
fi
