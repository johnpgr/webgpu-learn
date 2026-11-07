#include "base/base.h"
#include "base/base_mod.h"
#include "lib/webgpu.h"
#include "lib/webgpu_print.h"

// static Arena* transient_storage;
// static Arena* permanent_storage;

static WGPUDevice device;
static WGPUQueue queue;

static void adapter_request_callback(
    WGPURequestAdapterStatus status, // a success status, hopefully.
    WGPUAdapter adapter, // the adapter requested.
    WGPUStringView message, // error message or null.
    void* userdata1, void* userdata2 // custom user data, as provided when requesting the adapter.
) {
    WGPUAdapter* adapter_requested = (WGPUAdapter*)userdata1;
    bool* ended = (bool*)userdata2;

    if (WGPURequestAdapterStatus::Success == status) {
        *adapter_requested = adapter;
        ASSERT(adapter_requested->id != 0);
    } else {
        printf("Could not get WebGPU adapter: %.*s", (int)message.length, message.data);
    }

    *ended = true;
}

static WGPUAdapter request_adapter_sync(
    WGPUInstance inst,
    WGPURequestAdapterOptions* opts
) {
    WGPUAdapter adapter = {};
    bool request_ended = false;
    WGPUFutureWaitInfo wait = {};

    wait.future = wgpuInstanceRequestAdapter(inst, opts, {
        .mode = WGPUCallbackMode::WaitAnyOnly,
        .callback = adapter_request_callback,
        .userdata1 = &adapter,
        .userdata2 = &request_ended,
    });

    wgpuInstanceWaitAny(inst, 1, &wait, ~(ulong)0);
    ASSERT(request_ended);

    return adapter;
}

static void device_request_callback(
    WGPURequestDeviceStatus status,
    WGPUDevice device,
    WGPUStringView message,
    void* userdata1, void* userdata2
) {
    WGPUDevice* device_requested = (WGPUDevice*)userdata1;
    bool* ended = (bool*)userdata2;

    if (WGPURequestDeviceStatus::Success == status) {
        *device_requested = device;
        ASSERT(device_requested->id != 0);
    } else {
        printf("Could not get WebGPU device: %.*s", (int)message.length, message.data);
    }

    *ended = true;
}

static WGPUDevice request_device_sync(
    WGPUInstance inst,
    WGPUAdapter adapter,
    WGPUDeviceDescriptor* desc
) {
    WGPUDevice device = {};
    bool request_ended = false;
    WGPUFutureWaitInfo wait = {};

    wait.future = wgpuAdapterRequestDevice(adapter, desc, {
        .mode = WGPUCallbackMode::WaitAnyOnly,
        .callback = device_request_callback,
        .userdata1 = &device,
        .userdata2 = &request_ended,
    });

    wgpuInstanceWaitAny(inst, 1, &wait, ~(ulong)0);
    ASSERT(request_ended);

    return device;
}

static void on_device_lost(
    const WGPUDevice*,
    WGPUDeviceLostReason reason,
    WGPUStringView message,
    void*, void*
) {
    printf("Device lost: reason=%u %.*s", (uint)reason, (int)message.length, message.data);
}

static void on_device_error(
    const WGPUDevice*,
    WGPUErrorType type,
    WGPUStringView message,
    void*, void*
) {
    printf("Uncaptured device error: type=%u %.*s", (uint)type, (int)message.length, message.data);
}

EXPORT(init) void init() {
    WGPUInstanceFeatureName features[] = {
        WGPUInstanceFeatureName::TimedWaitAny
    };
    WGPUInstanceDescriptor desc = {
        .requiredFeatureCount = 1,
        .requiredFeatures = features
    };
    WGPUInstance instance = wgpuCreateInstance(&desc);

    WGPUAdapter adapter = request_adapter_sync(instance, nullptr);
    printf("Got adapter=%u", adapter.id);

    WGPUDeviceDescriptor device_desc = {};
    device_desc.label = "My Device";
    device_desc.requiredFeatureCount = 0;
    device_desc.requiredLimits = nullptr;
    device_desc.defaultQueue.label = "The default queue";
    device_desc.deviceLostCallbackInfo.mode = WGPUCallbackMode::AllowSpontaneous;
    device_desc.deviceLostCallbackInfo.callback = on_device_lost;
    device_desc.uncapturedErrorCallbackInfo.callback = on_device_error;

    device = request_device_sync(instance, adapter, &device_desc);
    printf("Got device=%u", device.id);

    wgpuAdapterRelease(adapter); // Once we have the device, we don't need the adapter.

    queue = wgpuDeviceGetQueue(device);
    printf("Got queue=%u", queue.id);
}

EXPORT(frame) void frame(float) { }
