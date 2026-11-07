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

## Debugging

See [DEBUGGING.md](DEBUGGING.md) for instructions on setting up source-level C++ debugging in Chrome DevTools.

## Learning resources used

- https://developer.chrome.com/docs/web-platform/webgpu/build-app?hl=pt-br
- https://eliemichel.github.io/LearnWebGPU/

## First goals

- [x] Render a blank webgpu canvas on the browser page
- [ ] Render a Triangle with shader coloring
- [ ] Render an image file directly on the canvas

