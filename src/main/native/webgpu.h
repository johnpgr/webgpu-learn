#pragma once

#include "core/base.h"

// ============================================================================
// webgpu.h C/C++ Binding Layer for Wasm32
//
// A subset of the official webgpu-headers `webgpu.h` (main branch) for wasm32,
// backed by the browser's WebGPU API through src/main/web/webgpu.js. Enum
// values, flag values (WGPUFlags is 64-bit) and struct layouts are identical to
// the official header: every struct layout below is checked with a static_assert
// generated from the official header compiled for wasm32.
// - Types & descriptors use the `WGPU` prefix and camelCase fields.
// - Enums & bitflags are `enum class WGPU<Type>` with unprefixed values: `WGPUTextureFormat::R8Unorm`.
//   Names starting with a digit get a `D` prefix (`WGPUTextureDimension::D2`).
// - Functions use `wgpu<Object><Method>` (imported via `GPU(...)` from `core/base.h`).
// - Every object type has wgpu<Object>AddRef and wgpu<Object>Release.
// - Handles are 32-bit ids into a table in the JS host, not pointers.
//
// Differences from the official header:
// - Only the functions and descriptors declared here exist. Instance, async
//   requests, error scopes, query sets, render bundles and buffer mapping are
//   not implemented.
// - Not implemented, and rejected at runtime instead of being ignored (the JS
//   host throws, which traps wasm): WGPUBufferDescriptor::mappedAtCreation,
//   WGPUBindGroupLayoutEntry::bindingArraySize, WGPUPipelineLayoutDescriptor::
//   immediateSize, and the occlusionQuerySet / timestampWrites pass fields.
// - Surface: wgpuSurfaceGetPreferredFormat is an extension (official code uses
//   wgpuSurfaceGetCapabilities, which needs a wasm-side allocator). Unpremultiplied
//   and Inherit alpha modes are rejected (browser canvases cannot do them).
//   width, height and presentMode in WGPUSurfaceConfiguration are ignored: the
//   host page owns the canvas size.
// - WGPUShaderModuleDescriptor takes WGSL only, via a chained WGPUShaderSourceWGSL.
// - Default member initializers (e.g. mipLevelCount = 1, depthClearValue = NaN)
//   match WebGPU's defaults. Some use the concrete default where the official
//   WGPU_*_INIT macro uses Undefined (e.g. topology = TriangleList, stepMode =
//   Vertex); the result is the same. The JS host forwards values as-is and lets
//   WebGPU validation report mistakes; it invents no defaults.
//
// Ownership: AddRef/Release are reference counted by the host. Reaching zero only
// drops the handle; the browser object (and any GPU memory) is freed when JS
// garbage collection gets to it. Call wgpuBufferDestroy / wgpuTextureDestroy to
// free big resources eagerly. wgpuSurfaceGetCurrentTexture returns a NEW texture
// handle on every call; you must wgpuTextureRelease it (and any view made from
// it) each frame.
// ============================================================================

typedef uint WGPUBool;
typedef ulong WGPUFlags;

constexpr WGPUBool WGPU_FALSE = 0u;
constexpr WGPUBool WGPU_TRUE = 1u;

constexpr ulong WGPU_WHOLE_SIZE = ~(ulong)0;
constexpr uint WGPU_MIP_LEVEL_COUNT_UNDEFINED = ~0u;
constexpr uint WGPU_ARRAY_LAYER_COUNT_UNDEFINED = ~0u;
constexpr uint WGPU_DEPTH_SLICE_UNDEFINED = ~0u;
constexpr uint WGPU_QUERY_SET_INDEX_UNDEFINED = ~0u;
constexpr uint WGPU_COPY_STRIDE_UNDEFINED = ~0u;
constexpr usz WGPU_STRLEN = ~(usz)0;
// Float NaN: "no depth clear value given". Forwarded as unset, so WebGPU rejects
// depthLoadOp = Clear unless you set depthClearValue explicitly.
constexpr float WGPU_DEPTH_CLEAR_VALUE_UNDEFINED = __builtin_nanf("");

struct WGPUStringView {
    const char* data;
    usz length;

    constexpr WGPUStringView() : data(nullptr), length(0) {}
    constexpr WGPUStringView(const char* s, usz n) : data(s), length(n) {}
    constexpr WGPUStringView(decltype(nullptr)) : data(nullptr), length(0) {}

    // Matches string literals and char arrays alike. The length is the number of
    // characters before the first NUL (or N if there is none), so `char buf[64]`
    // holding "hi" gives 2, not 63. A literal with an embedded NUL is cut there;
    // build it with the (data, length) constructor instead.
    template <usz N>
    constexpr WGPUStringView(const char (&s)[N]) : data(s), length(bounded_strlen(s, N)) {}

private:
    static constexpr usz bounded_strlen(const char* s, usz max) {
        usz n = 0;
        while (n < max && s[n] != '\0') n++;
        return n;
    }
};

static_assert(sizeof(WGPUStringView) == 8);
static_assert(__builtin_offsetof(WGPUStringView, data) == 0);
static_assert(__builtin_offsetof(WGPUStringView, length) == 4);

inline WGPUStringView wgpu_str(const char* cstr) {
    if (!cstr) return {};
    return { cstr, __builtin_strlen(cstr) };
}

// ----------------------------------------------------------------------------
// Strongly-Typed Object Handles (32-bit IDs in wasm32)
// ----------------------------------------------------------------------------

#define WGPU_DEFINE_HANDLE(Name)                                               \
    struct Name {                                                              \
        uint id;                                                               \
        constexpr explicit operator bool() const { return id != 0; }           \
        constexpr bool operator==(Name other) const { return id == other.id; } \
        constexpr bool operator!=(Name other) const { return id != other.id; } \
    };                                                                         \
    static_assert(sizeof(Name) == 4 && alignof(Name) == 4)

WGPU_DEFINE_HANDLE(WGPUInstance);
WGPU_DEFINE_HANDLE(WGPUAdapter);
WGPU_DEFINE_HANDLE(WGPUDevice);
WGPU_DEFINE_HANDLE(WGPUQueue);
WGPU_DEFINE_HANDLE(WGPUSurface);
WGPU_DEFINE_HANDLE(WGPUBuffer);
WGPU_DEFINE_HANDLE(WGPUTexture);
WGPU_DEFINE_HANDLE(WGPUTextureView);
WGPU_DEFINE_HANDLE(WGPUSampler);
WGPU_DEFINE_HANDLE(WGPUBindGroupLayout);
WGPU_DEFINE_HANDLE(WGPUBindGroup);
WGPU_DEFINE_HANDLE(WGPUPipelineLayout);
WGPU_DEFINE_HANDLE(WGPUShaderModule);
WGPU_DEFINE_HANDLE(WGPURenderPipeline);
WGPU_DEFINE_HANDLE(WGPUComputePipeline);
WGPU_DEFINE_HANDLE(WGPUCommandEncoder);
WGPU_DEFINE_HANDLE(WGPURenderPassEncoder);
WGPU_DEFINE_HANDLE(WGPUComputePassEncoder);
WGPU_DEFINE_HANDLE(WGPUCommandBuffer);
WGPU_DEFINE_HANDLE(WGPUQuerySet); // only for descriptor layout; query sets are not implemented

#undef WGPU_DEFINE_HANDLE

// ----------------------------------------------------------------------------
// Enumerations (values identical to webgpu-headers `main`; 0 is Undefined/default,
// except the *BindingType enums where 0 is BindingNotUsed and 1 is Undefined)
// ----------------------------------------------------------------------------

enum class WGPUSurfaceGetCurrentTextureStatus : uint {
    SuccessOptimal = 1,
    SuccessSuboptimal = 2,
    Timeout = 3,
    Outdated = 4,
    Lost = 5,
    Error = 6,
};

enum class WGPUCompositeAlphaMode : uint {
    Auto = 0,
    Opaque = 1,
    Premultiplied = 2,
    Unpremultiplied = 3,
    Inherit = 4,
};

enum class WGPUPresentMode : uint {
    Undefined = 0,
    Fifo = 1,
    FifoRelaxed = 2,
    Immediate = 3,
    Mailbox = 4,
};

enum class WGPUOptionalBool : uint {
    False = 0,
    True = 1,
    Undefined = 2,
};

enum class WGPUSType : uint {
    ShaderSourceSPIRV = 1,
    ShaderSourceWGSL = 2,
    RenderPassMaxDrawCount = 3,
    SurfaceSourceMetalLayer = 4,
    SurfaceSourceWindowsHWND = 5,
    SurfaceSourceXlibWindow = 6,
    SurfaceSourceWaylandSurface = 7,
    SurfaceSourceAndroidNativeWindow = 8,
    SurfaceSourceXCBWindow = 9,
    SurfaceColorManagement = 10,
    RequestAdapterWebXROptions = 11,
    TextureComponentSwizzleDescriptor = 12,
    ExternalTextureBindingLayout = 13,
    ExternalTextureBindingEntry = 14,
    CompatibilityModeLimits = 15,
    TextureBindingViewDimension = 16,
};

