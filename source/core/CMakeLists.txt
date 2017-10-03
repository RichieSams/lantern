include(SetSourceGroup)

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
	             scene/ray.h
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
)

SetSourceGroup(NAME "Materials/Media"
	PREFIX LANTERN_CORE
	SOURCE_FILES materials/media/medium.h
	             materials/media/non_scattering_medium.h
	             materials/media/isotropic_scattering_medium.h
)

SetSourceGroup(NAME Renderer
	PREFIX LANTERN_CORE
	SOURCE_FILES renderer/renderer.h
	             renderer/renderer.cpp
	             renderer/surface_interaction.h
)

SetSourceGroup(NAME Visualizer
	PREFIX LANTERN_CORE
	SOURCE_FILES visualizer/visualizer.h
	             visualizer/visualizer.cpp
)


# For header-only / single file libraries, it's easier to just compile them
# directly, rather than creating a seperate lib and linking
SetSourceGroup(NAME "Third Party/Tiny Obj Loader"
	PREFIX LANTERN_CORE
	SOURCE_FILES ../../third_party/tiny_obj_loader/include/tiny_obj_loader/tiny_obj_loader.h
	             ../../third_party/tiny_obj_loader/tiny_obj_loader.cpp
)

SetSourceGroup(NAME "Third Party/JSON"
	PREFIX LANTERN_CORE
	SOURCE_FILES ../../third_party/json/json.h
)

SetSourceGroup(NAME "Third Party/EZ Option Parser"
	PREFIX LANTERN_CORE
	SOURCE_FILES ../../third_party/ez_option_parser/ez_option_parser.h
)

SetSourceGroup(NAME "Third Party/STB"
	PREFIX LANTERN_CORE
	SOURCE_FILES ../../third_party/stb/include/stb/stb_image.h
	             ../../third_party/stb/stb.cpp
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
	${LANTERN_CORE_RENDERER}
	${LANTERN_CORE_VISUALIZER}
	${LANTERN_CORE_THIRD_PARTY_TINY_OBJ_LOADER}
	${LANTERN_CORE_THIRD_PARTY_JSON}
	${LANTERN_CORE_THIRD_PARTY_EZ_OPTION_PARSER}
	${LANTERN_CORE_THIRD_PARTY_STB}
)

# Create lib
add_library(lantern_core ${LANTERN_CORE_SRC})
target_link_libraries(lantern_core glfw ${GLFW_LIBRARIES} ${OPENGL_gl_LIBRARY} imgui tbb_static tbbmalloc_static embree)