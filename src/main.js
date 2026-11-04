// Thin JS host: owns the WebGPU objects, wasm only ever sees u32 handles.

const status = (msg) => {
    document.getElementById("status").textContent = msg;
};

async function main() {
    if (!navigator.gpu) throw new Error("WebGPU not supported in this browser");

    const adapter = await navigator.gpu.requestAdapter();
    if (!adapter) throw new Error("No GPU adapter found");
    const device = await adapter.requestDevice();
    device.lost.then((info) => status(`Device lost: ${info.message}`));

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

    // handle table: index 0 is reserved as the "null" handle
    const objs = [null];

    const imports = {
        env: {
            print: (ptr, len) => console.log(decoder.decode(new Uint8Array(mem(), ptr, len))),
        },
        gpu: {
            create_buffer: (size, usage) => objs.push(device.createBuffer({ size, usage })) - 1,
            write_buffer: (h, off, ptr, len) => device.queue.writeBuffer(objs[h], off, mem(), ptr, len),
            clear: (r, g, b, a) => {
                const encoder = device.createCommandEncoder();
                const pass = encoder.beginRenderPass({
                    colorAttachments: [{
                        view: context.getCurrentTexture().createView(),
                        clearValue: { r, g, b, a },
                        loadOp: "clear",
                        storeOp: "store",
                    }],
                });
                pass.end();
                device.queue.submit([encoder.finish()]);
            },
        },
    };

    // Always load the latest build while developing.
    const result = await WebAssembly.instantiateStreaming(fetch("game.wasm", { cache: "no-store" }), imports);
    inst = result.instance;
    inst.exports.init();
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
