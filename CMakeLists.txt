cmake_minimum_required(VERSION 3.10)
project(HelloTriangle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OSX_ARCHITECTURES arm64)

include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

find_package(OpenGL REQUIRED)

add_executable(mandelbrot_set main.cpp)

target_link_libraries(mandelbrot_set
    OpenGL::GL
    glfw
    glew
)
