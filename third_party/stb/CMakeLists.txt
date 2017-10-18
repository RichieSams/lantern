project(stb CXX)

cmake_minimum_required(VERSION 3.2)

add_library(stb stb.cpp include/stb/stb_image.h)
target_include_directories(stb PUBLIC include)
