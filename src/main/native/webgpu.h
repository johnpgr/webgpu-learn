#pragma once

#include "core/base.h"

// ============================================================================
// Standard webgpu.h C/C++ Binding Layer for Wasm32
//
// Follows the standard webgpu-headers (webgpu.h) naming and descriptor layout
// conventions used by Dawn/emdawnwebgpu, wgpu-native, and LearnWebGPU:
// - Types & descriptors use the `WGPU` prefix and camelCase fields.
// - Enums & bitflags use `WGPU<Type>_<Value>`.
// - Functions use `wgpu<Object><Method>` (imported via `GPU(...)` from `core/base.h`).
// - Every object type has a corresponding `wgpu<Object>Release` function.
// ============================================================================

typedef uint WGPUBool;
typedef uint WGPUFlags;

constexpr WGPUBool WGPU_FALSE = 0u;
constexpr WGPUBool WGPU_TRUE = 1u;

constexpr ulong WGPU_WHOLE_SIZE = ~(ulong)0;
constexpr uint WGPU_MIP_LEVEL_COUNT_UNDEFINED = ~0u;
constexpr uint WGPU_ARRAY_LAYER_COUNT_UNDEFINED = ~0u;
constexpr uint WGPU_DEPTH_SLICE_UNDEFINED = ~0u;
constexpr usz WGPU_STRLEN = ~(usz)0;

struct WGPUStringView {
    const char* data;
    usz length;

    constexpr WGPUStringView() : data(nullptr), length(0) {}
    constexpr WGPUStringView(const char* s, usz n) : data(s), length(n) {}
    constexpr WGPUStringView(decltype(nullptr)) : data(nullptr), length(0) {}

