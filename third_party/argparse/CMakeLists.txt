project(argparse C)

cmake_minimum_required(VERSION 3.2)

add_library(argparse STATIC argparse.c argparse.h)
target_include_directories(argparse PUBLIC ./)
