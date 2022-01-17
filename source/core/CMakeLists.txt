find_package(Vulkan REQUIRED)


set(LANTERN_CORE_CAMERA
	camera/frame_data.h
	camera/pinhole_camera.h
	camera/pinhole_camera.ispc
)

set(LANTERN_CORE_INTEGRATOR
	integrator/integrator.cpp
	integrator/integrator.ispc
	integrator/integrator.h
	integrator/surface_interaction.h
)

set(LANTERN_CORE_MATH
	math/constants.h
	math/ray.h
	math/uniform_sampler.h
)

set(LANTERN_CORE_RENDER_HOST
	render_host/render_host.cpp
	render_host/render_host.h
)

set(LANTERN_CORE_SCENE
	scene/scene.h
	scene/scene.ispc
	scene/scene.isph
	scene/sphere.h
	scene/sphere.ispc
	scene/sphere.isph
)

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
	${LANTERN_CORE_CAMERA}
	${LANTERN_CORE_INTEGRATOR}
	${LANTERN_CORE_MATH}
	${LANTERN_CORE_INTEGRATOR}
	${LANTERN_CORE_RENDER_HOST}
	${LANTERN_CORE_SCENE}
	${LANTERN_CORE_VISUALIZER}
	${LANTERN_CORE_VISUALIZER_SHADERS}
)

# Create lib
add_library(lantern_core STATIC ${LANTERN_CORE_SRC})
target_link_libraries(lantern_core glfw ${Vulkan_LIBRARIES} imgui VulkanMemoryAllocator stb)
target_include_directories(lantern_core PUBLIC ${Vulkan_INCLUDE_DIRS})

# Set ISPC settings
set_property(TARGET lantern_core PROPERTY ISPC_INSTRUCTION_SETS "${LATERN_ISPC_TARGET}")
target_compile_options(lantern_core PRIVATE $<$<COMPILE_LANGUAGE:ISPC>:${LANTERN_ISPC_FLAGS}>)
target_compile_options(lantern_core PRIVATE $<$<COMPILE_LANGUAGE:ISPC>:--arch=${LATERN_ISPC_ARCH}>)