    template <usz N>
    constexpr WGPUStringView(const char (&s)[N]) : data(s), length(N - 1) {}
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

#undef WGPU_DEFINE_HANDLE

// ----------------------------------------------------------------------------
// Chained Structs (for standard webgpu.h descriptor extensions like WGSL)
// ----------------------------------------------------------------------------

enum WGPUSType : uint {
    WGPUSType_Invalid = 0,
    WGPUSType_ShaderSourceWGSL = 1,
};

struct WGPUChainedStruct {
    const WGPUChainedStruct* next;
    WGPUSType sType;
};
static_assert(sizeof(WGPUChainedStruct) == 8);
static_assert(__builtin_offsetof(WGPUChainedStruct, next) == 0);
static_assert(__builtin_offsetof(WGPUChainedStruct, sType) == 4);

struct WGPUShaderSourceWGSL {
    WGPUChainedStruct chain = { nullptr, WGPUSType_ShaderSourceWGSL };
    WGPUStringView code;
};
static_assert(sizeof(WGPUShaderSourceWGSL) == 16);
static_assert(__builtin_offsetof(WGPUShaderSourceWGSL, chain) == 0);
static_assert(__builtin_offsetof(WGPUShaderSourceWGSL, code) == 8);

typedef WGPUShaderSourceWGSL WGPUShaderModuleWGSLDescriptor;

// ----------------------------------------------------------------------------
// Usage Bitflags (values match the WebGPU specification & webgpu.h)
// ----------------------------------------------------------------------------

typedef WGPUFlags WGPUBufferUsage;
constexpr WGPUBufferUsage WGPUBufferUsage_None         = 0x0000;
constexpr WGPUBufferUsage WGPUBufferUsage_MapRead      = 0x0001;
constexpr WGPUBufferUsage WGPUBufferUsage_MapWrite     = 0x0002;
constexpr WGPUBufferUsage WGPUBufferUsage_CopySrc      = 0x0004;
constexpr WGPUBufferUsage WGPUBufferUsage_CopyDst      = 0x0008;
constexpr WGPUBufferUsage WGPUBufferUsage_Index        = 0x0010;
constexpr WGPUBufferUsage WGPUBufferUsage_Vertex       = 0x0020;
constexpr WGPUBufferUsage WGPUBufferUsage_Uniform      = 0x0040;
constexpr WGPUBufferUsage WGPUBufferUsage_Storage      = 0x0080;
constexpr WGPUBufferUsage WGPUBufferUsage_Indirect     = 0x0100;
constexpr WGPUBufferUsage WGPUBufferUsage_QueryResolve = 0x0200;

typedef WGPUFlags WGPUTextureUsage;
constexpr WGPUTextureUsage WGPUTextureUsage_None             = 0x00;
constexpr WGPUTextureUsage WGPUTextureUsage_CopySrc          = 0x01;
constexpr WGPUTextureUsage WGPUTextureUsage_CopyDst          = 0x02;
constexpr WGPUTextureUsage WGPUTextureUsage_TextureBinding   = 0x04;
constexpr WGPUTextureUsage WGPUTextureUsage_StorageBinding   = 0x08;
constexpr WGPUTextureUsage WGPUTextureUsage_RenderAttachment = 0x10;

typedef WGPUFlags WGPUShaderStage;
constexpr WGPUShaderStage WGPUShaderStage_None     = 0x0;
constexpr WGPUShaderStage WGPUShaderStage_Vertex   = 0x1;
constexpr WGPUShaderStage WGPUShaderStage_Fragment = 0x2;
constexpr WGPUShaderStage WGPUShaderStage_Compute  = 0x4;

typedef WGPUFlags WGPUColorWriteMask;
constexpr WGPUColorWriteMask WGPUColorWriteMask_None  = 0x0;
constexpr WGPUColorWriteMask WGPUColorWriteMask_Red   = 0x1;
constexpr WGPUColorWriteMask WGPUColorWriteMask_Green = 0x2;
constexpr WGPUColorWriteMask WGPUColorWriteMask_Blue  = 0x4;
constexpr WGPUColorWriteMask WGPUColorWriteMask_Alpha = 0x8;
constexpr WGPUColorWriteMask WGPUColorWriteMask_All   = 0xF;

// ----------------------------------------------------------------------------
// Enumerations (0 = Undefined / default)
// ----------------------------------------------------------------------------

enum WGPUSurfaceGetCurrentTextureStatus : uint {
    WGPUSurfaceGetCurrentTextureStatus_Undefined = 0,
    WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal = 1,
    WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal = 2,
    WGPUSurfaceGetCurrentTextureStatus_Timeout = 3,
    WGPUSurfaceGetCurrentTextureStatus_Outdated = 4,
    WGPUSurfaceGetCurrentTextureStatus_Lost = 5,
    WGPUSurfaceGetCurrentTextureStatus_Error = 6,
};

enum WGPUCompositeAlphaMode : uint {
    WGPUCompositeAlphaMode_Auto = 0,
    WGPUCompositeAlphaMode_Opaque = 1,
    WGPUCompositeAlphaMode_Premultiplied = 2,
};

enum WGPUPresentMode : uint {
    WGPUPresentMode_Undefined = 0,
    WGPUPresentMode_Fifo = 1,
    WGPUPresentMode_FifoRelaxed = 2,
    WGPUPresentMode_Immediate = 3,
    WGPUPresentMode_Mailbox = 4,
};

enum WGPUTextureFormat : uint {
    WGPUTextureFormat_Undefined = 0,
    WGPUTextureFormat_R8Unorm = 1,
    WGPUTextureFormat_R8Snorm = 2,
    WGPUTextureFormat_R8Uint = 3,
    WGPUTextureFormat_R8Sint = 4,
    WGPUTextureFormat_R16Uint = 5,
    WGPUTextureFormat_R16Sint = 6,
    WGPUTextureFormat_R16Float = 7,
    WGPUTextureFormat_RG8Unorm = 8,
    WGPUTextureFormat_RG8Snorm = 9,
    WGPUTextureFormat_RG8Uint = 10,
    WGPUTextureFormat_RG8Sint = 11,
    WGPUTextureFormat_R32Float = 12,
    WGPUTextureFormat_R32Uint = 13,
    WGPUTextureFormat_R32Sint = 14,
    WGPUTextureFormat_RG16Uint = 15,
    WGPUTextureFormat_RG16Sint = 16,
    WGPUTextureFormat_RG16Float = 17,
    WGPUTextureFormat_RGBA8Unorm = 18,
    WGPUTextureFormat_RGBA8UnormSrgb = 19,
    WGPUTextureFormat_RGBA8Snorm = 20,
    WGPUTextureFormat_RGBA8Uint = 21,
    WGPUTextureFormat_RGBA8Sint = 22,
    WGPUTextureFormat_BGRA8Unorm = 23,
    WGPUTextureFormat_BGRA8UnormSrgb = 24,
    WGPUTextureFormat_RGB10A2Uint = 25,
    WGPUTextureFormat_RGB10A2Unorm = 26,
    WGPUTextureFormat_RG11B10Ufloat = 27,
    WGPUTextureFormat_RGB9E5Ufloat = 28,
    WGPUTextureFormat_RG32Float = 29,
    WGPUTextureFormat_RG32Uint = 30,
    WGPUTextureFormat_RG32Sint = 31,
    WGPUTextureFormat_RGBA16Uint = 32,
    WGPUTextureFormat_RGBA16Sint = 33,
    WGPUTextureFormat_RGBA16Float = 34,
    WGPUTextureFormat_RGBA32Float = 35,
    WGPUTextureFormat_RGBA32Uint = 36,
    WGPUTextureFormat_RGBA32Sint = 37,
    WGPUTextureFormat_Stencil8 = 38,
    WGPUTextureFormat_Depth16Unorm = 39,
    WGPUTextureFormat_Depth24Plus = 40,
    WGPUTextureFormat_Depth24PlusStencil8 = 41,
    WGPUTextureFormat_Depth32Float = 42,
    WGPUTextureFormat_Depth32FloatStencil8 = 43,
};

enum WGPUTextureDimension : uint {
    WGPUTextureDimension_Undefined = 0,
    WGPUTextureDimension_1D = 1,
    WGPUTextureDimension_2D = 2,
    WGPUTextureDimension_3D = 3,
};

enum WGPUTextureViewDimension : uint {
    WGPUTextureViewDimension_Undefined = 0,
    WGPUTextureViewDimension_1D = 1,
    WGPUTextureViewDimension_2D = 2,
    WGPUTextureViewDimension_2DArray = 3,
    WGPUTextureViewDimension_Cube = 4,
    WGPUTextureViewDimension_CubeArray = 5,
    WGPUTextureViewDimension_3D = 6,
};

enum WGPUTextureAspect : uint {
    WGPUTextureAspect_Undefined = 0,
    WGPUTextureAspect_All = 1,
    WGPUTextureAspect_StencilOnly = 2,
    WGPUTextureAspect_DepthOnly = 3,
};

enum WGPULoadOp : uint {
    WGPULoadOp_Undefined = 0,
    WGPULoadOp_Clear = 1,
    WGPULoadOp_Load = 2,
};

enum WGPUStoreOp : uint {
    WGPUStoreOp_Undefined = 0,
    WGPUStoreOp_Store = 1,
    WGPUStoreOp_Discard = 2,
};

enum WGPUPrimitiveTopology : uint {
    WGPUPrimitiveTopology_Undefined = 0,
    WGPUPrimitiveTopology_PointList = 1,
    WGPUPrimitiveTopology_LineList = 2,
    WGPUPrimitiveTopology_LineStrip = 3,
    WGPUPrimitiveTopology_TriangleList = 4,
    WGPUPrimitiveTopology_TriangleStrip = 5,
};

enum WGPUIndexFormat : uint {
    WGPUIndexFormat_Undefined = 0,
    WGPUIndexFormat_Uint16 = 1,
    WGPUIndexFormat_Uint32 = 2,
};

enum WGPUFrontFace : uint {
    WGPUFrontFace_Undefined = 0,
    WGPUFrontFace_CCW = 1,
    WGPUFrontFace_CW = 2,
};

enum WGPUCullMode : uint {
    WGPUCullMode_Undefined = 0,
    WGPUCullMode_None = 1,
    WGPUCullMode_Front = 2,
    WGPUCullMode_Back = 3,
};

enum WGPUCompareFunction : uint {
    WGPUCompareFunction_Undefined = 0,
    WGPUCompareFunction_Never = 1,
    WGPUCompareFunction_Less = 2,
    WGPUCompareFunction_Equal = 3,
    WGPUCompareFunction_LessEqual = 4,
    WGPUCompareFunction_Greater = 5,
    WGPUCompareFunction_NotEqual = 6,
    WGPUCompareFunction_GreaterEqual = 7,
    WGPUCompareFunction_Always = 8,
};

enum WGPUStencilOperation : uint {
    WGPUStencilOperation_Undefined = 0,
    WGPUStencilOperation_Keep = 1,
    WGPUStencilOperation_Zero = 2,
    WGPUStencilOperation_Replace = 3,
    WGPUStencilOperation_Invert = 4,
    WGPUStencilOperation_IncrementClamp = 5,
    WGPUStencilOperation_DecrementClamp = 6,
    WGPUStencilOperation_IncrementWrap = 7,
    WGPUStencilOperation_DecrementWrap = 8,
};

enum WGPUBlendFactor : uint {
    WGPUBlendFactor_Undefined = 0,
    WGPUBlendFactor_Zero = 1,
    WGPUBlendFactor_One = 2,
    WGPUBlendFactor_Src = 3,
    WGPUBlendFactor_OneMinusSrc = 4,
    WGPUBlendFactor_SrcAlpha = 5,
    WGPUBlendFactor_OneMinusSrcAlpha = 6,
    WGPUBlendFactor_Dst = 7,
    WGPUBlendFactor_OneMinusDst = 8,
    WGPUBlendFactor_DstAlpha = 9,
    WGPUBlendFactor_OneMinusDstAlpha = 10,
    WGPUBlendFactor_SrcAlphaSaturated = 11,
    WGPUBlendFactor_Constant = 12,
    WGPUBlendFactor_OneMinusConstant = 13,
    WGPUBlendFactor_Src1 = 14,
    WGPUBlendFactor_OneMinusSrc1 = 15,
    WGPUBlendFactor_Src1Alpha = 16,
    WGPUBlendFactor_OneMinusSrc1Alpha = 17,
};

enum WGPUBlendOperation : uint {
    WGPUBlendOperation_Undefined = 0,
    WGPUBlendOperation_Add = 1,
    WGPUBlendOperation_Subtract = 2,
    WGPUBlendOperation_ReverseSubtract = 3,
    WGPUBlendOperation_Min = 4,
    WGPUBlendOperation_Max = 5,
};

enum WGPUVertexStepMode : uint {
    WGPUVertexStepMode_Undefined = 0,
    WGPUVertexStepMode_Vertex = 1,
    WGPUVertexStepMode_Instance = 2,
};

enum WGPUVertexFormat : uint {
    WGPUVertexFormat_Undefined = 0,
    WGPUVertexFormat_Uint8 = 1,
    WGPUVertexFormat_Uint8x2 = 2,
    WGPUVertexFormat_Uint8x4 = 3,
    WGPUVertexFormat_Sint8 = 4,
    WGPUVertexFormat_Sint8x2 = 5,
    WGPUVertexFormat_Sint8x4 = 6,
    WGPUVertexFormat_Unorm8 = 7,
    WGPUVertexFormat_Unorm8x2 = 8,
    WGPUVertexFormat_Unorm8x4 = 9,
    WGPUVertexFormat_Snorm8 = 10,
    WGPUVertexFormat_Snorm8x2 = 11,
    WGPUVertexFormat_Snorm8x4 = 12,
    WGPUVertexFormat_Uint16 = 13,
    WGPUVertexFormat_Uint16x2 = 14,
    WGPUVertexFormat_Uint16x4 = 15,
    WGPUVertexFormat_Sint16 = 16,
    WGPUVertexFormat_Sint16x2 = 17,
    WGPUVertexFormat_Sint16x4 = 18,
    WGPUVertexFormat_Unorm16 = 19,
    WGPUVertexFormat_Unorm16x2 = 20,
    WGPUVertexFormat_Unorm16x4 = 21,
    WGPUVertexFormat_Snorm16 = 22,
    WGPUVertexFormat_Snorm16x2 = 23,
    WGPUVertexFormat_Snorm16x4 = 24,
    WGPUVertexFormat_Float16 = 25,
    WGPUVertexFormat_Float16x2 = 26,
    WGPUVertexFormat_Float16x4 = 27,
    WGPUVertexFormat_Float32 = 28,
    WGPUVertexFormat_Float32x2 = 29,
    WGPUVertexFormat_Float32x3 = 30,
    WGPUVertexFormat_Float32x4 = 31,
    WGPUVertexFormat_Uint32 = 32,
    WGPUVertexFormat_Uint32x2 = 33,
    WGPUVertexFormat_Uint32x3 = 34,
    WGPUVertexFormat_Uint32x4 = 35,
    WGPUVertexFormat_Sint32 = 36,
    WGPUVertexFormat_Sint32x2 = 37,
    WGPUVertexFormat_Sint32x3 = 38,
    WGPUVertexFormat_Sint32x4 = 39,
    WGPUVertexFormat_Unorm10_10_10_2 = 40,
    WGPUVertexFormat_Unorm8x4BGRA = 41,
};

enum WGPUAddressMode : uint {
    WGPUAddressMode_Undefined = 0,
    WGPUAddressMode_ClampToEdge = 1,
    WGPUAddressMode_Repeat = 2,
    WGPUAddressMode_MirrorRepeat = 3,
};

enum WGPUFilterMode : uint {
    WGPUFilterMode_Undefined = 0,
    WGPUFilterMode_Nearest = 1,
    WGPUFilterMode_Linear = 2,
};

enum WGPUMipmapFilterMode : uint {
    WGPUMipmapFilterMode_Undefined = 0,
    WGPUMipmapFilterMode_Nearest = 1,
    WGPUMipmapFilterMode_Linear = 2,
};

enum WGPUBufferBindingType : uint {
    WGPUBufferBindingType_BindingNotUsed = 0,
    WGPUBufferBindingType_Undefined = 0,
    WGPUBufferBindingType_Uniform = 1,
    WGPUBufferBindingType_Storage = 2,
    WGPUBufferBindingType_ReadOnlyStorage = 3,
};

enum WGPUSamplerBindingType : uint {
    WGPUSamplerBindingType_BindingNotUsed = 0,
    WGPUSamplerBindingType_Undefined = 0,
    WGPUSamplerBindingType_Filtering = 1,
    WGPUSamplerBindingType_NonFiltering = 2,
    WGPUSamplerBindingType_Comparison = 3,
};

enum WGPUTextureSampleType : uint {
    WGPUTextureSampleType_BindingNotUsed = 0,
    WGPUTextureSampleType_Undefined = 0,
    WGPUTextureSampleType_Float = 1,
    WGPUTextureSampleType_UnfilterableFloat = 2,
    WGPUTextureSampleType_Depth = 3,
    WGPUTextureSampleType_Sint = 4,
    WGPUTextureSampleType_Uint = 5,
};

enum WGPUStorageTextureAccess : uint {
    WGPUStorageTextureAccess_BindingNotUsed = 0,
    WGPUStorageTextureAccess_Undefined = 0,
    WGPUStorageTextureAccess_WriteOnly = 1,
    WGPUStorageTextureAccess_ReadOnly = 2,
    WGPUStorageTextureAccess_ReadWrite = 3,
};

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

struct WGPUSurfaceConfiguration {
    const WGPUChainedStruct* nextInChain;
    WGPUDevice device;
    WGPUTextureFormat format;
    WGPUTextureUsage usage = WGPUTextureUsage_RenderAttachment;
    usz viewFormatCount;
    const WGPUTextureFormat* viewFormats;
    WGPUCompositeAlphaMode alphaMode = WGPUCompositeAlphaMode_Opaque;
    uint width;
    uint height;
    WGPUPresentMode presentMode = WGPUPresentMode_Fifo;
};
static_assert(sizeof(WGPUSurfaceConfiguration) == 40);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, device) == 4);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, format) == 8);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, usage) == 12);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, viewFormatCount) == 16);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, viewFormats) == 20);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, alphaMode) == 24);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, width) == 28);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, height) == 32);
static_assert(__builtin_offsetof(WGPUSurfaceConfiguration, presentMode) == 36);

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
    WGPUBool mappedAtCreation;
    uint _pad0;
};
static_assert(sizeof(WGPUBufferDescriptor) == 32);
static_assert(__builtin_offsetof(WGPUBufferDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUBufferDescriptor, label) == 4);
static_assert(__builtin_offsetof(WGPUBufferDescriptor, usage) == 12);
static_assert(__builtin_offsetof(WGPUBufferDescriptor, size) == 16);
static_assert(__builtin_offsetof(WGPUBufferDescriptor, mappedAtCreation) == 24);

