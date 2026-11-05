#pragma once

typedef signed char ichar;
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

static_assert(sizeof(char) == 1 && char(-1) > 0);
static_assert(sizeof(short) == 2);
static_assert(sizeof(int) == 4);
static_assert(sizeof(long long) == 8);
static_assert(sizeof(void*) == 4, "These aliases target wasm32");

#define GPU(n) __attribute__((import_module("gpu"), import_name(#n)))
#define ENV(n) __attribute__((import_module("env"), import_name(#n)))
#define EXPORT(n) __attribute__((export_name(#n)))

#define KB(n) ((usz)(n) << 10)
#define MB(n) ((usz)(n) << 20)
#define WASM_PAGE_SIZE 65536u

#ifdef DEBUG
    #define ASSERT(c) do { if (!(c)) __builtin_trap(); } while(0)
#else
    #define ASSERT(c) ((void)0)
#endif /* ifdef DEBUG */

extern "C" char __heap_base;

inline void* operator new(usz, void* p) noexcept { return p; }

inline char* os_reserve(usz size) {
    static usz brk = 0;
    if (brk == 0) brk = (usz)&__heap_base;

    usz start = (brk + 15) & ~(usz)15; //align to 16-byte on every block
    usz end = start + size;
    if (end < start) {
        ASSERT(!"Arena allocation fail: overflow");
        return nullptr;
    }

    usz have = __builtin_wasm_memory_size(0) * WASM_PAGE_SIZE;
    if (end > have) {
        usz pages = (end - have + WASM_PAGE_SIZE - 1) / WASM_PAGE_SIZE;
        if (__builtin_wasm_memory_grow(0, pages) == (usz)-1) {
            ASSERT(!"Arena allocation fail: can't grow the arena");
            return nullptr;
        }
    }
    brk = end;

    return (char*)start;
}

struct Arena {
    char* base;
    usz cap;
    usz pos;
    usz peak;
};

inline Arena make_arena(usz cap) {
    Arena a = {};

    a.base = os_reserve(cap);
    a.cap = a.base ? cap : 0;

    return a;
}

inline Arena arena_sub(Arena* parent, usz cap);

inline void* arena_push_nz(Arena* a, usz size, usz align = 16) {
    ASSERT(align && (align & (align - 1)) == 0); // power of two
    usz p = (a->pos + (align - 1)) & ~(align - 1);
    if (p + size > a->cap || p + size < p) {
        __builtin_trap(); // out of budget
    }

    a->pos = p + size;
    if (a->pos > a->peak) a->peak = a->pos;

    return a->base + p;
}

inline void* arena_push_zero(Arena* a, usz size, usz align = 16) {
    void* p = arena_push_nz(a, size, align);
    __builtin_memset(p, 0, size);

    return p;
}

inline Arena arena_sub(Arena* parent, usz cap) {
    Arena a = {};

    a.base = (char*)arena_push_nz(parent, cap, 16);
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
inline T* push_array(Arena* a, usz n) {
    return (T*)arena_push_zero(a, sizeof(T) * n, alignof(T));
}

template <typename T>
inline T* push_array_nz(Arena* a, usz n) {
    return (T*)arena_push_nz(a, sizeof(T) * n, alignof(T));
}

// For the rare type that needs a constructor.
template <typename T> inline T* push_new(Arena* a) {
    return new (arena_push_nz(a, sizeof(T), alignof(T))) T();
}

struct Arena_Mark {
    Arena* arena;
    usz pos;
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