enum class WGPUTextureFormat : uint {
    Undefined = 0,
    R8Unorm = 1,
    R8Snorm = 2,
    R8Uint = 3,
    R8Sint = 4,
    R16Unorm = 5,
    R16Snorm = 6,
    R16Uint = 7,
    R16Sint = 8,
    R16Float = 9,
    RG8Unorm = 10,
    RG8Snorm = 11,
    RG8Uint = 12,
    RG8Sint = 13,
    R32Float = 14,
    R32Uint = 15,
    R32Sint = 16,
    RG16Unorm = 17,
    RG16Snorm = 18,
    RG16Uint = 19,
    RG16Sint = 20,
    RG16Float = 21,
    RGBA8Unorm = 22,
    RGBA8UnormSrgb = 23,
    RGBA8Snorm = 24,
    RGBA8Uint = 25,
    RGBA8Sint = 26,
    BGRA8Unorm = 27,
    BGRA8UnormSrgb = 28,
    RGB10A2Uint = 29,
    RGB10A2Unorm = 30,
    RG11B10Ufloat = 31,
    RGB9E5Ufloat = 32,
    RG32Float = 33,
    RG32Uint = 34,
    RG32Sint = 35,
    RGBA16Unorm = 36,
    RGBA16Snorm = 37,
    RGBA16Uint = 38,
    RGBA16Sint = 39,
    RGBA16Float = 40,
    RGBA32Float = 41,
    RGBA32Uint = 42,
    RGBA32Sint = 43,
    Stencil8 = 44,
    Depth16Unorm = 45,
    Depth24Plus = 46,
    Depth24PlusStencil8 = 47,
    Depth32Float = 48,
    Depth32FloatStencil8 = 49,
    BC1RGBAUnorm = 50,
    BC1RGBAUnormSrgb = 51,
    BC2RGBAUnorm = 52,
    BC2RGBAUnormSrgb = 53,
    BC3RGBAUnorm = 54,
    BC3RGBAUnormSrgb = 55,
    BC4RUnorm = 56,
    BC4RSnorm = 57,
    BC5RGUnorm = 58,
    BC5RGSnorm = 59,
    BC6HRGBUfloat = 60,
    BC6HRGBFloat = 61,
    BC7RGBAUnorm = 62,
    BC7RGBAUnormSrgb = 63,
    ETC2RGB8Unorm = 64,
    ETC2RGB8UnormSrgb = 65,
    ETC2RGB8A1Unorm = 66,
    ETC2RGB8A1UnormSrgb = 67,
    ETC2RGBA8Unorm = 68,
    ETC2RGBA8UnormSrgb = 69,
    EACR11Unorm = 70,
    EACR11Snorm = 71,
    EACRG11Unorm = 72,
    EACRG11Snorm = 73,
    ASTC4x4Unorm = 74,
    ASTC4x4UnormSrgb = 75,
    ASTC5x4Unorm = 76,
    ASTC5x4UnormSrgb = 77,
    ASTC5x5Unorm = 78,
    ASTC5x5UnormSrgb = 79,
    ASTC6x5Unorm = 80,
    ASTC6x5UnormSrgb = 81,
    ASTC6x6Unorm = 82,
    ASTC6x6UnormSrgb = 83,
    ASTC8x5Unorm = 84,
    ASTC8x5UnormSrgb = 85,
    ASTC8x6Unorm = 86,
    ASTC8x6UnormSrgb = 87,
    ASTC8x8Unorm = 88,
    ASTC8x8UnormSrgb = 89,
    ASTC10x5Unorm = 90,
    ASTC10x5UnormSrgb = 91,
    ASTC10x6Unorm = 92,
    ASTC10x6UnormSrgb = 93,
    ASTC10x8Unorm = 94,
    ASTC10x8UnormSrgb = 95,
    ASTC10x10Unorm = 96,
    ASTC10x10UnormSrgb = 97,
    ASTC12x10Unorm = 98,
    ASTC12x10UnormSrgb = 99,
    ASTC12x12Unorm = 100,
    ASTC12x12UnormSrgb = 101,
};

enum class WGPUTextureDimension : uint {
    Undefined = 0,
    D1 = 1,
    D2 = 2,
    D3 = 3,
};

enum class WGPUTextureViewDimension : uint {
    Undefined = 0,
    D1 = 1,
    D2 = 2,
    D2Array = 3,
    Cube = 4,
    CubeArray = 5,
    D3 = 6,
};

enum class WGPUTextureAspect : uint {
    Undefined = 0,
    All = 1,
    StencilOnly = 2,
    DepthOnly = 3,
};

enum class WGPULoadOp : uint {
    Undefined = 0,
    Load = 1,
    Clear = 2,
};

enum class WGPUStoreOp : uint {
    Undefined = 0,
    Store = 1,
    Discard = 2,
};

enum class WGPUPrimitiveTopology : uint {
    Undefined = 0,
    PointList = 1,
    LineList = 2,
    LineStrip = 3,
    TriangleList = 4,
    TriangleStrip = 5,
};

enum class WGPUIndexFormat : uint {
    Undefined = 0,
    Uint16 = 1,
    Uint32 = 2,
};

enum class WGPUFrontFace : uint {
    Undefined = 0,
    CCW = 1,
    CW = 2,
};

enum class WGPUCullMode : uint {
    Undefined = 0,
    None = 1,
    Front = 2,
    Back = 3,
};

enum class WGPUCompareFunction : uint {
    Undefined = 0,
    Never = 1,
    Less = 2,
    Equal = 3,
    LessEqual = 4,
    Greater = 5,
    NotEqual = 6,
    GreaterEqual = 7,
    Always = 8,
};

enum class WGPUStencilOperation : uint {
    Undefined = 0,
    Keep = 1,
    Zero = 2,
    Replace = 3,
    Invert = 4,
    IncrementClamp = 5,
    DecrementClamp = 6,
    IncrementWrap = 7,
    DecrementWrap = 8,
};

enum class WGPUBlendFactor : uint {
    Undefined = 0,
    Zero = 1,
    One = 2,
    Src = 3,
    OneMinusSrc = 4,
    SrcAlpha = 5,
    OneMinusSrcAlpha = 6,
    Dst = 7,
    OneMinusDst = 8,
    DstAlpha = 9,
    OneMinusDstAlpha = 10,
    SrcAlphaSaturated = 11,
    Constant = 12,
    OneMinusConstant = 13,
    Src1 = 14,
    OneMinusSrc1 = 15,
    Src1Alpha = 16,
    OneMinusSrc1Alpha = 17,
};

enum class WGPUBlendOperation : uint {
    Undefined = 0,
    Add = 1,
    Subtract = 2,
    ReverseSubtract = 3,
    Min = 4,
    Max = 5,
};

enum class WGPUVertexStepMode : uint {
    Undefined = 0,
    Vertex = 1,
    Instance = 2,
};

enum class WGPUVertexFormat : uint {
    Uint8 = 1,
    Uint8x2 = 2,
    Uint8x4 = 3,
    Sint8 = 4,
    Sint8x2 = 5,
    Sint8x4 = 6,
    Unorm8 = 7,
    Unorm8x2 = 8,
    Unorm8x4 = 9,
    Snorm8 = 10,
    Snorm8x2 = 11,
    Snorm8x4 = 12,
    Uint16 = 13,
    Uint16x2 = 14,
    Uint16x4 = 15,
    Sint16 = 16,
    Sint16x2 = 17,
    Sint16x4 = 18,
    Unorm16 = 19,
    Unorm16x2 = 20,
    Unorm16x4 = 21,
    Snorm16 = 22,
    Snorm16x2 = 23,
    Snorm16x4 = 24,
    Float16 = 25,
    Float16x2 = 26,
    Float16x4 = 27,
    Float32 = 28,
    Float32x2 = 29,
    Float32x3 = 30,
    Float32x4 = 31,
    Uint32 = 32,
    Uint32x2 = 33,
    Uint32x3 = 34,
    Uint32x4 = 35,
    Sint32 = 36,
    Sint32x2 = 37,
    Sint32x3 = 38,
    Sint32x4 = 39,
    Unorm10_10_10_2 = 40,
    Unorm8x4BGRA = 41,
    Snorm10_10_10_2 = 42,
};

enum class WGPUAddressMode : uint {
    Undefined = 0,
    ClampToEdge = 1,
    Repeat = 2,
    MirrorRepeat = 3,
};

enum class WGPUFilterMode : uint {
    Undefined = 0,
    Nearest = 1,
    Linear = 2,
};