struct WGPUTextureDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
    WGPUTextureUsage usage;
    WGPUTextureDimension dimension = WGPUTextureDimension_2D;
    WGPUExtent3D size;
    WGPUTextureFormat format;
    uint mipLevelCount = 1;
    uint sampleCount = 1;
    usz viewFormatCount;
    const WGPUTextureFormat* viewFormats;
};
static_assert(sizeof(WGPUTextureDescriptor) == 52);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, label) == 4);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, usage) == 12);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, dimension) == 16);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, size) == 20);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, format) == 32);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, mipLevelCount) == 36);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, sampleCount) == 40);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, viewFormatCount) == 44);
static_assert(__builtin_offsetof(WGPUTextureDescriptor, viewFormats) == 48);

struct WGPUTextureViewDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
    WGPUTextureFormat format;
    WGPUTextureViewDimension dimension;
    uint baseMipLevel;
    uint mipLevelCount = WGPU_MIP_LEVEL_COUNT_UNDEFINED;
    uint baseArrayLayer;
    uint arrayLayerCount = WGPU_ARRAY_LAYER_COUNT_UNDEFINED;
    WGPUTextureAspect aspect = WGPUTextureAspect_All;
    WGPUTextureUsage usage;
};
static_assert(sizeof(WGPUTextureViewDescriptor) == 44);
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
    WGPUAddressMode addressModeU = WGPUAddressMode_ClampToEdge;
    WGPUAddressMode addressModeV = WGPUAddressMode_ClampToEdge;
    WGPUAddressMode addressModeW = WGPUAddressMode_ClampToEdge;
    WGPUFilterMode magFilter = WGPUFilterMode_Nearest;
    WGPUFilterMode minFilter = WGPUFilterMode_Nearest;
    WGPUMipmapFilterMode mipmapFilter = WGPUMipmapFilterMode_Nearest;
    float lodMinClamp = 0.0f;
    float lodMaxClamp = 32.0f;
    WGPUCompareFunction compare = WGPUCompareFunction_Undefined;
    ushort maxAnisotropy = 1;
    ushort _pad0;
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

