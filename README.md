# WebGPU learning [webgpu](https://webgpu.org/)

## Run locally

Needs a browser with WebGPU and WebAssembly JSPI (JavaScript Promise
Integration): Chrome/Edge 137+ or Firefox 153+. The C++ code sets up WebGPU the
standard `webgpu.h` way (instance, surface, adapter, device) and blocks on the
async requests with `wgpuInstanceWaitAny`; JSPI suspends the wasm stack while it
waits, so no Emscripten or Asyncify is needed.

Rebuild after changing C++ or the JS/HTML host, then serve the repository root
so the debugger can also load the C++ sources:

```sh
./build.sh
python3 -m http.server 8080 --bind 127.0.0.1
```

If a server is already running on port 8080, stop it before starting this command.
Open http://localhost:8080/build/. Hard-refresh once (Cmd+Shift+R on macOS) if you previously loaded a cached build.

## Debug C++ in Chrome

The default build uses `-O0 -g -DDEBUG`: no compiler optimization, embedded DWARF
debug symbols, and the assertions in `core/base.h` enabled. This uses the existing
Clang and wasm-ld toolchain; no Emscripten or additional JavaScript is needed.

One-time browser setup:

1. Install [C/C++ DevTools Support (DWARF)](https://goo.gle/wasm-debugging-extension).
2. Open `chrome://extensions/`, find that extension, and select **Details > Extension options**.
3. Add a path substitution from your repository's absolute path (run `pwd` from
   the repository root) to `http://localhost:8080/`. If you move the repository,
   update the first path to its new absolute location. Keep the trailing slashes.

To try a breakpoint:

1. Build and serve the repository using the commands above, then open
   http://localhost:8080/build/ with DevTools open.
2. In **Sources**, open `src/main/native/main.cpp` and set a breakpoint on
   `printf("Hello, World! WebGPU instance=%u", instance.id);` inside `init()`.
3. Reload the page. When paused, inspect `persistent_storage` and
   `transient_storage`, including each arena's `current` block and its
   `capacity`, `base_pos`, and `pos` fields.
4. Try a breakpoint inside `frame()` or step into the arena functions in
   `core/base_arena.h`.

If initialization runs before the source files appear, set a temporary breakpoint
on `await init();` in `main.js`, reload, then set the C++ breakpoint and
resume. No JavaScript edits are necessary.

Enable **Pause on exceptions**, including **Pause on caught exceptions**, to stop
when an assertion calls `__builtin_trap()`. Initialization exceptions are caught
by the existing JavaScript error handler.

If DevTools shows WASM instructions instead of C++, check that the extension is
enabled, rebuild, and reload with DevTools open. If C++ files appear but cannot
be loaded, check the path substitution and confirm that
http://localhost:8080/src/main/native/main.cpp is accessible.

This debugs the C++ running as WASM on the CPU. GPU shader execution needs
separate tools. For details, see [Chrome's WASM debugging guide](https://developer.chrome.com/docs/devtools/wasm).

## Arena memory

`core/base_arena.h` adapts RAD Debugger's chained arenas to Wasm linear memory.
`arena_alloc(block_size)` creates the first block; the block size includes a
128-byte header. An allocation that does not fit adds another block without
moving earlier allocations. Larger individual requests get larger blocks.

`arena_push(arena, size, alignment, zero)` allocates bytes. `push_array<T>`
zeroes its allocation; `push_array_nz<T>` leaves it uninitialized. These helpers
are intended for plain data and do not run constructors or destructors.
Alignment, size overflow, growth failure, and invalid rollback positions trap
in both debug and release builds.

`temp_begin` saves an arena position and `temp_end` restores it. Scopes must
end in reverse order, and pointers allocated inside a scope become invalid
when it ends. `arena_clear` invalidates all allocations in that arena.
Discarded blocks are retained for reuse, including their existing contents;
zeroed allocation requests explicitly clear the returned bytes. Debug builds
fill discarded allocations with `0xCD` to help identify stale data.

Backing blocks remain owned by their arena for the module's lifetime. Wasm
linear memory cannot shrink, so arenas should be created once and reused.
The allocator is single-threaded. The game and WebGPU bridge share one
backing allocator in `core/base.h`; no libc, WASI, or Emscripten is required.
The adapted source's MIT license is in `LICENSES/raddebugger.txt`.

Run allocation and callback regression tests with Clang, wasm-ld, and a Node
version supporting WebAssembly JSPI:

```sh
./tests/run.sh
```

The tests compile and execute both debug and optimized release Wasm modules.

## Printing and strings

Headers do not include each other. The translation unit includes layers in
dependency order, and the `base` layer never names a `lib` type:

```cpp
#include "core/base.h"
#include "core/base_arena.h"
#include "base/base_print.h"
#include "lib/webgpu.h"
#include "lib/webgpu_print.h"

String name = {"triangle", 8};
ZString label = "renderer";
WGPUStringView message = {"adapter ready", WGPU_STRLEN};

print(name);
print(label);
print(message);
printf("%s: %S, vertices=%u, elapsed=%.3f", label, name, 3u, 1.25);
printf("WebGPU: %S", string_from_webgpu(message));
```

The core names follow C3: `String` is a non-owning counted byte view with
`data` and `len`; `ZString` aliases `const char*` and requires a terminating
NUL. Neither type allocates or owns its contents. `WGPUStringView` retains
its WebGPU name and layout at the API boundary. `string_from_zstring` and
`string_from_webgpu` (in `lib/webgpu_print.h`) produce core views when needed.

`print` sends text literally, including percent signs. Counted views preserve
embedded NUL bytes and need no terminator. WebGPU views with `WGPU_STRLEN`
are measured before output; null WebGPU views and null `ZString`s print
empty text. A core `String` with null data must have zero length.

`printf` supports printf conversions through a vendored `stb_sprintf`
v1.10 implementation, plus `%S` for `String`. Convert a `WGPUStringView` with `string_from_webgpu` first. `%S`
supports field width, left alignment, and precision measured in bytes.
`%s` expects a null-terminated pointer. As with printf, argument types must
match their conversions; the custom `%S` is not checked by the compiler's
standard printf format checker.

Formatting measures the output using a copied argument list, allocates the
complete buffer in a private reusable arena, and restores the arena position
after one synchronous `env.print` call. Messages are not silently truncated.
The formatter uses signed `int` lengths, so output must be smaller than
`INT_MAX` bytes. Printing is single-threaded, like the allocator.

The printing functions live in `base/base_print.h`. It includes only its formatter implementation. The WebGPU overloads live in
`lib/webgpu_print.h`. No application implementation macro is needed.
Core string types remain in `core/base.h`. `<stdarg.h>` and `<stddef.h>`
come from Clang without linking libc, WASI, or Emscripten. The formatter's
source and license
are in `src/main/native/third_party/stb_sprintf.h`, adapted from the RAD
Debugger checkout with its debugger-specific conversions removed and `%S`
changed to the core `String` type. RAD Debugger's license is retained in
`LICENSES/raddebugger.txt`.

`./tests/run.sh` also verifies formatting, memory reuse, and that printing
imports only `env.print`, in both debug and optimized release builds.

## Learning resources used

- https://developer.chrome.com/docs/web-platform/webgpu/build-app?hl=pt-br
- https://eliemichel.github.io/LearnWebGPU/

## First goals

- [x] Render a blank webgpu canvas on the browser page
- [ ] Render a Triangle with shader coloring
- [ ] Render an image file directly on the canvas