enum class WGPUMipmapFilterMode : uint {
    Undefined = 0,
    Nearest = 1,
    Linear = 2,
};

enum class WGPUBufferBindingType : uint {
    BindingNotUsed = 0,
    Undefined = 1,
    Uniform = 2,
    Storage = 3,
    ReadOnlyStorage = 4,
};

enum class WGPUSamplerBindingType : uint {
    BindingNotUsed = 0,
    Undefined = 1,
    Filtering = 2,
    NonFiltering = 3,
    Comparison = 4,
};

enum class WGPUTextureSampleType : uint {
    BindingNotUsed = 0,
    Undefined = 1,
    Float = 2,
    UnfilterableFloat = 3,
    Depth = 4,
    Sint = 5,
    Uint = 6,
};

enum class WGPUStorageTextureAccess : uint {
    BindingNotUsed = 0,
    Undefined = 1,
    WriteOnly = 2,
    ReadOnly = 3,
    ReadWrite = 4,
};

// ----------------------------------------------------------------------------
// Chained Structs (descriptor extensions, e.g. WGSL shader source)
// ----------------------------------------------------------------------------

struct WGPUChainedStruct {
    const WGPUChainedStruct* next;
    WGPUSType sType;
};

static_assert(sizeof(WGPUChainedStruct) == 8);
static_assert(__builtin_offsetof(WGPUChainedStruct, next) == 0);
static_assert(__builtin_offsetof(WGPUChainedStruct, sType) == 4);

struct WGPUShaderSourceWGSL {
    WGPUChainedStruct chain = { nullptr, WGPUSType::ShaderSourceWGSL };
    WGPUStringView code;
};

static_assert(sizeof(WGPUShaderSourceWGSL) == 16);
static_assert(__builtin_offsetof(WGPUShaderSourceWGSL, chain) == 0);
static_assert(__builtin_offsetof(WGPUShaderSourceWGSL, code) == 8);

// ----------------------------------------------------------------------------
// Usage Bitflags (values match webgpu.h; WGPUFlags is 64-bit)
// ----------------------------------------------------------------------------

// Bitflag enums: combine with |, test with (a & b) != Type::None.
// operator~ keeps only the bits defined for the type, so `~X` alone is a valid flag set.
#define WGPU_DEFINE_FLAG_OPERATORS(T, AllBits)                                              \
    constexpr T operator|(T a, T b) { return (T)((WGPUFlags)a | (WGPUFlags)b); }            \
    constexpr T operator&(T a, T b) { return (T)((WGPUFlags)a & (WGPUFlags)b); }            \
    constexpr T operator^(T a, T b) { return (T)((WGPUFlags)a ^ (WGPUFlags)b); }            \
    constexpr T operator~(T a) { return (T)(~(WGPUFlags)a & (WGPUFlags)(AllBits)); }        \
    constexpr T& operator|=(T& a, T b) { return a = a | b; }                                \
    constexpr T& operator&=(T& a, T b) { return a = a & b; }                                \
    constexpr T& operator^=(T& a, T b) { return a = a ^ b; }

enum class WGPUBufferUsage : WGPUFlags {
    None = 0x0000,
    MapRead = 0x0001,
    MapWrite = 0x0002,
    CopySrc = 0x0004,
    CopyDst = 0x0008,
    Index = 0x0010,
    Vertex = 0x0020,
    Uniform = 0x0040,
    Storage = 0x0080,
    Indirect = 0x0100,
    QueryResolve = 0x0200,
};
WGPU_DEFINE_FLAG_OPERATORS(WGPUBufferUsage, 0x03FF)

enum class WGPUTextureUsage : WGPUFlags {
    None = 0x00,
    CopySrc = 0x01,
    CopyDst = 0x02,
    TextureBinding = 0x04,
    StorageBinding = 0x08,
    RenderAttachment = 0x10,
    TransientAttachment = 0x20,
};
WGPU_DEFINE_FLAG_OPERATORS(WGPUTextureUsage, 0x3F)

enum class WGPUShaderStage : WGPUFlags {
    None = 0x0,
    Vertex = 0x1,
    Fragment = 0x2,
    Compute = 0x4,
};
WGPU_DEFINE_FLAG_OPERATORS(WGPUShaderStage, 0x7)

enum class WGPUColorWriteMask : WGPUFlags {
    None = 0x0,
    Red = 0x1,
    Green = 0x2,
    Blue = 0x4,
    Alpha = 0x8,
    All = 0xF,
};
WGPU_DEFINE_FLAG_OPERATORS(WGPUColorWriteMask, 0xF)
#undef WGPU_DEFINE_FLAG_OPERATORS

// ----------------------------------------------------------------------------
// Value Types & Descriptors (camelCase fields matching webgpu.h)
// ----------------------------------------------------------------------------

struct WGPUColor {
    double r;
    double g;
    double b;
    double a;
};

static_assert(sizeof(WGPUColor) == 32);
static_assert(__builtin_offsetof(WGPUColor, r) == 0);
static_assert(__builtin_offsetof(WGPUColor, g) == 8);
static_assert(__builtin_offsetof(WGPUColor, b) == 16);
static_assert(__builtin_offsetof(WGPUColor, a) == 24);

struct WGPUOrigin3D {
    uint x;
    uint y;
    uint z;
};

static_assert(sizeof(WGPUOrigin3D) == 12);
static_assert(__builtin_offsetof(WGPUOrigin3D, x) == 0);
static_assert(__builtin_offsetof(WGPUOrigin3D, y) == 4);
static_assert(__builtin_offsetof(WGPUOrigin3D, z) == 8);

struct WGPUExtent3D {
    uint width;
    uint height = 1;
    uint depthOrArrayLayers = 1;
};

static_assert(sizeof(WGPUExtent3D) == 12);
static_assert(__builtin_offsetof(WGPUExtent3D, width) == 0);
static_assert(__builtin_offsetof(WGPUExtent3D, height) == 4);
static_assert(__builtin_offsetof(WGPUExtent3D, depthOrArrayLayers) == 8);

// width/height/presentMode are accepted for layout compatibility but ignored: the
// host page owns the canvas size and the browser always presents with vsync.
struct WGPUSurfaceConfiguration {
    const WGPUChainedStruct* nextInChain;
    WGPUDevice device;
    WGPUTextureFormat format;
    WGPUTextureUsage usage = WGPUTextureUsage::RenderAttachment;
    uint width;
    uint height;
    usz viewFormatCount;
    const WGPUTextureFormat* viewFormats;
    WGPUCompositeAlphaMode alphaMode = WGPUCompositeAlphaMode::Auto;
    WGPUPresentMode presentMode = WGPUPresentMode::Undefined;
};

static_assert(sizeof(WGPUSurfaceConfiguration) == 48);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, device) == 4);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, format) == 8);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, usage) == 16);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, width) == 24);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, height) == 28);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, viewFormatCount) == 32);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, viewFormats) == 36);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, alphaMode) == 40);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, presentMode) == 44);

struct WGPUSurfaceTexture {
    const WGPUChainedStruct* nextInChain;
    WGPUTexture texture;
    WGPUSurfaceGetCurrentTextureStatus status;
};

static_assert(sizeof(WGPUSurfaceTexture) == 12);
static_assert(__builtin_offsetof(WGPUSurfaceTexture, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUSurfaceTexture, texture) == 4);
static_assert(__builtin_offsetof(WGPUSurfaceTexture, status) == 8);

struct WGPUBufferDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
    WGPUBufferUsage usage;
    ulong size;
    WGPUBool mappedAtCreation; // unsupported: wgpuDeviceCreateBuffer traps if set (no buffer mapping yet)
};

static_assert(sizeof(WGPUBufferDescriptor) == 40);
static_assert(__builtin_offsetof(WGPUBufferDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUBufferDescriptor, label) == 4);
static_assert(__builtin_offsetof(WGPUBufferDescriptor, usage) == 16);
static_assert(__builtin_offsetof(WGPUBufferDescriptor, size) == 24);
static_assert(__builtin_offsetof(WGPUBufferDescriptor, mappedAtCreation) == 32);

struct WGPUTextureDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
    WGPUTextureUsage usage;
    WGPUTextureDimension dimension = WGPUTextureDimension::D2;
    WGPUExtent3D size;
    WGPUTextureFormat format;
    uint mipLevelCount = 1;
    uint sampleCount = 1;
    usz viewFormatCount;
    const WGPUTextureFormat* viewFormats;
};

static_assert(sizeof(WGPUTextureDescriptor) == 64);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, label) == 4);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, usage) == 16);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, dimension) == 24);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, size) == 28);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, format) == 40);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, mipLevelCount) == 44);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, sampleCount) == 48);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, viewFormatCount) == 52);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, viewFormats) == 56);

