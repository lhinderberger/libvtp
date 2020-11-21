# Copyright 2020 Lucas Hinderberger
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required (VERSION 3.16)
set(CMAKE_C_STANDARD 90)
add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter) # TODO: Find a portable way of specifying this

project("libvtp")
include_directories(include)

add_library(vtp STATIC src/codec.c src/fold.c)

add_executable (cli src/cli.c)
target_link_libraries(cli PRIVATE vtp)
set_target_properties(cli PROPERTIES OUTPUT_NAME "vtp")

enable_testing()
add_executable(tests tests/main.c tests/codec.c tests/fold.c)
target_link_libraries(tests PRIVATE vtp)
add_test(NAME tests COMMAND tests)
