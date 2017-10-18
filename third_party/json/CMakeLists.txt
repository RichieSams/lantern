project(json CXX)

cmake_minimum_required(VERSION 3.2)

add_library(json INTERFACE)
target_include_directories(json INTERFACE ./)
