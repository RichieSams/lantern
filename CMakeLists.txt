cmake_minimum_required(VERSION 2.8)
project(lantern)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")


# Detect CPU and enable available vector units 
#include(OptimizeForArchitecture)
#OptimizeForArchitecture()

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Vc_ARCHITECTURE_FLAGS}")


# Add necessary dependencies
find_package(OpenGL REQUIRED)


# Add third party libs
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_TESTS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_INSTALL OFF CACHE BOOL  "GLFW lib only" )
add_subdirectory(libs/glfw)
include_directories(libs/glfw/include)


SET(ENABLE_ISPC_SUPPORT OFF CACHE BOOL "No ISPC support needed atm" )
SET(ENABLE_TUTORIALS OFF CACHE BOOL "Only build lib" )
add_subdirectory(libs/embree)
include_directories(libs/embree/include)


# Source files
set(SRC_ROOT
   source/main.cpp
)
source_group(" " FILES ${SRC_ROOT})

set(SRC_COMMON
   source/common/typedefs.h
   source/common/mesh_elements.h
   source/common/vector_types.h
)
source_group("Common" FILES ${SRC_COMMON})

set(SRC_RENDERER
   source/renderer/renderer.h
   source/renderer/renderer.cpp
)
source_group("Renderer" FILES ${SRC_RENDERER})

set(SRC_VISUALIZER
)
source_group("Visualizer" FILES ${SRC_VISUALIZER})

# Link all the sources into one
set(LANTERN_SRC
   ${SRC_ROOT}
   ${SRC_COMMON}
   ${SRC_RENDERER}
   ${SRC_VISUALIZER}
)


# Create exe
add_executable(lantern ${LANTERN_SRC})
target_link_libraries(lantern embree glfw ${GLFW_LIBRARIES})
