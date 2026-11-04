#include "base.h"

ENV(print) void print(const char* str, usize len);
GPU(create_buffer) u32 gpu_create_buffer(usize size, u32 usage);
GPU(write_buffer) void gpu_write_buffer(u32 buf, u32 off, const void* src, u32 len);
GPU(clear) void gpu_clear(f32 r, f32 g, f32 b, f32 a);

template <usize N>
static void print(const char (&s)[N]) { print(s, N - 1); }

static Arena persistent_storage;
static Arena transient_storage;

EXPORT(init) void init() {
    persistent_storage = make_arena(MB(64));
    transient_storage = make_arena(MB(8));
    
    print("Hello from WASM");
}

EXPORT(frame) void frame(f32) {
    arena_reset(&transient_storage);
    gpu_clear(0.0f, 0.0f, 0.0f, 1.0f);
}
