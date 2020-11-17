cmake_minimum_required (VERSION 3.16)

project("libvtp")

add_library(vtp STATIC src/codec.c)

add_executable (cli src/cli.c)
target_link_libraries(cli PRIVATE vtp)
set_target_properties(cli PROPERTIES OUTPUT_NAME "vtp")