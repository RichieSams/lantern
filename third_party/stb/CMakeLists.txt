project(stb CXX)

cmake_minimum_required(VERSION 3.2)

add_library(stb STATIC stb.cpp include/stb_image.h include/stb_image_write.h include/stb_rect_pack.h include/stb_textedit.h include/stb_truetype.h)
target_include_directories(stb PUBLIC include)
