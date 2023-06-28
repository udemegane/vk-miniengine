find_package(Vulkan REQUIRED)

# tinyobjloader
add_library(tinyobjloader INTERFACE)
set(TINYOBJLOADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tinyobjloader)
target_sources(tinyobjloader PRIVATE
    ${TINYOBJLOADER_DIR}/tiny_obj_loader.h
    ${TINYOBJLOADER_DIR}/tiny_obj_loader.cc
)
target_include_directories(tinyobjloader INTERFACE ${TINYOBJLOADER_DIR})

# stb
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)

if(MSVC)
    target_compile_definitions(stb INTERFACE STBI_MSC_SECURE_CRT)
endif()

# glm
add_library(glm INTERFACE)
set(GLM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glm)
target_sources(glm INTERFACE ${GLM_DIR}/glm/glm.hpp)
target_include_directories(glm INTERFACE ${GLM_DIR})

# fastgltf
add_subdirectory(fastgltf)

# vulkanmemoryallocator
add_library(VulkanMemoryAllocator INTERFACE)
target_include_directories(VulkanMemoryAllocator INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/VulkanMemoryAllocator/include)

# vk-bootstrap
add_library(vk-bootstrap STATIC)
set(VKBOOTSTRAP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vk-bootstrap)
target_sources(vk-bootstrap PRIVATE
    ${VKBOOTSTRAP_DIR}/src/VkBootstrap.h
    ${VKBOOTSTRAP_DIR}/src/VkBootstrap.cpp
)
target_include_directories(vk-bootstrap PUBLIC ${VKBOOTSTRAP_DIR}/src)
target_link_libraries(vk-bootstrap PUBLIC Vulkan::Vulkan $<$<BOOL:UNIX>:${CMAKE_DL_LIBS}>)

# sdl2
add_library(sdl2 INTERFACE)

target_include_directories(sdl2 INTERFACE $ENV{VULKAN_SDK}/Include/SDL2)
target_link_directories(sdl2 INTERFACE $ENV{VULKAN_SDK}/Lib)

target_link_libraries(sdl2 INTERFACE SDL2 SDL2main)

# # glfw
# set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# add_subdirectory(glfw)

# slang
set(SLANG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/slang)
add_library(slang SHARED IMPORTED GLOBAL)
set_target_properties(slang PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${SLANG_DIR}
    IMPORTED_IMPLIB ${SLANG_DIR}/bin/windows-x64/release/slang.lib
    IMPORTED_LOCATION ${SLANG_DIR}/bin/windows-x64/release/slang.dll
)
file(GLOB SLANG_BIN LIST_DIRECTORIES false "${SLANG_DIR}/bin/windows-x64/release/*.dll" "${SLANG_DIR}/bin/windows-x64/release/*.lib")

macro(_finalize_slang_dependency)
    foreach(bin ${SLANG_BIN})
        add_custom_command(
            TARGET ${PROJNAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${bin} $<TARGET_FILE_DIR:${PROJNAME}>
            COMMENT "Copy binaries to $<TARGET_FILE_DIR:${PROJNAME}>"
        )
    endforeach()
endmacro()