include(SetSourceGroup)

find_package(Vulkan)


SetSourceGroup(NAME Camera
	PREFIX LANTERN_CORE
	SOURCE_FILES camera/pinhole_camera.h
	             camera/pinhole_camera.cpp
	             camera/frame_buffer.h
	             camera/frame_buffer.cpp
	             camera/reconstruction_filter.h
	             camera/reconstruction_filter.cpp
)

SetSourceGroup(NAME IO
	PREFIX LANTERN_CORE
	SOURCE_FILES io/file_io.h
	             io/lantern_model_file.h
	             io/lantern_model_file.cpp
)

SetSourceGroup(NAME Math
	PREFIX LANTERN_CORE
	SOURCE_FILES math/sampling.h
	             math/uniform_sampler.h
	             math/vector_types.h
	             math/vector_math.h
	             math/vector_math.cpp
	             math/int_types.h
	             math/float_math.h
	             math/align.h
	             math/linearspace4.h
)

SetSourceGroup(NAME Scene
	PREFIX LANTERN_CORE
	SOURCE_FILES scene/area_light.h
	             scene/area_light.cpp
	             scene/geometry_generator.h
	             scene/geometry_generator.cpp
	             scene/image_cache.h
	             scene/image_cache.cpp
	             scene/light.h
	             scene/mesh_elements.h
	             scene/scene.h
	             scene/scene.cpp
	             scene/obj_loader.h
	             scene/obj_loader.cpp
)

SetSourceGroup(NAME Materials
	PREFIX LANTERN_CORE
	SOURCE_FILES materials/material.h
)

SetSourceGroup(NAME Materials/BSDFs
	PREFIX LANTERN_CORE
	SOURCE_FILES materials/bsdfs/bsdf.h
	             materials/bsdfs/lambert_bsdf.h
	             materials/bsdfs/mirror_bsdf.h
	             materials/bsdfs/bsdf_lobe.h
	             materials/bsdfs/ideal_specular_dielectric.h
)

SetSourceGroup(NAME Materials/Textures
	PREFIX LANTERN_CORE
	SOURCE_FILES materials/textures/texture.h
	             materials/textures/constant_texture.h
	             materials/textures/image_texture.h
	             materials/textures/uv_texture.h
)

SetSourceGroup(NAME "Materials/Media"
	PREFIX LANTERN_CORE
	SOURCE_FILES materials/media/medium.h
	             materials/media/non_scattering_medium.h
	             materials/media/isotropic_scattering_medium.h
)

SetSourceGroup(NAME Integrator
	PREFIX LANTERN_CORE
	SOURCE_FILES integrator/integrator.h
	             integrator/integrator.cpp
	             integrator/surface_interaction.h
)

SetSourceGroup(NAME Visualizer
	PREFIX LANTERN_CORE
	SOURCE_FILES visualizer/visualizer.h
	             visualizer/visualizer.cpp
	             visualizer/vulkan_function_loader.cpp
)

SetSourceGroup(NAME "Visualizer/Shaders"
	PREFIX LANTERN_CORE
	SOURCE_FILES visualizer/shaders/fullscreen_triangle_vs.glsl
	             visualizer/shaders/fullscreen_triangle_vs.spv.h
	             visualizer/shaders/fullscreen_triangle_vs.spv.cpp
	             visualizer/shaders/final_resolve_ps.glsl
	             visualizer/shaders/final_resolve_ps.spv.h
	             visualizer/shaders/final_resolve_ps.spv.cpp
)


# Link all the sources into one
set(LANTERN_CORE_SRC
	${LANTERN_CORE_CAMERA}
	${LANTERN_CORE_IO}
	${LANTERN_CORE_MATH}
	${LANTERN_CORE_SCENE}
	${LANTERN_CORE_MATERIALS}
	${LANTERN_CORE_MATERIALS_BSDFS}
	${LANTERN_CORE_MATERIALS_MEDIA}
	${LANTERN_CORE_MATERIALS_TEXTURES}
	${LANTERN_CORE_INTEGRATOR}
	${LANTERN_CORE_VISUALIZER}
	${LANTERN_CORE_VISUALIZER_SHADERS}
)

# Create lib
add_library(lantern_core STATIC ${LANTERN_CORE_SRC})
target_link_libraries(lantern_core glfw ${VULKAN_LIBRARY} imgui tbb_static tbbmalloc_static embree json json_schema_validator tiny_obj_loader stb vk_mem_alloc)
target_include_directories(lantern_core PUBLIC ${VULKAN_INCLUDE_DIR})