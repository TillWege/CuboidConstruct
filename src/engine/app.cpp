#include "app.hpp"
#include "GLFW/glfw3.h"
#include <iostream>

#ifdef WEBGPU_BACKEND_WGPU
    #include <webgpu/wgpu.h>
#endif // WEBGPU_BACKEND_WGPU
#include "wgpu_helpers.hpp"
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif // __EMSCRIPTEN__

App::App()
{
   	if (!glfwInit()) {
		std::cerr << "Could not initialize GLFW!" << std::endl;
		return;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_window = glfwCreateWindow(1280, 720,
		"Cuboid Construct",
		nullptr,
		nullptr
	);

	if (!m_window) {
		std::cerr << "Could not open window!" << std::endl;
		glfwTerminate();
		return;
	}

       auto instance = getInstance();

       if(instance == nullptr)
           return;

       std::cout << "WGPU instance: " << instance << std::endl;
       std::cout << "WebGPU initialized successfully!" << std::endl;

       std::cout << "Requesting adapter..." << std::endl;
       WGPUAdapter adapter = requestAdapterSync(instance);

       if(adapter == nullptr)
           return;

       std::cout << "Got adapter: " << adapter << std::endl;

       wgpuInstanceRelease(instance);

       printAdapterLimits(adapter);
       printAdapterFeatures(adapter);
       printAdapterProperties(adapter);

       std::cout << "Requesting device..." << std::endl;

       device = getDevice(adapter);

       if(device == nullptr)
           return;

       std::cout << "Got device: " << device << std::endl;

       wgpuAdapterRelease(adapter);

       printDeviceInfo(device);

	queue = getQueue(device);

    this->m_isReady = true;
    this->m_isRunning = true;
}

void App::TestWGPU()
{
    WGPUCommandEncoderDescriptor encoderDesc = {};
	encoderDesc.nextInChain = nullptr;
	encoderDesc.label = "Test encoder";
	WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDesc);

	WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
	cmdBufferDescriptor.nextInChain = nullptr;
	cmdBufferDescriptor.label = "Test buffer";
	WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
	wgpuCommandEncoderRelease(encoder); // release encoder after it's finished

	std::cout << "Submitting test command..." << std::endl;
	wgpuQueueSubmit(queue, 1, &command);
	wgpuCommandBufferRelease(command);
	std::cout << "Command test submitted." << std::endl;

	for (int i = 0 ; i < 5 ; ++i) {
		std::cout << "Tick/Poll device..." << std::endl;
      #if defined(WEBGPU_BACKEND_DAWN)
		wgpuDeviceTick(device);
      #elif defined(WEBGPU_BACKEND_WGPU)
		wgpuDevicePoll(device, false, nullptr);
      #elif defined(WEBGPU_BACKEND_EMSCRIPTEN)
		emscripten_sleep(100);
      #endif
	}
}


void App::Update()
{
    glfwPollEvents();

    if(glfwWindowShouldClose(m_window))
        this->m_isRunning = false;
}

void App::Terminate()
{
    wgpuQueueRelease(queue);
    wgpuDeviceRelease(device);

   	glfwDestroyWindow(m_window);
	glfwTerminate();


    this->m_isReady = false;
}

bool App::IsRunning()
{
    return this->m_isRunning;
}

bool App::IsReady()
{
    return this->m_isReady;
}
