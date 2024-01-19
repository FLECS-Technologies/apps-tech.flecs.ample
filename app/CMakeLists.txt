# Copyright 2021-2023 FLECS Technologies GmbH
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.21)

include(${CMAKE_SOURCE_DIR}/flecs-build/cmake/flecs-build.cmake)

project(tech.flecs.ample VERSION 0.1.0)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    add_subdirectory(flecs-external)
endif()
add_subdirectory(flunder)

if(FLECS_BUILD_TESTS)
    include(CTest)
    add_compile_options(-fprofile-arcs -ftest-coverage)
    add_link_options(-lgcov --coverage)
    add_definitions(-DFLECS_UNIT_TEST)
    add_definitions(-DFLECS_CONSTEXPR_UNLESS_TESTED=)
    add_definitions(-DFLECS_FINAL_UNLESS_TESTED=)
else()
    add_definitions(-DFLECS_CONSTEXPR_UNLESS_TESTED=constexpr)
    add_definitions(-DFLECS_FINAL_UNLESS_TESTED=final)
endif()

set(SRC
    src/example_app.cpp
)

add_executable(example_app ${SRC})

target_include_directories(example_app PRIVATE include)
target_link_libraries(example_app PUBLIC
    flunder.shared
)
