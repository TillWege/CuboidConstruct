cmake_minimum_required(VERSION 3.11)
project(cc)

file(GLOB sources CONFIGURE_DEPENDS
        "src/*.h" "src/*.cpp")

add_executable(cc ${sources})


set_target_properties(cc PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

if (MSVC)
    target_compile_options(cc PRIVATE /W4)
else()
    target_compile_options(cc PRIVATE -Wall -Wextra -pedantic)
endif()