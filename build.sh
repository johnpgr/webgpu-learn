#!/bin/bash
set -e

cd "$(dirname "$0")"
mkdir -p build

# Apple clang can emit wasm32 objects but has no wasm linker; use Homebrew's lld.
export PATH="/opt/homebrew/opt/lld/bin:$PATH"

clang++ @compile_flags.txt

cp src/main/web/index.html src/main/web/main.js src/main/web/webgpu.js build/
echo "built build/game.wasm"
