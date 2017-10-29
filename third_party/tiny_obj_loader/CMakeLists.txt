project(tiny_obj_loader CXX)

cmake_minimum_required(VERSION 3.2)

add_library(tiny_obj_loader STATIC tiny_obj_loader.cpp include/tiny_obj_loader/tiny_obj_loader.h)
target_include_directories(tiny_obj_loader PUBLIC include)
