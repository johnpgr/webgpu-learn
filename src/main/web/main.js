// Browser host: acquires the WebGPU adapter/device, manages canvas sizing,
// wires the standard `webgpu.h` Wasm binding layer, and drives the animation loop.

import { createWebGPUBindings } from "./webgpu.js";

const status = (msg) => {
    document.getElementById("status").textContent = msg;
};

async function main() {
    if (!navigator.gpu) throw new Error("WebGPU not supported in this browser");

    const adapter = await navigator.gpu.requestAdapter();
    if (!adapter) throw new Error("No GPU adapter found");
    const device = await adapter.requestDevice();
    device.lost.then((info) => status(`Device lost: ${info.message}`));
    device.addEventListener("uncapturederror", (event) => {
        console.error("WebGPU uncaptured error:", event.error);
        status(`GPU Error: ${event.error.message}`);
    });

    const canvas = document.getElementById("canvas");
    const context = canvas.getContext("webgpu");
    const format = navigator.gpu.getPreferredCanvasFormat();
    context.configure({ device, format, alphaMode: "opaque" });

    // Keep the backing store matched to the displayed size.
    const resize = () => {
        const dpr = window.devicePixelRatio || 1;
        canvas.width = Math.max(1, Math.floor(canvas.clientWidth * dpr));
        canvas.height = Math.max(1, Math.floor(canvas.clientHeight * dpr));
    };
    new ResizeObserver(resize).observe(canvas);
    resize();

    let inst;
    const mem = () => inst.exports.memory.buffer;
    const decoder = new TextDecoder();

    const wgpu = createWebGPUBindings({
        adapter,
        device,
        context,
        format,
        memory: () => inst.exports.memory,
    });

    const imports = {
        env: {
            print: (ptr, len) => console.log(decoder.decode(new Uint8Array(mem(), ptr, len))),
        },
        gpu: wgpu.imports,
    };

    // Always load the latest build while developing.
    const result = await WebAssembly.instantiateStreaming(fetch("game.wasm", { cache: "no-store" }), imports);
    inst = result.instance;
    inst.exports.init(wgpu.handles.device, wgpu.handles.surface);
    status(`WebGPU ok (${format})`);

    let last = performance.now();
    const loop = (now) => {
        inst.exports.frame((now - last) / 1000);
        last = now;
        requestAnimationFrame(loop);
    };
    requestAnimationFrame(loop);
}

main().catch((e) => {
    console.error(e);
    status(`Error: ${e.message}`);
});