struct WGPUTextureViewDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
    WGPUTextureFormat format;
    WGPUTextureViewDimension dimension;
    uint baseMipLevel;
    uint mipLevelCount = WGPU_MIP_LEVEL_COUNT_UNDEFINED;
    uint baseArrayLayer;
    uint arrayLayerCount = WGPU_ARRAY_LAYER_COUNT_UNDEFINED;
    WGPUTextureAspect aspect = WGPUTextureAspect::Undefined;
    WGPUTextureUsage usage;
};

static_assert(sizeof(WGPUTextureViewDescriptor) == 48);
static_assert(__builtin_offsetof(WGPUTextureViewDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUTextureViewDescriptor, label) == 4);
static_assert(__builtin_offsetof(WGPUTextureViewDescriptor, format) == 12);
static_assert(__builtin_offsetof(WGPUTextureViewDescriptor, dimension) == 16);
static_assert(__builtin_offsetof(WGPUTextureViewDescriptor, baseMipLevel) == 20);
static_assert(__builtin_offsetof(WGPUTextureViewDescriptor, mipLevelCount) == 24);
static_assert(__builtin_offsetof(WGPUTextureViewDescriptor, baseArrayLayer) == 28);
static_assert(__builtin_offsetof(WGPUTextureViewDescriptor, arrayLayerCount) == 32);
static_assert(__builtin_offsetof(WGPUTextureViewDescriptor, aspect) == 36);
static_assert(__builtin_offsetof(WGPUTextureViewDescriptor, usage) == 40);

struct WGPUSamplerDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
    WGPUAddressMode addressModeU = WGPUAddressMode::ClampToEdge;
    WGPUAddressMode addressModeV = WGPUAddressMode::ClampToEdge;
    WGPUAddressMode addressModeW = WGPUAddressMode::ClampToEdge;
    WGPUFilterMode magFilter = WGPUFilterMode::Nearest;
    WGPUFilterMode minFilter = WGPUFilterMode::Nearest;
    WGPUMipmapFilterMode mipmapFilter = WGPUMipmapFilterMode::Nearest;
    float lodMinClamp = 0.0f;
    float lodMaxClamp = 32.0f;
    WGPUCompareFunction compare = WGPUCompareFunction::Undefined;
    ushort maxAnisotropy = 1;
};

static_assert(sizeof(WGPUSamplerDescriptor) == 52);
static_assert(__builtin_offsetof(WGPUSamplerDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUSamplerDescriptor, label) == 4);
static_assert(__builtin_offsetof(WGPUSamplerDescriptor, addressModeU) == 12);
static_assert(__builtin_offsetof(WGPUSamplerDescriptor, addressModeV) == 16);
static_assert(__builtin_offsetof(WGPUSamplerDescriptor, addressModeW) == 20);
static_assert(__builtin_offsetof(WGPUSamplerDescriptor, magFilter) == 24);
static_assert(__builtin_offsetof(WGPUSamplerDescriptor, minFilter) == 28);
static_assert(__builtin_offsetof(WGPUSamplerDescriptor, mipmapFilter) == 32);
static_assert(__builtin_offsetof(WGPUSamplerDescriptor, lodMinClamp) == 36);
static_assert(__builtin_offsetof(WGPUSamplerDescriptor, lodMaxClamp) == 40);
static_assert(__builtin_offsetof(WGPUSamplerDescriptor, compare) == 44);
static_assert(__builtin_offsetof(WGPUSamplerDescriptor, maxAnisotropy) == 48);

// Source is supplied by chaining a WGPUShaderSourceWGSL through nextInChain.
struct WGPUShaderModuleDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
};

static_assert(sizeof(WGPUShaderModuleDescriptor) == 12);
static_assert(__builtin_offsetof(WGPUShaderModuleDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUShaderModuleDescriptor, label) == 4);

struct WGPUBufferBindingLayout {
    const WGPUChainedStruct* nextInChain;
    WGPUBufferBindingType type;
    WGPUBool hasDynamicOffset;
    ulong minBindingSize;
};

static_assert(sizeof(WGPUBufferBindingLayout) == 24);
static_assert(__builtin_offsetof(WGPUBufferBindingLayout, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUBufferBindingLayout, type) == 4);
static_assert(__builtin_offsetof(WGPUBufferBindingLayout, hasDynamicOffset) == 8);
static_assert(__builtin_offsetof(WGPUBufferBindingLayout, minBindingSize) == 16);

struct WGPUSamplerBindingLayout {
    const WGPUChainedStruct* nextInChain;
    WGPUSamplerBindingType type;
};

static_assert(sizeof(WGPUSamplerBindingLayout) == 8);
static_assert(__builtin_offsetof(WGPUSamplerBindingLayout, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUSamplerBindingLayout, type) == 4);

struct WGPUTextureBindingLayout {
    const WGPUChainedStruct* nextInChain;
    WGPUTextureSampleType sampleType;
    WGPUTextureViewDimension viewDimension;
    WGPUBool multisampled;
};

static_assert(sizeof(WGPUTextureBindingLayout) == 16);
static_assert(__builtin_offsetof(WGPUTextureBindingLayout, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUTextureBindingLayout, sampleType) == 4);
static_assert(__builtin_offsetof(WGPUTextureBindingLayout, viewDimension) == 8);
static_assert(__builtin_offsetof(WGPUTextureBindingLayout, multisampled) == 12);

struct WGPUStorageTextureBindingLayout {
    const WGPUChainedStruct* nextInChain;
    WGPUStorageTextureAccess access;
    WGPUTextureFormat format;
    WGPUTextureViewDimension viewDimension;
};

static_assert(sizeof(WGPUStorageTextureBindingLayout) == 16);
static_assert(__builtin_offsetof(WGPUStorageTextureBindingLayout, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUStorageTextureBindingLayout, access) == 4);
static_assert(__builtin_offsetof(WGPUStorageTextureBindingLayout, format) == 8);
static_assert(__builtin_offsetof(WGPUStorageTextureBindingLayout, viewDimension) == 12);

struct WGPUBindGroupLayoutEntry {
    const WGPUChainedStruct* nextInChain;
    uint binding;
    WGPUShaderStage visibility;
    uint bindingArraySize; // unsupported by browsers; must be 0
    WGPUBufferBindingLayout buffer;
    WGPUSamplerBindingLayout sampler;
    WGPUTextureBindingLayout texture;
    WGPUStorageTextureBindingLayout storageTexture;
};

static_assert(sizeof(WGPUBindGroupLayoutEntry) == 88);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutEntry, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutEntry, binding) == 4);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutEntry, visibility) == 8);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutEntry, bindingArraySize) == 16);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutEntry, buffer) == 24);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutEntry, sampler) == 48);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutEntry, texture) == 56);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutEntry, storageTexture) == 72);

struct WGPUBindGroupLayoutDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
    usz entryCount;
    const WGPUBindGroupLayoutEntry* entries;
};

static_assert(sizeof(WGPUBindGroupLayoutDescriptor) == 20);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutDescriptor, label) == 4);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutDescriptor, entryCount) == 12);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutDescriptor, entries) == 16);

struct WGPUBindGroupEntry {
    const WGPUChainedStruct* nextInChain;
    uint binding;
    WGPUBuffer buffer;
    ulong offset;
    ulong size = WGPU_WHOLE_SIZE;
    WGPUSampler sampler;
    WGPUTextureView textureView;
};

static_assert(sizeof(WGPUBindGroupEntry) == 40);
static_assert(__builtin_offsetof(WGPUBindGroupEntry, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUBindGroupEntry, binding) == 4);
static_assert(__builtin_offsetof(WGPUBindGroupEntry, buffer) == 8);
static_assert(__builtin_offsetof(WGPUBindGroupEntry, offset) == 16);
static_assert(__builtin_offsetof(WGPUBindGroupEntry, size) == 24);
static_assert(__builtin_offsetof(WGPUBindGroupEntry, sampler) == 32);
static_assert(__builtin_offsetof(WGPUBindGroupEntry, textureView) == 36);

struct WGPUBindGroupDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
    WGPUBindGroupLayout layout;
    usz entryCount;
    const WGPUBindGroupEntry* entries;
};

static_assert(sizeof(WGPUBindGroupDescriptor) == 24);
static_assert(__builtin_offsetof(WGPUBindGroupDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUBindGroupDescriptor, label) == 4);
static_assert(__builtin_offsetof(WGPUBindGroupDescriptor, layout) == 12);
static_assert(__builtin_offsetof(WGPUBindGroupDescriptor, entryCount) == 16);
static_assert(__builtin_offsetof(WGPUBindGroupDescriptor, entries) == 20);

struct WGPUPipelineLayoutDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
    usz bindGroupLayoutCount;
    const WGPUBindGroupLayout* bindGroupLayouts;
    uint immediateSize; // unsupported by browsers; must be 0
};

