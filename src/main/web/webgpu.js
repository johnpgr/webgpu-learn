// Handwritten JavaScript implementation of the standard `webgpu.h` C API for Wasm32.
// Translates `wgpu*` calls and `WGPU*` descriptors from Wasm linear memory into
// direct browser WebGPU API calls.

const WHOLE_SIZE = 0xffffffffffffffffn;
const UINT32_UNDEFINED = 0xffffffff;
// Also 0xffffffff; WGPU_COPY_STRIDE_UNDEFINED in webgpu.h.
const COPY_STRIDE_UNDEFINED = 0xffffffff;
const STRLEN = 0xffffffff;
const STYPE_SHADER_SOURCE_WGSL = 2;
const BINDING_NOT_USED = 0; // *BindingType value 0; value 1 is Undefined (= default)
const COMPOSITE_ALPHA_UNPREMULTIPLIED = 3;
const COMPOSITE_ALPHA_INHERIT = 4;
const OPTIONAL_BOOL_FALSE = 0;
const OPTIONAL_BOOL_TRUE = 1;
const OPTIONAL_BOOL_UNDEFINED = 2;
const SURFACE_STATUS_SUCCESS_OPTIMAL = 1;
const SURFACE_STATUS_ERROR = 6;

// emdawnwebgpu extension sType (Emscripten enums start at 0x0004_0000).
const STYPE_EMSCRIPTEN_CANVAS_SELECTOR = 0x00040000;
const STATUS_SUCCESS = 1;
const CALLBACK_MODE_WAIT_ANY_ONLY = 1;
const CALLBACK_MODE_ALLOW_PROCESS_EVENTS = 2;
const CALLBACK_MODE_ALLOW_SPONTANEOUS = 3;
const WAIT_STATUS_SUCCESS = 1;
const WAIT_STATUS_TIMED_OUT = 2;
const WAIT_STATUS_ERROR = 3;
const REQUEST_ADAPTER_STATUS_SUCCESS = 1;
const REQUEST_ADAPTER_STATUS_UNAVAILABLE = 3;
const REQUEST_ADAPTER_STATUS_ERROR = 4;
const REQUEST_DEVICE_STATUS_SUCCESS = 1;
const REQUEST_DEVICE_STATUS_ERROR = 3;
const FEATURE_LEVEL_COMPATIBILITY = 1;
const FEATURE_LEVEL_CORE = 2;
const BACKEND_TYPE_WEBGPU = 2;
const ADAPTER_TYPE_CPU = 3;
const ADAPTER_TYPE_UNKNOWN = 4;
const INSTANCE_FEATURE_TIMED_WAIT_ANY = 1;
const MAX_TIMED_WAIT_ANY_COUNT = 64;
const ERROR_TYPE_VALIDATION = 2;
const ERROR_TYPE_OUT_OF_MEMORY = 3;
const ERROR_TYPE_INTERNAL = 4;
const ERROR_TYPE_UNKNOWN = 5;
const DEVICE_LOST_REASON_UNKNOWN = 1;
const DEVICE_LOST_REASON_DESTROYED = 2;
const PRESENT_MODE_FIFO = 1;
const COMPOSITE_ALPHA_OPAQUE = 1;
const COMPOSITE_ALPHA_PREMULTIPLIED = 2;
// CopySrc | CopyDst | TextureBinding | RenderAttachment. StorageBinding is left
// out: it depends on the format (bgra8unorm needs a feature).
const CANVAS_TEXTURE_USAGES = 0x01 | 0x02 | 0x04 | 0x10;
// Bytes of wasm memory the host reserves (once) for strings passed to callbacks.
// Longer messages are truncated at a UTF-8 character boundary.
const CALLBACK_SCRATCH_SIZE = 64 * 1024;

// Enum lookup tables, indexed by the WGPU* enum value (values identical to
// webgpu-headers). `undefined` = Undefined/unset; `null` = the Undefined value of the
// *BindingType enums (1), which also means "use the WebGPU default".
// Generated from webgpu.h `main`; keep in sync with the enums in webgpu.h.

// WGPUCompositeAlphaMode: Auto, Opaque, Premultiplied. Unpremultiplied (3) and
// Inherit (4) are rejected in wgpuSurfaceConfigure: browser canvases cannot do them.
const COMPOSITE_ALPHA_MODES = [
    undefined,
    "opaque",
    "premultiplied",
];

const TEXTURE_FORMATS = [
    undefined,
    "r8unorm",
    "r8snorm",
    "r8uint",
    "r8sint",
    "r16unorm",
    "r16snorm",
    "r16uint",
    "r16sint",
    "r16float",
    "rg8unorm",
    "rg8snorm",
    "rg8uint",
    "rg8sint",
    "r32float",
    "r32uint",
    "r32sint",
    "rg16unorm",
    "rg16snorm",
    "rg16uint",
    "rg16sint",
    "rg16float",
    "rgba8unorm",
    "rgba8unorm-srgb",
    "rgba8snorm",
    "rgba8uint",
    "rgba8sint",
    "bgra8unorm",
    "bgra8unorm-srgb",
    "rgb10a2uint",
    "rgb10a2unorm",
    "rg11b10ufloat",
    "rgb9e5ufloat",
    "rg32float",
    "rg32uint",
    "rg32sint",
    "rgba16unorm",
    "rgba16snorm",
    "rgba16uint",
    "rgba16sint",
    "rgba16float",
    "rgba32float",
    "rgba32uint",
    "rgba32sint",
    "stencil8",
    "depth16unorm",
    "depth24plus",
    "depth24plus-stencil8",
    "depth32float",
    "depth32float-stencil8",
    "bc1-rgba-unorm",
    "bc1-rgba-unorm-srgb",
    "bc2-rgba-unorm",
    "bc2-rgba-unorm-srgb",
    "bc3-rgba-unorm",
    "bc3-rgba-unorm-srgb",
    "bc4-r-unorm",
    "bc4-r-snorm",
    "bc5-rg-unorm",
    "bc5-rg-snorm",
    "bc6h-rgb-ufloat",
    "bc6h-rgb-float",
    "bc7-rgba-unorm",
    "bc7-rgba-unorm-srgb",
    "etc2-rgb8unorm",
    "etc2-rgb8unorm-srgb",
    "etc2-rgb8a1unorm",
    "etc2-rgb8a1unorm-srgb",
    "etc2-rgba8unorm",
    "etc2-rgba8unorm-srgb",
    "eac-r11unorm",
    "eac-r11snorm",
    "eac-rg11unorm",
    "eac-rg11snorm",
    "astc-4x4-unorm",
    "astc-4x4-unorm-srgb",
    "astc-5x4-unorm",
    "astc-5x4-unorm-srgb",
    "astc-5x5-unorm",
    "astc-5x5-unorm-srgb",
    "astc-6x5-unorm",
    "astc-6x5-unorm-srgb",
    "astc-6x6-unorm",
    "astc-6x6-unorm-srgb",
    "astc-8x5-unorm",
    "astc-8x5-unorm-srgb",
    "astc-8x6-unorm",
    "astc-8x6-unorm-srgb",
    "astc-8x8-unorm",
    "astc-8x8-unorm-srgb",
    "astc-10x5-unorm",
    "astc-10x5-unorm-srgb",
    "astc-10x6-unorm",
    "astc-10x6-unorm-srgb",
    "astc-10x8-unorm",
    "astc-10x8-unorm-srgb",
    "astc-10x10-unorm",
    "astc-10x10-unorm-srgb",
    "astc-12x10-unorm",
    "astc-12x10-unorm-srgb",
    "astc-12x12-unorm",
    "astc-12x12-unorm-srgb",
];

const TEXTURE_DIMENSIONS = [
    undefined,
    "1d",
    "2d",
    "3d",
];

const TEXTURE_VIEW_DIMENSIONS = [
    undefined,
    "1d",
    "2d",
    "2d-array",
    "cube",
    "cube-array",
    "3d",
];

const TEXTURE_ASPECTS = [
    undefined,
    "all",
    "stencil-only",
    "depth-only",
];

const LOAD_OPS = [
    undefined,
    "load",
    "clear",
];

const STORE_OPS = [
    undefined,
    "store",
    "discard",
];

const PRIMITIVE_TOPOLOGIES = [
    undefined,
    "point-list",
    "line-list",
    "line-strip",
    "triangle-list",
    "triangle-strip",
];

const INDEX_FORMATS = [
    undefined,
    "uint16",
    "uint32",
];

const FRONT_FACES = [
    undefined,
    "ccw",
    "cw",
];

const CULL_MODES = [
    undefined,
    "none",
    "front",
    "back",
];

const COMPARE_FUNCTIONS = [
    undefined,
    "never",
    "less",
    "equal",
    "less-equal",
    "greater",
    "not-equal",
    "greater-equal",
    "always",
];

const STENCIL_OPERATIONS = [
    undefined,
    "keep",
    "zero",
    "replace",
    "invert",
    "increment-clamp",
    "decrement-clamp",
    "increment-wrap",
    "decrement-wrap",
];

const BLEND_FACTORS = [
    undefined,
    "zero",
    "one",
    "src",
    "one-minus-src",
    "src-alpha",
    "one-minus-src-alpha",
    "dst",
    "one-minus-dst",
    "dst-alpha",
    "one-minus-dst-alpha",
    "src-alpha-saturated",
    "constant",
    "one-minus-constant",
    "src1",
    "one-minus-src1",
    "src1-alpha",
    "one-minus-src1-alpha",
];

const BLEND_OPERATIONS = [
    undefined,
    "add",
    "subtract",
    "reverse-subtract",
    "min",
    "max",
];

const VERTEX_STEP_MODES = [
    undefined,
    "vertex",
    "instance",
];

const ADDRESS_MODES = [
    undefined,
    "clamp-to-edge",
    "repeat",
    "mirror-repeat",
];

const FILTER_MODES = [
    undefined,
    "nearest",
    "linear",
];

const MIPMAP_FILTER_MODES = [
    undefined,
    "nearest",
    "linear",
];

const BUFFER_BINDING_TYPES = [
    undefined,
    null,
    "uniform",
    "storage",
    "read-only-storage",
];

const SAMPLER_BINDING_TYPES = [
    undefined,
    null,
    "filtering",
    "non-filtering",
    "comparison",
];

const TEXTURE_SAMPLE_TYPES = [
    undefined,
    null,
    "float",
    "unfilterable-float",
    "depth",
    "sint",
    "uint",
];

const STORAGE_TEXTURE_ACCESS = [
    undefined,
    null,
    "write-only",
    "read-only",
    "read-write",
];

const VERTEX_FORMATS = [
    undefined,
    "uint8",
    "uint8x2",
    "uint8x4",
    "sint8",
    "sint8x2",
    "sint8x4",
    "unorm8",
    "unorm8x2",
    "unorm8x4",
    "snorm8",
    "snorm8x2",
    "snorm8x4",
    "uint16",
    "uint16x2",
    "uint16x4",
    "sint16",
    "sint16x2",
    "sint16x4",
    "unorm16",
    "unorm16x2",
    "unorm16x4",
    "snorm16",
    "snorm16x2",
    "snorm16x4",
    "float16",
    "float16x2",
    "float16x4",
    "float32",
    "float32x2",
    "float32x3",
    "float32x4",
    "uint32",
    "uint32x2",
    "uint32x3",
    "uint32x4",
    "sint32",
    "sint32x2",
    "sint32x3",
    "sint32x4",
    "unorm10-10-10-2",
    "unorm8x4-bgra",
    "snorm10-10-10-2",
];

const TEXTURE_FORMAT_TO_ENUM = new Map(
    TEXTURE_FORMATS.map((name, index) => [name, index]),
);

