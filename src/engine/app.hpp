#include "GLFW/glfw3.h"
#include <webgpu/webgpu.h>

class App
{
    public:
        App();
        void Update();
        void Terminate();

        bool IsReady();
        bool IsRunning();

        void TestWGPU();
    private:
        bool m_isReady = false;
        bool m_isRunning = false;

        GLFWwindow* m_window = nullptr;
        WGPUDevice m_device;
        WGPUQueue m_queue;
        WGPUSurface m_surface;
};
