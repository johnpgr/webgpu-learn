#pragma once

#define STB_SPRINTF_IMPLEMENTATION
#include "../lib/stb_sprintf.h"
#undef STB_SPRINTF_IMPLEMENTATION

ENV(print) void print_bytes(const char* data, usz length);

inline void print(String text) {
    if (!text.data && text.len != 0) TRAP();
    print_bytes(text.data, text.len);
}

inline void print(ZString text) {
    String string = string_from_zstring(text);
    print_bytes(string.data, string.len);
}

inline void printf(ZString format, ...) {
    if (!format) TRAP();
    static Arena* arena;
    if (!arena) arena = arena_alloc(KB(4));
    Temp temporary = temp_begin(arena);

    va_list arguments;
    va_start(arguments, format);
    va_list measurement;
    va_copy(measurement, arguments);
    int length = stbsp_vsnprintf(nullptr, 0, format, measurement);
    va_end(measurement);
    // stb_sprintf's buffer capacity is signed and includes the terminator.
    if (length < 0 || length == __INT_MAX__) TRAP();
    char* buffer = push_array_nz<char>(arena, (usz)length + 1);
    int written = stbsp_vsnprintf(buffer, length + 1, format, arguments);
    va_end(arguments);
    if (written != length) TRAP();

    print_bytes(buffer, (usz)length);
    temp_end(temporary);
}