// Supports both standard `nextInChain` (pointing to WGPUShaderSourceWGSL)
// and direct `.code` initialization for convenience.
struct WGPUShaderModuleDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
    WGPUStringView code;
};
static_assert(sizeof(WGPUShaderModuleDescriptor) == 20);
static_assert(__builtin_offsetof(WGPUShaderModuleDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUShaderModuleDescriptor, label) == 4);
static_assert(__builtin_offsetof(WGPUShaderModuleDescriptor, code) == 12);

struct WGPUBufferBindingLayout {
    const WGPUChainedStruct* nextInChain;
    WGPUBufferBindingType type;
    WGPUBool hasDynamicOffset;
    uint _pad0;
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
    uint _pad0;
    WGPUBufferBindingLayout buffer;
    WGPUSamplerBindingLayout sampler;
    WGPUTextureBindingLayout texture;
    WGPUStorageTextureBindingLayout storageTexture;
};
static_assert(sizeof(WGPUBindGroupLayoutEntry) == 80);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutEntry, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutEntry, binding) == 4);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutEntry, visibility) == 8);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutEntry, buffer) == 16);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutEntry, sampler) == 40);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutEntry, texture) == 48);
static_assert(__builtin_offsetof(WGPUBindGroupLayoutEntry, storageTexture) == 64);

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
    uint _pad0;
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
};
static_assert(sizeof(WGPUPipelineLayoutDescriptor) == 20);
static_assert(__builtin_offsetof(WGPUPipelineLayoutDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUPipelineLayoutDescriptor, label) == 4);
static_assert(__builtin_offsetof(WGPUPipelineLayoutDescriptor, bindGroupLayoutCount) == 12);
static_assert(__builtin_offsetof(WGPUPipelineLayoutDescriptor, bindGroupLayouts) == 16);

