cmake_minimum_required(VERSION 2.8)
project(lantern)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")


set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11


# Add necessary dependencies
find_package(OpenGL REQUIRED)


# Add third party libs
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_TESTS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
add_subdirectory(third_party/glfw)
include_directories(third_party/glfw/include)

add_subdirectory(third_party/imgui)
include_directories(third_party/imgui/include)

set( TBB_BUILD_SHARED OFF CACHE BOOL  "Only build TBB static" )
set( TBB_BUILD_TBBMALLOC_PROXY OFF CACHE BOOL  "Don't need tbbmalloc_proxy" )
set( TBB_BUILD_TESTS OFF CACHE BOOL  "Build TBB lib only" )
add_subdirectory(third_party/tbb)
include_directories(third_party/tbb/include)


SET(EMBREE_ISPC_SUPPORT OFF CACHE BOOL "No ISPC support needed atm" )
SET(EMBREE_TUTORIALS OFF CACHE BOOL "Only build lib" )
SET(EMBREE_STATIC_LIB ON CACHE BOOL "Build Embree as a static lib")
add_subdirectory(third_party/embree)
include_directories(third_party/embree/include)
include_directories(third_party/embree/common)

include_directories(third_party/tiny_obj_loader/include)
include_directories(third_party/json)
include_directories(third_party/ez_option_parser)
include_directories(third_party/stb/include)


# Add the main source
add_subdirectory(source)
