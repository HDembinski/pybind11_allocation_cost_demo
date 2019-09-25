cmake_minimum_required(VERSION 2.8.12)
project(foo)

add_subdirectory(pybind11)
pybind11_add_module(foo foo.cpp)
target_compile_options(foo PRIVATE -O3)