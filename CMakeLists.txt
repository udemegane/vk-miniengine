cmake_minimum_required(VERSION 3.19)

set(PROJNAME "vulkan-pathtracer")
project(${PROJNAME})
set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(third_party)
set(THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

add_subdirectory(src)

#