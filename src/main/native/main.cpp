#include "core/base.h"
#include "webgpu.h"

static Arena persistent_storage;
static Arena transient_storage;

static WGPUInstance instance;

EXPORT(init) void init() {
    persistent_storage = make_arena(MB(64));
    transient_storage = make_arena(MB(8));

    WGPUInstanceDescriptor desc = { .nextInChain = nullptr };
    instance = wgpuCreateInstance(&desc);

    print("Hello, World!");
}

EXPORT(frame) void frame(float) {
    arena_reset(&transient_storage);
}
