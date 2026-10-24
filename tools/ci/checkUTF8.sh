#!/bin/bash
#
# Copyright (C) 2005 - 2026 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

# Check that all source files in the current folder are valid UTF-8.
# Prints the filenames of files that are not valid UTF-8 to stderr.
# Exits 0 if all files are valid UTF-8, 1 otherwise.

set -euo pipefail

failed=0
while IFS= read -r -d '' file; do
    # Try to convert all source files from UTF-8 to detect encoding errors
    if ! iconv -f UTF-8 "$file" > /dev/null; then
        printf '%s\n' "$file" >&2
        failed=1
    fi
done < <(find . --exclude-dir=.git -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.tpp' -o -name '*.h' \) -print0)

exit "$failed"
