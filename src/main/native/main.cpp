#include "base/base_mod.h"
#include "lib/webgpu.h"
#include "lib/webgpu_print.h"

static Arena* storage;

static WGPUInstance instance;

EXPORT(init) void init() {
    storage = arena_alloc(MB(64));

    WGPUInstanceDescriptor desc = {};
     desc.nextInChain = nullptr;
    instance = wgpuCreateInstance(&desc);

    printf("Hello, World! WebGPU instance=%u", instance.id);
}

EXPORT(frame) void frame(float) { }
