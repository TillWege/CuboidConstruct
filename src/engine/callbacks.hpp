#include "iostream"
#include <webgpu/webgpu.h>

void onQueueWorkDone(WGPUQueueWorkDoneStatus status, void* /* pUserData */)
{
	std::cout << "Queued work finished with status: " << status << std::endl;
}

void onDeviceLost(WGPUDeviceLostReason reason, char const* message, void* /* pUserData */)
{
    std::cout << "Device lost: reason " << reason;
    if (message) std::cout << " (" << message << ")";
    std::cout << std::endl;
}

void onDeviceError(WGPUErrorType type, char const* message, void* /* pUserData */)
{
    std::cout << "Uncaptured device error: type " << type;
    if (message) std::cout << " (" << message << ")";
    std::cout << std::endl;
}
