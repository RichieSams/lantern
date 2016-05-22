cmake_minimum_required(VERSION 2.8)
project(lantern)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")


# Detect CPU and enable available vector units 
#include(OptimizeForArchitecture)
#OptimizeForArchitecture()

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Vc_ARCHITECTURE_FLAGS}")


# Add necessary dependencies
find_package(OpenGL REQUIRED)

find_package(TBB REQUIRED)
include_directories(${TBB_INCLUDE_DIRS})


# Add third party libs
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_TESTS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_INSTALL OFF CACHE BOOL  "GLFW lib only" )
add_subdirectory(third_party/glfw)
include_directories(third_party/glfw/include)

add_subdirectory(third_party/imgui)
include_directories(third_party/imgui/include)

SET(ENABLE_ISPC_SUPPORT OFF CACHE BOOL "No ISPC support needed atm" )
SET(ENABLE_TUTORIALS OFF CACHE BOOL "Only build lib" )
SET(ENABLE_TESTS OFF CACHE BOOL "Only build lib")
add_subdirectory(third_party/embree)
include_directories(third_party/embree/include)
include_directories(third_party/embree/common)


# Add the main source
include_directories(source)
add_subdirectory(source)
