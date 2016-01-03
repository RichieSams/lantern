cmake_minimum_required(VERSION 2.8)
project(imgui)

include_directories(include)

set(IMGUI_SRC
   source/imgui.cpp
   source/imgui_demo.cpp
   source/imgui_draw.cpp
   source/imgui_impl_glfw.cpp
)

set(IMGUI_HEADERS
   include/imconfig.h
   include/imgui.h
   include/imgui_impl_glfw.h
)

add_library(imgui STATIC ${IMGUI_SRC} ${IMGUI_HEADERS})
target_link_libraries(imgui glfw ${GLFW_LIBRARIES})
