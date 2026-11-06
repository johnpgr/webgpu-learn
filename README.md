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
3. Add a path substitution from `/Users/gabriel/dev/webgpu-learn/` to
   `http://localhost:8080/`. If you move the repository, update the first path to
   its new absolute location. Keep the trailing slashes.

To try a breakpoint:

1. Build and serve the repository using the commands above, then open
   http://localhost:8080/build/ with DevTools open.
2. In **Sources**, open `src/main/native/main.cpp` and set a breakpoint on
   `print("Hello from WASM: WebGPU initialized");` inside `init()`.
3. Reload the page. When paused, inspect `persistent_storage` and
   `transient_storage`, including their `base`, `cap`, and `pos` fields.
4. Try a breakpoint inside `frame()` or step into the arena functions in
   `core/base.h`.

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

## Learning resources used

- https://developer.chrome.com/docs/web-platform/webgpu/build-app?hl=pt-br
- https://eliemichel.github.io/LearnWebGPU/

## First goals

- [x] Render a blank webgpu canvas on the browser page
- [ ] Render a Triangle with shader coloring
- [ ] Render an image file directly on the canvas

