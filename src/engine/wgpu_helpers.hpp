#include <webgpu/webgpu.h>

WGPUInstance getInstance();
WGPUAdapter requestAdapterSync(WGPUInstance instance);
WGPUDevice requestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const * descriptor);
WGPUDevice getDevice(WGPUAdapter adapter);
WGPUQueue getQueue(WGPUDevice device);
void configureSurface(WGPUSurface surface, WGPUAdapter adapater, WGPUDevice device);

void printAdapterLimits(WGPUAdapter adapter);
void printAdapterFeatures(WGPUAdapter adapter);
void printAdapterProperties(WGPUAdapter adapter);
void printDeviceInfo(WGPUDevice device);
