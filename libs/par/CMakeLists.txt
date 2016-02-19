cmake_minimum_required(VERSION 2.8)
project(par)

include_directories(include)

set(PAR_SRC
   par_shapes.cpp
)

set(IMGUI_HEADERS
   include/par_shapes.h
)

add_library(par STATIC ${PAR_SRC} ${PAR_HEADERS})