struct WGPUVertexAttribute {
    const WGPUChainedStruct* nextInChain;
    WGPUVertexFormat format;
    ulong offset;
    uint shaderLocation;
    uint _pad0;
};
static_assert(sizeof(WGPUVertexAttribute) == 24);
static_assert(__builtin_offsetof(WGPUVertexAttribute, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUVertexAttribute, format) == 4);
static_assert(__builtin_offsetof(WGPUVertexAttribute, offset) == 8);
static_assert(__builtin_offsetof(WGPUVertexAttribute, shaderLocation) == 16);

struct WGPUVertexBufferLayout {
    const WGPUChainedStruct* nextInChain;
    WGPUVertexStepMode stepMode = WGPUVertexStepMode_Vertex;
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
    WGPUPrimitiveTopology topology = WGPUPrimitiveTopology_TriangleList;
    WGPUIndexFormat stripIndexFormat = WGPUIndexFormat_Undefined;
    WGPUFrontFace frontFace = WGPUFrontFace_CCW;
    WGPUCullMode cullMode = WGPUCullMode_None;
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
    WGPUCompareFunction compare = WGPUCompareFunction_Always;
    WGPUStencilOperation failOp = WGPUStencilOperation_Keep;
    WGPUStencilOperation depthFailOp = WGPUStencilOperation_Keep;
    WGPUStencilOperation passOp = WGPUStencilOperation_Keep;
};
static_assert(sizeof(WGPUStencilFaceState) == 16);
static_assert(__builtin_offsetof(WGPUStencilFaceState, compare) == 0);
static_assert(__builtin_offsetof(WGPUStencilFaceState, failOp) == 4);
static_assert(__builtin_offsetof(WGPUStencilFaceState, depthFailOp) == 8);
static_assert(__builtin_offsetof(WGPUStencilFaceState, passOp) == 12);

