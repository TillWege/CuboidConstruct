#include <cstddef>
#include <iostream>
#include <webgpu/webgpu.h>
#include <iostream>
#include "cassert"
#include "vector"

WGPUInstance getInstance()
{
    // We create a descriptor
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = NULL;

    // We create the instance using this descriptor
    WGPUInstance instance = wgpuCreateInstance(&desc);

    // We can check whether there is actually an instance created
    if (!instance) {
        std::cerr << "Could not initialize WebGPU!" << std::endl;
        return nullptr;
    }

    return instance;
}

WGPUAdapter requestAdapterSync(WGPUInstance instance) {
    struct UserData {
        WGPUAdapter adapter = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    WGPURequestAdapterOptions adapterOpts = {};
    adapterOpts.nextInChain = nullptr;

    auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const * message, void * pUserData) {
        UserData& userData = *reinterpret_cast<UserData*>(pUserData);
        if (status == WGPURequestAdapterStatus_Success) {
            userData.adapter = adapter;
        } else {
            std::cout << "Could not get WebGPU adapter: " << message << std::endl;
        }
        userData.requestEnded = true;
    };

    wgpuInstanceRequestAdapter(
        instance,
        &adapterOpts,
        onAdapterRequestEnded,
        (void*)&userData
    );

    assert(userData.requestEnded);

    return userData.adapter;
}

void printAdapterLimits(WGPUAdapter adapter)
{
    WGPUSupportedLimits supportedLimits = {};
    supportedLimits.nextInChain = nullptr;

    wgpuAdapterGetLimits(adapter, &supportedLimits);

#ifdef WEBGPU_BACKEND_DAWN
    bool success = wgpuAdapterGetLimits(adapter, &supportedLimits) == WGPUStatus_Success;
#else
    bool success = wgpuAdapterGetLimits(adapter, &supportedLimits);
#endif

    if (success) {
        std::cout << "Adapter limits:" << std::endl;
        std::cout << " - maxTextureDimension1D: " << supportedLimits.limits.maxTextureDimension1D << std::endl;
        std::cout << " - maxTextureDimension2D: " << supportedLimits.limits.maxTextureDimension2D << std::endl;
        std::cout << " - maxTextureDimension3D: " << supportedLimits.limits.maxTextureDimension3D << std::endl;
        std::cout << " - maxTextureArrayLayers: " << supportedLimits.limits.maxTextureArrayLayers << std::endl;
    }
}

void printAdapterFeatures(WGPUAdapter adapter)
{
    std::vector<WGPUFeatureName> features;

    size_t featureCount = wgpuAdapterEnumerateFeatures(adapter, nullptr);
    features.resize(featureCount);

    wgpuAdapterEnumerateFeatures(adapter, features.data());

    std::cout << "Adapter features:" << std::endl;
    std::cout << std::hex; // Write integers as hexadecimal to ease comparison with webgpu.h literals
    for (auto f : features) {
        std::cout << " - 0x" << f << std::endl;
    }
    std::cout << std::dec; // Restore decimal numbers
}

void printAdapterProperties(WGPUAdapter adapter)
{
    WGPUAdapterProperties properties = {};
    properties.nextInChain = nullptr;
    wgpuAdapterGetProperties(adapter, &properties);
    std::cout << "Adapter properties:" << std::endl;
    std::cout << " - vendorID: " << properties.vendorID << std::endl;
    if (properties.vendorName) {
        std::cout << " - vendorName: " << properties.vendorName << std::endl;
    }
    if (properties.architecture) {
        std::cout << " - architecture: " << properties.architecture << std::endl;
    }
    std::cout << " - deviceID: " << properties.deviceID << std::endl;
    if (properties.name) {
        std::cout << " - name: " << properties.name << std::endl;
    }
    if (properties.driverDescription) {
        std::cout << " - driverDescription: " << properties.driverDescription << std::endl;
    }
    std::cout << std::hex;
    std::cout << " - adapterType: 0x" << properties.adapterType << std::endl;
    std::cout << " - backendType: 0x" << properties.backendType << std::endl;
    std::cout << std::dec;
}

int main()
{
    auto instance = getInstance();

    if(instance == nullptr)
        return 1;

    std::cout << "WGPU instance: " << instance << std::endl;
    std::cout << "WebGPU initialized successfully!" << std::endl;

    std::cout << "Requesting adapter..." << std::endl;
    WGPUAdapter adapter = requestAdapterSync(instance);

    if(adapter == nullptr)
        return 1;

    std::cout << "Got adapter: " << adapter << std::endl;

    wgpuInstanceRelease(instance);

    printAdapterLimits(adapter);
    printAdapterFeatures(adapter);
    printAdapterProperties(adapter);

    wgpuAdapterRelease(adapter);

    return 0;
}
