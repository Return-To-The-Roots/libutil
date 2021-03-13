#!/bin/bash

set -euo pipefail

if ! [ -f coverage.info ]; then
    "$(dirname "$0")/collectCoverageData.sh" "$@"
fi

# Coverage.io
bash <(curl -s https://codecov.io/bash) -f coverage.info || echo "Codecov did not collect coverage reports"
