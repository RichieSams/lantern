include(SetSourceGroup)

SetSourceGroup(NAME Root
	SOURCE_FILES main.cpp
)

SetSourceGroup(NAME Camera
	SOURCE_FILES camera/pinhole_camera.h
	             camera/pinhole_camera.cpp
	             camera/frame_buffer.h
	             camera/frame_buffer.cpp
	             camera/reconstruction_filter.h
	             camera/reconstruction_filter.cpp
)

SetSourceGroup(NAME Math
	SOURCE_FILES math/sampling.h
	             math/uniform_sampler.h
	             math/vector_types.h
	             math/vector_math.h
	             math/vector_math.cpp
	             math/int_types.h
	             math/float_math.h
)

SetSourceGroup(NAME Scene
	SOURCE_FILES scene/area_light.h
	             scene/area_light.cpp
	             scene/geometry_generator.h
	             scene/geometry_generator.cpp
	             scene/light.h
	             scene/mesh_elements.h
	             scene/ray.h
	             scene/scene.h
	             scene/scene.cpp
	             scene/obj_loader.h
	             scene/obj_loader.cpp
)

SetSourceGroup(NAME BSDFs
	SOURCE_FILES bsdfs/bsdf.h
	             bsdfs/lambert_bsdf.h
	             bsdfs/mirror_bsdf.h
	             bsdfs/bsdf_lobe.h
)

SetSourceGroup(NAME Renderer
	SOURCE_FILES renderer/renderer.h
	             renderer/renderer.cpp
	             renderer/surface_interaction.h
)

SetSourceGroup(NAME Visualizer
	SOURCE_FILES visualizer/visualizer.h
	             visualizer/visualizer.cpp
)

# For header-only / single file libraries, it's easier to just compile them
# directly, rather than creating a seperate lib and linking
SetSourceGroup(NAME "Third Party"
	SOURCE_FILES ../third_party/tiny_obj_loader/include/tiny_obj_loader/tiny_obj_loader.h
	             ../third_party/tiny_obj_loader/tiny_obj_loader.cpp
)
include_directories(../third_party/tiny_obj_loader/include)


# Link all the sources into one
set(LANTERN_SRC
	${SRC_ROOT}
	${SRC_CAMERA}
	${SRC_MATH}
	${SRC_SCENE}
	${SRC_BSDFS}
	${SRC_RENDERER}
	${SRC_VISUALIZER}
	${SRC_THIRD_PARTY}
)


# Create exe
add_executable(lantern ${LANTERN_SRC})
target_link_libraries(lantern embree glfw ${GLFW_LIBRARIES} imgui ${TBB_LIBRARIES})
