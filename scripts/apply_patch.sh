#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 ]]; then
  echo "usage: $0 <path-to-llvm-project-root>"
  exit 2
fi

upstream_root="$1"
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

mkdir -p "${upstream_root}/llvm/lib/Passes"
cp -a "${repo_root}/llvm-project/llvm/lib/Passes/." "${upstream_root}/llvm/lib/Passes/"

mkdir -p "${upstream_root}/llvm/test/Transforms/Obfuscation"
cp -a "${repo_root}/llvm-project/llvm/test/Transforms/Obfuscation/." "${upstream_root}/llvm/test/Transforms/Obfuscation/"

echo "Applied OLLVM21 patch to: ${upstream_root}"

