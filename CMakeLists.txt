cmake_minimum_required(VERSION 3.19.2)
project(lantern CXX ISPC)

# Options
set(ISPC_VALID_TARGETS sse2-i32x4 sse2-i32x8 sse4-i8x16 sse4-i16x8 sse4-i32x4 sse4-i32x8 avx1-i32x4 avx1-i32x8 avx1-i32x16 avx1-i64x4 avx2-i8x32 avx2-i16x16 avx2-i32x4 avx2-i32x8 avx2-i32x16 avx2-i64x4 avx512knl-i32x16 avx512skx-i32x8 avx512skx-i32x16 avx512skx-i8x64 avx512skx-i16x32 neon-i8x16 neon-i16x8 neon-i32x4 neon-i32x8 genx-x8 genx-x16)
set(LATERN_ISPC_TARGET avx2-i32x8 CACHE STRING "ISPC Target instruction set")
set(LANTERN_ISPC_EXTRA_FLAGS "" CACHE STRING "Additional args to use with ISPC")

set(CMAKE_CXX_STANDARD 17) # C++17...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

# Calculate arch for ISPC
if (UNIX)
    if(NOT LATERN_ISPC_ARCH)
        execute_process( COMMAND bash "-c" "uname -m | sed -e s/x86_64/x86/ -e s/amd64/x86/ -e s/i686/x86/ -e s/arm64/aarch64/ -e s/arm.*/arm/ -e s/sa110/arm/" OUTPUT_VARIABLE ARCH)

        string(STRIP ${ARCH} ARCH)
        execute_process( COMMAND getconf LONG_BIT OUTPUT_VARIABLE ARCH_BIT)
        string(STRIP ${ARCH_BIT} arch_bit)
        if ("${ARCH}" STREQUAL "x86")
            if (${arch_bit} EQUAL 32)
                set(ispc_arch "x86")
            else()
                set(ispc_arch "x86-64")
            endif()
        elseif ("${ARCH}" STREQUAL "arm")
            set(ispc_arch "arm")
        elseif ("${ARCH}" STREQUAL "aarch64")
            set(ispc_arch "aarch64")
        endif()
    endif()

    set(LATERN_ISPC_ARCH "${ispc_arch}" CACHE STRING "ISPC CPU ARCH")
    set(LATERN_ISPC_ARCH_BIT "${arch_bit}" CACHE STRING "ISPC CPU BIT")
else()
    if(NOT LATERN_ISPC_ARCH)
        set(ispc_arch "x86")
        if (CMAKE_SIZEOF_VOID_P EQUAL 8 )
            set(ispc_arch "x86-64")
        endif()
    endif()

    set(LATERN_ISPC_ARCH "${ispc_arch}" CACHE STRING "ISPC CPU ARCH")
    set(LATERN_ISPC_ARCH_BIT "${arch_bit}" CACHE STRING "ISPC CPU BIT")
endif()

set(LANTERN_ISPC_FLAGS ${LANTERN_ISPC_EXTRA_FLAGS})
# For unix platforms, we have to enable compiling with Position Independent Code
# Otherwise linking will fail
if (UNIX)
    set(LANTERN_ISPC_FLAGS ${LANTERN_ISPC_FLAGS} "--pic")
endif()


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	# Force iterator debugging off so we can link libraries compiled in Release
	add_definitions(-D_HAS_ITERATOR_DEBUGGING=0 -D_ITERATOR_DEBUG_LEVEL=0)
endif()


# Add third party libs
add_subdirectory(third_party)

# Add the main source
add_subdirectory(source)
