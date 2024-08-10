#include "engine/app.hpp"
#include <iostream>
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif // __EMSCRIPTEN__

int main()
{
    std::cout << "Initializing app..." << std::endl;

    App app;

    if(!app.IsReady())
    {
        std::cerr << "Error: App failed to initialize" << std::endl;
        return 1;
    }

    app.TestWGPU();


    #ifdef __EMSCRIPTEN__
        // Equivalent of the main loop when using Emscripten:
        auto callback = [](void *arg) {
            //                   ^^^ 2. We get the address of the app in the callback.
            App* pApp = reinterpret_cast<App*>(arg);
            //                  ^^^^^^^^^^^^^^^^ 3. We force this address to be interpreted
            //                                      as a pointer to an Application object.
            pApp->Update(); // 4. We can use the application object
        };
        emscripten_set_main_loop_arg(callback, &app, 0, true);
        //                                     ^^^^ 1. We pass the address of our application object.
    #else // __EMSCRIPTEN__
        while(app.IsRunning())
        {
            app.UpdateRunning(app.Update());
        }
    #endif // __EMSCRIPTEN__

    std::cout << "Terminating app..." << std::endl;

    app.Terminate();

    return 0;
}
