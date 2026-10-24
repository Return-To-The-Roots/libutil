#!/bin/bash

# Copyright (C) 2005 - 2026 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

set -euo pipefail

SRC_DIR="${1:?SRC_DIR missing}"
scriptPath=$(dirname "$(readlink -f "$0")")

cd "${SRC_DIR}"

if ! "$scriptPath/checkUTF8.sh"; then
  echo "Some files are not in UTF8 encoding (see filenames above)"
  exit 1
fi

canonicalBugs="$(grep -r -l --include '*.h' --include '*.cpp' --include '*.hpp' --include '*.tpp' "::canonical(" || true)"
if [[ "${canonicalBugs}" != "" ]]; then
  echo "Following files use buggy boost::filesystem::canonical: ${canonicalBugs}"
  echo "If this is only used on non-windows use canonical unqualified: using bfs::canonical; canonical(...)"
  exit 1
fi
