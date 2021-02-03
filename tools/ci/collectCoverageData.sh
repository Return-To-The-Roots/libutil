#!/bin/bash

set -euo pipefail

EXCLUSIONS=("$@")

# Install lcov if required
LCOV_VERSION="1.15"
if [[ ! "$(lcov --version 2> /dev/null || true)" =~ "version ${LCOV_VERSION}" ]]; then
    LCOV_ROOT="/tmp/lcov"
    LCOV_URL="https://github.com/linux-test-project/lcov/releases/download/v${LCOV_VERSION}/lcov-${LCOV_VERSION}.tar.gz"
    mkdir -p "${LCOV_ROOT}"
    wget --quiet -O - "${LCOV_URL}" | tar --strip-components=1 -xz -C "${LCOV_ROOT}"
    export PATH="${LCOV_ROOT}/bin:${PATH}"
fi

# Information
"${GCOV}" --version
lcov --version || true

# capture coverage info
lcov --gcov-tool "${GCOV}" --directory build --capture --output-file coverage.info --rc lcov_branch_coverage=1 > /dev/null
# Remove everything from /usr (unrelated), external folder (3rd party data), test code
lcov --remove coverage.info '/usr/*' '*/.cache/*' --output-file coverage.info > /dev/null
for excl in "${EXCLUSIONS[@]}"; do
    lcov --remove coverage.info "${excl}" --output-file coverage.info > /dev/null
done

# Debug output
lcov --list coverage.info
