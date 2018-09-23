project(imgui)

set(IMGUI_SRC
   impl/imgui_impl_glfw.cpp
   impl/imgui_impl_glfw.h
   impl/imgui_impl_vulkan.cpp
   impl/imgui_impl_vulkan.h
   include/imconfig.h
   include/imgui.h
   include/imstb_rectpack.h
   include/imstb_textedit.h
   include/imstb_truetype.h
   source/imgui.cpp
   source/imgui_demo.cpp
   source/imgui_draw.cpp
   source/imgui_internal.h
   source/imgui_widgets.cpp
)

add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC impl include PRIVATE source)

target_link_libraries(imgui glfw ${VULKAN_LIBRARY})
target_include_directories(imgui PUBLIC ${VULKAN_INCLUDE_DIR})
