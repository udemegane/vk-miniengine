cmake_minimum_required(VERSION 3.19)

set(PROJNAME "vk-miniengine")
project(${PROJNAME})
set(CMAKE_CXX_STANDARD 17)
set(VCPKG_TARGET_TRIPLET x64-windows)

find_package(Vulkan REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set and include cmake script path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
include(media)

# Set project output pathes
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(CMAKE_CONFIGURATION_TYPES)
    set(APP_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)
else()
    set(APP_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

set(SHADER_OUTPUT_DIRECTORY ${APP_OUTPUT_DIRECTORY}/shaders)

#
add_subdirectory(third_party)
set(THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

add_subdirectory(src)

#