static_assert(sizeof(WGPUPipelineLayoutDescriptor) == 24);
static_assert(__builtin_offsetof(WGPUPipelineLayoutDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUPipelineLayoutDescriptor, label) == 4);
static_assert(__builtin_offsetof(WGPUPipelineLayoutDescriptor, bindGroupLayoutCount) == 12);
static_assert(__builtin_offsetof(WGPUPipelineLayoutDescriptor, bindGroupLayouts) == 16);
static_assert(__builtin_offsetof(WGPUPipelineLayoutDescriptor, immediateSize) == 20);

struct WGPUVertexAttribute {
    const WGPUChainedStruct* nextInChain;
    WGPUVertexFormat format;
    ulong offset;
    uint shaderLocation;
};

static_assert(sizeof(WGPUVertexAttribute) == 24);
static_assert(__builtin_offsetof(WGPUVertexAttribute, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUVertexAttribute, format) == 4);
static_assert(__builtin_offsetof(WGPUVertexAttribute, offset) == 8);
static_assert(__builtin_offsetof(WGPUVertexAttribute, shaderLocation) == 16);

struct WGPUVertexBufferLayout {
    const WGPUChainedStruct* nextInChain;
    WGPUVertexStepMode stepMode = WGPUVertexStepMode::Vertex;
    ulong arrayStride;
    usz attributeCount;
    const WGPUVertexAttribute* attributes;
};

static_assert(sizeof(WGPUVertexBufferLayout) == 24);
static_assert(__builtin_offsetof(WGPUVertexBufferLayout, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUVertexBufferLayout, stepMode) == 4);
static_assert(__builtin_offsetof(WGPUVertexBufferLayout, arrayStride) == 8);
static_assert(__builtin_offsetof(WGPUVertexBufferLayout, attributeCount) == 16);
static_assert(__builtin_offsetof(WGPUVertexBufferLayout, attributes) == 20);

struct WGPUConstantEntry {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView key;
    double value;
};

static_assert(sizeof(WGPUConstantEntry) == 24);
static_assert(__builtin_offsetof(WGPUConstantEntry, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUConstantEntry, key) == 4);
static_assert(__builtin_offsetof(WGPUConstantEntry, value) == 16);

struct WGPUVertexState {
    const WGPUChainedStruct* nextInChain;
    WGPUShaderModule module;
    WGPUStringView entryPoint;
    usz constantCount;
    const WGPUConstantEntry* constants;
    usz bufferCount;
    const WGPUVertexBufferLayout* buffers;
};

static_assert(sizeof(WGPUVertexState) == 32);
static_assert(__builtin_offsetof(WGPUVertexState, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUVertexState, module) == 4);
static_assert(__builtin_offsetof(WGPUVertexState, entryPoint) == 8);
static_assert(__builtin_offsetof(WGPUVertexState, constantCount) == 16);
static_assert(__builtin_offsetof(WGPUVertexState, constants) == 20);
static_assert(__builtin_offsetof(WGPUVertexState, bufferCount) == 24);
static_assert(__builtin_offsetof(WGPUVertexState, buffers) == 28);

struct WGPUPrimitiveState {
    const WGPUChainedStruct* nextInChain;
    WGPUPrimitiveTopology topology = WGPUPrimitiveTopology::TriangleList;
    WGPUIndexFormat stripIndexFormat = WGPUIndexFormat::Undefined;
    WGPUFrontFace frontFace = WGPUFrontFace::CCW;
    WGPUCullMode cullMode = WGPUCullMode::None;
    WGPUBool unclippedDepth = WGPU_FALSE;
};

static_assert(sizeof(WGPUPrimitiveState) == 24);
static_assert(__builtin_offsetof(WGPUPrimitiveState, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUPrimitiveState, topology) == 4);
static_assert(__builtin_offsetof(WGPUPrimitiveState, stripIndexFormat) == 8);
static_assert(__builtin_offsetof(WGPUPrimitiveState, frontFace) == 12);
static_assert(__builtin_offsetof(WGPUPrimitiveState, cullMode) == 16);
static_assert(__builtin_offsetof(WGPUPrimitiveState, unclippedDepth) == 20);

struct WGPUStencilFaceState {
    WGPUCompareFunction compare = WGPUCompareFunction::Always;
    WGPUStencilOperation failOp = WGPUStencilOperation::Keep;
    WGPUStencilOperation depthFailOp = WGPUStencilOperation::Keep;
    WGPUStencilOperation passOp = WGPUStencilOperation::Keep;
};

static_assert(sizeof(WGPUStencilFaceState) == 16);
static_assert(__builtin_offsetof(WGPUStencilFaceState, compare) == 0);
static_assert(__builtin_offsetof(WGPUStencilFaceState, failOp) == 4);
static_assert(__builtin_offsetof(WGPUStencilFaceState, depthFailOp) == 8);
static_assert(__builtin_offsetof(WGPUStencilFaceState, passOp) == 12);

struct WGPUDepthStencilState {
    const WGPUChainedStruct* nextInChain;
    WGPUTextureFormat format;
    WGPUOptionalBool depthWriteEnabled = WGPUOptionalBool::Undefined;
    WGPUCompareFunction depthCompare = WGPUCompareFunction::Undefined;
    WGPUStencilFaceState stencilFront;
    WGPUStencilFaceState stencilBack;
    uint stencilReadMask = 0xFFFFFFFFu;
    uint stencilWriteMask = 0xFFFFFFFFu;
    int depthBias = 0;
    float depthBiasSlopeScale = 0.0f;
    float depthBiasClamp = 0.0f;
};

static_assert(sizeof(WGPUDepthStencilState) == 68);
static_assert(__builtin_offsetof(WGPUDepthStencilState, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUDepthStencilState, format) == 4);
static_assert(__builtin_offsetof(WGPUDepthStencilState, depthWriteEnabled) == 8);
static_assert(__builtin_offsetof(WGPUDepthStencilState, depthCompare) == 12);
static_assert(__builtin_offsetof(WGPUDepthStencilState, stencilFront) == 16);
static_assert(__builtin_offsetof(WGPUDepthStencilState, stencilBack) == 32);
static_assert(__builtin_offsetof(WGPUDepthStencilState, stencilReadMask) == 48);
static_assert(__builtin_offsetof(WGPUDepthStencilState, stencilWriteMask) == 52);
static_assert(__builtin_offsetof(WGPUDepthStencilState, depthBias) == 56);
static_assert(__builtin_offsetof(WGPUDepthStencilState, depthBiasSlopeScale) == 60);
static_assert(__builtin_offsetof(WGPUDepthStencilState, depthBiasClamp) == 64);

struct WGPUMultisampleState {
    const WGPUChainedStruct* nextInChain;
    uint count = 1;
    uint mask = 0xFFFFFFFFu;
    WGPUBool alphaToCoverageEnabled = WGPU_FALSE;
};

static_assert(sizeof(WGPUMultisampleState) == 16);
static_assert(__builtin_offsetof(WGPUMultisampleState, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUMultisampleState, count) == 4);
static_assert(__builtin_offsetof(WGPUMultisampleState, mask) == 8);
static_assert(__builtin_offsetof(WGPUMultisampleState, alphaToCoverageEnabled) == 12);

struct WGPUBlendComponent {
    WGPUBlendOperation operation = WGPUBlendOperation::Add;
    WGPUBlendFactor srcFactor = WGPUBlendFactor::One;
    WGPUBlendFactor dstFactor = WGPUBlendFactor::Zero;
};

static_assert(sizeof(WGPUBlendComponent) == 12);
static_assert(__builtin_offsetof(WGPUBlendComponent, operation) == 0);
static_assert(__builtin_offsetof(WGPUBlendComponent, srcFactor) == 4);
static_assert(__builtin_offsetof(WGPUBlendComponent, dstFactor) == 8);

struct WGPUBlendState {
    WGPUBlendComponent color;
    WGPUBlendComponent alpha;
};

static_assert(sizeof(WGPUBlendState) == 24);
static_assert(__builtin_offsetof(WGPUBlendState, color) == 0);
static_assert(__builtin_offsetof(WGPUBlendState, alpha) == 12);

struct WGPUColorTargetState {
    const WGPUChainedStruct* nextInChain;
    WGPUTextureFormat format;
    const WGPUBlendState* blend;
    WGPUColorWriteMask writeMask = WGPUColorWriteMask::All;
};

static_assert(sizeof(WGPUColorTargetState) == 24);
static_assert(__builtin_offsetof(WGPUColorTargetState, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUColorTargetState, format) == 4);
static_assert(__builtin_offsetof(WGPUColorTargetState, blend) == 8);
static_assert(__builtin_offsetof(WGPUColorTargetState, writeMask) == 16);

struct WGPUFragmentState {
    const WGPUChainedStruct* nextInChain;
    WGPUShaderModule module;
    WGPUStringView entryPoint;
    usz constantCount;
    const WGPUConstantEntry* constants;
    usz targetCount;
    const WGPUColorTargetState* targets;
};

