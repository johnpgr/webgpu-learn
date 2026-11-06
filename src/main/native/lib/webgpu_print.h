#pragma once

inline String string_from_webgpu(WGPUStringView text) {
    if (!text.data) return {};
    if (text.length == WGPU_STRLEN) return string_from_zstring(text.data);
    return {text.data, text.length};
}

inline void print(WGPUStringView text) {
    print(string_from_webgpu(text));
}
