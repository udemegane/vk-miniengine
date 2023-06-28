cmake_minimum_required(VERSION 3.10)

set(PROJNAME "vulkan-pathtracer")
project(${PROJNAME})
set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

add_subdirectory(third_party)
set(THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

add_subdirectory(src)

_finalize_slang_dependency()

#