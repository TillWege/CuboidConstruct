cmake_minimum_required(VERSION 3.11)
project(cc)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

file(GLOB sources CONFIGURE_DEPENDS
        "src/*.h" "src/*.cpp"
        "src/**/*.h" "src/**/*.cpp")

add_executable(cc ${sources})

if (NOT EMSCRIPTEN)
    FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw
            GIT_TAG        master
    )
    FetchContent_MakeAvailable(glfw)
endif()


FetchContent_Declare(
  glfw3webgpu
  GIT_REPOSITORY https://github.com/eliemichel/glfw3webgpu.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(glfw3webgpu)

FetchContent_Declare(
  webgpu
  GIT_REPOSITORY https://github.com/eliemichel/WebGPU-distribution
  GIT_TAG        wgpu
)
FetchContent_MakeAvailable(webgpu)

set_target_properties(cc PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(cc PRIVATE glfw)
target_link_libraries(cc PRIVATE glfw3webgpu)
target_link_libraries(cc PRIVATE webgpu)

# Options that are specific to Emscripten
if (EMSCRIPTEN)
    set_target_properties(cc PROPERTIES SUFFIX ".html")
    target_link_options(cc PRIVATE
        -sUSE_GLFW=3 # Use Emscripten-provided GLFW
        -sUSE_WEBGPU # Handle WebGPU symbols
        -sASYNCIFY # Required by WebGPU-C++
    )
endif()

if (MSVC)
    target_compile_options(cc PRIVATE /W4)
else()
    target_compile_options(cc PRIVATE -Wall -Wextra -pedantic)
endif()

target_copy_webgpu_binaries(cc)
