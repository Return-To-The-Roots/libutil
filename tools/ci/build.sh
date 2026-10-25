#!/bin/bash

# Copyright (C) 2005 - 2026 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

set -euo pipefail

function print_on_gha {
    [[ "${GITHUB_ACTIONS:-false}" != "true" ]] || echo "$@"
}
function startGroup {
	print_on_gha "::group::$1"
}
function endGroup {
	print_on_gha "::endgroup::"
}

# Required by the tests
export USER="${USER:-TestUser}"

BUILD_TYPE="${1}"
CMAKE_FLAGS="${2}"
TARGET="${3:-install}"

[[ -d build ]] || mkdir build
cd build

if [[ -z ${GENERATOR:-} ]]; then
    if [[ "${RUNNER_OS:-}" == "Linux" ]] || [[ "${RUNNER_OS:-}" == "macOS" ]]; then
        GENERATOR="Unix Makefiles"
    else
      echo 'Missing $GENERATOR!'
      exit 1
    fi
fi
if [[ $GENERATOR == "Visual Studio"* ]]; then
    BUILD_FLAGS=""
else
    BUILD_FLAGS="-j3"
fi

cmake --version

set -x
# shellcheck disable=SC2086
cmake \
    -G "$GENERATOR" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DRTTR_ENABLE_WERROR=ON \
    -DCMAKE_INSTALL_PREFIX="$PWD/installed" \
    ${CMAKE_FLAGS} \
    ..

cmake --build . --target "$TARGET" --config "${BUILD_TYPE}" -- $BUILD_FLAGS

# Execute tests
export RTTR_DISABLE_ASSERT_BREAKPOINT=1
export BOOST_TEST_LOG_LEVEL=message
export BOOST_TEST_REPORT_LEVEL=detailed
if ! ctest --output-on-failure -C "${BUILD_TYPE}" -j2; then
    startGroup "CMake cache"
    cat CMakeCache.txt
    endGroup
    echo "LD:${LD_LIBRARY_PATH:-}"
    echo "DYLD:${DYLD_LIBRARY_PATH:-}"
    echo "PATH:$PATH"
    startGroup "Rerun failed tests only"
    ctest --output-on-failure -C "${BUILD_TYPE}" --rerun-failed --extra-verbose
    endGroup
fi
