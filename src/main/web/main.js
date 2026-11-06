// Browser host: manages canvas sizing, wires the `webgpu.h` Wasm binding layer,
// and drives the animation loop. WebGPU setup (instance, surface, adapter, device)
// happens in C++ (see init in main.cpp).

import { createWebGPUBindings } from "./webgpu.js";

const status = (msg) => {
    document.getElementById("status").textContent = msg;
};

async function main() {
    if (!navigator.gpu) throw new Error("WebGPU not supported in this browser");
    // wgpuInstanceWaitAny blocks C++ by suspending wasm with JSPI.
    if (!("Suspending" in WebAssembly && "promising" in WebAssembly)) {
        throw new Error("WebAssembly JSPI not supported in this browser (needs Chrome/Edge 137+ or Firefox 153+)");
    }

    // This host owns the canvas backing-store size. wgpuSurfaceConfigure ignores
    // the width/height in WGPUSurfaceConfiguration.
    const canvas = document.getElementById("canvas");
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

    const wgpu = createWebGPUBindings({ getExports: () => inst.exports });

    const imports = {
        env: {
            print: (ptr, len) => console.log(decoder.decode(new Uint8Array(mem(), ptr, len))),
        },
        gpu: wgpu.imports,
    };

    // Always load the latest build while developing.
    const result = await WebAssembly.instantiateStreaming(fetch("game.wasm", { cache: "no-store" }), imports);
    inst = result.instance;

    // Only init is entered through promising(), so only init may block in
    // wgpuInstanceWaitAny (adapter/device setup). Nothing else may call into wasm
    // until its promise settles.
    const init = WebAssembly.promising(inst.exports.init);

    status("initializing WebGPU…");
    await init();
    status("WebGPU ok");

    // frame is called directly: it must never block. wgpuInstanceWaitAny inside it
    // throws SuspendError (even with timeout 0); handle async results with
    // AllowSpontaneous / AllowProcessEvents callbacks instead.
    const frame = inst.exports.frame;
    let last = performance.now();
    const loop = (now) => {
        try {
            frame((now - last) / 1000);
        } catch (e) {
            // Wasm state is unreliable after a trap, so report once and stop the loop.
            console.error(e);
            status(`Frame error (loop stopped): ${e.message}`);
            return;
        }
        last = now;
        requestAnimationFrame(loop);
    };
    requestAnimationFrame(loop);
}

main().catch((e) => {
    console.error(e);
    status(`Error: ${e.message}`);
});
