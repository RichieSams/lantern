cmake_minimum_required(VERSION 2.8)
project(imgui)

include_directories(include)
add_library(imgui STATIC source/imgui.cpp
                         source/imgui_demo.cpp
                         source/imgui_draw.cpp
                         source/imgui_impl_glfw.cpp)
target_link_libraries(imgui glfw ${GLFW_LIBRARIES})
