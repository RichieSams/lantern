project(vk_mem_alloc CXX)

cmake_minimum_required(VERSION 3.2)

add_library(vk_mem_alloc STATIC src/vk_mem_alloc.cpp include/vk_mem_alloc.h)
target_include_directories(vk_mem_alloc PUBLIC include)
target_link_libraries(vk_mem_alloc ${VULKAN_LIBRARY})
target_include_directories(vk_mem_alloc PUBLIC ${VULKAN_INCLUDE_DIR})
