# Debug C++ in Chrome

The default build uses `-O0 -g -DDEBUG`: no compiler optimization, embedded DWARF
debug symbols, and the assertions in `base/base.h` enabled. This uses the existing
Clang and wasm-ld toolchain; no Emscripten or additional JavaScript is needed.

One-time browser setup:

1. Install [C/C++ DevTools Support (DWARF)](https://goo.gle/wasm-debugging-extension).
2. Open `chrome://extensions/`, find that extension, and select **Details > Extension options**.
3. Add a path substitution from your repository's absolute path (run `pwd` from
   the repository root) to `http://localhost:8080/`. If you move the repository,
   update the first path to its new absolute location. Keep the trailing slashes.

To try a breakpoint:

1. Build and serve the repository using the commands in [README.md](README.md), then open
   http://localhost:8080/build/ with DevTools open:

   ```sh
   ./build.sh
   python3 -m http.server 8080 --bind 127.0.0.1
   ```

2. In **Sources**, open `src/main/native/main.cpp` and set a breakpoint on
   `printf("Hello, World! WebGPU instance=%u", instance.id);` inside `init()`.
3. Reload the page. When paused, inspect `persistent_storage` and
   `transient_storage`, including each arena's `current` block and its
   `capacity`, `base_pos`, and `pos` fields.
4. Try a breakpoint inside `frame()` or step into the arena functions in
   `base/base_arena.h`.

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
