project(linalg CXX)

cmake_minimum_required(VERSION 3.2)

add_library(linalg INTERFACE)
target_include_directories(linalg INTERFACE ./)
