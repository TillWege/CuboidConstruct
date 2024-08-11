#include "app.hpp"
#include "GLFW/glfw3.h"
#include <iostream>
#include <glfw3webgpu.h>
#include <webgpu/webgpu.h>
#ifdef WEBGPU_BACKEND_WGPU
    #include <webgpu/wgpu.h>
#endif // WEBGPU_BACKEND_WGPU
#include "wgpu_helpers.hpp"
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif // __EMSCRIPTEN__
#include "consts.hpp"

App::App()
{
   	if (!glfwInit()) {
		std::cerr << "Could not initialize GLFW!" << std::endl;
		return;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
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



    printAdapterLimits(adapter);
    printAdapterFeatures(adapter);
    printAdapterProperties(adapter);

    std::cout << "Requesting device..." << std::endl;

    m_device = getDevice(adapter);

    if(m_device == nullptr)
        return;

    std::cout << "Got device: " << m_device << std::endl;

    printDeviceInfo(m_device);

	m_queue = getQueue(m_device);

	m_surface = glfwGetWGPUSurface(instance, m_window);

	configureSurface(m_surface, adapter, m_device);

    wgpuInstanceRelease(instance);
    wgpuAdapterRelease(adapter);

    this->m_isReady = true;
    this->m_isRunning = true;
}

void App::UpdateRunning(bool isRunning)
{
    this->m_isRunning = isRunning;
}


WGPUTextureView App::GetNextSurfaceTextureView() {
    WGPUSurfaceTexture surfaceTexture;
    wgpuSurfaceGetCurrentTexture(m_surface, &surfaceTexture);

    if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_Success) {
        return nullptr;
    }

    WGPUTextureViewDescriptor viewDescriptor;
    viewDescriptor.nextInChain = nullptr;
    viewDescriptor.label = "Surface texture view";
    viewDescriptor.format = wgpuTextureGetFormat(surfaceTexture.texture);
    viewDescriptor.dimension = WGPUTextureViewDimension_2D;
    viewDescriptor.baseMipLevel = 0;
    viewDescriptor.mipLevelCount = 1;
    viewDescriptor.baseArrayLayer = 0;
    viewDescriptor.arrayLayerCount = 1;
    viewDescriptor.aspect = WGPUTextureAspect_All;
    WGPUTextureView targetView = wgpuTextureCreateView(surfaceTexture.texture, &viewDescriptor);


    return targetView;
}


void App::TestWGPU()
{
    WGPUCommandEncoderDescriptor encoderDesc = {};
	encoderDesc.nextInChain = nullptr;
	encoderDesc.label = "Test encoder";
	WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(m_device, &encoderDesc);

	WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
	cmdBufferDescriptor.nextInChain = nullptr;
	cmdBufferDescriptor.label = "Test buffer";
	WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
	wgpuCommandEncoderRelease(encoder); // release encoder after it's finished

	std::cout << "Submitting test command..." << std::endl;
	wgpuQueueSubmit(m_queue, 1, &command);
	wgpuCommandBufferRelease(command);
	std::cout << "Command test submitted." << std::endl;

	for (int i = 0 ; i < 5 ; ++i) {
		std::cout << "Tick/Poll device..." << std::endl;
      #if defined(WEBGPU_BACKEND_DAWN)
		wgpuDeviceTick(device);
      #elif defined(WEBGPU_BACKEND_WGPU)
		wgpuDevicePoll(m_device, false, nullptr);
      #elif defined(WEBGPU_BACKEND_EMSCRIPTEN)
		emscripten_sleep(100);
      #endif
	}
}


bool App::Update()
{
    glfwPollEvents();

    if(glfwWindowShouldClose(m_window))
        return false;

    WGPUTextureView targetView = GetNextSurfaceTextureView();
    if (!targetView)
        return false;




    wgpuTextureViewRelease(targetView);
#ifndef __EMSCRIPTEN__
    wgpuSurfacePresent(m_surface);
#endif

    return true;
}

void App::Terminate()
{
    wgpuSurfaceUnconfigure(m_surface);
    wgpuSurfaceRelease(m_surface);
    wgpuQueueRelease(m_queue);
    wgpuDeviceRelease(m_device);

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
