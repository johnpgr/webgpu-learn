# WebGPU learning [webgpu](https://webgpu.org/)

## Run locally

Rebuild after changing C++ or the JS/HTML host, then serve the build without browser caching:

```sh
./build.sh
npx http-server build -p 8080 -c-1
```

If a server is already running on port 8080, stop it before starting this command.
Open http://localhost:8080. Hard-refresh once (Cmd+Shift+R on macOS) if you previously loaded a cached build.

## Learning resources used

- https://developer.chrome.com/docs/web-platform/webgpu/build-app?hl=pt-br
- https://eliemichel.github.io/LearnWebGPU/

## First goals

- [ ] Render a blank webgpu canvas on the browser page
- [ ] Render a Triangle with shader coloring
- [ ] Render an image file directly on the canvas