// WGPUFeatureName -> GPUFeatureName.
const FEATURE_NAMES = [
    undefined,
    "core-features-and-limits",
    "depth-clip-control",
    "depth32float-stencil8",
    "texture-compression-bc",
    "texture-compression-bc-sliced-3d",
    "texture-compression-etc2",
    "texture-compression-astc",
    "texture-compression-astc-sliced-3d",
    "timestamp-query",
    "indirect-first-instance",
    "shader-f16",
    "rg11b10ufloat-renderable",
    "bgra8unorm-storage",
    "float32-filterable",
    "float32-blendable",
    "clip-distances",
    "dual-source-blending",
    "subgroups",
    "texture-formats-tier1",
    "texture-formats-tier2",
    "primitive-index",
    "texture-component-swizzle",
    "subgroup-size-control",
    "texture-compression-unaligned",
    "atomic-vec2u-min-max",
];

const FEATURE_NAME_TO_ENUM = new Map(
    FEATURE_NAMES.map((name, index) => [name, index]),
);

// WGPULimits fields in declaration order; offsets follow wasm32 C layout (u64 is
// 8-byte aligned), starting after nextInChain. Checked against webgpu.h by
// static_asserts on the C++ side (e.g. maxBufferSize at 96, size 152).
const LIMIT_FIELDS = (() => {
    const fields = [
        ["maxTextureDimension1D", 32], ["maxTextureDimension2D", 32], ["maxTextureDimension3D", 32],
        ["maxTextureArrayLayers", 32], ["maxBindGroups", 32], ["maxBindGroupsPlusVertexBuffers", 32],
        ["maxBindingsPerBindGroup", 32], ["maxDynamicUniformBuffersPerPipelineLayout", 32],
        ["maxDynamicStorageBuffersPerPipelineLayout", 32], ["maxSampledTexturesPerShaderStage", 32],
        ["maxSamplersPerShaderStage", 32], ["maxStorageBuffersPerShaderStage", 32],
        ["maxStorageTexturesPerShaderStage", 32], ["maxUniformBuffersPerShaderStage", 32],
        ["maxUniformBufferBindingSize", 64], ["maxStorageBufferBindingSize", 64],
        ["minUniformBufferOffsetAlignment", 32], ["minStorageBufferOffsetAlignment", 32],
        ["maxVertexBuffers", 32], ["maxBufferSize", 64], ["maxVertexAttributes", 32],
        ["maxVertexBufferArrayStride", 32], ["maxInterStageShaderVariables", 32],
        ["maxColorAttachments", 32], ["maxColorAttachmentBytesPerSample", 32],
        ["maxComputeWorkgroupStorageSize", 32], ["maxComputeInvocationsPerWorkgroup", 32],
        ["maxComputeWorkgroupSizeX", 32], ["maxComputeWorkgroupSizeY", 32],
        ["maxComputeWorkgroupSizeZ", 32], ["maxComputeWorkgroupsPerDimension", 32],
        ["maxImmediateSize", 32],
    ];
    let offset = 4;
    return fields.map(([name, bits]) => {
        if (bits === 64) offset = (offset + 7) & ~7;
        const field = { name, bits, offset };
        offset += bits / 8;
        return field;
    });
})();

function decodeEnum(table, value, name) {
    const resolved = table[value >>> 0];
    if (resolved === null) {
        return undefined;
    }
    if (resolved === undefined && value !== 0) {
        throw new RangeError(`Invalid ${name} enum value: ${value >>> 0}`);
    }
    return resolved;
}

// WGPUFlags is 64-bit; every WebGPU usage/mask flag set fits in 32 bits.
function toFlags(bigIntValue, name) {
    const value = BigInt.asUintN(64, bigIntValue);
    if (value > 0xffffffffn) {
        throw new RangeError(`${name} has bits above 32 set: 0x${value.toString(16)}`);
    }
    return Number(value);
}

function toOptionalBool(value, name) {
    switch (value >>> 0) {
        case OPTIONAL_BOOL_FALSE: return false;
        case OPTIONAL_BOOL_TRUE: return true;
        case OPTIONAL_BOOL_UNDEFINED: return undefined;
        default: throw new RangeError(`Invalid ${name} WGPUOptionalBool value: ${value >>> 0}`);
    }
}

function requireZero(value, what) {
    if ((value >>> 0) !== 0) {
        throw new TypeError(`${what} is not supported by this WebGPU binding (must be 0/null)`);
    }
}

function requireEnum(table, value, name) {
    const resolved = decodeEnum(table, value, name);
    if (resolved === undefined) {
        throw new TypeError(`Missing required ${name} enum value`);
    }
    return resolved;
}

function toSafeSize64(bigIntValue, name) {
    const unsigned = BigInt.asUintN(64, bigIntValue);
    if (unsigned > BigInt(Number.MAX_SAFE_INTEGER)) {
        throw new RangeError(`${name} exceeds Number.MAX_SAFE_INTEGER: ${unsigned}`);
    }
    return Number(unsigned);
}

function toOptionalSize64(bigIntValue, name) {
    const unsigned = BigInt.asUintN(64, bigIntValue);
    if (unsigned === WHOLE_SIZE) {
        return undefined;
    }
    return toSafeSize64(unsigned, name);
}

// Slot-based handle table with 12-bit generation counter and 20-bit slot index.
// Handle 0 is reserved as the null sentinel. Each slot is reference counted:
// store() starts at 1, wgpu<Object>AddRef adds one, wgpu<Object>Release removes one
// and frees the slot at zero.
class HandleTable {
    constructor() {
        this.slots = [{ value: null, generation: 0, refs: 0 }];
        this.freeSlots = [];
    }

    store(object) {
        if (object === null || object === undefined) {
            return 0;
        }
        let index;
        let generation;
        if (this.freeSlots.length > 0) {
            index = this.freeSlots.pop();
            const slot = this.slots[index];
            slot.value = object;
            slot.refs = 1;
            generation = slot.generation;
        } else {
            index = this.slots.length;
            if (index > 0xfffff) {
                throw new RangeError("WebGPU handle table capacity exceeded");
            }
            generation = 1;
            this.slots.push({ value: object, generation, refs: 1 });
        }
        return (((generation & 0xfff) << 20) | index) >>> 0;
    }

    get(handle, kind = "WGPUObject") {
        const h = handle >>> 0;
        if (h === 0) {
            throw new TypeError(`Expected valid ${kind} handle, got null (0)`);
        }
        const index = h & 0xfffff;
        const generation = (h >>> 20) & 0xfff;
        const slot = this.slots[index];
        if (!slot || slot.value === null || slot.generation !== generation) {
            throw new TypeError(`Invalid or released ${kind} handle: 0x${h.toString(16)}`);
        }
        return slot.value;
    }

    getOptional(handle, kind = "WGPUObject") {
        const h = handle >>> 0;
        if (h === 0) {
            return undefined;
        }
        return this.get(h, kind);
    }

    addRef(handle) {
        const h = handle >>> 0;
        if (h === 0) {
            throw new TypeError("Cannot AddRef a null WebGPU handle");
        }
        const slot = this.slots[h & 0xfffff];
        if (!slot || slot.value === null || slot.generation !== ((h >>> 20) & 0xfff)) {
            throw new TypeError(`AddRef on invalid or released WebGPU handle: 0x${h.toString(16)}`);
        }
        slot.refs++;
    }

    isLive(handle) {
        const h = handle >>> 0;
        const slot = this.slots[h & 0xfffff];
        return h !== 0 && !!slot && slot.value !== null && slot.generation === ((h >>> 20) & 0xfff);
    }

    // Returns the object if this dropped the last reference, otherwise null.
    release(handle) {
        const h = handle >>> 0;
        if (h === 0) {
            return null;
        }
        const index = h & 0xfffff;
        const generation = (h >>> 20) & 0xfff;
        const slot = this.slots[index];
        if (!slot || slot.value === null || slot.generation !== generation) {
            throw new TypeError(`Double release or invalid WebGPU handle: 0x${h.toString(16)}`);
        }
        if (--slot.refs > 0) {
            return null;
        }
        const object = slot.value;
        slot.value = null;
        slot.generation = ((slot.generation + 1) & 0xfff) || 1;
        this.freeSlots.push(index);
        return object;
    }
}

