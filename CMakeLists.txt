cmake_minimum_required (VERSION 3.16)
set(CMAKE_C_STANDARD 90)
add_compile_options(-Wall -Wextra -Wpedantic -Werror) # TODO: Find a portable way of specifying this

project("libvtp")
include_directories(include)

add_library(vtp STATIC src/codec.c)

add_executable (cli src/cli.c)
target_link_libraries(cli PRIVATE vtp)
set_target_properties(cli PROPERTIES OUTPUT_NAME "vtp")

enable_testing()
add_executable(tests tests/main.c tests/codec.c)
target_link_libraries(tests PRIVATE vtp)
add_test(NAME tests COMMAND tests)
