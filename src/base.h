#pragma once

typedef __UINT8_TYPE__ u8;
typedef __UINT16_TYPE__ u16;
typedef __UINT32_TYPE__ u32;
typedef __SIZE_TYPE__ usize;

typedef __INT8_TYPE__ i8;
typedef __INT16_TYPE__ i16;
typedef __INT32_TYPE__ i32;

typedef float f32;
typedef double f64;

#define GPU(n) __attribute__((import_module("gpu"), import_name(#n)))
#define ENV(n) __attribute__((import_module("env"), import_name(#n)))
#define EXPORT(n) __attribute__((export_name(#n)))

#define KB(n) ((usize)(n) << 10)
#define MB(n) ((usize)(n) << 20)
#define WASM_PAGE_SIZE 65536u

#ifdef DEBUG
    #define ASSERT(c) do { if (!(c)) __builtin_trap(); } while(0)
#else
    #define ASSERT(c) ((void)0)
#endif /* ifdef DEBUG */

extern "C" u8 __heap_base;

inline void* operator new(usize, void* p) noexcept { return p; }

inline u8* os_reserve(usize size) {
    static usize brk = 0;
    if (brk == 0) brk = (usize)&__heap_base;

    usize start = (brk + 15) & ~(usize)15; //align to 16-byte on every block
    usize end = start + size;
    if (end < start) {
        ASSERT(!"Arena allocation fail: overflow");
        return nullptr;
    }

    usize have = __builtin_wasm_memory_size(0) * WASM_PAGE_SIZE;
    if (end > have) {
        usize pages = (end - have + WASM_PAGE_SIZE - 1) / WASM_PAGE_SIZE;
        if (__builtin_wasm_memory_grow(0, pages) == (usize)-1) {
            ASSERT(!"Arena allocation fail: can't grow the arena");
            return nullptr;
        }
    }
    brk = end;

    return (u8*)start;
}

struct Arena {
    u8* base;
    usize cap;
    usize pos;
    usize peak;
};

inline Arena make_arena(usize cap) {
    Arena a = {};

    a.base = os_reserve(cap);
    a.cap = a.base ? cap : 0;

    return a;
}

inline Arena arena_sub(Arena* parent, usize cap);

inline void* arena_push_nz(Arena* a, usize size, usize align = 16) {
    ASSERT(align && (align & (align - 1)) == 0); // power of two
    usize p = (a->pos + (align - 1)) & ~(align - 1);
    if (p + size > a->cap || p + size < p) {
        __builtin_trap(); // out of budget
    }

    a->pos = p + size;
    if (a->pos > a->peak) a->peak = a->pos;

    return a->base + p;
}

inline void* arena_push_zero(Arena* a, usize size, usize align = 16) {
    void* p = arena_push_nz(a, size, align);
    __builtin_memset(p, 0, size);

    return p;
}

inline Arena arena_sub(Arena* parent, usize cap) {
    Arena a = {};

    a.base = (u8*)arena_push_nz(parent, cap, 16);
    a.cap = cap;

    return a;
}

inline void arena_reset(Arena* a) {
#ifdef DEBUG
    __builtin_memset(a->base, 0xCD, a->pos); // poison: catches use-after-reset
#endif
    a->pos = 0;
}

template <typename T>
inline T* push_struct(Arena* a) {
    return (T*)arena_push_zero(a, sizeof(T), alignof(T));
}

template <typename T>
inline T* push_array(Arena* a, usize n) {
    return (T*)arena_push_zero(a, sizeof(T) * n, alignof(T));
}

template <typename T>
inline T* push_array_nz(Arena* a, usize n) {
    return (T*)arena_push_nz(a, sizeof(T) * n, alignof(T));
}

// For the rare type that needs a constructor.
template <typename T> inline T* push_new(Arena* a) {
    return new (arena_push_nz(a, sizeof(T), alignof(T))) T();
}

struct Arena_Mark {
    Arena* arena;
    usize pos;
};

inline Arena_Mark arena_mark(Arena* a) {
    return { a, a->pos };
}

inline void arena_pop_to(Arena_Mark m) {
    ASSERT(m.pos <= m.arena->pos); // marks must be popped in LIFO order
#ifdef DEBUG
    __builtin_memset(m.arena->base + m.pos, 0xCD, m.arena->pos - m.pos);
#endif
    m.arena->pos = m.pos;
}

// Optional scope guard: pops automatically at end of scope.
struct Temp_Scope {
    Arena_Mark mark;

    explicit Temp_Scope(Arena* a) : mark(arena_mark(a)) {}
    ~Temp_Scope() { arena_pop_to(mark); }
    Temp_Scope(const Temp_Scope&) = delete;
    Temp_Scope& operator=(const Temp_Scope&) = delete;
};
