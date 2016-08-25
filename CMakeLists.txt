cmake_minimum_required (VERSION 2.6)
project (Example)
add_executable(t4 t4.c)
target_link_libraries (t4 yices)
