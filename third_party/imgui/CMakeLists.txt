project(imgui)

set(IMGUI_SRC
   source/imgui.cpp
   source/imgui_draw.cpp
   source/imgui_internal.h
   include/imconfig.h
   include/imgui.h
)

add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC include PRIVATE source)

target_link_libraries(imgui glfw stb)
