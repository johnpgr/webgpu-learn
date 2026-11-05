#include "core/base.h"
#include "webgpu.h"

ENV(print) void print(const char* str, usz len);

template <usz N>
static void print(const char (&s)[N]) { print(s, N - 1); }

static Arena persistent_storage;
static Arena transient_storage;

EXPORT(init) void init(WGPUDevice, WGPUSurface) {
    persistent_storage = make_arena(MB(64));
    transient_storage = make_arena(MB(8));
    
    print("Hello from WASM");
}

EXPORT(frame) void frame(float) {
    arena_reset(&transient_storage);
}