static_assert(sizeof(WGPUFragmentState) == 32);
static_assert(__builtin_offsetof(WGPUFragmentState, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUFragmentState, module) == 4);
static_assert(__builtin_offsetof(WGPUFragmentState, entryPoint) == 8);
static_assert(__builtin_offsetof(WGPUFragmentState, constantCount) == 16);
static_assert(__builtin_offsetof(WGPUFragmentState, constants) == 20);
static_assert(__builtin_offsetof(WGPUFragmentState, targetCount) == 24);
static_assert(__builtin_offsetof(WGPUFragmentState, targets) == 28);

struct WGPURenderPipelineDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
    WGPUPipelineLayout layout; // {0} means "auto"
    WGPUVertexState vertex;
    WGPUPrimitiveState primitive;
    const WGPUDepthStencilState* depthStencil;
    WGPUMultisampleState multisample;
    const WGPUFragmentState* fragment;
};

static_assert(sizeof(WGPURenderPipelineDescriptor) == 96);
static_assert(__builtin_offsetof(WGPURenderPipelineDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPURenderPipelineDescriptor, label) == 4);
static_assert(__builtin_offsetof(WGPURenderPipelineDescriptor, layout) == 12);
static_assert(__builtin_offsetof(WGPURenderPipelineDescriptor, vertex) == 16);
static_assert(__builtin_offsetof(WGPURenderPipelineDescriptor, primitive) == 48);
static_assert(__builtin_offsetof(WGPURenderPipelineDescriptor, depthStencil) == 72);
static_assert(__builtin_offsetof(WGPURenderPipelineDescriptor, multisample) == 76);
static_assert(__builtin_offsetof(WGPURenderPipelineDescriptor, fragment) == 92);

struct WGPUComputeState {
    const WGPUChainedStruct* nextInChain;
    WGPUShaderModule module;
    WGPUStringView entryPoint;
    usz constantCount;
    const WGPUConstantEntry* constants;
};

static_assert(sizeof(WGPUComputeState) == 24);
static_assert(__builtin_offsetof(WGPUComputeState, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUComputeState, module) == 4);
static_assert(__builtin_offsetof(WGPUComputeState, entryPoint) == 8);
static_assert(__builtin_offsetof(WGPUComputeState, constantCount) == 16);
static_assert(__builtin_offsetof(WGPUComputeState, constants) == 20);

struct WGPUComputePipelineDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
    WGPUPipelineLayout layout; // {0} means "auto"
    WGPUComputeState compute;
};

static_assert(sizeof(WGPUComputePipelineDescriptor) == 40);
static_assert(__builtin_offsetof(WGPUComputePipelineDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUComputePipelineDescriptor, label) == 4);
static_assert(__builtin_offsetof(WGPUComputePipelineDescriptor, layout) == 12);
static_assert(__builtin_offsetof(WGPUComputePipelineDescriptor, compute) == 16);

struct WGPUCommandEncoderDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
};

static_assert(sizeof(WGPUCommandEncoderDescriptor) == 12);
static_assert(__builtin_offsetof(WGPUCommandEncoderDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUCommandEncoderDescriptor, label) == 4);

struct WGPUCommandBufferDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
};

static_assert(sizeof(WGPUCommandBufferDescriptor) == 12);
static_assert(__builtin_offsetof(WGPUCommandBufferDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUCommandBufferDescriptor, label) == 4);

struct WGPUPassTimestampWrites {
    const WGPUChainedStruct* nextInChain;
    WGPUQuerySet querySet;
    uint beginningOfPassWriteIndex = WGPU_QUERY_SET_INDEX_UNDEFINED;
    uint endOfPassWriteIndex = WGPU_QUERY_SET_INDEX_UNDEFINED;
};

static_assert(sizeof(WGPUPassTimestampWrites) == 16);
static_assert(__builtin_offsetof(WGPUPassTimestampWrites, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUPassTimestampWrites, querySet) == 4);
static_assert(__builtin_offsetof(WGPUPassTimestampWrites, beginningOfPassWriteIndex) == 8);
static_assert(__builtin_offsetof(WGPUPassTimestampWrites, endOfPassWriteIndex) == 12);

struct WGPURenderPassColorAttachment {
    const WGPUChainedStruct* nextInChain;
    WGPUTextureView view;
    uint depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
    WGPUTextureView resolveTarget;
    WGPULoadOp loadOp;
    WGPUStoreOp storeOp;
    WGPUColor clearValue;
};

static_assert(sizeof(WGPURenderPassColorAttachment) == 56);
static_assert(__builtin_offsetof(WGPURenderPassColorAttachment, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPURenderPassColorAttachment, view) == 4);
static_assert(__builtin_offsetof(WGPURenderPassColorAttachment, depthSlice) == 8);
static_assert(__builtin_offsetof(WGPURenderPassColorAttachment, resolveTarget) == 12);
static_assert(__builtin_offsetof(WGPURenderPassColorAttachment, loadOp) == 16);
static_assert(__builtin_offsetof(WGPURenderPassColorAttachment, storeOp) == 20);
static_assert(__builtin_offsetof(WGPURenderPassColorAttachment, clearValue) == 24);

struct WGPURenderPassDepthStencilAttachment {
    const WGPUChainedStruct* nextInChain;
    WGPUTextureView view;
    WGPULoadOp depthLoadOp;
    WGPUStoreOp depthStoreOp;
    float depthClearValue = WGPU_DEPTH_CLEAR_VALUE_UNDEFINED;
    WGPUBool depthReadOnly;
    WGPULoadOp stencilLoadOp;
    WGPUStoreOp stencilStoreOp;
    uint stencilClearValue;
    WGPUBool stencilReadOnly;
};

static_assert(sizeof(WGPURenderPassDepthStencilAttachment) == 40);
static_assert(__builtin_offsetof(WGPURenderPassDepthStencilAttachment, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPURenderPassDepthStencilAttachment, view) == 4);
static_assert(__builtin_offsetof(WGPURenderPassDepthStencilAttachment, depthLoadOp) == 8);
static_assert(__builtin_offsetof(WGPURenderPassDepthStencilAttachment, depthStoreOp) == 12);
static_assert(__builtin_offsetof(WGPURenderPassDepthStencilAttachment, depthClearValue) == 16);
static_assert(__builtin_offsetof(WGPURenderPassDepthStencilAttachment, depthReadOnly) == 20);
static_assert(__builtin_offsetof(WGPURenderPassDepthStencilAttachment, stencilLoadOp) == 24);
static_assert(__builtin_offsetof(WGPURenderPassDepthStencilAttachment, stencilStoreOp) == 28);
static_assert(__builtin_offsetof(WGPURenderPassDepthStencilAttachment, stencilClearValue) == 32);
static_assert(__builtin_offsetof(WGPURenderPassDepthStencilAttachment, stencilReadOnly) == 36);

struct WGPURenderPassDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
    usz colorAttachmentCount;
    const WGPURenderPassColorAttachment* colorAttachments;
    const WGPURenderPassDepthStencilAttachment* depthStencilAttachment;
    WGPUQuerySet occlusionQuerySet;                // unsupported: must be null
    const WGPUPassTimestampWrites* timestampWrites; // unsupported: must be null
};

static_assert(sizeof(WGPURenderPassDescriptor) == 32);
static_assert(__builtin_offsetof(WGPURenderPassDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPURenderPassDescriptor, label) == 4);
static_assert(__builtin_offsetof(WGPURenderPassDescriptor, colorAttachmentCount) == 12);
static_assert(__builtin_offsetof(WGPURenderPassDescriptor, colorAttachments) == 16);
static_assert(__builtin_offsetof(WGPURenderPassDescriptor, depthStencilAttachment) == 20);
static_assert(__builtin_offsetof(WGPURenderPassDescriptor, occlusionQuerySet) == 24);
static_assert(__builtin_offsetof(WGPURenderPassDescriptor, timestampWrites) == 28);

struct WGPUComputePassDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
    const WGPUPassTimestampWrites* timestampWrites; // unsupported: must be null
};

static_assert(sizeof(WGPUComputePassDescriptor) == 16);
static_assert(__builtin_offsetof(WGPUComputePassDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUComputePassDescriptor, label) == 4);
static_assert(__builtin_offsetof(WGPUComputePassDescriptor, timestampWrites) == 12);

struct WGPUTexelCopyBufferLayout {
    ulong offset;
    uint bytesPerRow = WGPU_COPY_STRIDE_UNDEFINED;
    uint rowsPerImage = WGPU_COPY_STRIDE_UNDEFINED;
};

