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



# Make sure we're using C++11
if(UNIX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()
if(APPLE)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()


# Source files
set(LANTERN_SRC
	source/main.cpp)

set(LANTERN_HEADERS
	)


add_executable(lantern ${LANTERN_SRC})
target_link_libraries(lantern glfw embree ${GLFW_LIBRARIES})
