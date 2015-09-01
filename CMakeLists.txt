cmake_minimum_required(VERSION 2.8)

project(lantern)


# Add necessary dependencies
find_package(OpenGL REQUIRED)

# Add third party libs
add_subdirectory(libs/glfw)
include_directories(libs/glfw/include)
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_TESTS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_INSTALL OFF CACHE BOOL  "GLFW lib only" )


# Make sure we're using C++11
if(UNIX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()
if(APPLE)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()


# Source files
include_directories(include)

set(LANTERN_SRC
	source/main.cpp)

set(LANTERN_HEADERS
	)


add_executable(lantern ${LANTERN_SRC})
target_link_libraries(lantern glfw ${GLFW_LIBRARIES})