static_assert(sizeof(WGPUTexelCopyBufferLayout) == 16);
static_assert(__builtin_offsetof(WGPUTexelCopyBufferLayout, offset) == 0);
static_assert(__builtin_offsetof(WGPUTexelCopyBufferLayout, bytesPerRow) == 8);
static_assert(__builtin_offsetof(WGPUTexelCopyBufferLayout, rowsPerImage) == 12);

struct WGPUTexelCopyBufferInfo {
    WGPUTexelCopyBufferLayout layout;
    WGPUBuffer buffer;
};

static_assert(sizeof(WGPUTexelCopyBufferInfo) == 24);
static_assert(__builtin_offsetof(WGPUTexelCopyBufferInfo, layout) == 0);
static_assert(__builtin_offsetof(WGPUTexelCopyBufferInfo, buffer) == 16);

struct WGPUTexelCopyTextureInfo {
    WGPUTexture texture;
    uint mipLevel;
    WGPUOrigin3D origin;
    WGPUTextureAspect aspect = WGPUTextureAspect::All;
};

static_assert(sizeof(WGPUTexelCopyTextureInfo) == 24);
static_assert(__builtin_offsetof(WGPUTexelCopyTextureInfo, texture) == 0);
static_assert(__builtin_offsetof(WGPUTexelCopyTextureInfo, mipLevel) == 4);
static_assert(__builtin_offsetof(WGPUTexelCopyTextureInfo, origin) == 8);
static_assert(__builtin_offsetof(WGPUTexelCopyTextureInfo, aspect) == 20);

// ----------------------------------------------------------------------------
// Standard webgpu.h Functions
// ----------------------------------------------------------------------------

// WGPUSurface
GPU(wgpuSurfaceConfigure) void wgpuSurfaceConfigure(
    WGPUSurface surface,
    const WGPUSurfaceConfiguration* config
);

GPU(wgpuSurfaceUnconfigure) void wgpuSurfaceUnconfigure(
    WGPUSurface surface
);

GPU(wgpuSurfaceGetCurrentTexture) void wgpuSurfaceGetCurrentTexture(
    WGPUSurface surface,
    WGPUSurfaceTexture* surfaceTexture
);

GPU(wgpuSurfaceGetPreferredFormat) WGPUTextureFormat wgpuSurfaceGetPreferredFormat(
    WGPUSurface surface,
    WGPUAdapter adapter = {}
);

GPU(wgpuSurfacePresent) void wgpuSurfacePresent(
    WGPUSurface surface
);

GPU(wgpuSurfaceAddRef) void wgpuSurfaceAddRef(
    WGPUSurface surface
);

GPU(wgpuSurfaceRelease) void wgpuSurfaceRelease(
    WGPUSurface surface
);

// WGPUAdapter
GPU(wgpuAdapterAddRef) void wgpuAdapterAddRef(
    WGPUAdapter adapter
);

GPU(wgpuAdapterRelease) void wgpuAdapterRelease(
    WGPUAdapter adapter
);

// WGPUDevice
GPU(wgpuDeviceGetQueue) WGPUQueue wgpuDeviceGetQueue(
    WGPUDevice device
);

GPU(wgpuDeviceCreateBuffer) WGPUBuffer wgpuDeviceCreateBuffer(
    WGPUDevice device,
    const WGPUBufferDescriptor* descriptor
);

GPU(wgpuDeviceCreateTexture) WGPUTexture wgpuDeviceCreateTexture(
    WGPUDevice device,
    const WGPUTextureDescriptor* descriptor
);

GPU(wgpuDeviceCreateSampler) WGPUSampler wgpuDeviceCreateSampler(
    WGPUDevice device,
    const WGPUSamplerDescriptor* descriptor = nullptr
);

GPU(wgpuDeviceCreateBindGroupLayout) WGPUBindGroupLayout wgpuDeviceCreateBindGroupLayout(
    WGPUDevice device,
    const WGPUBindGroupLayoutDescriptor* descriptor
);

GPU(wgpuDeviceCreatePipelineLayout) WGPUPipelineLayout wgpuDeviceCreatePipelineLayout(
    WGPUDevice device,
    const WGPUPipelineLayoutDescriptor* descriptor
);

GPU(wgpuDeviceCreateBindGroup) WGPUBindGroup wgpuDeviceCreateBindGroup(
    WGPUDevice device,
    const WGPUBindGroupDescriptor* descriptor
);

GPU(wgpuDeviceCreateShaderModule) WGPUShaderModule wgpuDeviceCreateShaderModule(
    WGPUDevice device,
    const WGPUShaderModuleDescriptor* descriptor
);

GPU(wgpuDeviceCreateRenderPipeline) WGPURenderPipeline wgpuDeviceCreateRenderPipeline(
    WGPUDevice device,
    const WGPURenderPipelineDescriptor* descriptor
);

GPU(wgpuDeviceCreateComputePipeline) WGPUComputePipeline wgpuDeviceCreateComputePipeline(
    WGPUDevice device,
    const WGPUComputePipelineDescriptor* descriptor
);

GPU(wgpuDeviceCreateCommandEncoder) WGPUCommandEncoder wgpuDeviceCreateCommandEncoder(
    WGPUDevice device,
    const WGPUCommandEncoderDescriptor* descriptor = nullptr
);

GPU(wgpuDeviceDestroy) void wgpuDeviceDestroy(
    WGPUDevice device
);

GPU(wgpuDeviceAddRef) void wgpuDeviceAddRef(
    WGPUDevice device
);

GPU(wgpuDeviceRelease) void wgpuDeviceRelease(
    WGPUDevice device
);

// WGPUBuffer
GPU(wgpuBufferDestroy) void wgpuBufferDestroy(
    WGPUBuffer buffer
);

GPU(wgpuBufferAddRef) void wgpuBufferAddRef(
    WGPUBuffer buffer
);

GPU(wgpuBufferRelease) void wgpuBufferRelease(
    WGPUBuffer buffer
);

// WGPUTexture
GPU(wgpuTextureCreateView) WGPUTextureView wgpuTextureCreateView(
    WGPUTexture texture,
    const WGPUTextureViewDescriptor* descriptor = nullptr
);

GPU(wgpuTextureDestroy) void wgpuTextureDestroy(
    WGPUTexture texture
);

GPU(wgpuTextureAddRef) void wgpuTextureAddRef(
    WGPUTexture texture
);

GPU(wgpuTextureRelease) void wgpuTextureRelease(
    WGPUTexture texture
);

// WGPUTextureView
GPU(wgpuTextureViewAddRef) void wgpuTextureViewAddRef(
    WGPUTextureView textureView
);

GPU(wgpuTextureViewRelease) void wgpuTextureViewRelease(
    WGPUTextureView textureView
);

// WGPUSampler / WGPUBindGroupLayout / WGPUBindGroup / WGPUPipelineLayout / WGPUShaderModule
GPU(wgpuSamplerAddRef) void wgpuSamplerAddRef(
    WGPUSampler sampler
);

GPU(wgpuSamplerRelease) void wgpuSamplerRelease(
    WGPUSampler sampler
);

GPU(wgpuBindGroupLayoutAddRef) void wgpuBindGroupLayoutAddRef(
    WGPUBindGroupLayout bindGroupLayout
);

GPU(wgpuBindGroupLayoutRelease) void wgpuBindGroupLayoutRelease(
    WGPUBindGroupLayout bindGroupLayout
);

GPU(wgpuBindGroupAddRef) void wgpuBindGroupAddRef(
    WGPUBindGroup bindGroup
);

GPU(wgpuBindGroupRelease) void wgpuBindGroupRelease(
    WGPUBindGroup bindGroup
);

GPU(wgpuPipelineLayoutAddRef) void wgpuPipelineLayoutAddRef(
    WGPUPipelineLayout pipelineLayout
);

GPU(wgpuPipelineLayoutRelease) void wgpuPipelineLayoutRelease(
    WGPUPipelineLayout pipelineLayout
);

GPU(wgpuShaderModuleAddRef) void wgpuShaderModuleAddRef(
    WGPUShaderModule shaderModule
);

GPU(wgpuShaderModuleRelease) void wgpuShaderModuleRelease(
    WGPUShaderModule shaderModule
);

// WGPURenderPipeline / WGPUComputePipeline
GPU(wgpuRenderPipelineGetBindGroupLayout) WGPUBindGroupLayout wgpuRenderPipelineGetBindGroupLayout(
    WGPURenderPipeline pipeline,
    uint groupIndex
);

GPU(wgpuRenderPipelineAddRef) void wgpuRenderPipelineAddRef(
    WGPURenderPipeline renderPipeline
);

GPU(wgpuRenderPipelineRelease) void wgpuRenderPipelineRelease(
    WGPURenderPipeline renderPipeline
);

GPU(wgpuComputePipelineGetBindGroupLayout) WGPUBindGroupLayout wgpuComputePipelineGetBindGroupLayout(
    WGPUComputePipeline pipeline,
    uint groupIndex
);

