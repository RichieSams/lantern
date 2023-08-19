add_subdirectory(argparse)

add_subdirectory(gl3w)

set( GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE )
set( GLFW_INSTALL OFF CACHE BOOL "" FORCE )
add_subdirectory(glfw)

add_subdirectory(imgui)

add_subdirectory(stb)
