find_package(Vulkan REQUIRED)


set(LANTERN_CORE_VISUALIZER
	visualizer/visualizer.h
	visualizer/visualizer.cpp
	visualizer/vulkan_function_loader.cpp
)

set(LANTERN_CORE_VISUALIZER_SHADERS
	visualizer/shaders/fullscreen_triangle_vs.glsl
	visualizer/shaders/fullscreen_triangle_vs.spv.h
	visualizer/shaders/fullscreen_triangle_vs.spv.cpp
	visualizer/shaders/final_resolve_ps.glsl
	visualizer/shaders/final_resolve_ps.spv.h
	visualizer/shaders/final_resolve_ps.spv.cpp
)


# Link all the sources into one
set(LANTERN_CORE_SRC
	${LANTERN_CORE_VISUALIZER}
	${LANTERN_CORE_VISUALIZER_SHADERS}
)

# Create lib
add_library(lantern_core STATIC ${LANTERN_CORE_SRC})
target_link_libraries(lantern_core glfw ${Vulkan_LIBRARIES} imgui VulkanMemoryAllocator)
target_include_directories(lantern_core PUBLIC ${Vulkan_INCLUDE_DIRS})
