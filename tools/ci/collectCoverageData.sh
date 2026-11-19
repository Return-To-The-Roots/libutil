#!/bin/bash

# Copyright (C) 2005 - 2026 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

set -euo pipefail

EXCLUSIONS=("$@")

# Install lcov if required
LCOV_VERSION="2.3"
if which lcov &> /dev/null; then
  installed_lcov=$(lcov --version 2> /dev/null || true)
else
  installed_lcov="None"
fi
echo "Installed lcov: $installed_lcov"

if [[ ! "$installed_lcov" =~ "version ${LCOV_VERSION}" ]]; then
    echo "Downloading Lcov $LCOV_VERSION"
    LCOV_ROOT="/tmp/lcov"
    LCOV_URL="https://github.com/linux-test-project/lcov/releases/download/v${LCOV_VERSION}/lcov-${LCOV_VERSION}.tar.gz"
    mkdir -p "${LCOV_ROOT}"
    wget --quiet -O - "${LCOV_URL}" | tar --strip-components=1 -xz -C "${LCOV_ROOT}"
    export PATH="${LCOV_ROOT}/bin:${PATH}"
    if [[ -n ${CI:-} ]]; then
       echo "::group::Install LCOV dependencies"
       sudo apt-get -o Acquire::Retries=3 -y -q --no-install-suggests --no-install-recommends install \
         libcapture-tiny-perl libdatetime-perl libjson-xs-perl || true
       echo "::endgroup::"
    fi
fi

# Information
lcov --version || true
"${GCOV}" --version

LCOV_ARGS=(--ignore-errors inconsistent,inconsistent)

set -x
# capture coverage info
lcov "${LCOV_ARGS[@]}" --gcov-tool "${GCOV}" --directory build --capture --output-file coverage.info --rc branch_coverage=0 > /dev/null
# Remove everything from /usr (unrelated), external folder (3rd party data), test code
lcov "${LCOV_ARGS[@]}" --remove coverage.info '/usr/*' '*/.cache/*' --output-file coverage.info > /dev/null
for excl in "${EXCLUSIONS[@]}"; do
    lcov "${LCOV_ARGS[@]}" --remove coverage.info "${excl}" --output-file coverage.info > /dev/null
done

# Debug output
lcov --list coverage.info
