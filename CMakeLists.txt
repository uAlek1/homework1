cmake_minimum_required(VERSION 3.10.0)
project(homework1 VERSION 0.1.0 LANGUAGES C CXX)

add_executable(homework1 main.cpp)


target_compile_options(homework1 PRIVATE
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Release>:-fno-tree-vectorize>
)

target_compile_options(homework1 PRIVATE -mavx2)