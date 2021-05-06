#!/bin/bash

# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

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

echo "Downloading and extracting file..."
FILE_NAME="boost_${VERSION//./_}"
URL="https://sourceforge.net/projects/boost/files/boost/${VERSION}/${FILE_NAME}.tar.bz2/download"
wget "${URL}" -qO- | tar jx
if [ ! -f "${FILE_NAME}/bootstrap.sh" ]; then
    echo "Download failed or files invalid" >&2
    exit 1
fi

echo "Downloaded and extracted file"
cd "${FILE_NAME}"

if [[ "${TRAVIS_OS_NAME:-}" == "windows" ]] || [[ "${RUNNER_OS:-}" == "Windows" ]]; then
	NPROC=3
	TOOLSET="toolset=msvc"
    VARIANT="debug,release"
else
	# Linux and OSX version
	NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu)
	TOOLSET=
    VARIANT="release"
fi

libraries=$(join , "${required_libs[@]}")

echo "Bootstrapping B2..."
if ! ./bootstrap.sh --with-libraries="$libraries" threading=multi > "$BUILD_LOG"; then
    cat "$BUILD_LOG" || true
    cat bootstrap.log
    exit 1
fi
echo "Building ${libraries}, variants ${VARIANT} to ${INSTALL_DIR} using ${TOOLSET}"
if ! ./b2 link="${link}" ${TOOLSET} variant="${VARIANT}" --prefix="${INSTALL_DIR}" -j${NPROC} install > "$BUILD_LOG"; then
    cat "$BUILD_LOG"
    exit 1
fi
