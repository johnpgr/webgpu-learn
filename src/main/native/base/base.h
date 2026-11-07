#pragma once

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long long ulong;
typedef int iptr;
typedef unsigned int uptr;
typedef long sz;
typedef unsigned long usz;

#ifdef __SIZEOF_INT128__
typedef __int128 int128;
typedef unsigned __int128 uint128;
#endif

static_assert(sizeof(char) == 1 && char(-1) < 0, "char must be signed; use uchar for unsigned bytes");
static_assert(sizeof(short) == 2);
static_assert(sizeof(int) == 4);
static_assert(sizeof(long long) == 8);
static_assert(sizeof(void*) == 4, "These aliases target wasm32");

// ----------------------------------------------------------------------------
// Compiler intrinsics. Code uses these macros instead of compiler builtins
// directly; each compiler maps them to its own builtin. A compiler without a
// branch here fails to build instead of silently getting a wrong fallback.
// ----------------------------------------------------------------------------

#if defined(__clang__) || defined(__GNUC__)
    #define OFFSET_OF(type, member) __builtin_offsetof(type, member)
    #define TRAP() __builtin_trap()
    #define MEMSET(dst, byte, size) __builtin_memset((dst), (byte), (size))
    #define STRLEN(cstr) __builtin_strlen(cstr)
    #define NAN_F32 __builtin_nanf("")
#else
    #error "base/base.h: unsupported compiler; map the intrinsic macros to its builtins"
#endif

// WebAssembly memory instructions (memory index 0). Sizes are in 64 KiB pages;
// WASM_MEMORY_GROW returns the previous size, or (usz)-1 on failure.
#if defined(__wasm__)
    #if defined(__clang__)
        #define WASM_MEMORY_SIZE() __builtin_wasm_memory_size(0)
        #define WASM_MEMORY_GROW(pages) __builtin_wasm_memory_grow(0, (pages))
    #else
        #error "base/base.h: unsupported compiler for wasm memory intrinsics"
    #endif
#endif

#define GPU(n) __attribute__((import_module("gpu"), import_name(#n)))
#define ENV(n) __attribute__((import_module("env"), import_name(#n)))
#define EXPORT(n) __attribute__((export_name(#n)))

#define KB(n) ((usz)(n) << 10)
#define MB(n) ((usz)(n) << 20)
#define WASM_PAGE_SIZE 65536u

#ifdef DEBUG
    #define ASSERT(c) do { if (!(c)) TRAP(); } while(0)
#else
    #define ASSERT(c) ((void)0)
#endif /* ifdef DEBUG */

extern "C" char __heap_base;

inline void* operator new(usz, void* p) noexcept { return p; }

// One allocator owns the linear-memory tail, including host bridge allocations.
inline char* os_reserve(usz size) {
    static ulong end_of_allocations = (uptr)&__heap_base;
    ulong start = (end_of_allocations + 15) & ~15ull;
    ulong end = start + size;
    if (size == 0 || end > (1ull << 32)) TRAP();

    ulong available_pages = WASM_MEMORY_SIZE();
    ulong required_pages = (end + WASM_PAGE_SIZE - 1) / WASM_PAGE_SIZE;
    if (required_pages > available_pages) {
        if (WASM_MEMORY_GROW((usz)(required_pages - available_pages)) == (usz)-1) TRAP();
    }
    end_of_allocations = end;
    return (char*)(uptr)start;
}

using ZString = const char*;

struct String {
    const char* data;
    usz len;
};

inline String string_from_zstring(ZString text) {
    usz length = 0;
    if (text) {
        while (text[length] != '\0') ++length;
    }
    return {text, length};
}
