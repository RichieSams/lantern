include(SetSourceGroup)

SetSourceGroup(NAME Root
	SOURCE_FILES main.cpp
)

SetSourceGroup(NAME Camera
	SOURCE_FILES camera/thin_lens_camera.h
	             camera/thin_lens_camera.cpp
	             camera/frame_buffer.h
	             camera/frame_buffer.cpp
	             camera/reconstruction_filter.h
	             camera/reconstruction_filter.cpp
)

SetSourceGroup(NAME Math
	SOURCE_FILES math/uniform_sampler.h
	             math/vector_types.h
	             math/int_types.h
)

SetSourceGroup(NAME Scene
	SOURCE_FILES scene/geometry_generator.h
	             scene/geometry_generator.cpp
	             scene/mesh_elements.h
	             scene/ray.h
	             scene/scene.h
	             scene/scene.cpp
)

SetSourceGroup(NAME Materials
	SOURCE_FILES materials/material.h
	             materials/lambert_material.h
	             materials/mirror.h
)

SetSourceGroup(NAME Renderer
	SOURCE_FILES renderer/renderer.h
	             renderer/renderer.cpp
)

SetSourceGroup(NAME Visualizer
	SOURCE_FILES visualizer/visualizer.h
	             visualizer/visualizer.cpp
)

# Link all the sources into one
set(LANTERN_SRC
	${SRC_ROOT}
	${SRC_CAMERA}
	${SRC_MATH}
	${SRC_SCENE}
	${SRC_MATERIALS}
	${SRC_RENDERER}
	${SRC_VISUALIZER}
)


# Create exe
add_executable(lantern ${LANTERN_SRC})
target_link_libraries(lantern embree glfw ${GLFW_LIBRARIES} imgui ${TBB_LIBRARIES})