struct WGPUDepthStencilState {
    const WGPUChainedStruct* nextInChain;
    WGPUTextureFormat format;
    WGPUBool depthWriteEnabled;
    WGPUCompareFunction depthCompare = WGPUCompareFunction_Always;
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
    WGPUBlendOperation operation = WGPUBlendOperation_Add;
    WGPUBlendFactor srcFactor = WGPUBlendFactor_One;
    WGPUBlendFactor dstFactor = WGPUBlendFactor_Zero;
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
    WGPUColorWriteMask writeMask = WGPUColorWriteMask_All;
};
static_assert(sizeof(WGPUColorTargetState) == 16);
static_assert(__builtin_offsetof(WGPUColorTargetState, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUColorTargetState, format) == 4);
static_assert(__builtin_offsetof(WGPUColorTargetState, blend) == 8);
static_assert(__builtin_offsetof(WGPUColorTargetState, writeMask) == 12);

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

struct WGPUProgrammableStageDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUShaderModule module;
    WGPUStringView entryPoint;
    usz constantCount;
    const WGPUConstantEntry* constants;
};
static_assert(sizeof(WGPUProgrammableStageDescriptor) == 24);
static_assert(__builtin_offsetof(WGPUProgrammableStageDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUProgrammableStageDescriptor, module) == 4);
static_assert(__builtin_offsetof(WGPUProgrammableStageDescriptor, entryPoint) == 8);
static_assert(__builtin_offsetof(WGPUProgrammableStageDescriptor, constantCount) == 16);
static_assert(__builtin_offsetof(WGPUProgrammableStageDescriptor, constants) == 20);

