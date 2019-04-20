cmake_minimum_required(VERSION 3.2)
project(lantern)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")


set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	# Force iterator debugging off so we can link libraries compiled in Release
	add_definitions(-D_HAS_ITERATOR_DEBUGGING=0 -D_ITERATOR_DEBUG_LEVEL=0)
	# Force all builds to use "Release" version of the runtime
	foreach(flag_var
	        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
	        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
		if(${flag_var} MATCHES "/MTd")
			string(REGEX REPLACE "/MTd" "/MT" ${flag_var} "${${flag_var}}")
		endif(${flag_var} MATCHES "/MTd")
		if(${flag_var} MATCHES "/MDd")
			string(REGEX REPLACE "/MDd" "/MD" ${flag_var} "${${flag_var}}")
		endif(${flag_var} MATCHES "/MDd")
	endforeach(flag_var)
endif()


# Add third party libs
add_subdirectory(third_party)

# Add the main source
add_subdirectory(source)
