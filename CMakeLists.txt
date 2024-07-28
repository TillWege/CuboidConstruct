cmake_minimum_required(VERSION 3.11)
project(cc)

file(GLOB sources CONFIGURE_DEPENDS
        "src/*.h" "src/*.cpp")

add_executable(cc ${sources})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

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


target_link_libraries(cc PRIVATE webgpu)

if (MSVC)
    target_compile_options(cc PRIVATE /W4)
else()
    target_compile_options(cc PRIVATE -Wall -Wextra -pedantic)
endif()

target_copy_webgpu_binaries(cc)