struct WGPUComputePipelineDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
    WGPUPipelineLayout layout; // {0} means "auto"
    WGPUProgrammableStageDescriptor compute;
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
    float depthClearValue;
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
};
static_assert(sizeof(WGPURenderPassDescriptor) == 24);
static_assert(__builtin_offsetof(WGPURenderPassDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPURenderPassDescriptor, label) == 4);
static_assert(__builtin_offsetof(WGPURenderPassDescriptor, colorAttachmentCount) == 12);
static_assert(__builtin_offsetof(WGPURenderPassDescriptor, colorAttachments) == 16);
static_assert(__builtin_offsetof(WGPURenderPassDescriptor, depthStencilAttachment) == 20);

struct WGPUComputePassDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUStringView label;
};
static_assert(sizeof(WGPUComputePassDescriptor) == 12);
static_assert(__builtin_offsetof(WGPUComputePassDescriptor, nextInChain) == 0);
static_assert(__builtin_offsetof(WGPUComputePassDescriptor, label) == 4);

struct WGPUTexelCopyBufferLayout {
    ulong offset;
    uint bytesPerRow;
    uint rowsPerImage;
};
static_assert(sizeof(WGPUTexelCopyBufferLayout) == 16);
static_assert(__builtin_offsetof(WGPUTexelCopyBufferLayout, offset) == 0);
static_assert(__builtin_offsetof(WGPUTexelCopyBufferLayout, bytesPerRow) == 8);
static_assert(__builtin_offsetof(WGPUTexelCopyBufferLayout, rowsPerImage) == 12);

