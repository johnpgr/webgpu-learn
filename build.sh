#!/bin/bash
set -e

cd "$(dirname "$0")"
mkdir -p build

# Apple clang can emit wasm32 objects but has no wasm linker; use Homebrew's lld.
export PATH="/opt/homebrew/opt/lld/bin:$PATH"

clang++ \
    --target=wasm32 \
    -std=c++20 \
    -nostdlib \
    -ffreestanding \
    -Wall \
    -Wextra \
    -Wno-c99-designator \
    -Wno-unknown-attributes \
    -O2 \
    -g \
    -fno-exceptions \
    -fno-rtti \
    -mbulk-memory \
    -Wl,--no-entry \
    src/main.cpp \
    -o build/game.wasm

cp src/index.html src/main.js build/
echo "built build/game.wasm"