// `getExports` returns the wasm instance's exports. It is called lazily (the
// imports are built before instantiation). Needed exports: `memory`,
// `__indirect_function_table` (link with --export-table) to call C callbacks,
// and `wgpu_host_alloc` (defined in webgpu.h).
//
// wgpuInstanceWaitAny is a JSPI suspending import: wasm must be entered through
// WebAssembly.promising(...) for it to block.
export function createWebGPUBindings({ getExports }) {
    const handles = new HandleTable();
    const decoder = new TextDecoder();
    const encoder = new TextEncoder();
    let surfaceErrorLogged = false;

    // Always construct views from the live memory.buffer so wasm memory.grow
    // never leaves us reading from a detached ArrayBuffer.
    const buffer = () => getExports().memory.buffer;
    const view = () => new DataView(buffer());

    // ---- Calling C callbacks ------------------------------------------------
    // Struct arguments passed by value (WGPUStringView) are passed as a pointer to
    // a caller-owned copy in the wasm32 C ABI, so the host writes them into a
    // scratch region. The region is used like a stack, so a callback that makes
    // another callback fire (e.g. via wgpuInstanceProcessEvents) is safe.
    let scratchBase = 0;
    let scratchTop = 0;

    function scratchAlloc(size, align) {
        if (scratchBase === 0) {
            scratchBase = getExports().wgpu_host_alloc(CALLBACK_SCRATCH_SIZE) >>> 0;
            if (scratchBase === 0) throw new RangeError("wgpu_host_alloc failed");
        }
        const p = Math.ceil((scratchBase + scratchTop) / align) * align;
        if (p + size > scratchBase + CALLBACK_SCRATCH_SIZE) {
            throw new RangeError("WebGPU callback scratch space exhausted (callbacks nested too deeply)");
        }
        scratchTop = p + size - scratchBase;
        return p;
    }

    // Writes a WGPUStringView (and its bytes) to scratch; returns the view's address.
    function scratchStringView(text) {
        const sv = scratchAlloc(8, 4);
        const data = scratchBase + scratchTop;
        const room = scratchBase + CALLBACK_SCRATCH_SIZE - data;
        const { written } = encoder.encodeInto(text ?? "", new Uint8Array(buffer(), data, room));
        scratchTop += written;
        const dv = view();
        dv.setUint32(sv, written > 0 ? data : 0, true);
        dv.setUint32(sv + 4, written, true);
        return sv;
    }

    // Calls a C function pointer with arguments built by `makeArgs` (which may use
    // scratch); scratch is released when the callback returns.
    function callC(fnPtr, makeArgs) {
        if ((fnPtr >>> 0) === 0) return;
        const mark = scratchTop;
        try {
            const fn = getExports().__indirect_function_table.get(fnPtr >>> 0);
            fn(...makeArgs());
        } finally {
            scratchTop = mark;
        }
    }

    function readCallbackInfo(dv, p) {
        // { nextInChain, mode, callback, userdata1, userdata2 }
        return {
            mode: dv.getUint32(p + 4, true),
            callback: dv.getUint32(p + 8, true),
            userdata1: dv.getUint32(p + 12, true),
            userdata2: dv.getUint32(p + 16, true),
        };
    }

    // Calls a WGPURequest{Adapter,Device}Callback: (status, object, message, ud1, ud2).
    function callRequestCallback(info, status, objectHandle, message) {
        callC(info.callback, () => [status, objectHandle, scratchStringView(message), info.userdata1, info.userdata2]);
    }

    // ---- Futures --------------------------------------------------------------
    // A future completes when its promise settles. Its callback fires according
    // to its WGPUCallbackMode: AllowSpontaneous right away; AllowProcessEvents in
    // wgpuInstanceProcessEvents or wgpuInstanceWaitAny; WaitAnyOnly only in
    // wgpuInstanceWaitAny. Completed futures are kept so a later WaitAny on them
    // reports completed.
    let nextFutureId = 1n;
    const futures = new Map();

    // `promise` must resolve (never reject) to the function that fires the callback.
    function createFuture(mode, promise) {
        if (mode !== CALLBACK_MODE_WAIT_ANY_ONLY && mode !== CALLBACK_MODE_ALLOW_PROCESS_EVENTS
            && mode !== CALLBACK_MODE_ALLOW_SPONTANEOUS) {
            throw new TypeError(`Invalid WGPUCallbackMode: ${mode}`);
        }
        const id = nextFutureId++;
        const future = { mode, ready: false, fired: false, fire: null, settled: null };
        future.settled = promise.then((fire) => {
            future.ready = true;
            future.fire = fire;
            if (mode === CALLBACK_MODE_ALLOW_SPONTANEOUS) fireFuture(future);
        });
        futures.set(id, future);
        return id;
    }

    function fireFuture(future) {
        if (!future.ready || future.fired) return;
        future.fired = true;
        future.fire();
    }

    function errorMessage(error) {
        return String(error?.message ?? error);
    }

    // ---- Instance -----------------------------------------------------------
    function waitAny(instanceH, futureCount, futuresPtr, timeoutNS) {
        const instance = handles.get(instanceH, "WGPUInstance");
        const count = futureCount >>> 0;
        const p = futuresPtr >>> 0;
        const timeout = BigInt.asUintN(64, timeoutNS);
        if (count === 0) {
            return WAIT_STATUS_SUCCESS;
        }

        // WGPUFutureWaitInfo is { WGPUFuture future (u64), WGPUBool completed } = 16 bytes.
        const waited = [];
        {
            const dv = view();
            for (let i = 0; i < count; i++) {
                const id = dv.getBigUint64(p + i * 16, true);
                const future = futures.get(id);
                if (future === undefined) {
                    console.error(`wgpuInstanceWaitAny: unknown WGPUFuture id ${id}`);
                    return WAIT_STATUS_ERROR;
                }
                waited.push(future);
            }
        }

        // Fires the ready callbacks, then marks completed futures.
        const poll = () => {
            for (const future of waited) fireFuture(future);
            const dv = view(); // callbacks may have grown memory
            let any = false;
            waited.forEach((future, i) => {
                if (future.fired) {
                    dv.setUint32(p + i * 16 + 8, 1, true);
                    any = true;
                }
            });
            return any;
        };

        if (poll()) return WAIT_STATUS_SUCCESS;
        if (timeout === 0n) return WAIT_STATUS_TIMED_OUT;
        if (!instance.timedWaitAny) {
            console.error("wgpuInstanceWaitAny with a timeout needs WGPUInstanceFeatureName::TimedWaitAny in WGPUInstanceDescriptor");
            return WAIT_STATUS_ERROR;
        }
        if (count > instance.timedWaitAnyMaxCount) {
            console.error(`wgpuInstanceWaitAny: ${count} futures exceeds timedWaitAnyMaxCount (${instance.timedWaitAnyMaxCount})`);
            return WAIT_STATUS_ERROR;
        }

        // Returning a promise suspends the wasm stack (JSPI) until it settles.
        const waits = waited.map((future) => future.settled);
        if (timeout !== WHOLE_SIZE) {
            const ms = Math.min(Number(timeout / 1000000n), 0x7fffffff);
            waits.push(new Promise((resolve) => setTimeout(resolve, ms)));
        }
        return Promise.race(waits).then(() => (poll() ? WAIT_STATUS_SUCCESS : WAIT_STATUS_TIMED_OUT));
    }

    // ---- Devices ------------------------------------------------------------
    const deviceLostFutures = new WeakMap(); // GPUDevice -> WGPUFuture id

    function deviceTypeOfError(error) {
        if (typeof GPUValidationError !== "undefined" && error instanceof GPUValidationError) return ERROR_TYPE_VALIDATION;
        if (typeof GPUOutOfMemoryError !== "undefined" && error instanceof GPUOutOfMemoryError) return ERROR_TYPE_OUT_OF_MEMORY;
        if (typeof GPUInternalError !== "undefined" && error instanceof GPUInternalError) return ERROR_TYPE_INTERNAL;
        return ERROR_TYPE_UNKNOWN;
    }

    // Callbacks taking `WGPUDevice const* device`: the pointed-to handle is null
    // once the device's last reference is released.
    function scratchDevicePointer(deviceH) {
        const ptr = scratchAlloc(4, 4);
        view().setUint32(ptr, handles.isLive(deviceH) ? deviceH : 0, true);
        return ptr;
    }

    function setUpDevice(deviceH, device, lostInfo, errorInfo) {
        const lostCallback = lostInfo?.callback ?? 0;
        const lostMode = lostCallback !== 0 ? lostInfo.mode : CALLBACK_MODE_ALLOW_SPONTANEOUS;
        const lostFuture = createFuture(lostMode, device.lost.then((info) => () => {
            const reason = info.reason === "destroyed" ? DEVICE_LOST_REASON_DESTROYED : DEVICE_LOST_REASON_UNKNOWN;
            callC(lostCallback, () => [
                scratchDevicePointer(deviceH), reason, scratchStringView(info.message),
                lostInfo.userdata1, lostInfo.userdata2,
            ]);
        }));
        deviceLostFutures.set(device, lostFuture);

        // Without a callback the browser's default (a console warning) stays.
        if (errorInfo.callback !== 0) {
            device.addEventListener("uncapturederror", (event) => {
                event.preventDefault();
                callC(errorInfo.callback, () => [
                    scratchDevicePointer(deviceH), deviceTypeOfError(event.error),
                    scratchStringView(event.error.message), errorInfo.userdata1, errorInfo.userdata2,
                ]);
            });
        }
    }

    function readRequiredLimits(dv, p, adapter) {
        const limits = {};
        for (const { name, bits, offset } of LIMIT_FIELDS) {
            const undefinedValue = bits === 64 ? WHOLE_SIZE : UINT32_UNDEFINED;
            const raw = bits === 64 ? dv.getBigUint64(p + offset, true) : dv.getUint32(p + offset, true);
            if (raw === undefinedValue) continue;
            if (!(name in adapter.limits)) {
                // e.g. maxImmediateSize: not exposed by browsers yet. Copying all of
                // wgpuAdapterGetLimits into requiredLimits gives 0 here, which is fine.
                if (raw !== 0 && raw !== 0n) console.warn(`requiredLimits.${name} is not supported by this browser; ignored`);
                continue;
            }
            limits[name] = bits === 64 ? toSafeSize64(raw, `requiredLimits.${name}`) : raw;
        }
        return limits;
    }

    function writeLimits(limits, ptr) {
        const dv = view();
        const p = ptr >>> 0;
        for (const { name, bits, offset } of LIMIT_FIELDS) {
            const value = limits[name] ?? 0;
            if (bits === 64) {
                dv.setBigUint64(p + offset, BigInt(value), true);
            } else {
                dv.setUint32(p + offset, value, true);
            }
        }
        return STATUS_SUCCESS;
    }

    // ---- Features and adapter info ----------------------------------------------
    // Immutable data is allocated once per object and owned by the host.
    const featureArrays = new WeakMap();
    const adapterInfoStrings = new WeakMap();
    const deviceAdapters = new WeakMap();

    function getFeatureArray(gpuObject) {
        const cached = featureArrays.get(gpuObject);
        if (cached !== undefined) return cached;
        const features = [];
        for (const name of gpuObject.features) {
            const value = FEATURE_NAME_TO_ENUM.get(name);
            if (value !== undefined) features.push(value);
        }
        let ptr = 0;
        if (features.length > 0) {
            ptr = getExports().wgpu_host_alloc(features.length * 4) >>> 0;
            if (ptr === 0) throw new RangeError("wgpu_host_alloc failed");
            const dv = view();
            features.forEach((value, i) => dv.setUint32(ptr + i * 4, value, true));
        }
        const array = { ptr, count: features.length };
        featureArrays.set(gpuObject, array);
        return array;
    }

    function writeSupportedFeatures(gpuObject, ptr) {
        const array = getFeatureArray(gpuObject);
        const dv = view();
        const p = ptr >>> 0;
        dv.setUint32(p, array.count, true);
        dv.setUint32(p + 4, array.ptr, true);
    }

    function hasFeature(gpuObject, value) {
        const name = FEATURE_NAMES[value >>> 0];
        return name !== undefined && gpuObject.features.has(name) ? 1 : 0;
    }

    function getAdapterInfoStrings(info) {
        const cached = adapterInfoStrings.get(info);
        if (cached !== undefined) return cached;
        const strings = [info.vendor, info.architecture, info.device, info.description]
            .map((text) => encoder.encode(text));
        const size = strings.reduce((total, bytes) => total + bytes.length, 0);
        let ptr = 0;
        if (size > 0) {
            ptr = getExports().wgpu_host_alloc(size) >>> 0;
            if (ptr === 0) throw new RangeError("wgpu_host_alloc failed");
        }
        const memory = new Uint8Array(buffer());
        const views = strings.map((bytes) => {
            memory.set(bytes, ptr);
            const string = { ptr: bytes.length > 0 ? ptr : 0, len: bytes.length };
            ptr += bytes.length;
            return string;
        });
        adapterInfoStrings.set(info, views);
        return views;
    }

    function writeAdapterInfo(info, ptr) {
        const strings = getAdapterInfoStrings(info);
        const dv = view();
        const p = ptr >>> 0;
        strings.forEach((string, i) => {
            dv.setUint32(p + 4 + i * 8, string.ptr, true);
            dv.setUint32(p + 8 + i * 8, string.len, true);
        });
        dv.setUint32(p + 36, BACKEND_TYPE_WEBGPU, true); // the browser hides the native backend
        dv.setUint32(p + 40, info.isFallbackAdapter ? ADAPTER_TYPE_CPU : ADAPTER_TYPE_UNKNOWN, true);
        dv.setUint32(p + 44, 0, true); // vendorID is not exposed by browsers
        dv.setUint32(p + 48, 0, true); // deviceID is not exposed by browsers
        dv.setUint32(p + 52, info.subgroupMinSize ?? 0, true);
        dv.setUint32(p + 56, info.subgroupMaxSize ?? 0, true);
        return STATUS_SUCCESS;
    }

    // ---- Surface capabilities ---------------------------------------------------
    // Arrays live in wasm memory owned by the host, allocated once and shared by
    // every WGPUSurfaceCapabilities (FreeMembers is a no-op).
    let surfaceCapabilityArrays = null;

    function getSurfaceCapabilityArrays() {
        if (surfaceCapabilityArrays !== null) return surfaceCapabilityArrays;
        const preferred = TEXTURE_FORMAT_TO_ENUM.get(navigator.gpu.getPreferredCanvasFormat());
        const formats = [preferred];
        for (const name of ["bgra8unorm", "rgba8unorm", "rgba16float"]) {
            const value = TEXTURE_FORMAT_TO_ENUM.get(name);
            if (value !== preferred) formats.push(value);
        }
        const presentModes = [PRESENT_MODE_FIFO];
        const alphaModes = [COMPOSITE_ALPHA_OPAQUE, COMPOSITE_ALPHA_PREMULTIPLIED];
        const all = [...formats, ...presentModes, ...alphaModes];
        const base = getExports().wgpu_host_alloc(all.length * 4) >>> 0;
        if (base === 0) throw new RangeError("wgpu_host_alloc failed");
        const dv = view();
        all.forEach((value, i) => dv.setUint32(base + i * 4, value, true));
        surfaceCapabilityArrays = {
            formats: base, formatCount: formats.length,
            presentModes: base + formats.length * 4, presentModeCount: presentModes.length,
            alphaModes: base + (formats.length + presentModes.length) * 4, alphaModeCount: alphaModes.length,
        };
        return surfaceCapabilityArrays;
    }

    function readString(dv, offset) {
        const ptr = dv.getUint32(offset, true);
        let len = dv.getUint32(offset + 4, true);
        if (ptr === 0) {
            return undefined;
        }
        const bytes = new Uint8Array(buffer());
        if (len === STRLEN) {
            let end = ptr;
            while (end < bytes.length && bytes[end] !== 0) {
                end++;
            }
            len = end - ptr;
        }
        if (len === 0) {
            return undefined;
        }
        return decoder.decode(bytes.subarray(ptr, ptr + len));
    }

    function readColor(dv, offset) {
        return {
            r: dv.getFloat64(offset, true),
            g: dv.getFloat64(offset + 8, true),
            b: dv.getFloat64(offset + 16, true),
            a: dv.getFloat64(offset + 24, true),
        };
    }

    function readOrigin3D(dv, offset) {
        return {
            x: dv.getUint32(offset, true),
            y: dv.getUint32(offset + 4, true),
            z: dv.getUint32(offset + 8, true),
        };
    }

    function readExtent3D(dv, offset) {
        // Defaults (height/depthOrArrayLayers = 1) live in WGPUExtent3D on the C++ side.
        return {
            width: dv.getUint32(offset, true),
            height: dv.getUint32(offset + 4, true),
            depthOrArrayLayers: dv.getUint32(offset + 8, true),
        };
    }

    function readFormatArray(dv, ptr, count) {
        if (ptr === 0 || count === 0) {
            return undefined;
        }
        const formats = new Array(count);
        for (let i = 0; i < count; i++) {
            formats[i] = requireEnum(
                TEXTURE_FORMATS,
                dv.getUint32(ptr + i * 4, true),
                "WGPUTextureFormat",
            );
        }
        return formats;
    }

    function readConstants(dv, count, ptr) {
        if (ptr === 0 || count === 0) {
            return undefined;
        }
        const constants = {};
        for (let i = 0; i < count; i++) {
            const cp = ptr + i * 24;
            const key = readString(dv, cp + 4);
            if (key !== undefined) {
                constants[key] = dv.getFloat64(cp + 16, true);
            }
        }
        return constants;
    }

    function readTexelCopyBufferLayout(dv, offset) {
        const layout = {
            offset: toSafeSize64(dv.getBigUint64(offset, true), "layout.offset"),
        };
        const bytesPerRow = dv.getUint32(offset + 8, true);
        const rowsPerImage = dv.getUint32(offset + 12, true);
        if (bytesPerRow !== COPY_STRIDE_UNDEFINED) layout.bytesPerRow = bytesPerRow;
        if (rowsPerImage !== COPY_STRIDE_UNDEFINED) layout.rowsPerImage = rowsPerImage;
        return layout;
    }

    function readTexelCopyBufferInfo(dv, ptr) {
        const p = ptr >>> 0;
        const layout = readTexelCopyBufferLayout(dv, p);
        return {
            ...layout,
            buffer: handles.get(dv.getUint32(p + 16, true), "WGPUBuffer"),
        };
    }

    function readTexelCopyTextureInfo(dv, ptr) {
        const p = ptr >>> 0;
        const info = {
            texture: handles.get(dv.getUint32(p, true), "WGPUTexture"),
            mipLevel: dv.getUint32(p + 4, true),
            origin: readOrigin3D(dv, p + 8),
        };
        const aspect = decodeEnum(TEXTURE_ASPECTS, dv.getUint32(p + 20, true), "WGPUTextureAspect");
        if (aspect !== undefined) info.aspect = aspect;
        return info;
    }

    function readStencilFaceState(dv, offset) {
        const state = {};
        const compare = decodeEnum(COMPARE_FUNCTIONS, dv.getUint32(offset, true), "WGPUCompareFunction");
        const failOp = decodeEnum(STENCIL_OPERATIONS, dv.getUint32(offset + 4, true), "WGPUStencilOperation");
        const depthFailOp = decodeEnum(STENCIL_OPERATIONS, dv.getUint32(offset + 8, true), "WGPUStencilOperation");
        const passOp = decodeEnum(STENCIL_OPERATIONS, dv.getUint32(offset + 12, true), "WGPUStencilOperation");
        if (compare !== undefined) state.compare = compare;
        if (failOp !== undefined) state.failOp = failOp;
        if (depthFailOp !== undefined) state.depthFailOp = depthFailOp;
        if (passOp !== undefined) state.passOp = passOp;
        return state;
    }

    function readBlendComponent(dv, offset) {
        const comp = {};
        const operation = decodeEnum(BLEND_OPERATIONS, dv.getUint32(offset, true), "WGPUBlendOperation");
        const srcFactor = decodeEnum(BLEND_FACTORS, dv.getUint32(offset + 4, true), "WGPUBlendFactor");
        const dstFactor = decodeEnum(BLEND_FACTORS, dv.getUint32(offset + 8, true), "WGPUBlendFactor");
        if (operation !== undefined) comp.operation = operation;
        if (srcFactor !== undefined) comp.srcFactor = srcFactor;
        if (dstFactor !== undefined) comp.dstFactor = dstFactor;
        return comp;
    }

    function readDynamicOffsets(count, ptr) {
        const c = count >>> 0;
        if (ptr === 0 || c === 0) {
            return undefined;
        }
        return new Uint32Array(buffer(), ptr >>> 0, c);
    }

    const release = (handle) => {
        handles.release(handle);
    };
    const addRef = (handle) => {
        handles.addRef(handle);
    };

    const imports = {
        // Instance
        wgpuCreateInstance(descPtr) {
            if (!navigator.gpu) {
                console.error("wgpuCreateInstance: WebGPU is not supported by this browser");
                return 0;
            }
            const instance = { timedWaitAny: false, timedWaitAnyMaxCount: 0 };
            const p = descPtr >>> 0;
            if (p !== 0) {
                const dv = view();
                const featureCount = dv.getUint32(p + 4, true);
                const featuresPtr = dv.getUint32(p + 8, true);
                for (let i = 0; i < featureCount; i++) {
                    const feature = dv.getUint32(featuresPtr + i * 4, true);
                    if (feature !== INSTANCE_FEATURE_TIMED_WAIT_ANY) {
                        console.error(`wgpuCreateInstance: instance feature ${feature} is not supported`);
                        return 0;
                    }
                    instance.timedWaitAny = true;
                }
                const limitsPtr = dv.getUint32(p + 12, true);
                if (limitsPtr !== 0) {
                    const requested = dv.getUint32(limitsPtr + 4, true);
                    if (requested > MAX_TIMED_WAIT_ANY_COUNT) {
                        console.error(`wgpuCreateInstance: timedWaitAnyMaxCount ${requested} exceeds the supported ${MAX_TIMED_WAIT_ANY_COUNT}`);
                        return 0;
                    }
                    instance.timedWaitAnyMaxCount = requested;
                }
            }
            if (instance.timedWaitAny && instance.timedWaitAnyMaxCount === 0) {
                instance.timedWaitAnyMaxCount = MAX_TIMED_WAIT_ANY_COUNT;
            }
            return handles.store(instance);
        },

        wgpuGetInstanceLimits(limitsPtr) {
            view().setUint32((limitsPtr >>> 0) + 4, MAX_TIMED_WAIT_ANY_COUNT, true);
            return STATUS_SUCCESS;
        },

        wgpuHasInstanceFeature(feature) {
            return (feature >>> 0) === INSTANCE_FEATURE_TIMED_WAIT_ANY ? 1 : 0;
        },

        // WGPUInstance
        wgpuInstanceCreateSurface(instanceH, descPtr) {
            handles.get(instanceH, "WGPUInstance");
            const dv = view();
            let selector;
            for (let chain = dv.getUint32(descPtr >>> 0, true); chain !== 0; chain = dv.getUint32(chain, true)) {
                const sType = dv.getUint32(chain + 4, true);
                if (sType !== STYPE_EMSCRIPTEN_CANVAS_SELECTOR) {
                    throw new TypeError(`Unsupported WGPUSurfaceDescriptor chain sType: 0x${sType.toString(16)}`);
                }
                selector = readString(dv, chain + 8);
            }
            if (selector === undefined) {
                throw new TypeError("WGPUSurfaceDescriptor needs a chained WGPUEmscriptenSurfaceSourceCanvasHTMLSelector");
            }
            const canvas = document.querySelector(selector);
            if (!(canvas instanceof HTMLCanvasElement)) {
                throw new TypeError(`wgpuInstanceCreateSurface: "${selector}" does not match a <canvas>`);
            }
            return handles.store(canvas.getContext("webgpu"));
        },

        wgpuInstanceRequestAdapter(instanceH, optionsPtr, callbackInfoPtr) {
            handles.get(instanceH, "WGPUInstance");
            const dv = view();
            const p = optionsPtr >>> 0;
            const options = {};
            if (p !== 0) {
                const featureLevel = dv.getUint32(p + 4, true);
                if (featureLevel === FEATURE_LEVEL_COMPATIBILITY) {
                    options.featureLevel = "compatibility";
                } else if (featureLevel !== 0 && featureLevel !== FEATURE_LEVEL_CORE) {
                    throw new RangeError(`Invalid WGPUFeatureLevel value: ${featureLevel}`);
                }
                const powerPreference = dv.getUint32(p + 8, true);
                if (powerPreference === 1) options.powerPreference = "low-power";
                else if (powerPreference === 2) options.powerPreference = "high-performance";
                if (dv.getUint32(p + 12, true) !== 0) options.forceFallbackAdapter = true;
                // backendType (p + 16) and compatibleSurface (p + 20) do not apply in a browser.
            }
            const info = readCallbackInfo(dv, callbackInfoPtr >>> 0);
            const promise = navigator.gpu.requestAdapter(options).then(
                (adapter) => adapter
                    ? () => callRequestCallback(info, REQUEST_ADAPTER_STATUS_SUCCESS, handles.store(adapter), "")
                    : () => callRequestCallback(info, REQUEST_ADAPTER_STATUS_UNAVAILABLE, 0, "No WebGPU adapter is available"),
                (error) => () => callRequestCallback(info, REQUEST_ADAPTER_STATUS_ERROR, 0, errorMessage(error)),
            );
            return createFuture(info.mode, promise);
        },

        wgpuInstanceWaitAny: new WebAssembly.Suspending(waitAny),

        wgpuInstanceProcessEvents(instanceH) {
            handles.get(instanceH, "WGPUInstance");
            for (const future of futures.values()) {
                if (future.mode === CALLBACK_MODE_ALLOW_PROCESS_EVENTS) fireFuture(future);
            }
        },

        wgpuInstanceRelease: release,
        wgpuInstanceAddRef: addRef,

        // WGPUAdapter
        wgpuAdapterGetFeatures(adapterH, featuresPtr) {
            const adapter = handles.get(adapterH, "WGPUAdapter");
            writeSupportedFeatures(adapter, featuresPtr);
        },

        wgpuAdapterHasFeature(adapterH, feature) {
            const adapter = handles.get(adapterH, "WGPUAdapter");
            return hasFeature(adapter, feature);
        },

        wgpuAdapterGetInfo(adapterH, infoPtr) {
            const adapter = handles.get(adapterH, "WGPUAdapter");
            return writeAdapterInfo(adapter.info, infoPtr);
        },

        wgpuSupportedFeaturesFreeMembers() {
            // The arrays are cached and owned by the host.
        },

        wgpuAdapterInfoFreeMembers() {
            // The strings are cached and owned by the host.
        },

        wgpuAdapterGetLimits(adapterH, limitsPtr) {
            return writeLimits(handles.get(adapterH, "WGPUAdapter").limits, limitsPtr);
        },

        wgpuAdapterRequestDevice(adapterH, descPtr, callbackInfoPtr) {
            const adapter = handles.get(adapterH, "WGPUAdapter");
            const dv = view();
            const p = descPtr >>> 0;
            const desc = {};
            let lostInfo = null;
            let errorInfo = { callback: 0 };
            if (p !== 0) {
                const label = readString(dv, p + 4);
                if (label !== undefined) desc.label = label;
                const featureCount = dv.getUint32(p + 12, true);
                const featuresPtr = dv.getUint32(p + 16, true);
                if (featureCount > 0) {
                    desc.requiredFeatures = [];
                    for (let i = 0; i < featureCount; i++) {
                        desc.requiredFeatures.push(requireEnum(FEATURE_NAMES, dv.getUint32(featuresPtr + i * 4, true), "WGPUFeatureName"));
                    }
                }
                const limitsPtr = dv.getUint32(p + 20, true);
                if (limitsPtr !== 0) desc.requiredLimits = readRequiredLimits(dv, limitsPtr, adapter);
                const queueLabel = readString(dv, p + 28); // defaultQueue at p + 24
                if (queueLabel !== undefined) desc.defaultQueue = { label: queueLabel };
                lostInfo = readCallbackInfo(dv, p + 36);
                // WGPUUncapturedErrorCallbackInfo at p + 56: { nextInChain, callback, userdata1, userdata2 }
                errorInfo = {
                    callback: dv.getUint32(p + 60, true),
                    userdata1: dv.getUint32(p + 64, true),
                    userdata2: dv.getUint32(p + 68, true),
                };
            }
            const info = readCallbackInfo(dv, callbackInfoPtr >>> 0);
            const promise = adapter.requestDevice(desc).then(
                (device) => () => {
                    const deviceH = handles.store(device);
                    deviceAdapters.set(device, adapter);
                    setUpDevice(deviceH, device, lostInfo, errorInfo);
                    callRequestCallback(info, REQUEST_DEVICE_STATUS_SUCCESS, deviceH, "");
                },
                (error) => () => callRequestCallback(info, REQUEST_DEVICE_STATUS_ERROR, 0, errorMessage(error)),
            );
            return createFuture(info.mode, promise);
        },

        // WGPUSurface
        wgpuSurfaceGetCapabilities(surfaceH, adapterH, capabilitiesPtr) {
            handles.get(surfaceH, "WGPUSurface");
            handles.get(adapterH, "WGPUAdapter");
            const arrays = getSurfaceCapabilityArrays();
            const p = capabilitiesPtr >>> 0;
            const dv = view();
            dv.setBigUint64(p + 8, BigInt(CANVAS_TEXTURE_USAGES), true);
            dv.setUint32(p + 16, arrays.formatCount, true);
            dv.setUint32(p + 20, arrays.formats, true);
            dv.setUint32(p + 24, arrays.presentModeCount, true);
            dv.setUint32(p + 28, arrays.presentModes, true);
            dv.setUint32(p + 32, arrays.alphaModeCount, true);
            dv.setUint32(p + 36, arrays.alphaModes, true);
            return STATUS_SUCCESS;
        },

        wgpuSurfaceCapabilitiesFreeMembers() {
            // The arrays are shared and owned by the host.
        },

        wgpuSurfaceConfigure(surfaceH, configPtr) {
            const ctx = handles.get(surfaceH, "WGPUSurface");
            const p = configPtr >>> 0;
            const dv = view();
            const cfgDevice = handles.get(dv.getUint32(p + 4, true), "WGPUDevice");
            const cfgFormat = requireEnum(TEXTURE_FORMATS, dv.getUint32(p + 8, true), "WGPUTextureFormat");
            const usage = toFlags(dv.getBigUint64(p + 16, true), "WGPUSurfaceConfiguration.usage");
            // width (p + 24), height (p + 28) and presentMode (p + 44) are ignored.
            const viewFormatCount = dv.getUint32(p + 32, true);
            const viewFormatsPtr = dv.getUint32(p + 36, true);
            const alphaModeValue = dv.getUint32(p + 40, true);
            if (alphaModeValue === COMPOSITE_ALPHA_UNPREMULTIPLIED || alphaModeValue === COMPOSITE_ALPHA_INHERIT) {
                throw new TypeError("WGPUCompositeAlphaMode::Unpremultiplied/Inherit are not supported by browser canvases");
            }
            const alphaMode = decodeEnum(COMPOSITE_ALPHA_MODES, alphaModeValue, "WGPUCompositeAlphaMode");

            // No width/height: the host page owns the canvas backing-store size (see
            // the ResizeObserver in main.js), and a canvas context always matches its canvas.
            const config = {
                device: cfgDevice,
                format: cfgFormat,
                usage,
            };
            if (alphaMode !== undefined) config.alphaMode = alphaMode;
            const viewFormats = readFormatArray(dv, viewFormatsPtr, viewFormatCount);
            if (viewFormats !== undefined) config.viewFormats = viewFormats;
            ctx.configure(config);
        },

        wgpuSurfaceUnconfigure(surfaceH) {
            handles.get(surfaceH, "WGPUSurface").unconfigure();
        },

        wgpuSurfaceGetCurrentTexture(surfaceH, surfaceTexturePtr) {
            const ctx = handles.get(surfaceH, "WGPUSurface");
            const p = surfaceTexturePtr >>> 0;
            const dv = view();
            // Each call stores a NEW handle. The caller owns it and must call
            // wgpuTextureRelease on it once per frame, or the handle table fills up.
            try {
                const texHandle = handles.store(ctx.getCurrentTexture());
                dv.setUint32(p + 4, texHandle, true);
                dv.setUint32(p + 8, SURFACE_STATUS_SUCCESS_OPTIMAL, true);
                surfaceErrorLogged = false;
            } catch (error) {
                // Log once per failure streak: this is called every frame.
                if (!surfaceErrorLogged) {
                    console.error("wgpuSurfaceGetCurrentTexture failed (further errors suppressed until it succeeds):", error);
                    surfaceErrorLogged = true;
                }
                dv.setUint32(p + 4, 0, true);
                dv.setUint32(p + 8, SURFACE_STATUS_ERROR, true);
            }
        },

        wgpuSurfacePresent(surfaceH) {
            // Browser canvases present automatically at the end of the animation frame.
            handles.get(surfaceH, "WGPUSurface");
            return STATUS_SUCCESS;
        },

        wgpuSurfaceRelease: release,
        wgpuSurfaceAddRef: addRef,
        wgpuAdapterRelease: release,
        wgpuAdapterAddRef: addRef,

        // WGPUDevice
        wgpuDeviceGetFeatures(deviceH, featuresPtr) {
            const device = handles.get(deviceH, "WGPUDevice");
            writeSupportedFeatures(device, featuresPtr);
        },

        wgpuDeviceHasFeature(deviceH, feature) {
            const device = handles.get(deviceH, "WGPUDevice");
            return hasFeature(device, feature);
        },

        wgpuDeviceGetAdapterInfo(deviceH, infoPtr) {
            const device = handles.get(deviceH, "WGPUDevice");
            return writeAdapterInfo(device.adapterInfo ?? deviceAdapters.get(device).info, infoPtr);
        },

        wgpuDeviceGetQueue(deviceH) {
            const dev = handles.get(deviceH, "WGPUDevice");
            return handles.store(dev.queue);
        },

        wgpuDeviceGetLimits(deviceH, limitsPtr) {
            return writeLimits(handles.get(deviceH, "WGPUDevice").limits, limitsPtr);
        },

        wgpuDeviceGetLostFuture(deviceH) {
            const id = deviceLostFutures.get(handles.get(deviceH, "WGPUDevice"));
            if (id === undefined) throw new TypeError("wgpuDeviceGetLostFuture: device has no lost future");
            return id;
        },

        wgpuDeviceCreateBuffer(deviceH, descPtr) {
            const dev = handles.get(deviceH, "WGPUDevice");
            const p = descPtr >>> 0;
            const dv = view();
            const label = readString(dv, p + 4);
            if (dv.getUint32(p + 32, true) !== 0) {
                throw new TypeError("WGPUBufferDescriptor.mappedAtCreation is not supported: buffer mapping is not implemented, use wgpuQueueWriteBuffer");
            }
            const desc = {
                usage: toFlags(dv.getBigUint64(p + 16, true), "WGPUBufferDescriptor.usage"),
                size: toSafeSize64(dv.getBigUint64(p + 24, true), "WGPUBufferDescriptor.size"),
            };
            if (label !== undefined) desc.label = label;
            return handles.store(dev.createBuffer(desc));
        },

        wgpuDeviceCreateTexture(deviceH, descPtr) {
            const dev = handles.get(deviceH, "WGPUDevice");
            const p = descPtr >>> 0;
            const dv = view();
            const label = readString(dv, p + 4);
            const usage = toFlags(dv.getBigUint64(p + 16, true), "WGPUTextureDescriptor.usage");
            const dimension = decodeEnum(TEXTURE_DIMENSIONS, dv.getUint32(p + 24, true), "WGPUTextureDimension");
            const size = readExtent3D(dv, p + 28);
            const formatVal = requireEnum(TEXTURE_FORMATS, dv.getUint32(p + 40, true), "WGPUTextureFormat");
            const mipLevelCount = dv.getUint32(p + 44, true);
            const sampleCount = dv.getUint32(p + 48, true);
            const viewFormatCount = dv.getUint32(p + 52, true);
            const viewFormatsPtr = dv.getUint32(p + 56, true);

            const desc = {
                size,
                mipLevelCount,
                sampleCount,
                format: formatVal,
                usage,
            };
            if (label !== undefined) desc.label = label;
            if (dimension !== undefined) desc.dimension = dimension;
            const viewFormats = readFormatArray(dv, viewFormatsPtr, viewFormatCount);
            if (viewFormats !== undefined) desc.viewFormats = viewFormats;
            return handles.store(dev.createTexture(desc));
        },

        wgpuDeviceCreateSampler(deviceH, descPtr) {
            const dev = handles.get(deviceH, "WGPUDevice");
            const p = descPtr >>> 0;
            if (p === 0) {
                return handles.store(dev.createSampler());
            }
            const dv = view();
            const desc = {};
            const label = readString(dv, p + 4);
            const addressModeU = decodeEnum(ADDRESS_MODES, dv.getUint32(p + 12, true), "WGPUAddressMode");
            const addressModeV = decodeEnum(ADDRESS_MODES, dv.getUint32(p + 16, true), "WGPUAddressMode");
            const addressModeW = decodeEnum(ADDRESS_MODES, dv.getUint32(p + 20, true), "WGPUAddressMode");
            const magFilter = decodeEnum(FILTER_MODES, dv.getUint32(p + 24, true), "WGPUFilterMode");
            const minFilter = decodeEnum(FILTER_MODES, dv.getUint32(p + 28, true), "WGPUFilterMode");
            const mipmapFilter = decodeEnum(MIPMAP_FILTER_MODES, dv.getUint32(p + 32, true), "WGPUMipmapFilterMode");
            const lodMinClamp = dv.getFloat32(p + 36, true);
            const lodMaxClamp = dv.getFloat32(p + 40, true);
            const compare = decodeEnum(COMPARE_FUNCTIONS, dv.getUint32(p + 44, true), "WGPUCompareFunction");
            const maxAnisotropy = dv.getUint16(p + 48, true);

            if (label !== undefined) desc.label = label;
            if (addressModeU !== undefined) desc.addressModeU = addressModeU;
            if (addressModeV !== undefined) desc.addressModeV = addressModeV;
            if (addressModeW !== undefined) desc.addressModeW = addressModeW;
            if (magFilter !== undefined) desc.magFilter = magFilter;
            if (minFilter !== undefined) desc.minFilter = minFilter;
            if (mipmapFilter !== undefined) desc.mipmapFilter = mipmapFilter;
            desc.lodMinClamp = lodMinClamp;
            desc.lodMaxClamp = lodMaxClamp;
            if (compare !== undefined) desc.compare = compare;
            desc.maxAnisotropy = maxAnisotropy;
            return handles.store(dev.createSampler(desc));
        },

        wgpuDeviceCreateBindGroupLayout(deviceH, descPtr) {
            const dev = handles.get(deviceH, "WGPUDevice");
            const p = descPtr >>> 0;
            const dv = view();
            const label = readString(dv, p + 4);
            const entryCount = dv.getUint32(p + 12, true);
            const entriesPtr = dv.getUint32(p + 16, true);
            const entries = new Array(entryCount);

            for (let i = 0; i < entryCount; i++) {
                const ep = entriesPtr + i * 88;
                requireZero(dv.getUint32(ep + 16, true), "WGPUBindGroupLayoutEntry.bindingArraySize");
                const entry = {
                    binding: dv.getUint32(ep + 4, true),
                    visibility: toFlags(dv.getBigUint64(ep + 8, true), "WGPUBindGroupLayoutEntry.visibility"),
                };

                // For each of the four binding kinds: type BindingNotUsed (0) means the
                // entry is not of that kind; Undefined (1) means "use the WebGPU default".
                const bufferTypeValue = dv.getUint32(ep + 28, true);
                if (bufferTypeValue !== BINDING_NOT_USED) {
                    const buffer = {
                        hasDynamicOffset: dv.getUint32(ep + 32, true) !== 0,
                        minBindingSize: toSafeSize64(dv.getBigUint64(ep + 40, true), "minBindingSize"),
                    };
                    const type = decodeEnum(BUFFER_BINDING_TYPES, bufferTypeValue, "WGPUBufferBindingType");
                    if (type !== undefined) buffer.type = type;
                    entry.buffer = buffer;
                }

                const samplerTypeValue = dv.getUint32(ep + 52, true);
                if (samplerTypeValue !== BINDING_NOT_USED) {
                    const sampler = {};
                    const type = decodeEnum(SAMPLER_BINDING_TYPES, samplerTypeValue, "WGPUSamplerBindingType");
                    if (type !== undefined) sampler.type = type;
                    entry.sampler = sampler;
                }

                const sampleTypeValue = dv.getUint32(ep + 60, true);
                if (sampleTypeValue !== BINDING_NOT_USED) {
                    const tex = { multisampled: dv.getUint32(ep + 68, true) !== 0 };
                    const sampleType = decodeEnum(TEXTURE_SAMPLE_TYPES, sampleTypeValue, "WGPUTextureSampleType");
                    if (sampleType !== undefined) tex.sampleType = sampleType;
                    const viewDim = decodeEnum(TEXTURE_VIEW_DIMENSIONS, dv.getUint32(ep + 64, true), "WGPUTextureViewDimension");
                    if (viewDim !== undefined) tex.viewDimension = viewDim;
                    entry.texture = tex;
                }

                const storageAccessValue = dv.getUint32(ep + 76, true);
                if (storageAccessValue !== BINDING_NOT_USED) {
                    const st = {
                        format: requireEnum(TEXTURE_FORMATS, dv.getUint32(ep + 80, true), "WGPUTextureFormat"),
                    };
                    const access = decodeEnum(STORAGE_TEXTURE_ACCESS, storageAccessValue, "WGPUStorageTextureAccess");
                    if (access !== undefined) st.access = access;
                    const viewDim = decodeEnum(TEXTURE_VIEW_DIMENSIONS, dv.getUint32(ep + 84, true), "WGPUTextureViewDimension");
                    if (viewDim !== undefined) st.viewDimension = viewDim;
                    entry.storageTexture = st;
                }

                entries[i] = entry;
            }

            const desc = { entries };
            if (label !== undefined) desc.label = label;
            return handles.store(dev.createBindGroupLayout(desc));
        },

        wgpuDeviceCreatePipelineLayout(deviceH, descPtr) {
            const dev = handles.get(deviceH, "WGPUDevice");
            const p = descPtr >>> 0;
            const dv = view();
            const label = readString(dv, p + 4);
            const layoutCount = dv.getUint32(p + 12, true);
            const layoutsPtr = dv.getUint32(p + 16, true);
            requireZero(dv.getUint32(p + 20, true), "WGPUPipelineLayoutDescriptor.immediateSize");
            const bindGroupLayouts = new Array(layoutCount);
            for (let i = 0; i < layoutCount; i++) {
                bindGroupLayouts[i] = handles.getOptional(dv.getUint32(layoutsPtr + i * 4, true), "WGPUBindGroupLayout") ?? null;
            }
            const desc = { bindGroupLayouts };
            if (label !== undefined) desc.label = label;
            return handles.store(dev.createPipelineLayout(desc));
        },

        wgpuDeviceCreateBindGroup(deviceH, descPtr) {
            const dev = handles.get(deviceH, "WGPUDevice");
            const p = descPtr >>> 0;
            const dv = view();
            const label = readString(dv, p + 4);
            const layout = handles.get(dv.getUint32(p + 12, true), "WGPUBindGroupLayout");
            const entryCount = dv.getUint32(p + 16, true);
            const entriesPtr = dv.getUint32(p + 20, true);
            const entries = new Array(entryCount);

            for (let i = 0; i < entryCount; i++) {
                const ep = entriesPtr + i * 40;
                const binding = dv.getUint32(ep + 4, true);
                const bufHandle = dv.getUint32(ep + 8, true);
                const samplerHandle = dv.getUint32(ep + 32, true);
                const texViewHandle = dv.getUint32(ep + 36, true);

                let resource;
                if (bufHandle !== 0) {
                    resource = {
                        buffer: handles.get(bufHandle, "WGPUBuffer"),
                        offset: toSafeSize64(dv.getBigUint64(ep + 16, true), "WGPUBindGroupEntry.offset"),
                    };
                    const size = toOptionalSize64(dv.getBigUint64(ep + 24, true), "WGPUBindGroupEntry.size");
                    if (size !== undefined) resource.size = size;
                } else if (samplerHandle !== 0) {
                    resource = handles.get(samplerHandle, "WGPUSampler");
                } else if (texViewHandle !== 0) {
                    resource = handles.get(texViewHandle, "WGPUTextureView");
                } else {
                    throw new TypeError(`WGPUBindGroupEntry at index ${i} has no buffer, sampler, or textureView`);
                }

                entries[i] = { binding, resource };
            }

            const desc = { layout, entries };
            if (label !== undefined) desc.label = label;
            return handles.store(dev.createBindGroup(desc));
        },

        wgpuDeviceCreateShaderModule(deviceH, descPtr) {
            const dev = handles.get(deviceH, "WGPUDevice");
            const p = descPtr >>> 0;
            const dv = view();
            const nextInChain = dv.getUint32(p, true);
            const label = readString(dv, p + 4);

            // The source comes from a chained WGPUShaderSourceWGSL.
            let code;
            let foundWgsl = false;
            for (let chain = nextInChain; chain !== 0; chain = dv.getUint32(chain, true)) {
                const sType = dv.getUint32(chain + 4, true);
                if (sType !== STYPE_SHADER_SOURCE_WGSL) {
                    throw new TypeError(`Unsupported WGPUShaderModuleDescriptor chain sType: ${sType}`);
                }
                code = readString(dv, chain + 8);
                foundWgsl = true;
            }
            if (!foundWgsl) {
                throw new TypeError("WGPUShaderModuleDescriptor needs a chained WGPUShaderSourceWGSL");
            }

            const desc = { code: code ?? "" };
            if (label !== undefined) desc.label = label;
            return handles.store(dev.createShaderModule(desc));
        },

        wgpuDeviceCreateRenderPipeline(deviceH, descPtr) {
            const dev = handles.get(deviceH, "WGPUDevice");
            const p = descPtr >>> 0;
            const dv = view();
            const label = readString(dv, p + 4);
            const layoutHandle = dv.getUint32(p + 12, true);

            // Vertex state at p + 16
            const vertexModule = handles.get(dv.getUint32(p + 20, true), "WGPUShaderModule");
            const vertexEntryPoint = readString(dv, p + 24);
            const vertexConstants = readConstants(dv, dv.getUint32(p + 32, true), dv.getUint32(p + 36, true));
            const vertexBufferCount = dv.getUint32(p + 40, true);
            const vertexBuffersPtr = dv.getUint32(p + 44, true);
            const vertexBuffers = [];

            for (let i = 0; i < vertexBufferCount; i++) {
                const bp = vertexBuffersPtr + i * 24;
                const stepMode = decodeEnum(VERTEX_STEP_MODES, dv.getUint32(bp + 4, true), "WGPUVertexStepMode");
                const arrayStride = toSafeSize64(dv.getBigUint64(bp + 8, true), "WGPUVertexBufferLayout.arrayStride");
                const attrCount = dv.getUint32(bp + 16, true);
                const attrsPtr = dv.getUint32(bp + 20, true);
                const attributes = new Array(attrCount);

                for (let j = 0; j < attrCount; j++) {
                    const ap = attrsPtr + j * 24;
                    attributes[j] = {
                        format: requireEnum(VERTEX_FORMATS, dv.getUint32(ap + 4, true), "WGPUVertexFormat"),
                        offset: toSafeSize64(dv.getBigUint64(ap + 8, true), "WGPUVertexAttribute.offset"),
                        shaderLocation: dv.getUint32(ap + 16, true),
                    };
                }

                const layoutObj = { arrayStride, attributes };
                if (stepMode !== undefined) layoutObj.stepMode = stepMode;
                vertexBuffers.push(layoutObj);
            }

            const vertex = { module: vertexModule };
            if (vertexEntryPoint !== undefined) vertex.entryPoint = vertexEntryPoint;
            if (vertexConstants !== undefined) vertex.constants = vertexConstants;
            if (vertexBuffers.length > 0) vertex.buffers = vertexBuffers;

            // Primitive state at p + 48
            const primitive = {};
            const topology = decodeEnum(PRIMITIVE_TOPOLOGIES, dv.getUint32(p + 52, true), "WGPUPrimitiveTopology");
            const stripIndexFormat = decodeEnum(INDEX_FORMATS, dv.getUint32(p + 56, true), "WGPUIndexFormat");
            const frontFace = decodeEnum(FRONT_FACES, dv.getUint32(p + 60, true), "WGPUFrontFace");
            const cullMode = decodeEnum(CULL_MODES, dv.getUint32(p + 64, true), "WGPUCullMode");
            const unclippedDepth = dv.getUint32(p + 68, true) !== 0;

            if (topology !== undefined) primitive.topology = topology;
            if (stripIndexFormat !== undefined) primitive.stripIndexFormat = stripIndexFormat;
            if (frontFace !== undefined) primitive.frontFace = frontFace;
            if (cullMode !== undefined) primitive.cullMode = cullMode;
            if (unclippedDepth) primitive.unclippedDepth = true;

            // Optional depthStencil at p + 72
            const depthStencilPtr = dv.getUint32(p + 72, true);
            let depthStencil;
            if (depthStencilPtr !== 0) {
                const dp = depthStencilPtr;
                const depthWriteEnabled = toOptionalBool(dv.getUint32(dp + 8, true), "depthWriteEnabled");
                depthStencil = {
                    format: requireEnum(TEXTURE_FORMATS, dv.getUint32(dp + 4, true), "WGPUTextureFormat"),
                    stencilFront: readStencilFaceState(dv, dp + 16),
                    stencilBack: readStencilFaceState(dv, dp + 32),
                    stencilReadMask: dv.getUint32(dp + 48, true),
                    stencilWriteMask: dv.getUint32(dp + 52, true),
                    depthBias: dv.getInt32(dp + 56, true),
                    depthBiasSlopeScale: dv.getFloat32(dp + 60, true),
                    depthBiasClamp: dv.getFloat32(dp + 64, true),
                };
                const depthCompare = decodeEnum(COMPARE_FUNCTIONS, dv.getUint32(dp + 12, true), "WGPUCompareFunction");
                if (depthCompare !== undefined) depthStencil.depthCompare = depthCompare;
                // Undefined is left unset so WebGPU validation requires it for depth formats.
                if (depthWriteEnabled !== undefined) depthStencil.depthWriteEnabled = depthWriteEnabled;
            }

            // Multisample state at p + 76
            const sampleCount = dv.getUint32(p + 80, true);
            const sampleMask = dv.getUint32(p + 84, true);
            const alphaToCoverageEnabled = dv.getUint32(p + 88, true) !== 0;
            const multisample = {
                count: sampleCount,
                mask: sampleMask,
                alphaToCoverageEnabled,
            };

            // Optional fragment state at p + 92
            const fragmentPtr = dv.getUint32(p + 92, true);
            let fragment;
            if (fragmentPtr !== 0) {
                const fp = fragmentPtr;
                const fragModule = handles.get(dv.getUint32(fp + 4, true), "WGPUShaderModule");
                const fragEntryPoint = readString(dv, fp + 8);
                const fragConstants = readConstants(dv, dv.getUint32(fp + 16, true), dv.getUint32(fp + 20, true));
                const targetCount = dv.getUint32(fp + 24, true);
                const targetsPtr = dv.getUint32(fp + 28, true);
                const targets = new Array(targetCount);

                for (let i = 0; i < targetCount; i++) {
                    const tp = targetsPtr + i * 24;
                    const formatVal = dv.getUint32(tp + 4, true);
                    if (formatVal === 0) {
                        targets[i] = null;
                        continue;
                    }
                    const target = {
                        format: requireEnum(TEXTURE_FORMATS, formatVal, "WGPUTextureFormat"),
                        writeMask: toFlags(dv.getBigUint64(tp + 16, true), "WGPUColorTargetState.writeMask"),
                    };
                    const blendPtr = dv.getUint32(tp + 8, true);
                    if (blendPtr !== 0) {
                        target.blend = {
                            color: readBlendComponent(dv, blendPtr),
                            alpha: readBlendComponent(dv, blendPtr + 12),
                        };
                    }
                    targets[i] = target;
                }

                fragment = { module: fragModule, targets };
                if (fragEntryPoint !== undefined) fragment.entryPoint = fragEntryPoint;
                if (fragConstants !== undefined) fragment.constants = fragConstants;
            }

            const desc = {
                layout: layoutHandle === 0 ? "auto" : handles.get(layoutHandle, "WGPUPipelineLayout"),
                vertex,
                primitive,
                multisample,
            };
            if (label !== undefined) desc.label = label;
            if (depthStencil !== undefined) desc.depthStencil = depthStencil;
            if (fragment !== undefined) desc.fragment = fragment;
            return handles.store(dev.createRenderPipeline(desc));
        },

        wgpuDeviceCreateComputePipeline(deviceH, descPtr) {
            const dev = handles.get(deviceH, "WGPUDevice");
            const p = descPtr >>> 0;
            const dv = view();
            const label = readString(dv, p + 4);
            const layoutHandle = dv.getUint32(p + 12, true);
            const module = handles.get(dv.getUint32(p + 20, true), "WGPUShaderModule");
            const entryPoint = readString(dv, p + 24);
            const constants = readConstants(dv, dv.getUint32(p + 32, true), dv.getUint32(p + 36, true));

            const compute = { module };
            if (entryPoint !== undefined) compute.entryPoint = entryPoint;
            if (constants !== undefined) compute.constants = constants;

            const desc = {
                layout: layoutHandle === 0 ? "auto" : handles.get(layoutHandle, "WGPUPipelineLayout"),
                compute,
            };
            if (label !== undefined) desc.label = label;
            return handles.store(dev.createComputePipeline(desc));
        },

        wgpuDeviceCreateCommandEncoder(deviceH, descPtr) {
            const dev = handles.get(deviceH, "WGPUDevice");
            const p = descPtr >>> 0;
            if (p === 0) {
                return handles.store(dev.createCommandEncoder());
            }
            const dv = view();
            const label = readString(dv, p + 4);
            return handles.store(dev.createCommandEncoder(label !== undefined ? { label } : undefined));
        },

        wgpuDeviceDestroy(deviceH) {
            handles.get(deviceH, "WGPUDevice").destroy();
        },

        // As in the official API, releasing the last reference destroys the device.
        wgpuDeviceRelease(deviceH) {
            handles.release(deviceH)?.destroy();
        },
        wgpuDeviceAddRef: addRef,

        // WGPUBuffer
        wgpuBufferDestroy(bufferH) {
            handles.get(bufferH, "WGPUBuffer").destroy();
        },
        wgpuBufferRelease: release,
        wgpuBufferAddRef: addRef,

        // WGPUTexture
        wgpuTextureCreateView(textureH, descPtr) {
            const tex = handles.get(textureH, "WGPUTexture");
            const p = descPtr >>> 0;
            if (p === 0) {
                return handles.store(tex.createView());
            }
            const dv = view();
            const desc = {};
            const label = readString(dv, p + 4);
            const fmt = decodeEnum(TEXTURE_FORMATS, dv.getUint32(p + 12, true), "WGPUTextureFormat");
            const dim = decodeEnum(TEXTURE_VIEW_DIMENSIONS, dv.getUint32(p + 16, true), "WGPUTextureViewDimension");
            const baseMipLevel = dv.getUint32(p + 20, true);
            const mipLevelCount = dv.getUint32(p + 24, true);
            const baseArrayLayer = dv.getUint32(p + 28, true);
            const arrayLayerCount = dv.getUint32(p + 32, true);
            const aspect = decodeEnum(TEXTURE_ASPECTS, dv.getUint32(p + 36, true), "WGPUTextureAspect");
            const usage = toFlags(dv.getBigUint64(p + 40, true), "WGPUTextureViewDescriptor.usage");

            if (label !== undefined) desc.label = label;
            if (fmt !== undefined) desc.format = fmt;
            if (dim !== undefined) desc.dimension = dim;
            if (baseMipLevel !== 0) desc.baseMipLevel = baseMipLevel;
            if (mipLevelCount !== UINT32_UNDEFINED) desc.mipLevelCount = mipLevelCount;
            if (baseArrayLayer !== 0) desc.baseArrayLayer = baseArrayLayer;
            if (arrayLayerCount !== UINT32_UNDEFINED) desc.arrayLayerCount = arrayLayerCount;
            if (aspect !== undefined) desc.aspect = aspect;
            if (usage !== 0) desc.usage = usage;
            return handles.store(tex.createView(desc));
        },

        wgpuTextureDestroy(textureH) {
            handles.get(textureH, "WGPUTexture").destroy();
        },
        wgpuTextureRelease: release,
        wgpuTextureAddRef: addRef,
        wgpuTextureViewRelease: release,
        wgpuTextureViewAddRef: addRef,
        wgpuSamplerRelease: release,
        wgpuSamplerAddRef: addRef,
        wgpuBindGroupLayoutRelease: release,
        wgpuBindGroupLayoutAddRef: addRef,
        wgpuBindGroupRelease: release,
        wgpuBindGroupAddRef: addRef,
        wgpuPipelineLayoutRelease: release,
        wgpuPipelineLayoutAddRef: addRef,
        wgpuShaderModuleRelease: release,
        wgpuShaderModuleAddRef: addRef,

        // WGPURenderPipeline / WGPUComputePipeline
        wgpuRenderPipelineGetBindGroupLayout(pipelineH, groupIndex) {
            const pipeline = handles.get(pipelineH, "WGPURenderPipeline");
            return handles.store(pipeline.getBindGroupLayout(groupIndex >>> 0));
        },
        wgpuRenderPipelineRelease: release,
        wgpuRenderPipelineAddRef: addRef,

        wgpuComputePipelineGetBindGroupLayout(pipelineH, groupIndex) {
            const pipeline = handles.get(pipelineH, "WGPUComputePipeline");
            return handles.store(pipeline.getBindGroupLayout(groupIndex >>> 0));
        },
        wgpuComputePipelineRelease: release,
        wgpuComputePipelineAddRef: addRef,

        // WGPUCommandEncoder
        wgpuCommandEncoderBeginRenderPass(encoderH, descPtr) {
            const encoder = handles.get(encoderH, "WGPUCommandEncoder");
            const p = descPtr >>> 0;
            const dv = view();
            const label = readString(dv, p + 4);
            const attachmentCount = dv.getUint32(p + 12, true);
            const attachmentsPtr = dv.getUint32(p + 16, true);
            const depthStencilPtr = dv.getUint32(p + 20, true);
            requireZero(dv.getUint32(p + 24, true), "WGPURenderPassDescriptor.occlusionQuerySet");
            requireZero(dv.getUint32(p + 28, true), "WGPURenderPassDescriptor.timestampWrites");

            const colorAttachments = new Array(attachmentCount);
            for (let i = 0; i < attachmentCount; i++) {
                const ap = attachmentsPtr + i * 56;
                const viewHandle = dv.getUint32(ap + 4, true);
                if (viewHandle === 0) {
                    colorAttachments[i] = null;
                    continue;
                }
                const attachment = {
                    view: handles.get(viewHandle, "WGPUTextureView"),
                    loadOp: requireEnum(LOAD_OPS, dv.getUint32(ap + 16, true), "WGPULoadOp"),
                    storeOp: requireEnum(STORE_OPS, dv.getUint32(ap + 20, true), "WGPUStoreOp"),
                    clearValue: readColor(dv, ap + 24),
                };
                const depthSlice = dv.getUint32(ap + 8, true);
                const resolveTarget = handles.getOptional(dv.getUint32(ap + 12, true), "WGPUTextureView");
                if (depthSlice !== UINT32_UNDEFINED) {
                    attachment.depthSlice = depthSlice;
                }
                if (resolveTarget !== undefined) attachment.resolveTarget = resolveTarget;
                colorAttachments[i] = attachment;
            }

            const desc = { colorAttachments };
            if (label !== undefined) desc.label = label;

            if (depthStencilPtr !== 0) {
                const dp = depthStencilPtr;
                const ds = {
                    view: handles.get(dv.getUint32(dp + 4, true), "WGPUTextureView"),
                };
                const depthLoadOp = decodeEnum(LOAD_OPS, dv.getUint32(dp + 8, true), "WGPULoadOp");
                const depthStoreOp = decodeEnum(STORE_OPS, dv.getUint32(dp + 12, true), "WGPUStoreOp");
                const depthClearValue = dv.getFloat32(dp + 16, true);
                const depthReadOnly = dv.getUint32(dp + 20, true) !== 0;
                const stencilLoadOp = decodeEnum(LOAD_OPS, dv.getUint32(dp + 24, true), "WGPULoadOp");
                const stencilStoreOp = decodeEnum(STORE_OPS, dv.getUint32(dp + 28, true), "WGPUStoreOp");
                const stencilClearValue = dv.getUint32(dp + 32, true);
                const stencilReadOnly = dv.getUint32(dp + 36, true) !== 0;

                if (depthLoadOp !== undefined) ds.depthLoadOp = depthLoadOp;
                if (depthStoreOp !== undefined) ds.depthStoreOp = depthStoreOp;
                // NaN means WGPU_DEPTH_CLEAR_VALUE_UNDEFINED: leave it unset so WebGPU
                // validation rejects depthLoadOp = Clear without an explicit value.
                if (!Number.isNaN(depthClearValue)) ds.depthClearValue = depthClearValue;
                if (depthReadOnly) ds.depthReadOnly = true;
                if (stencilLoadOp !== undefined) ds.stencilLoadOp = stencilLoadOp;
                if (stencilStoreOp !== undefined) ds.stencilStoreOp = stencilStoreOp;
                ds.stencilClearValue = stencilClearValue;
                if (stencilReadOnly) ds.stencilReadOnly = true;
                desc.depthStencilAttachment = ds;
            }

            return handles.store(encoder.beginRenderPass(desc));
        },

        wgpuCommandEncoderBeginComputePass(encoderH, descPtr) {
            const encoder = handles.get(encoderH, "WGPUCommandEncoder");
            const p = descPtr >>> 0;
            if (p === 0) {
                return handles.store(encoder.beginComputePass());
            }
            const dv = view();
            requireZero(dv.getUint32(p + 12, true), "WGPUComputePassDescriptor.timestampWrites");
            const label = readString(dv, p + 4);
            return handles.store(encoder.beginComputePass(label !== undefined ? { label } : undefined));
        },

        wgpuCommandEncoderCopyBufferToBuffer(encoderH, srcH, srcOffset, dstH, dstOffset, size) {
            const encoder = handles.get(encoderH, "WGPUCommandEncoder");
            const src = handles.get(srcH, "WGPUBuffer");
            const resolvedSrcOffset = toSafeSize64(srcOffset, "sourceOffset");
            // WGPU_WHOLE_SIZE copies the rest of the source buffer. Resolve it here
            // instead of relying on browsers supporting the optional size argument.
            const resolvedSize = toOptionalSize64(size, "size") ?? Math.max(0, src.size - resolvedSrcOffset);
            encoder.copyBufferToBuffer(
                src,
                resolvedSrcOffset,
                handles.get(dstH, "WGPUBuffer"),
                toSafeSize64(dstOffset, "destinationOffset"),
                resolvedSize,
            );
        },

        wgpuCommandEncoderCopyBufferToTexture(encoderH, srcPtr, dstPtr, sizePtr) {
            const encoder = handles.get(encoderH, "WGPUCommandEncoder");
            const dv = view();
            encoder.copyBufferToTexture(
                readTexelCopyBufferInfo(dv, srcPtr),
                readTexelCopyTextureInfo(dv, dstPtr),
                readExtent3D(dv, sizePtr >>> 0),
            );
        },

        wgpuCommandEncoderCopyTextureToBuffer(encoderH, srcPtr, dstPtr, sizePtr) {
            const encoder = handles.get(encoderH, "WGPUCommandEncoder");
            const dv = view();
            encoder.copyTextureToBuffer(
                readTexelCopyTextureInfo(dv, srcPtr),
                readTexelCopyBufferInfo(dv, dstPtr),
                readExtent3D(dv, sizePtr >>> 0),
            );
        },

        wgpuCommandEncoderCopyTextureToTexture(encoderH, srcPtr, dstPtr, sizePtr) {
            const encoder = handles.get(encoderH, "WGPUCommandEncoder");
            const dv = view();
            encoder.copyTextureToTexture(
                readTexelCopyTextureInfo(dv, srcPtr),
                readTexelCopyTextureInfo(dv, dstPtr),
                readExtent3D(dv, sizePtr >>> 0),
            );
        },

        wgpuCommandEncoderClearBuffer(encoderH, bufferH, offset, size) {
            const encoder = handles.get(encoderH, "WGPUCommandEncoder");
            const buf = handles.get(bufferH, "WGPUBuffer");
            const resolvedOffset = toSafeSize64(offset, "offset");
            const resolvedSize = toOptionalSize64(size, "size");
            if (resolvedSize === undefined) {
                encoder.clearBuffer(buf, resolvedOffset);
            } else {
                encoder.clearBuffer(buf, resolvedOffset, resolvedSize);
            }
        },

        wgpuCommandEncoderFinish(encoderH, descPtr) {
            const encoder = handles.get(encoderH, "WGPUCommandEncoder");
            const p = descPtr >>> 0;
            if (p === 0) {
                return handles.store(encoder.finish());
            }
            const dv = view();
            const label = readString(dv, p + 4);
            return handles.store(encoder.finish(label !== undefined ? { label } : undefined));
        },

        wgpuCommandEncoderRelease: release,
        wgpuCommandEncoderAddRef: addRef,

        // WGPURenderPassEncoder
        wgpuRenderPassEncoderSetPipeline(passH, pipelineH) {
            handles.get(passH, "WGPURenderPassEncoder").setPipeline(
                handles.get(pipelineH, "WGPURenderPipeline"),
            );
        },

        wgpuRenderPassEncoderSetBindGroup(passH, groupIndex, groupH, dynamicOffsetCount, dynamicOffsetsPtr) {
            const pass = handles.get(passH, "WGPURenderPassEncoder");
            const bg = handles.getOptional(groupH, "WGPUBindGroup") ?? null;
            const offsets = readDynamicOffsets(dynamicOffsetCount, dynamicOffsetsPtr);
            if (offsets !== undefined) {
                pass.setBindGroup(groupIndex >>> 0, bg, offsets);
            } else {
                pass.setBindGroup(groupIndex >>> 0, bg);
            }
        },

        wgpuRenderPassEncoderSetVertexBuffer(passH, slot, bufferH, offset, size) {
            const pass = handles.get(passH, "WGPURenderPassEncoder");
            const buf = handles.getOptional(bufferH, "WGPUBuffer") ?? null;
            const resolvedOffset = toSafeSize64(offset, "offset");
            const resolvedSize = toOptionalSize64(size, "size");
            if (resolvedSize === undefined) {
                pass.setVertexBuffer(slot >>> 0, buf, resolvedOffset);
            } else {
                pass.setVertexBuffer(slot >>> 0, buf, resolvedOffset, resolvedSize);
            }
        },

        wgpuRenderPassEncoderSetIndexBuffer(passH, bufferH, indexFormat, offset, size) {
            const pass = handles.get(passH, "WGPURenderPassEncoder");
            const buf = handles.get(bufferH, "WGPUBuffer");
            const fmt = requireEnum(INDEX_FORMATS, indexFormat, "WGPUIndexFormat");
            const resolvedOffset = toSafeSize64(offset, "offset");
            const resolvedSize = toOptionalSize64(size, "size");
            if (resolvedSize === undefined) {
                pass.setIndexBuffer(buf, fmt, resolvedOffset);
            } else {
                pass.setIndexBuffer(buf, fmt, resolvedOffset, resolvedSize);
            }
        },

        wgpuRenderPassEncoderSetViewport(passH, x, y, width, height, minDepth, maxDepth) {
            handles.get(passH, "WGPURenderPassEncoder").setViewport(
                x, y, width, height, minDepth, maxDepth,
            );
        },

        wgpuRenderPassEncoderSetScissorRect(passH, x, y, width, height) {
            handles.get(passH, "WGPURenderPassEncoder").setScissorRect(
                x >>> 0, y >>> 0, width >>> 0, height >>> 0,
            );
        },

        wgpuRenderPassEncoderSetBlendConstant(passH, colorPtr) {
            const pass = handles.get(passH, "WGPURenderPassEncoder");
            pass.setBlendConstant(readColor(view(), colorPtr >>> 0));
        },

        wgpuRenderPassEncoderSetStencilReference(passH, reference) {
            handles.get(passH, "WGPURenderPassEncoder").setStencilReference(reference >>> 0);
        },

        wgpuRenderPassEncoderDraw(passH, vertexCount, instanceCount, firstVertex, firstInstance) {
            handles.get(passH, "WGPURenderPassEncoder").draw(
                vertexCount >>> 0,
                instanceCount >>> 0,
                firstVertex >>> 0,
                firstInstance >>> 0,
            );
        },

        wgpuRenderPassEncoderDrawIndexed(passH, indexCount, instanceCount, firstIndex, baseVertex, firstInstance) {
            handles.get(passH, "WGPURenderPassEncoder").drawIndexed(
                indexCount >>> 0,
                instanceCount >>> 0,
                firstIndex >>> 0,
                baseVertex | 0,
                firstInstance >>> 0,
            );
        },

        wgpuRenderPassEncoderEnd(passH) {
            handles.get(passH, "WGPURenderPassEncoder").end();
        },

        wgpuRenderPassEncoderRelease: release,
        wgpuRenderPassEncoderAddRef: addRef,

        // WGPUComputePassEncoder
        wgpuComputePassEncoderSetPipeline(passH, pipelineH) {
            handles.get(passH, "WGPUComputePassEncoder").setPipeline(
                handles.get(pipelineH, "WGPUComputePipeline"),
            );
        },

        wgpuComputePassEncoderSetBindGroup(passH, groupIndex, groupH, dynamicOffsetCount, dynamicOffsetsPtr) {
            const pass = handles.get(passH, "WGPUComputePassEncoder");
            const bg = handles.getOptional(groupH, "WGPUBindGroup") ?? null;
            const offsets = readDynamicOffsets(dynamicOffsetCount, dynamicOffsetsPtr);
            if (offsets !== undefined) {
                pass.setBindGroup(groupIndex >>> 0, bg, offsets);
            } else {
                pass.setBindGroup(groupIndex >>> 0, bg);
            }
        },

        wgpuComputePassEncoderDispatchWorkgroups(passH, x, y, z) {
            handles.get(passH, "WGPUComputePassEncoder").dispatchWorkgroups(
                x >>> 0, y >>> 0, z >>> 0,
            );
        },

        wgpuComputePassEncoderEnd(passH) {
            handles.get(passH, "WGPUComputePassEncoder").end();
        },

        wgpuComputePassEncoderRelease: release,
        wgpuComputePassEncoderAddRef: addRef,
        wgpuCommandBufferRelease: release,
        wgpuCommandBufferAddRef: addRef,

        // WGPUQueue
        wgpuQueueSubmit(queueH, commandCount, commandsPtr) {
            const queue = handles.get(queueH, "WGPUQueue");
            const c = commandCount >>> 0;
            const p = commandsPtr >>> 0;
            const dv = view();
            const commandBuffers = new Array(c);
            for (let i = 0; i < c; i++) {
                commandBuffers[i] = handles.get(dv.getUint32(p + i * 4, true), "WGPUCommandBuffer");
            }
            queue.submit(commandBuffers);
        },

        wgpuQueueWriteBuffer(queueH, bufferH, bufferOffset, dataPtr, size) {
            const queue = handles.get(queueH, "WGPUQueue");
            const buf = handles.get(bufferH, "WGPUBuffer");
            queue.writeBuffer(
                buf,
                toSafeSize64(bufferOffset, "bufferOffset"),
                buffer(),
                dataPtr >>> 0,
                size >>> 0,
            );
        },

        wgpuQueueWriteTexture(queueH, dstPtr, dataPtr, dataSize, layoutPtr, writeSizePtr) {
            const queue = handles.get(queueH, "WGPUQueue");
            const dv = view();
            queue.writeTexture(
                readTexelCopyTextureInfo(dv, dstPtr),
                new Uint8Array(buffer(), dataPtr >>> 0, dataSize >>> 0),
                readTexelCopyBufferLayout(dv, layoutPtr >>> 0),
                readExtent3D(dv, writeSizePtr >>> 0),
            );
        },

        wgpuQueueRelease: release,
        wgpuQueueAddRef: addRef,
    };

    return { imports };
}