struct WGPUTexelCopyBufferInfo {
    WGPUTexelCopyBufferLayout layout;
    WGPUBuffer buffer;
    uint _pad0;
};
static_assert(sizeof(WGPUTexelCopyBufferInfo) == 24);
static_assert(__builtin_offsetof(WGPUTexelCopyBufferInfo, layout) == 0);
static_assert(__builtin_offsetof(WGPUTexelCopyBufferInfo, buffer) == 16);

struct WGPUTexelCopyTextureInfo {
    WGPUTexture texture;
    uint mipLevel;
    WGPUOrigin3D origin;
    WGPUTextureAspect aspect = WGPUTextureAspect_All;
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
GPU(wgpuSurfaceRelease) void wgpuSurfaceRelease(
    WGPUSurface surface
);

// WGPUAdapter
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
GPU(wgpuDeviceRelease) void wgpuDeviceRelease(
    WGPUDevice device
);

// WGPUBuffer
GPU(wgpuBufferDestroy) void wgpuBufferDestroy(
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
GPU(wgpuTextureRelease) void wgpuTextureRelease(
    WGPUTexture texture
);

// WGPUTextureView
GPU(wgpuTextureViewRelease) void wgpuTextureViewRelease(
    WGPUTextureView textureView
);

// WGPUSampler / WGPUBindGroupLayout / WGPUBindGroup / WGPUPipelineLayout / WGPUShaderModule
GPU(wgpuSamplerRelease) void wgpuSamplerRelease(
    WGPUSampler sampler
);
GPU(wgpuBindGroupLayoutRelease) void wgpuBindGroupLayoutRelease(
    WGPUBindGroupLayout bindGroupLayout
);
GPU(wgpuBindGroupRelease) void wgpuBindGroupRelease(
    WGPUBindGroup bindGroup
);
GPU(wgpuPipelineLayoutRelease) void wgpuPipelineLayoutRelease(
    WGPUPipelineLayout pipelineLayout
);
GPU(wgpuShaderModuleRelease) void wgpuShaderModuleRelease(
    WGPUShaderModule shaderModule
);

// WGPURenderPipeline / WGPUComputePipeline
GPU(wgpuRenderPipelineGetBindGroupLayout) WGPUBindGroupLayout wgpuRenderPipelineGetBindGroupLayout(
    WGPURenderPipeline pipeline,
    uint groupIndex
);
GPU(wgpuRenderPipelineRelease) void wgpuRenderPipelineRelease(
    WGPURenderPipeline renderPipeline
);
GPU(wgpuComputePipelineGetBindGroupLayout) WGPUBindGroupLayout wgpuComputePipelineGetBindGroupLayout(
    WGPUComputePipeline pipeline,
    uint groupIndex
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
GPU(wgpuComputePassEncoderRelease) void wgpuComputePassEncoderRelease(
    WGPUComputePassEncoder computePassEncoder
);

// WGPUCommandBuffer
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
GPU(wgpuQueueRelease) void wgpuQueueRelease(
    WGPUQueue queue
);