GPU(wgpuComputePipelineAddRef) void wgpuComputePipelineAddRef(
    WGPUComputePipeline computePipeline
);

GPU(wgpuComputePipelineRelease) void wgpuComputePipelineRelease(
    WGPUComputePipeline computePipeline
);

// WGPUCommandEncoder
GPU(wgpuCommandEncoderBeginRenderPass) WGPURenderPassEncoder wgpuCommandEncoderBeginRenderPass(
    WGPUCommandEncoder commandEncoder,
    const WGPURenderPassDescriptor* descriptor
);

GPU(wgpuCommandEncoderBeginComputePass) WGPUComputePassEncoder wgpuCommandEncoderBeginComputePass(
    WGPUCommandEncoder commandEncoder,
    const WGPUComputePassDescriptor* descriptor = nullptr
);

GPU(wgpuCommandEncoderCopyBufferToBuffer) void wgpuCommandEncoderCopyBufferToBuffer(
    WGPUCommandEncoder commandEncoder,
    WGPUBuffer source,
    ulong sourceOffset,
    WGPUBuffer destination,
    ulong destinationOffset,
    ulong size
);

GPU(wgpuCommandEncoderCopyBufferToTexture) void wgpuCommandEncoderCopyBufferToTexture(
    WGPUCommandEncoder commandEncoder,
    const WGPUTexelCopyBufferInfo* source,
    const WGPUTexelCopyTextureInfo* destination,
    const WGPUExtent3D* copySize
);

GPU(wgpuCommandEncoderCopyTextureToBuffer) void wgpuCommandEncoderCopyTextureToBuffer(
    WGPUCommandEncoder commandEncoder,
    const WGPUTexelCopyTextureInfo* source,
    const WGPUTexelCopyBufferInfo* destination,
    const WGPUExtent3D* copySize
);

GPU(wgpuCommandEncoderCopyTextureToTexture) void wgpuCommandEncoderCopyTextureToTexture(
    WGPUCommandEncoder commandEncoder,
    const WGPUTexelCopyTextureInfo* source,
    const WGPUTexelCopyTextureInfo* destination,
    const WGPUExtent3D* copySize
);

GPU(wgpuCommandEncoderClearBuffer) void wgpuCommandEncoderClearBuffer(
    WGPUCommandEncoder commandEncoder,
    WGPUBuffer buffer,
    ulong offset = 0,
    ulong size = WGPU_WHOLE_SIZE
);

GPU(wgpuCommandEncoderFinish) WGPUCommandBuffer wgpuCommandEncoderFinish(
    WGPUCommandEncoder commandEncoder,
    const WGPUCommandBufferDescriptor* descriptor = nullptr
);

GPU(wgpuCommandEncoderAddRef) void wgpuCommandEncoderAddRef(
    WGPUCommandEncoder commandEncoder
);

GPU(wgpuCommandEncoderRelease) void wgpuCommandEncoderRelease(
    WGPUCommandEncoder commandEncoder
);

// WGPURenderPassEncoder
GPU(wgpuRenderPassEncoderSetPipeline) void wgpuRenderPassEncoderSetPipeline(
    WGPURenderPassEncoder renderPassEncoder,
    WGPURenderPipeline pipeline
);

GPU(wgpuRenderPassEncoderSetBindGroup) void wgpuRenderPassEncoderSetBindGroup(
    WGPURenderPassEncoder renderPassEncoder,
    uint groupIndex,
    WGPUBindGroup group,
    usz dynamicOffsetCount = 0,
    const uint* dynamicOffsets = nullptr
);

GPU(wgpuRenderPassEncoderSetVertexBuffer) void wgpuRenderPassEncoderSetVertexBuffer(
    WGPURenderPassEncoder renderPassEncoder,
    uint slot,
    WGPUBuffer buffer,
    ulong offset = 0,
    ulong size = WGPU_WHOLE_SIZE
);

GPU(wgpuRenderPassEncoderSetIndexBuffer) void wgpuRenderPassEncoderSetIndexBuffer(
    WGPURenderPassEncoder renderPassEncoder,
    WGPUBuffer buffer,
    WGPUIndexFormat format,
    ulong offset = 0,
    ulong size = WGPU_WHOLE_SIZE
);

GPU(wgpuRenderPassEncoderSetViewport) void wgpuRenderPassEncoderSetViewport(
    WGPURenderPassEncoder renderPassEncoder,
    float x,
    float y,
    float width,
    float height,
    float minDepth,
    float maxDepth
);

GPU(wgpuRenderPassEncoderSetScissorRect) void wgpuRenderPassEncoderSetScissorRect(
    WGPURenderPassEncoder renderPassEncoder,
    uint x,
    uint y,
    uint width,
    uint height
);

GPU(wgpuRenderPassEncoderSetBlendConstant) void wgpuRenderPassEncoderSetBlendConstant(
    WGPURenderPassEncoder renderPassEncoder,
    const WGPUColor* color
);

GPU(wgpuRenderPassEncoderSetStencilReference) void wgpuRenderPassEncoderSetStencilReference(
    WGPURenderPassEncoder renderPassEncoder,
    uint reference
);

GPU(wgpuRenderPassEncoderDraw) void wgpuRenderPassEncoderDraw(
    WGPURenderPassEncoder renderPassEncoder,
    uint vertexCount,
    uint instanceCount = 1,
    uint firstVertex = 0,
    uint firstInstance = 0
);

GPU(wgpuRenderPassEncoderDrawIndexed) void wgpuRenderPassEncoderDrawIndexed(
    WGPURenderPassEncoder renderPassEncoder,
    uint indexCount,
    uint instanceCount = 1,
    uint firstIndex = 0,
    int baseVertex = 0,
    uint firstInstance = 0
);

GPU(wgpuRenderPassEncoderEnd) void wgpuRenderPassEncoderEnd(
    WGPURenderPassEncoder renderPassEncoder
);

GPU(wgpuRenderPassEncoderAddRef) void wgpuRenderPassEncoderAddRef(
    WGPURenderPassEncoder renderPassEncoder
);

GPU(wgpuRenderPassEncoderRelease) void wgpuRenderPassEncoderRelease(
    WGPURenderPassEncoder renderPassEncoder
);

// WGPUComputePassEncoder
GPU(wgpuComputePassEncoderSetPipeline) void wgpuComputePassEncoderSetPipeline(
    WGPUComputePassEncoder computePassEncoder,
    WGPUComputePipeline pipeline
);

GPU(wgpuComputePassEncoderSetBindGroup) void wgpuComputePassEncoderSetBindGroup(
    WGPUComputePassEncoder computePassEncoder,
    uint groupIndex,
    WGPUBindGroup group,
    usz dynamicOffsetCount = 0,
    const uint* dynamicOffsets = nullptr
);

GPU(wgpuComputePassEncoderDispatchWorkgroups) void wgpuComputePassEncoderDispatchWorkgroups(
    WGPUComputePassEncoder computePassEncoder,
    uint workgroupCountX,
    uint workgroupCountY = 1,
    uint workgroupCountZ = 1
);

GPU(wgpuComputePassEncoderEnd) void wgpuComputePassEncoderEnd(
    WGPUComputePassEncoder computePassEncoder
);

GPU(wgpuComputePassEncoderAddRef) void wgpuComputePassEncoderAddRef(
    WGPUComputePassEncoder computePassEncoder
);

GPU(wgpuComputePassEncoderRelease) void wgpuComputePassEncoderRelease(
    WGPUComputePassEncoder computePassEncoder
);

// WGPUCommandBuffer
GPU(wgpuCommandBufferAddRef) void wgpuCommandBufferAddRef(
    WGPUCommandBuffer commandBuffer
);

GPU(wgpuCommandBufferRelease) void wgpuCommandBufferRelease(
    WGPUCommandBuffer commandBuffer
);

// WGPUQueue
GPU(wgpuQueueSubmit) void wgpuQueueSubmit(
    WGPUQueue queue,
    usz commandCount,
    const WGPUCommandBuffer* commands
);

GPU(wgpuQueueWriteBuffer) void wgpuQueueWriteBuffer(
    WGPUQueue queue,
    WGPUBuffer buffer,
    ulong bufferOffset,
    const void* data,
    usz size
);

GPU(wgpuQueueWriteTexture) void wgpuQueueWriteTexture(
    WGPUQueue queue,
    const WGPUTexelCopyTextureInfo* destination,
    const void* data,
    usz dataSize,
    const WGPUTexelCopyBufferLayout* dataLayout,
    const WGPUExtent3D* writeSize
);

GPU(wgpuQueueAddRef) void wgpuQueueAddRef(
    WGPUQueue queue
);

GPU(wgpuQueueRelease) void wgpuQueueRelease(
    WGPUQueue queue
);
